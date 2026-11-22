/****************************************************************************\
**  gltfAnimRead.cpp
**
**      see gltfAnimRead.hpp
**
**	The rule: which animations become clips, and on what
**
**	- Every glTF animation that moves a skinned or morph-animated mesh
**	  becomes one Anim-Full clip on that mesh's character: a skinned mesh
**	  takes the animations with a translation, rotation or scale channel
**	  on a node of its skeleton (gltfSkeletonRead) or a weights channel on
**	  its own node; a plain mesh takes the animations with a weights
**	  channel on its node. A clip carries only those channels.
**	- Channels on a rigid node's own transform become keys, or clips on a
**	  transform-clip mesh, by gltfAnimTracks' rule (gltfAnimTracks.cpp).
**
**	Targets (ResolveTarget)
**
**	- A channel targets a node's translation, rotation, scale or weights,
**	  or, through KHR_animation_pointer (target path "pointer", the
**	  pointer in the target's extension), one of:
**	  /nodes/N/{translation,rotation,scale,weights}: the same as a node
**	  channel, so it is read wherever node channels are;
**	  /extensions/KHR_lights_punctual/lights/N/{color,intensity,range,
**	  spot/innerConeAngle,spot/outerConeAngle};
**	  /cameras/N/{perspective/yfov,perspective/znear,perspective/zfar,
**	  orthographic/xmag,orthographic/ymag,orthographic/znear,
**	  orthographic/zfar};
**	  /materials/N/{pbrMetallicRoughness/baseColorFactor,
**	  pbrMetallicRoughness/metallicFactor,pbrMetallicRoughness/
**	  roughnessFactor,emissiveFactor,alphaCutoff,normalTexture/scale,
**	  occlusionTexture/strength,extensions/
**	  KHR_materials_emissive_strength/emissiveStrength}.
**	  Anything else, or an index out of range, is not read
**	  (gltfAnimTracks warns once per pointer).
**	- Light, camera and material channels go in gltfClip::m_Pointers with
**	  the file's values; their components are the output accessor's.
**	- Identity: the animation's name when no other animation has it, else
**	  "#<animation index>"; display name: the name, else "Animation <index>".
**
**	Keys
**
**	- Times are frames at the given rate, from the clip's earliest input
**	  time over every channel read, which becomes frame 0. The clip's
**	  length is its last time.
**	- LINEAR keys are taken as given. STEP: each key, plus a key
**	  c_StepEpsilon frames before the next one holding its value; or,
**	  for keys (i_bStepAsHolds false), each key as given, marked m_bStep.
**	  CUBICSPLINE: the glTF spec's Hermite, with tangents scaled by the
**	  key interval in seconds, sampled at the keys and every whole frame
**	  between them; rotations normalized. MachStudio's channels are
**	  linear, so these become linear keys.
**	- Node translations (values and tangents) are metres to centimetres;
**	  rotations, scales, weights and pointer values are unchanged.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GLTF_ANIMREAD_HPP
#include "ImportExport/gltf/import/private/gltfAnimRead.hpp"
#endif
#ifndef GLTF_SKELETONREAD_HPP
#include "ImportExport/gltf/import/private/gltfSkeletonRead.hpp"
#endif
#ifndef GLTF_SKINREAD_HPP
#include "ImportExport/gltf/import/private/gltfSkinRead.hpp"
#endif
#ifndef GLTF_MESHIMPORT_HPP
#include "ImportExport/gltf/import/private/gltfMeshImport.hpp"
#endif

#include "ImportExport/gltf/import/private/gltfAccessorUtil.hpp"

#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif

#include <algorithm>
#include <cmath>
#include <set>
#include <sstream>

namespace
{
	bool parse_path(const std::string& i_Path, gltfAnimChannel::Path& o_Path)
	{
		if (i_Path == "translation")
			o_Path = gltfAnimChannel::e_Translation;
		else if (i_Path == "rotation")
			o_Path = gltfAnimChannel::e_Rotation;
		else if (i_Path == "scale")
			o_Path = gltfAnimChannel::e_Scale;
		else if (i_Path == "weights")
			o_Path = gltfAnimChannel::e_Weights;
		else
			return false;
		return true;
	}

	bool is_node(const tinygltf::Model& i_Model, int i_Node)
	{
		return i_Node >= 0 && i_Node < (int)i_Model.nodes.size();
	}

	// the properties a pointer may name after its object's index
	const char* const c_LightProperties[] = { "color", "intensity", "range", "spot/innerConeAngle", "spot/outerConeAngle" };
	const char* const c_CameraProperties[] = { "perspective/yfov", "perspective/znear", "perspective/zfar",
											   "orthographic/xmag", "orthographic/ymag", "orthographic/znear",
											   "orthographic/zfar" };
	const char* const c_MaterialProperties[] = { "pbrMetallicRoughness/baseColorFactor",
												 "pbrMetallicRoughness/metallicFactor",
												 "pbrMetallicRoughness/roughnessFactor", "emissiveFactor", "alphaCutoff",
												 "normalTexture/scale", "occlusionTexture/strength",
												 "extensions/KHR_materials_emissive_strength/emissiveStrength" };

	template <size_t N>
	bool is_one_of(const std::string& i_Property, const char* const (&i_List)[N])
	{
		for (size_t i = 0; i < N; i++)
		{
			if (i_Property == i_List[i])
				return true;
		}
		return false;
	}

	// i_Pointer is i_Prefix, an index in [0, i_Count), '/' and more:
	// the index and what follows the '/'
	bool split_pointer(const std::string& i_Pointer, const std::string& i_Prefix, size_t i_Count,
					   int& o_Index, std::string& o_Property)
	{
		if (i_Pointer.compare(0, i_Prefix.size(), i_Prefix) != 0)
			return false;
		const size_t first = i_Prefix.size();
		const size_t slash = i_Pointer.find('/', first);
		if (slash == std::string::npos || slash == first)
			return false;
		size_t index = 0;
		for (size_t i = first; i < slash; i++)
		{
			const char c = i_Pointer[i];
			if (c < '0' || c > '9' || index > i_Count)
				return false;
			index = index * 10 + (size_t)(c - '0');
		}
		if (index >= i_Count)
			return false;
		o_Index = (int)index;
		o_Property = i_Pointer.substr(slash + 1);
		return true;
	}

	int component_count(const tinygltf::Model& i_Model, int i_Node,
						gltfAnimChannel::Path i_Path, size_t i_NumKeys, size_t i_NumValues, bool i_bCubic)
	{
		switch (i_Path)
		{
		case gltfAnimChannel::e_Rotation:
			return 4;
		case gltfAnimChannel::e_Weights:
		{
			const tinygltf::Node& node = i_Model.nodes[i_Node];
			if (node.mesh >= 0 && node.mesh < (int)i_Model.meshes.size())
				return (int)gltfSkinRead::NumMorphTargets(i_Model.meshes[node.mesh]);
			const size_t perKey = i_bCubic ? 3 : 1;
			return i_NumKeys ? (int)(i_NumValues / (i_NumKeys * perKey)) : 0;
		}
		default:
			return 3;
		}
	}

	void normalize(float* io_pQuat)
	{
		const float length = sqrtf(io_pQuat[0] * io_pQuat[0] + io_pQuat[1] * io_pQuat[1]
								   + io_pQuat[2] * io_pQuat[2] + io_pQuat[3] * io_pQuat[3]);
		if (length > 0)
		{
			for (int i = 0; i < 4; i++)
				io_pQuat[i] /= length;
		}
	}

	// one channel's keys; i_Start in seconds, the clip's start
	bool read_channel(const tinygltf::Model& i_Model, const tinygltf::Animation& i_Animation,
					  const tinygltf::AnimationChannel& i_Channel, const gltfAnimTarget& i_Target, float i_Start,
					  float i_Fps, bool i_bStepAsHolds, gltfAnimChannel& o_Channel)
	{
		if (i_Target.m_Kind == gltfAnimTarget::e_None)
			return false;
		if (i_Channel.sampler < 0 || i_Channel.sampler >= (int)i_Animation.samplers.size())
			return false;
		const tinygltf::AnimationSampler& sampler = i_Animation.samplers[i_Channel.sampler];
		const bool bCubic = sampler.interpolation == "CUBICSPLINE";
		const bool bStep = sampler.interpolation == "STEP";
		const bool bNode = i_Target.m_Kind == gltfAnimTarget::e_Node;
		if (bNode)
			o_Channel.m_Path = i_Target.m_Path;

		std::vector<float> times, values;
		if (!gltfAccessorUtil::ReadFloats(i_Model, sampler.input, 1, times) || times.empty())
			return false;
		int outputComponents = 0;
		if (bNode)
		{
			outputComponents = (o_Channel.m_Path == gltfAnimChannel::e_Weights) ? 1
							 : (o_Channel.m_Path == gltfAnimChannel::e_Rotation) ? 4 : 3;
		}
		else
		{
			if (sampler.output < 0 || sampler.output >= (int)i_Model.accessors.size())
				return false;
			outputComponents = tinygltf::GetNumComponentsInType((uint32_t)i_Model.accessors[sampler.output].type);
			if (outputComponents <= 0)
				return false;
		}
		if (!gltfAccessorUtil::ReadFloats(i_Model, sampler.output, outputComponents, values))
			return false;

		const size_t numKeys = times.size();
		const int n = bNode ? component_count(i_Model, i_Target.m_Index, o_Channel.m_Path, numKeys, values.size(), bCubic)
							: outputComponents;
		const size_t perKey = (size_t)n * (bCubic ? 3 : 1);
		if (n <= 0 || values.size() != numKeys * perKey)
			return false;
		o_Channel.m_Node = bNode ? i_Target.m_Index : -1;
		o_Channel.m_Components = n;
		o_Channel.m_bStep = bStep && !i_bStepAsHolds;

		if (bNode && o_Channel.m_Path == gltfAnimChannel::e_Translation)
		{
			for (float& v : values)
				v *= gltfMeshImport::c_UnitScale;
		}
		const bool bRotation = bNode && o_Channel.m_Path == gltfAnimChannel::e_Rotation;

		// the value (cubic: the middle of in-tangent, value, out-tangent) of key i
		const auto value = [&](size_t i_Key) { return &values[i_Key * perKey + (bCubic ? n : 0)]; };
		const auto frame = [&](float i_Seconds) { return (i_Seconds - i_Start) * i_Fps; };
		const auto add = [&](float i_Frame, const float* i_pValue)
		{
			o_Channel.m_Times.push_back(i_Frame);
			o_Channel.m_Values.insert(o_Channel.m_Values.end(), i_pValue, i_pValue + n);
		};

		for (size_t k = 0; k < numKeys; k++)
		{
			const float f0 = frame(times[k]);
			add(f0, value(k));
			if (k + 1 == numKeys)
				break;
			const float f1 = frame(times[k + 1]);
			if (bStep)
			{
				if (i_bStepAsHolds && f1 - gltfAnimRead::c_StepEpsilon > f0)
					add(f1 - gltfAnimRead::c_StepEpsilon, value(k));
			}
			else if (bCubic)
			{
				const float delta = times[k + 1] - times[k];	// seconds
				if (delta <= 0)
					continue;
				const float* v0 = value(k);
				const float* b0 = v0 + n;					// key k's out-tangent
				const float* v1 = value(k + 1);
				const float* a1 = v1 - n;					// key k + 1's in-tangent
				std::vector<float> sample(n);
				for (float f = floorf(f0) + 1; f < f1; f += 1)
				{
					const float t = (f - f0) / (f1 - f0);
					const float t2 = t * t, t3 = t2 * t;
					for (int c = 0; c < n; c++)
					{
						sample[c] = (2 * t3 - 3 * t2 + 1) * v0[c] + (t3 - 2 * t2 + t) * delta * b0[c]
								  + (-2 * t3 + 3 * t2) * v1[c] + (t3 - t2) * delta * a1[c];
					}
					if (bRotation)
						normalize(&sample[0]);
					add(f, &sample[0]);
				}
			}
		}
		return true;
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
namespace gltfAnimRead
{
	std::vector<std::string> ClipIdentities(const tinygltf::Model& i_Model)
	{
		std::vector<std::string> identities;
		for (size_t a = 0; a < i_Model.animations.size(); a++)
		{
			const std::string& name = i_Model.animations[a].name;
			bool bUnique = !name.empty();
			for (size_t b = 0; b < i_Model.animations.size() && bUnique; b++)
			{
				if (b != a && i_Model.animations[b].name == name)
					bUnique = false;
			}
			if (bUnique)
			{
				identities.push_back(name);
			}
			else
			{
				std::ostringstream identity;
				identity << "#" << a;
				identities.push_back(identity.str());
			}
		}
		return identities;
	}

	std::string ClipName(const tinygltf::Model& i_Model, int i_Animation)
	{
		if (i_Animation >= 0 && i_Animation < (int)i_Model.animations.size() && !i_Model.animations[i_Animation].name.empty())
			return i_Model.animations[i_Animation].name;
		std::ostringstream name;
		name << "Animation " << i_Animation;
		return name.str();
	}

	gltfAnimTarget ResolveTarget(const tinygltf::Model& i_Model, const tinygltf::AnimationChannel& i_Channel)
	{
		gltfAnimTarget target;
		if (i_Channel.target_path != "pointer")
		{
			if (is_node(i_Model, i_Channel.target_node) && parse_path(i_Channel.target_path, target.m_Path))
			{
				target.m_Kind = gltfAnimTarget::e_Node;
				target.m_Index = i_Channel.target_node;
			}
			return target;
		}

		const tinygltf::ExtensionMap::const_iterator ext = i_Channel.target_extensions.find("KHR_animation_pointer");
		if (ext == i_Channel.target_extensions.end() || !ext->second.IsObject() || !ext->second.Has("pointer")
			|| !ext->second.Get("pointer").IsString())
			return target;
		const std::string pointer = ext->second.Get("pointer").Get<std::string>();
		target.m_Property = pointer;

		int index = -1;
		std::string property;
		gltfAnimTarget::Kind kind = gltfAnimTarget::e_None;
		if (split_pointer(pointer, "/nodes/", i_Model.nodes.size(), index, property))
		{
			if (parse_path(property, target.m_Path))
			{
				target.m_Kind = gltfAnimTarget::e_Node;
				target.m_Index = index;
				target.m_Property.clear();
			}
			return target;
		}
		if (split_pointer(pointer, "/extensions/KHR_lights_punctual/lights/", i_Model.lights.size(), index, property))
			kind = is_one_of(property, c_LightProperties) ? gltfAnimTarget::e_Light : gltfAnimTarget::e_None;
		else if (split_pointer(pointer, "/cameras/", i_Model.cameras.size(), index, property))
			kind = is_one_of(property, c_CameraProperties) ? gltfAnimTarget::e_Camera : gltfAnimTarget::e_None;
		else if (split_pointer(pointer, "/materials/", i_Model.materials.size(), index, property))
			kind = is_one_of(property, c_MaterialProperties) ? gltfAnimTarget::e_Material : gltfAnimTarget::e_None;
		if (kind != gltfAnimTarget::e_None)
		{
			target.m_Kind = kind;
			target.m_Index = index;
			target.m_Property = property;
		}
		return target;
	}

	bool ReadClip(const tinygltf::Model& i_Model, const std::string& i_Identity, float i_FramesPerSecond, gltfClip& o_Clip,
				  bool i_bStepAsHolds)
	{
		o_Clip = gltfClip();
		const std::vector<std::string> identities = ClipIdentities(i_Model);
		const std::vector<std::string>::const_iterator it = std::find(identities.begin(), identities.end(), i_Identity);
		if (it == identities.end())
			return false;
		const int index = (int)(it - identities.begin());
		const tinygltf::Animation& animation = i_Model.animations[index];
		o_Clip.m_Identity = i_Identity;
		o_Clip.m_Name = ClipName(i_Model, index);

		// the clip starts at its earliest input time
		float start = 0;
		bool bFirst = true;
		for (const tinygltf::AnimationChannel& channel : animation.channels)
		{
			if (ResolveTarget(i_Model, channel).m_Kind == gltfAnimTarget::e_None
				|| channel.sampler < 0 || channel.sampler >= (int)animation.samplers.size())
				continue;
			std::vector<float> times;
			if (!gltfAccessorUtil::ReadFloats(i_Model, animation.samplers[channel.sampler].input, 1, times) || times.empty())
				continue;
			const float first = *std::min_element(times.begin(), times.end());
			if (bFirst || first < start)
				start = first;
			bFirst = false;
		}

		for (const tinygltf::AnimationChannel& channel : animation.channels)
		{
			const gltfAnimTarget target = ResolveTarget(i_Model, channel);
			if (target.m_Kind == gltfAnimTarget::e_None)
				continue;		// gltfAnimTracks warns
			gltfPointerChannel read;
			read.m_Target = target;
			if (!read_channel(i_Model, animation, channel, target, start, i_FramesPerSecond, i_bStepAsHolds, read))
			{
				if (target.m_Kind == gltfAnimTarget::e_Node)
					DBG_WARNING("glTF animation " << o_Clip.m_Name << ": a " << channel.target_path << " channel on node "
								<< target.m_Index << " can't be read; skipped");
				else
					DBG_WARNING("glTF animation " << o_Clip.m_Name << ": a pointer channel to " << target.m_Property
								<< " can't be read; skipped");
				continue;
			}
			o_Clip.m_Length = std::max(o_Clip.m_Length, read.m_Times.back());
			if (target.m_Kind == gltfAnimTarget::e_Node)
				o_Clip.m_Channels.push_back(read);
			else
				o_Clip.m_Pointers.push_back(read);
		}
		return true;
	}

	bool AnimatesWeights(const tinygltf::Model& i_Model, int i_Node)
	{
		for (const tinygltf::Animation& animation : i_Model.animations)
		{
			for (const tinygltf::AnimationChannel& channel : animation.channels)
			{
				const gltfAnimTarget target = ResolveTarget(i_Model, channel);
				if (target.m_Kind == gltfAnimTarget::e_Node && target.m_Index == i_Node
					&& target.m_Path == gltfAnimChannel::e_Weights)
					return true;
			}
		}
		return false;
	}

	std::vector<int> AnimationsFor(const tinygltf::Model& i_Model, const gltfSceneObject& i_Object)
	{
		std::vector<int> animations;
		std::set<int> skeletonNodes;
		if (i_Object.m_Kind == gltfSceneObject::e_SkinnedMesh)
		{
			gltfSkeleton skeleton;
			if (gltfSkeletonRead::Read(i_Model, i_Object.m_Skin, skeleton))
			{
				for (const gltfSkeletonNode& node : skeleton.m_Nodes)
					skeletonNodes.insert(node.m_Node);
			}
		}
		else if (i_Object.m_Kind != gltfSceneObject::e_Mesh)
		{
			return animations;
		}
		// a transform-clip mesh's clips carry its node's transform too
		const bool bOwnTransform = i_Object.m_Kind == gltfSceneObject::e_Mesh && i_Object.m_bTransformClip;

		for (size_t a = 0; a < i_Model.animations.size(); a++)
		{
			bool bTouches = false;
			for (const tinygltf::AnimationChannel& channel : i_Model.animations[a].channels)
			{
				const gltfAnimTarget target = ResolveTarget(i_Model, channel);
				if (target.m_Kind != gltfAnimTarget::e_Node)
					continue;
				if (target.m_Path == gltfAnimChannel::e_Weights)
					bTouches |= target.m_Index == i_Object.m_Node;
				else
					bTouches |= skeletonNodes.count(target.m_Index) != 0 || (bOwnTransform && target.m_Index == i_Object.m_Node);
			}
			if (bTouches)
				animations.push_back((int)a);
		}
		return animations;
	}
}
