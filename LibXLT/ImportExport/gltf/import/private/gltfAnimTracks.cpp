/****************************************************************************\
**  gltfAnimTracks.cpp
**
**      see gltfAnimTracks.hpp
**
**	The rule: what an object's animations become
**
**	- Skinned and morph-animated meshes play their animations as Anim-Full
**	  clips (gltfAnimRead::AnimationsFor).
**	- The object that carries a node's transform takes the node's
**	  translation, rotation and scale channels: a group, or a mesh, light
**	  or camera that isn't under its own node's group (a node with
**	  children is a group, and its content sits under it with an identity
**	  transform). A skinned mesh takes none (glTF ignores its node's
**	  transform).
**	- Rigid meshes take keys when they can: every scale key uniform
**	  (within c_ScaleTolerance), no animated scale on a non-uniform rest
**	  scale (it is baked into the template), and at most c_MaxKeys keys
**	  per track after Reduce. Otherwise the mesh is a transform-clip mesh:
**	  it sits at its parent with an identity transform, its template's
**	  base node carries the node's transform (gltfModelImport), and every
**	  animation that moves it is a clip that animates that node
**	  (gltfAnimKeys). Reasons it can't always be keys: the character's
**	  Scale channel is one float; MachStudio blends keys with its own
**	  blend types, which can't express CUBICSPLINE tangents (sampled every
**	  1/c_SampleRate s and reduced); and dense exports would make
**	  hundreds of key drivers per object.
**	- Groups always take keys (a group can't play a clip): a non-uniform
**	  scale is keyed as it is here and meaned by the importer, with a
**	  warning; a track over c_MaxKeys is kept, with a warning.
**	- Lights and cameras always take keys.
**	- KHR_animation_pointer channels (gltfAnimRead::ResolveTarget) are
**	  keys too: a light's on every light object of that light, a camera's
**	  on every camera object of that camera, a material's on every mesh
**	  that uses the material (one material object per mesh object in
**	  MachStudio), named by gltfMaterialRead::MaterialKey. Light colour,
**	  intensity, range (centimetres) and cone half-angles; camera yfov,
**	  xmag, ymag, znear, zfar (centimetres); material parameters by their
**	  effPbrData names (baseColorFactor, metallicFactor, roughnessFactor,
**	  emissiveFactor, alphaCutoff, occlusionTexture/strength,
**	  emissiveStrength) and the normal map scale. Other pointers are
**	  skipped, one warning each.
**
**	When they play
**
**	- An object's animations (any that touch it: its transform, its
**	  weights, its skeleton, its light, camera or materials) play end to end in file order from time 0,
**	  each for its whole length; its keys and clips share this schedule.
**	  Objects touched by different animations each start at 0.
**	- Keys are in seconds: a channel's frames from its animation's start,
**	  plus the animation's begin. Where one animation's first key falls
**	  on the last key of the one before, it replaces it.
**	- STEP keys stay STEP (each holds until the next). A track that mixes
**	  STEP and other channels is linear, the STEP keys held until
**	  c_HoldSeconds before the next key.
**
**	Reduce
**
**	- A linear track keeps its first and last keys and, between them, the
**	  fewest keys such that interpolating the kept keys gives every
**	  dropped key's value within tolerance (translation
**	  c_TranslationTolerance, rotation c_RotationTolerance as an angle,
**	  scale c_ScaleTolerance, others c_ValueTolerance times
**	  max(1, |value|)); greedy from the start. A step track drops keys
**	  equal (within the same tolerances) to the key before.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GLTF_ANIMTRACKS_HPP
#include "ImportExport/gltf/import/private/gltfAnimTracks.hpp"
#endif
#ifndef GLTF_ANIMREAD_HPP
#include "ImportExport/gltf/import/private/gltfAnimRead.hpp"
#endif
#ifndef GLTF_MATERIALREAD_HPP
#include "ImportExport/gltf/import/private/gltfMaterialRead.hpp"
#endif
#ifndef GLTF_MESHIMPORT_HPP
#include "ImportExport/gltf/import/private/gltfMeshImport.hpp"
#endif
#ifndef EFF_PBRDATA_HPP
#include "Graphics/Eff/effPbrData.hpp"
#endif

#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif

#include <algorithm>
#include <cmath>
#include <map>
#include <set>

namespace
{
	const float c_SameTime = 1e-6f;			// seconds: two keys at the same time
	const float c_HoldSeconds = 1e-3f;		// a STEP key in a linear track holds until this long before the next

	//--------------------------------------------------------------------
	// every animation, read once, STEP keys as given
	//--------------------------------------------------------------------
	std::vector<gltfClip> read_clips(const tinygltf::Model& i_Model, float i_Fps)
	{
		const std::vector<std::string> identities = gltfAnimRead::ClipIdentities(i_Model);
		std::vector<gltfClip> clips(identities.size());
		for (size_t a = 0; a < identities.size(); a++)
			gltfAnimRead::ReadClip(i_Model, identities[a], i_Fps, clips[a], false);
		return clips;
	}

	//--------------------------------------------------------------------
	// each animation's length, seconds
	//--------------------------------------------------------------------
	std::vector<float> animation_lengths(const std::vector<gltfClip>& i_Clips, float i_Fps)
	{
		std::vector<float> lengths;
		for (const gltfClip& clip : i_Clips)
			lengths.push_back(clip.m_Length / i_Fps);
		return lengths;
	}

	//--------------------------------------------------------------------
	// true when the channel's target is the object's transform, light,
	// camera or one of its materials
	//--------------------------------------------------------------------
	bool is_target(const gltfSceneObject& i_Object, const gltfAnimTarget& i_Target)
	{
		switch (i_Target.m_Kind)
		{
			case gltfAnimTarget::e_Node:
				return i_Object.m_bNodeTransform && i_Object.m_Kind != gltfSceneObject::e_SkinnedMesh
					   && i_Target.m_Index == i_Object.m_Node && i_Target.m_Path != gltfAnimChannel::e_Weights;
			case gltfAnimTarget::e_Light:
				return i_Object.m_Kind == gltfSceneObject::e_Light && i_Target.m_Index == i_Object.m_Light;
			case gltfAnimTarget::e_Camera:
				return i_Object.m_Kind == gltfSceneObject::e_Camera && i_Target.m_Index == i_Object.m_Camera;
			case gltfAnimTarget::e_Material:
				return std::find(i_Object.m_Materials.begin(), i_Object.m_Materials.end(), i_Target.m_Index)
					   != i_Object.m_Materials.end();
			default:
				return false;
		}
	}

	//--------------------------------------------------------------------
	// true when the animation touches the object
	//--------------------------------------------------------------------
	bool touches(const tinygltf::Model& i_Model, const gltfSceneObject& i_Object, int i_Animation,
				 const std::vector<int>& i_Clips)
	{
		if (std::find(i_Clips.begin(), i_Clips.end(), i_Animation) != i_Clips.end())
			return true;
		for (const tinygltf::AnimationChannel& channel : i_Model.animations[i_Animation].channels)
		{
			if (is_target(i_Object, gltfAnimRead::ResolveTarget(i_Model, channel)))
				return true;
		}
		return false;
	}

	//--------------------------------------------------------------------
	// the track a pointer's property becomes, and the scale of its values;
	// false for a property no track takes
	//--------------------------------------------------------------------
	bool pointer_track(const tinygltf::Model& i_Model, const gltfAnimTarget& i_Target, gltfTrack& o_Track, float& o_Scale)
	{
		const std::string& p = i_Target.m_Property;
		o_Scale = 1;
		if (i_Target.m_Kind == gltfAnimTarget::e_Light)
		{
			if (p == "color")
				o_Track.m_Property = gltfTrack::e_LightColor;
			else if (p == "intensity")
				o_Track.m_Property = gltfTrack::e_LightIntensity;
			else if (p == "range")
			{
				o_Track.m_Property = gltfTrack::e_LightRange;
				o_Scale = gltfMeshImport::c_UnitScale;
			}
			else if (p == "spot/innerConeAngle")
				o_Track.m_Property = gltfTrack::e_SpotInnerCone;
			else if (p == "spot/outerConeAngle")
				o_Track.m_Property = gltfTrack::e_SpotOuterCone;
			else
				return false;
			return true;
		}
		if (i_Target.m_Kind == gltfAnimTarget::e_Camera)
		{
			const std::string field = p.substr(p.find('/') + 1);
			if (field == "yfov")
				o_Track.m_Property = gltfTrack::e_CameraYFov;
			else if (field == "xmag")
				o_Track.m_Property = gltfTrack::e_CameraXMag;
			else if (field == "ymag")
				o_Track.m_Property = gltfTrack::e_CameraYMag;
			else if (field == "znear")
				o_Track.m_Property = gltfTrack::e_CameraNear;
			else if (field == "zfar")
				o_Track.m_Property = gltfTrack::e_CameraFar;
			else
				return false;
			if (field != "yfov")
				o_Scale = gltfMeshImport::c_UnitScale;
			return true;
		}
		if (i_Target.m_Kind == gltfAnimTarget::e_Material)
		{
			o_Track.m_Material = gltfMaterialRead::MaterialKey(i_Model, i_Target.m_Index);
			o_Track.m_Property = gltfTrack::e_MaterialParam;
			if (p == "pbrMetallicRoughness/baseColorFactor")
				o_Track.m_Param = effPbrData::c_BaseColor;
			else if (p == "pbrMetallicRoughness/metallicFactor")
				o_Track.m_Param = effPbrData::c_Metallic;
			else if (p == "pbrMetallicRoughness/roughnessFactor")
				o_Track.m_Param = effPbrData::c_Roughness;
			else if (p == "emissiveFactor")
				o_Track.m_Param = effPbrData::c_Emissive;
			else if (p == "alphaCutoff")
				o_Track.m_Param = effPbrData::c_AlphaCutoff;
			else if (p == "occlusionTexture/strength")
				o_Track.m_Param = effPbrData::c_OcclusionStrength;
			else if (p == "extensions/KHR_materials_emissive_strength/emissiveStrength")
				o_Track.m_Param = effPbrData::c_EmissiveStrength;
			else if (p == "normalTexture/scale")
				o_Track.m_Property = gltfTrack::e_MaterialNormalScale;
			else
				return false;
			return true;
		}
		return false;
	}

	std::vector<gltfScheduledAnim> schedule(const tinygltf::Model& i_Model, const gltfSceneObject& i_Object,
											const std::vector<float>& i_Lengths)
	{
		std::vector<gltfScheduledAnim> scheduled;
		const std::vector<int> clips = gltfAnimRead::AnimationsFor(i_Model, i_Object);
		float begin = 0;
		for (int a = 0; a < (int)i_Model.animations.size(); a++)
		{
			if (!touches(i_Model, i_Object, a, clips))
				continue;
			gltfScheduledAnim anim;
			anim.m_Animation = a;
			anim.m_Begin = begin;
			scheduled.push_back(anim);
			begin += i_Lengths[a];
		}
		return scheduled;
	}

	//--------------------------------------------------------------------
	// keys k and k + 1 become: k, a hold of k's value c_HoldSeconds before
	// k + 1, k + 1
	//--------------------------------------------------------------------
	void add_holds(std::vector<float>& io_Times, std::vector<float>& io_Values, int i_Components)
	{
		std::vector<float> times, values;
		for (size_t k = 0; k < io_Times.size(); k++)
		{
			const float* v = &io_Values[k * i_Components];
			times.push_back(io_Times[k]);
			values.insert(values.end(), v, v + i_Components);
			if (k + 1 < io_Times.size() && io_Times[k + 1] - c_HoldSeconds > io_Times[k])
			{
				times.push_back(io_Times[k + 1] - c_HoldSeconds);
				values.insert(values.end(), v, v + i_Components);
			}
		}
		io_Times.swap(times);
		io_Values.swap(values);
	}

	//--------------------------------------------------------------------
	// appends a channel's keys (frames from its animation's start) to a
	// track at i_Begin seconds
	//--------------------------------------------------------------------
	void append(gltfTrack& io_Track, const gltfAnimChannel& i_Channel, float i_Begin, float i_Fps)
	{
		std::vector<float> times, values = i_Channel.m_Values;
		for (float frame : i_Channel.m_Times)
			times.push_back(i_Begin + frame / i_Fps);
		const int n = i_Channel.m_Components;

		if (io_Track.m_Times.empty())
		{
			io_Track.m_Components = n;
			io_Track.m_bStep = i_Channel.m_bStep;
		}
		else if (io_Track.m_bStep != i_Channel.m_bStep)
		{
			if (io_Track.m_bStep)
				add_holds(io_Track.m_Times, io_Track.m_Values, n);
			else
				add_holds(times, values, n);
			io_Track.m_bStep = false;
		}
		if (n != io_Track.m_Components)
			return;

		for (size_t k = 0; k < times.size(); k++)
		{
			const float* v = &values[k * n];
			if (!io_Track.m_Times.empty() && times[k] <= io_Track.m_Times.back() + c_SameTime)
			{
				// the first key of the next animation replaces the last of the one before
				io_Track.m_Times.pop_back();
				io_Track.m_Values.resize(io_Track.m_Values.size() - n);
			}
			io_Track.m_Times.push_back(times[k]);
			io_Track.m_Values.insert(io_Track.m_Values.end(), v, v + n);
		}
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void slerp(const float* i_pA, const float* i_pB, float i_T, float* o_pQ)
	{
		maRotation q;
		q.Slerp(maRotation(i_pA[0], i_pA[1], i_pA[2], i_pA[3]), maRotation(i_pB[0], i_pB[1], i_pB[2], i_pB[3]), i_T);
		o_pQ[0] = q.GetX();
		o_pQ[1] = q.GetY();
		o_pQ[2] = q.GetZ();
		o_pQ[3] = q.GetW();
	}

	// the value between keys a and b at time i_Time
	void interpolate(const gltfTrack& i_Track, size_t a, size_t b, float i_Time, float* o_pValue)
	{
		const int n = i_Track.m_Components;
		const float* va = &i_Track.m_Values[a * n];
		const float* vb = &i_Track.m_Values[b * n];
		const float span = i_Track.m_Times[b] - i_Track.m_Times[a];
		const float t = span > 0 ? (i_Time - i_Track.m_Times[a]) / span : 1.0f;
		if (i_Track.m_Property == gltfTrack::e_Rotation && n == 4)
		{
			slerp(va, vb, t, o_pValue);
			return;
		}
		for (int c = 0; c < n; c++)
			o_pValue[c] = va[c] + (vb[c] - va[c]) * t;
	}

	// true when two values of the track are the same within its tolerance
	bool close(const gltfTrack& i_Track, const float* i_pA, const float* i_pB)
	{
		const int n = i_Track.m_Components;
		switch (i_Track.m_Property)
		{
			case gltfTrack::e_Rotation:
			{
				if (n != 4)
					break;
				// the angle between them: 2 asin(|a - b| / 2), b on a's side
				double dot = 0;
				for (int c = 0; c < 4; c++)
					dot += (double)i_pA[c] * i_pB[c];
				const double sign = dot < 0 ? -1 : 1;
				double d2 = 0;
				for (int c = 0; c < 4; c++)
				{
					const double d = i_pA[c] - sign * i_pB[c];
					d2 += d * d;
				}
				const double angle = 4 * asin(std::min(1.0, sqrt(d2) / 2)) * 180 / 3.14159265358979;
				return angle <= gltfAnimTracks::c_RotationTolerance;
			}
			case gltfTrack::e_Translation:
				for (int c = 0; c < n; c++)
				{
					if (fabsf(i_pA[c] - i_pB[c]) > gltfAnimTracks::c_TranslationTolerance)
						return false;
				}
				return true;
			case gltfTrack::e_Scale:
				for (int c = 0; c < n; c++)
				{
					if (fabsf(i_pA[c] - i_pB[c]) > gltfAnimTracks::c_ScaleTolerance)
						return false;
				}
				return true;
			default:
				break;
		}
		for (int c = 0; c < n; c++)
		{
			const float tolerance = gltfAnimTracks::c_ValueTolerance * std::max(1.0f, fabsf(i_pB[c]));
			if (fabsf(i_pA[c] - i_pB[c]) > tolerance)
				return false;
		}
		return true;
	}

	// true when keys a and b reproduce every key between them
	bool spans(const gltfTrack& i_Track, size_t a, size_t b)
	{
		std::vector<float> value(i_Track.m_Components);
		for (size_t k = a + 1; k < b; k++)
		{
			interpolate(i_Track, a, b, i_Track.m_Times[k], &value[0]);
			if (!close(i_Track, &value[0], &i_Track.m_Values[k * i_Track.m_Components]))
				return false;
		}
		return true;
	}

	bool uniform_scales(const gltfTrack& i_Track)
	{
		for (size_t k = 0; k < i_Track.m_Times.size(); k++)
		{
			const float* s = &i_Track.m_Values[k * 3];
			const float tolerance = gltfAnimTracks::c_ScaleTolerance * std::max(1.0f, fabsf(s[0]));
			if (fabsf(s[0] - s[1]) > tolerance || fabsf(s[0] - s[2]) > tolerance)
				return false;
		}
		return true;
	}

	//--------------------------------------------------------------------
	// the object's tracks on its schedule: its node's translation,
	// rotation and scale, then its light, camera and material properties
	//--------------------------------------------------------------------
	std::vector<gltfTrack> object_tracks(const tinygltf::Model& i_Model, const gltfSceneObject& i_Object,
										 const std::vector<gltfScheduledAnim>& i_Schedule,
										 const std::vector<gltfClip>& i_Clips, float i_Fps)
	{
		std::map<std::string, gltfTrack> tracks;		// by property, material and parameter
		const auto track_for = [&](const gltfTrack& i_Track) -> gltfTrack&
		{
			const std::string key = std::to_string((int)i_Track.m_Property) + "/" + i_Track.m_Material + "/" + i_Track.m_Param;
			gltfTrack& track = tracks[key];
			track.m_Property = i_Track.m_Property;
			track.m_Material = i_Track.m_Material;
			track.m_Param = i_Track.m_Param;
			return track;
		};
		for (const gltfScheduledAnim& anim : i_Schedule)
		{
			const gltfClip& clip = i_Clips[anim.m_Animation];
			if (i_Object.m_bNodeTransform && i_Object.m_Kind != gltfSceneObject::e_SkinnedMesh)
			{
				for (const gltfAnimChannel& channel : clip.m_Channels)
				{
					if (channel.m_Node != i_Object.m_Node || channel.m_Path == gltfAnimChannel::e_Weights)
						continue;
					gltfTrack property;
					property.m_Property = channel.m_Path == gltfAnimChannel::e_Translation ? gltfTrack::e_Translation
										: channel.m_Path == gltfAnimChannel::e_Rotation ? gltfTrack::e_Rotation
										: gltfTrack::e_Scale;
					append(track_for(property), channel, anim.m_Begin, i_Fps);
				}
			}
			for (const gltfPointerChannel& channel : clip.m_Pointers)
			{
				gltfTrack property;
				float scale = 1;
				if (!is_target(i_Object, channel.m_Target) || !pointer_track(i_Model, channel.m_Target, property, scale))
					continue;
				gltfAnimChannel scaled = channel;
				for (float& v : scaled.m_Values)
					v *= scale;
				append(track_for(property), scaled, anim.m_Begin, i_Fps);
			}
		}
		std::vector<gltfTrack> result;
		for (std::map<std::string, gltfTrack>::iterator it = tracks.begin(); it != tracks.end(); ++it)
		{
			gltfAnimTracks::Reduce(it->second);
			result.push_back(it->second);
		}
		return result;
	}

	bool is_transform(const gltfTrack& i_Track)
	{
		return i_Track.m_Property == gltfTrack::e_Translation || i_Track.m_Property == gltfTrack::e_Rotation
			   || i_Track.m_Property == gltfTrack::e_Scale;
	}

	//--------------------------------------------------------------------
	// one warning per pointer this importer doesn't read
	//--------------------------------------------------------------------
	void warn_unsupported_pointers(const tinygltf::Model& i_Model)
	{
		std::set<std::string> warned;
		for (const tinygltf::Animation& animation : i_Model.animations)
		{
			for (const tinygltf::AnimationChannel& channel : animation.channels)
			{
				if (channel.target_path != "pointer")
					continue;
				const gltfAnimTarget target = gltfAnimRead::ResolveTarget(i_Model, channel);
				if (target.m_Kind == gltfAnimTarget::e_None && warned.insert(target.m_Property).second)
					DBG_WARNING("glTF animation pointer " << (target.m_Property.empty() ? "(none)" : target.m_Property)
								<< " isn't supported; its channels are skipped");
			}
		}
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
namespace gltfAnimTracks
{
	std::vector<gltfScheduledAnim> Schedule(const tinygltf::Model& i_Model, const gltfSceneObject& i_Object,
											float i_FramesPerSecond)
	{
		return schedule(i_Model, i_Object, animation_lengths(read_clips(i_Model, i_FramesPerSecond), i_FramesPerSecond));
	}

	void Reduce(gltfTrack& io_Track)
	{
		const size_t num = io_Track.m_Times.size();
		const int n = io_Track.m_Components;
		if (num < 2 || n <= 0)
			return;

		std::vector<size_t> kept(1, 0);
		if (io_Track.m_bStep)
		{
			for (size_t k = 1; k < num; k++)
			{
				if (!close(io_Track, &io_Track.m_Values[kept.back() * n], &io_Track.m_Values[k * n]))
					kept.push_back(k);
			}
		}
		else
		{
			size_t a = 0;
			for (size_t b = 2; b < num; b++)
			{
				if (!spans(io_Track, a, b))
				{
					a = b - 1;
					kept.push_back(a);
				}
			}
			kept.push_back(num - 1);
		}

		std::vector<float> times, values;
		for (size_t k : kept)
		{
			times.push_back(io_Track.m_Times[k]);
			values.insert(values.end(), io_Track.m_Values.begin() + k * n, io_Track.m_Values.begin() + (k + 1) * n);
		}
		io_Track.m_Times.swap(times);
		io_Track.m_Values.swap(values);
	}

	void Sample(const gltfTrack& i_Track, float i_Time, float* o_pValue)
	{
		const int n = i_Track.m_Components;
		const size_t num = i_Track.m_Times.size();
		if (num == 0)
			return;
		const std::vector<float>::const_iterator upper = std::upper_bound(i_Track.m_Times.begin(), i_Track.m_Times.end(), i_Time);
		if (upper == i_Track.m_Times.begin() || upper == i_Track.m_Times.end())
		{
			const size_t k = (upper == i_Track.m_Times.begin()) ? 0 : num - 1;
			std::copy(i_Track.m_Values.begin() + k * n, i_Track.m_Values.begin() + (k + 1) * n, o_pValue);
			return;
		}
		const size_t b = upper - i_Track.m_Times.begin();
		const size_t a = b - 1;
		if (i_Track.m_bStep)
		{
			std::copy(i_Track.m_Values.begin() + a * n, i_Track.m_Values.begin() + (a + 1) * n, o_pValue);
			return;
		}
		interpolate(i_Track, a, b, i_Time, o_pValue);
	}

	void Decide(const tinygltf::Model& i_Model, std::vector<gltfSceneObject>& io_Objects, float i_FramesPerSecond)
	{
		warn_unsupported_pointers(i_Model);

		// who carries each node's transform
		for (gltfSceneObject& object : io_Objects)
		{
			const bool bUnderOwnGroup = object.m_Parent >= 0 && object.m_Parent < (int)io_Objects.size()
										&& io_Objects[object.m_Parent].m_Node == object.m_Node;
			object.m_bNodeTransform = object.m_Kind != gltfSceneObject::e_SkinnedMesh && !bUnderOwnGroup;
		}

		// every animation read once (ReadScene, and so Decide, runs for every template and clip loaded)
		const std::vector<gltfClip> clips = read_clips(i_Model, i_FramesPerSecond);
		const std::vector<float> lengths = animation_lengths(clips, i_FramesPerSecond);
		const std::vector<std::string> identities = gltfAnimRead::ClipIdentities(i_Model);
		for (gltfSceneObject& object : io_Objects)
		{
			object.m_Tracks.clear();
			object.m_Clips.clear();
			object.m_bTransformClip = false;
			const std::vector<gltfScheduledAnim> scheduled = schedule(i_Model, object, lengths);
			if (scheduled.empty())
				continue;

			object.m_Tracks = object_tracks(i_Model, object, scheduled, clips, i_FramesPerSecond);

			// rigid meshes: keys, or a transform clip
			bool bScaleUniform = true, bTooMany = false, bScaled = false;
			for (const gltfTrack& track : object.m_Tracks)
			{
				if (!is_transform(track))
					continue;
				if (track.m_Property == gltfTrack::e_Scale)
				{
					bScaled = true;
					bScaleUniform &= uniform_scales(track);
				}
				bTooMany |= (int)track.m_Times.size() > c_MaxKeys;
			}
			if (object.m_Kind == gltfSceneObject::e_Mesh)
			{
				if (!bScaleUniform || bTooMany || (bScaled && !object.m_bUniformScale))
				{
					object.m_bTransformClip = true;
					object.m_Tracks.erase(std::remove_if(object.m_Tracks.begin(), object.m_Tracks.end(), is_transform),
										  object.m_Tracks.end());
				}
			}
			else if (object.m_Kind == gltfSceneObject::e_Group)
			{
				if (!bScaleUniform)
					DBG_WARNING("glTF node " << object.m_Identity << " has a non-uniform scale animation; its group's keys use the mean");
				if (bTooMany)
					DBG_WARNING("glTF node " << object.m_Identity << " has more than " << c_MaxKeys << " keys in a track; all are kept");
			}

			// clips, where the schedule puts them
			for (int animation : gltfAnimRead::AnimationsFor(i_Model, object))
			{
				gltfClipRef clip;
				clip.m_Identity = identities[animation];
				clip.m_Name = gltfAnimRead::ClipName(i_Model, animation);
				for (const gltfScheduledAnim& anim : scheduled)
				{
					if (anim.m_Animation == animation)
						clip.m_Begin = anim.m_Begin;
				}
				object.m_Clips.push_back(clip);
			}
		}
	}
}
