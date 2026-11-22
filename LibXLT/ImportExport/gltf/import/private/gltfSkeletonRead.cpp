/****************************************************************************\
**  gltfSkeletonRead.cpp
**
**      see gltfSkeletonRead.hpp
**
**	The skeleton root R is the skin's "skeleton" node when it is an
**	ancestor (or itself one) of every joint; else the joints' lowest
**	common ancestor in the default scene. R's subtree is taken whole,
**	depth first with children in glTF order, so non-joint nodes between
**	joints keep their transforms. Bones are the joint nodes in that
**	order: the order smdlSkeletonObject counts joints in, since the
**	template's scene graph is built from m_Nodes in order.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GLTF_SKELETONREAD_HPP
#include "ImportExport/gltf/import/private/gltfSkeletonRead.hpp"
#endif
#ifndef GLTF_NODETRANSFORM_HPP
#include "ImportExport/gltf/import/private/gltfNodeTransform.hpp"
#endif
#ifndef GLTF_MESHIMPORT_HPP
#include "ImportExport/gltf/import/private/gltfMeshImport.hpp"
#endif

#include "ImportExport/gltf/import/private/gltfAccessorUtil.hpp"

#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif

#include <map>
#include <set>

namespace
{
	bool is_node(const tinygltf::Model& i_Model, int i_Node)
	{
		return i_Node >= 0 && i_Node < (int)i_Model.nodes.size();
	}

	// i_Ancestor is i_Node or above it
	bool is_ancestor(const std::vector<int>& i_Parents, int i_Ancestor, int i_Node)
	{
		for (int n = i_Node; n >= 0; n = i_Parents[n])
		{
			if (n == i_Ancestor)
				return true;
		}
		return false;
	}

	bool is_ancestor_of_all(const std::vector<int>& i_Parents, int i_Ancestor, const std::vector<int>& i_Joints)
	{
		for (int joint : i_Joints)
		{
			if (!is_ancestor(i_Parents, i_Ancestor, joint))
				return false;
		}
		return true;
	}

	void add_subtree(const tinygltf::Model& i_Model, const std::vector<int>& i_Parents, int i_Node, int i_Parent,
					 const std::map<int, int>& i_JointIndex, std::vector<gltfSkeletonNode>& io_Nodes)
	{
		gltfSkeletonNode n;
		n.m_Node = i_Node;
		n.m_Parent = i_Parent;
		const gltfNodeTransform::Transform t = gltfNodeTransform::NodeTransform(i_Model.nodes[i_Node]);
		n.m_Translation = t.m_Translation;
		n.m_Rotation = t.m_Rotation;
		n.m_Scale = t.m_Scale;
		n.m_bJoint = i_JointIndex.count(i_Node) != 0;
		io_Nodes.push_back(n);

		const int index = (int)io_Nodes.size() - 1;
		for (int child : i_Model.nodes[i_Node].children)
		{
			// SceneParents visited each node once, so this walk can't loop
			if (is_node(i_Model, child) && i_Parents[child] == i_Node)
				add_subtree(i_Model, i_Parents, child, index, i_JointIndex, io_Nodes);
		}
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
namespace gltfSkeletonRead
{
	std::vector<int> SceneParents(const tinygltf::Model& i_Model)
	{
		std::vector<int> parents(i_Model.nodes.size(), c_NotInScene);
		if (i_Model.scenes.empty())
			return parents;
		int scene = i_Model.defaultScene;
		if (scene < 0 || scene >= (int)i_Model.scenes.size())
			scene = 0;

		std::vector<int> stack;
		for (int root : i_Model.scenes[scene].nodes)
		{
			if (is_node(i_Model, root) && parents[root] == c_NotInScene)
			{
				parents[root] = -1;
				stack.push_back(root);
			}
		}
		while (!stack.empty())
		{
			const int node = stack.back();
			stack.pop_back();
			for (int child : i_Model.nodes[node].children)
			{
				if (is_node(i_Model, child) && parents[child] == c_NotInScene)
				{
					parents[child] = node;
					stack.push_back(child);
				}
			}
		}
		return parents;
	}

	bool Read(const tinygltf::Model& i_Model, int i_Skin, gltfSkeleton& o_Skeleton)
	{
		o_Skeleton = gltfSkeleton();
		if (i_Skin < 0 || i_Skin >= (int)i_Model.skins.size())
			return false;
		const tinygltf::Skin& skin = i_Model.skins[i_Skin];
		if (skin.joints.empty())
		{
			DBG_WARNING("glTF skin " << i_Skin << " (" << skin.name << ") has no joints");
			return false;
		}

		const std::vector<int> parents = SceneParents(i_Model);
		for (int joint : skin.joints)
		{
			if (!is_node(i_Model, joint) || parents[joint] == c_NotInScene)
			{
				DBG_WARNING("glTF skin " << i_Skin << " (" << skin.name << ") has a joint outside the scene");
				return false;
			}
		}

		// the root: the skin's skeleton when it is above every joint, else the lowest common ancestor
		int root = -1;
		if (is_node(i_Model, skin.skeleton) && parents[skin.skeleton] != c_NotInScene
			&& is_ancestor_of_all(parents, skin.skeleton, skin.joints))
		{
			root = skin.skeleton;
		}
		else
		{
			if (skin.skeleton >= 0)
				DBG_WARNING("glTF skin " << i_Skin << " (" << skin.name << "): its skeleton node isn't above all its joints; the joints' common ancestor is used");
			for (int n = skin.joints[0]; n >= 0 && root < 0; n = parents[n])
			{
				if (is_ancestor_of_all(parents, n, skin.joints))
					root = n;
			}
		}
		if (root < 0)
		{
			DBG_WARNING("glTF skin " << i_Skin << " (" << skin.name << ")'s joints share no ancestor");
			return false;
		}

		std::vector<float> ibms;
		if (skin.inverseBindMatrices >= 0)
		{
			if (!gltfAccessorUtil::ReadFloats(i_Model, skin.inverseBindMatrices, 16, ibms)
				|| ibms.size() < skin.joints.size() * 16)
			{
				DBG_WARNING("glTF skin " << i_Skin << " (" << skin.name << ")'s inverse bind matrices can't be read");
				return false;
			}
		}

		std::map<int, int> jointIndex;		// node -> its first index in skin.joints
		for (size_t j = 0; j < skin.joints.size(); j++)
			jointIndex.insert(std::make_pair(skin.joints[j], (int)j));

		o_Skeleton.m_Root = root;
		o_Skeleton.m_Base = parents[root];
		add_subtree(i_Model, parents, root, -1, jointIndex, o_Skeleton.m_Nodes);

		std::map<int, int> boneOfNode;
		for (gltfSkeletonNode& node : o_Skeleton.m_Nodes)
		{
			if (!node.m_bJoint)
				continue;
			const int bone = (int)boneOfNode.size();
			boneOfNode[node.m_Node] = bone;
			if (!ibms.empty())
			{
				// column-major glTF has our row-vector layout; translation to centimetres
				const float* pMatrix = &ibms[jointIndex[node.m_Node] * 16];
				for (int i = 0; i < 16; i++)
					node.m_InvBindPose.Ptr()[i] = pMatrix[i];
				for (int i = 12; i < 15; i++)
					node.m_InvBindPose.Ptr()[i] *= gltfMeshImport::c_UnitScale;
			}
		}

		for (int joint : skin.joints)
			o_Skeleton.m_BoneOfJoint.push_back(boneOfNode[joint]);
		return true;
	}
}
