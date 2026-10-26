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
**	- Channels on a rigid node's own transform, and KHR_animation_pointer
**	  channels (no target node), are decided in Plan 6; they are skipped.
**	- Identity: the animation's name when no other animation has it, else
**	  "#<animation index>"; display name: the name, else "Animation <index>".
**
**	Keys
**
**	- Times are frames at the given rate, from the clip's earliest input
**	  time, which becomes frame 0. The clip's length is its last time.
**	- LINEAR keys are taken as given. STEP: each key, plus a key
**	  c_StepEpsilon frames before the next one holding its value.
**	  CUBICSPLINE: the glTF spec's Hermite, with tangents scaled by the
**	  key interval in seconds, sampled at the keys and every whole frame
**	  between them; rotations normalized. MachStudio's channels are
**	  linear, so these become linear keys.
**	- Translations (values and tangents) are metres to centimetres;
**	  rotations, scales and weights are unchanged.
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

	int component_count(const tinygltf::Model& i_Model, const tinygltf::AnimationChannel& i_Channel,
						gltfAnimChannel::Path i_Path, size_t i_NumKeys, size_t i_NumValues, bool i_bCubic)
	{
		switch (i_Path)
		{
		case gltfAnimChannel::e_Rotation:
			return 4;
		case gltfAnimChannel::e_Weights:
		{
			const tinygltf::Node& node = i_Model.nodes[i_Channel.target_node];
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
					  const tinygltf::AnimationChannel& i_Channel, float i_Start, float i_Fps, gltfAnimChannel& o_Channel)
	{
		if (!parse_path(i_Channel.target_path, o_Channel.m_Path))
			return false;
		if (i_Channel.sampler < 0 || i_Channel.sampler >= (int)i_Animation.samplers.size())
			return false;
		const tinygltf::AnimationSampler& sampler = i_Animation.samplers[i_Channel.sampler];
		const bool bCubic = sampler.interpolation == "CUBICSPLINE";
		const bool bStep = sampler.interpolation == "STEP";

		std::vector<float> times, values;
		if (!gltfAccessorUtil::ReadFloats(i_Model, sampler.input, 1, times) || times.empty())
			return false;
		const int outputComponents = (o_Channel.m_Path == gltfAnimChannel::e_Weights) ? 1
								   : (o_Channel.m_Path == gltfAnimChannel::e_Rotation) ? 4 : 3;
		if (!gltfAccessorUtil::ReadFloats(i_Model, sampler.output, outputComponents, values))
			return false;

		const size_t numKeys = times.size();
		const int n = component_count(i_Model, i_Channel, o_Channel.m_Path, numKeys, values.size(), bCubic);
		const size_t perKey = (size_t)n * (bCubic ? 3 : 1);
		if (n <= 0 || values.size() != numKeys * perKey)
			return false;
		o_Channel.m_Node = i_Channel.target_node;
		o_Channel.m_Components = n;

		if (o_Channel.m_Path == gltfAnimChannel::e_Translation)
		{
			for (float& v : values)
				v *= gltfMeshImport::c_UnitScale;
		}

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
				if (f1 - gltfAnimRead::c_StepEpsilon > f0)
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
					if (o_Channel.m_Path == gltfAnimChannel::e_Rotation)
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

	bool ReadClip(const tinygltf::Model& i_Model, const std::string& i_Identity, float i_FramesPerSecond, gltfClip& o_Clip)
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
			if (!is_node(i_Model, channel.target_node) || channel.sampler < 0 || channel.sampler >= (int)animation.samplers.size())
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
			if (!is_node(i_Model, channel.target_node))
				continue;		// KHR_animation_pointer: Plan 6
			gltfAnimChannel read;
			if (!read_channel(i_Model, animation, channel, start, i_FramesPerSecond, read))
			{
				DBG_WARNING("glTF animation " << o_Clip.m_Name << ": a " << channel.target_path << " channel on node "
							<< channel.target_node << " can't be read; skipped");
				continue;
			}
			o_Clip.m_Length = std::max(o_Clip.m_Length, read.m_Times.back());
			o_Clip.m_Channels.push_back(read);
		}
		return true;
	}

	bool AnimatesWeights(const tinygltf::Model& i_Model, int i_Node)
	{
		for (const tinygltf::Animation& animation : i_Model.animations)
		{
			for (const tinygltf::AnimationChannel& channel : animation.channels)
			{
				if (channel.target_node >= 0 && channel.target_node == i_Node && channel.target_path == "weights")
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

		for (size_t a = 0; a < i_Model.animations.size(); a++)
		{
			bool bTouches = false;
			for (const tinygltf::AnimationChannel& channel : i_Model.animations[a].channels)
			{
				gltfAnimChannel::Path path;
				if (channel.target_node < 0 || !parse_path(channel.target_path, path))
					continue;
				if (path == gltfAnimChannel::e_Weights)
					bTouches |= channel.target_node == i_Object.m_Node;
				else
					bTouches |= skeletonNodes.count(channel.target_node) != 0;
			}
			if (bTouches)
				animations.push_back((int)a);
		}
		return animations;
	}
}
