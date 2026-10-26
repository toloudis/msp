/****************************************************************************\
**  gltfAnimKeys.cpp
**
**      see gltfAnimKeys.hpp
**
**	The keys
**
**	- One key root, named "" (the character's hierarchy root, which
**	  smdlHierarchyObject animates first). Its head is the template's base
**	  node and has no channels; for a skinned object it has one child
**	  mirroring gltfSkeleton::m_Nodes in order, so child i of a key node
**	  is child i of its scene node, as the template was built.
**	- MachStudio replaces a node's whole transform when it has any
**	  channel, and reads a missing translation as 0: so a node with any
**	  channel gets all three, the missing ones one key at frame 0 holding
**	  the node's rest value. Nodes the clip doesn't touch have no channels
**	  and keep their template transform.
**	- Weights become one morph key per target, named
**	  gltfSkinRead::MorphTargetKey(i) as the template's morph targets are.
**	- Times are the clip's frames, played at i_FramesPerSecond from 0; the
**	  keys aren't additive and ignore joint orientation (glTF has none).
**	- MachStudio joints don't pass their animated scale on to child
**	  joints (smdlHierarchyObject divides it out); glTF does. A clip whose
**	  scale on a joint with child joints isn't 1 gets one warning naming
**	  the joint; nothing else is done.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GLTF_ANIMKEYS_HPP
#include "ImportExport/gltf/import/private/gltfAnimKeys.hpp"
#endif
#ifndef GLTF_SKELETONREAD_HPP
#include "ImportExport/gltf/import/private/gltfSkeletonRead.hpp"
#endif
#ifndef GLTF_SKINREAD_HPP
#include "ImportExport/gltf/import/private/gltfSkinRead.hpp"
#endif

#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif
#ifndef EMDL_CHARACTERANIMKEYS_HPP
#include "Graphics/emdl/emdlCharacterAnimKeys.hpp"
#endif

#include <cmath>
#include <map>
#include <memory>

namespace
{
	typedef smdlTree<smdlGeoAnimKeys> KeyTree;

	// the clip's channels on one node, by path (NULL: none)
	struct NodeChannels
	{
		const gltfAnimChannel* m_pPath[4] = { NULL, NULL, NULL, NULL };
	};

	anKeyData<maVector3d>* vector_keys(const gltfAnimChannel* i_pChannel, const maVector3d& i_Rest)
	{
		anKeyData<maVector3d>* keys = new anKeyData<maVector3d>();
		if (!i_pChannel)
		{
			keys->AddKey(0, i_Rest);
			return keys;
		}
		for (size_t k = 0; k < i_pChannel->m_Times.size(); k++)
		{
			const float* v = &i_pChannel->m_Values[k * 3];
			keys->AddKey(i_pChannel->m_Times[k], maVector3d(v[0], v[1], v[2]));
		}
		return keys;
	}

	anKeyData<maRotation>* rotation_keys(const gltfAnimChannel* i_pChannel, const maRotation& i_Rest)
	{
		anKeyData<maRotation>* keys = new anKeyData<maRotation>();
		if (!i_pChannel)
		{
			keys->AddKey(0, i_Rest);
			return keys;
		}
		for (size_t k = 0; k < i_pChannel->m_Times.size(); k++)
		{
			const float* q = &i_pChannel->m_Values[k * 4];
			keys->AddKey(i_pChannel->m_Times[k], maRotation(q[0], q[1], q[2], q[3]));
		}
		return keys;
	}

	bool scale_is_one(const anKeyData<maVector3d>& i_Keys)
	{
		const float c_Tolerance = 1e-4f;
		for (int k = 0; k < i_Keys.GetNumKeys(); k++)
		{
			float time;
			maVector3d scale;
			i_Keys.GetKeyData(k, time, scale);
			if (fabsf(scale.m_X - 1) > c_Tolerance || fabsf(scale.m_Y - 1) > c_Tolerance || fabsf(scale.m_Z - 1) > c_Tolerance)
				return false;
		}
		return true;
	}

	//--------------------------------------------------------------------
	// the keys of one skeleton node: all three channels when the clip
	// has any of them, else none
	//--------------------------------------------------------------------
	smdlGeoAnimKeys node_keys(const tinygltf::Model& i_Model, const gltfSkeleton& i_Skeleton, int i_Index,
							  const std::map<int, NodeChannels>& i_Channels, const std::string& i_ClipName,
							  emdlCharacterAnimKeys& io_Keys)
	{
		smdlGeoAnimKeys keys;
		const gltfSkeletonNode& node = i_Skeleton.m_Nodes[i_Index];
		std::map<int, NodeChannels>::const_iterator it = i_Channels.find(node.m_Node);
		if (it == i_Channels.end())
			return keys;
		const NodeChannels& channels = it->second;

		anKeyData<maVector3d>* translate = vector_keys(channels.m_pPath[gltfAnimChannel::e_Translation], node.m_Translation);
		anKeyData<maRotation>* rotate = rotation_keys(channels.m_pPath[gltfAnimChannel::e_Rotation], node.m_Rotation);
		anKeyData<maVector3d>* scale = vector_keys(channels.m_pPath[gltfAnimChannel::e_Scale], node.m_Scale);
		io_Keys.TranslateChannels().push_back(translate);
		io_Keys.RotateChannels().push_back(rotate);
		io_Keys.ScaleChannels().push_back(scale);
		keys.SetTranslate(translate);
		keys.SetRotate(rotate);
		keys.SetScale(scale);

		if (node.m_bJoint && !scale_is_one(*scale))
		{
			bool bChildJoint = false;
			for (const gltfSkeletonNode& child : i_Skeleton.m_Nodes)
				bChildJoint |= (child.m_Parent == i_Index) && child.m_bJoint;
			if (bChildJoint)
				DBG_WARNING("glTF animation " << i_ClipName << " scales joint " << i_Model.nodes[node.m_Node].name
							<< ", whose child joints don't follow its scale in MachStudio");
		}
		return keys;
	}

	//--------------------------------------------------------------------
	// the children of skeleton node i_Parent (-1: the root) under io_It
	//--------------------------------------------------------------------
	void add_children(const tinygltf::Model& i_Model, const gltfSkeleton& i_Skeleton, int i_Parent,
					  const std::map<int, NodeChannels>& i_Channels, const std::string& i_ClipName,
					  KeyTree::iterator& io_It, emdlCharacterAnimKeys& io_Keys)
	{
		int child = 0;
		for (int i = 0; i < (int)i_Skeleton.m_Nodes.size(); i++)
		{
			if (i_Skeleton.m_Nodes[i].m_Parent != i_Parent)
				continue;
			io_It.AddChild(node_keys(i_Model, i_Skeleton, i, i_Channels, i_ClipName, io_Keys));
			io_It.MoveToChild(child++);
			add_children(i_Model, i_Skeleton, i, i_Channels, i_ClipName, io_It, io_Keys);
			io_It.MoveToParent();
		}
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
namespace gltfAnimKeys
{
	emdlCharacterAnimKeys* Build(const tinygltf::Model& i_Model, const gltfSceneObject& i_Object,
								 const gltfClip& i_Clip, float i_FramesPerSecond)
	{
		std::unique_ptr<emdlCharacterAnimKeys> keys(new emdlCharacterAnimKeys());
		keys->SetFramesPerSecond(i_FramesPerSecond);
		keys->SetStartTime(0);
		keys->SetAdditiveAnimation(false);
		keys->SetIgnoreJointOrientation(true);
		keys->SetAttachToRootJoint(false);

		// the clip's channels by node
		std::map<int, NodeChannels> channels;
		for (const gltfAnimChannel& channel : i_Clip.m_Channels)
			channels[channel.m_Node].m_pPath[channel.m_Path] = &channel;

		bool bTouched = false;
		shared_ptr<KeyTree> tree(new KeyTree());
		tree->SetHead(smdlGeoAnimKeys());

		gltfSkeleton skeleton;
		if (i_Object.m_Kind == gltfSceneObject::e_SkinnedMesh && gltfSkeletonRead::Read(i_Model, i_Object.m_Skin, skeleton))
		{
			for (const gltfSkeletonNode& node : skeleton.m_Nodes)
			{
				std::map<int, NodeChannels>::const_iterator it = channels.find(node.m_Node);
				if (it != channels.end() && (it->second.m_pPath[gltfAnimChannel::e_Translation]
											 || it->second.m_pPath[gltfAnimChannel::e_Rotation]
											 || it->second.m_pPath[gltfAnimChannel::e_Scale]))
					bTouched = true;
			}
			KeyTree::iterator head = tree->GetIterator();
			add_children(i_Model, skeleton, -1, channels, i_Clip.m_Name, head, *keys);
		}

		// morph weights on the object's own node
		std::map<int, NodeChannels>::const_iterator own = channels.find(i_Object.m_Node);
		const gltfAnimChannel* pWeights = (own != channels.end()) ? own->second.m_pPath[gltfAnimChannel::e_Weights] : NULL;
		if (pWeights && (i_Object.m_Kind == gltfSceneObject::e_Mesh || i_Object.m_Kind == gltfSceneObject::e_SkinnedMesh))
		{
			bTouched = true;
			for (int t = 0; t < pWeights->m_Components; t++)
			{
				anKeyData<float>* blend = new anKeyData<float>();
				for (size_t k = 0; k < pWeights->m_Times.size(); k++)
					blend->AddKey(pWeights->m_Times[k], pWeights->m_Values[k * pWeights->m_Components + t]);
				keys->MorphChannels().push_back(blend);
				keys->MorphKeys()[gltfSkinRead::MorphTargetKey(t)].SetBlend(blend);
			}
		}

		if (!bTouched)
			return NULL;
		keys->KeyRoots()[""] = tree;
		return keys.release();
	}
}
