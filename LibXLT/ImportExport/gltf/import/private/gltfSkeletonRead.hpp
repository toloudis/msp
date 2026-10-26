/****************************************************************************\
**  gltfSkeletonRead.hpp
**
**      gltfSkeletonRead reads the node tree a glTF skin needs: its root
**	(the skin's skeleton, else the joints' lowest common ancestor), the
**	root's parent, the root's subtree with local transforms in
**	centimetres, the bone order and the inverse bind matrices. It needs
**	no app or GPU, so it is tested on its own; the skinned template, the
**	scene reader and the clip readers use it.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GLTF_SKELETONREAD_HPP
#error gltfSkeletonRead.hpp multiply included
#endif
#define GLTF_SKELETONREAD_HPP

#ifndef MA_VECTOR3D_HPP
#include "Core/ma/maVector3d.hpp"
#endif
#ifndef MA_ROTATION_HPP
#include "Core/ma/maRotation.hpp"
#endif
#ifndef MA_MATRIX4X4_HPP
#include "Core/ma/maMatrix4x4.hpp"
#endif

#include "tiny_gltf.h"

#include <vector>

//----------------------------------------------------------------------------
//	One node of a skeleton
//----------------------------------------------------------------------------
struct gltfSkeletonNode
{
	int m_Node = -1;			// glTF node
	int m_Parent = -1;			// index in gltfSkeleton::m_Nodes, -1 for the root
	maVector3d m_Translation;	// local, centimetres
	maRotation m_Rotation;
	maVector3d m_Scale = maVector3d(1, 1, 1);
	bool m_bJoint = false;		// listed in the skin's joints
	maMatrix4x4 m_InvBindPose;	// joints: the skin's inverse bind matrix, centimetres; identity when absent
};

//----------------------------------------------------------------------------
//	The node tree a skin needs
//----------------------------------------------------------------------------
struct gltfSkeleton
{
	int m_Root = -1;			// glTF node R
	int m_Base = -1;			// R's parent B, -1 when R is a scene root
	std::vector<gltfSkeletonNode> m_Nodes;	// R's subtree, depth first, children in glTF order, R first
	std::vector<int> m_BoneOfJoint;	// skin.joints[i] -> its bone index (its position among the m_bJoint nodes)
};

namespace gltfSkeletonRead
{
	// false (with a warning) when the skin has no joints, its joints share no
	// ancestor in the default scene, or its inverse bind matrices can't be read
	bool Read(const tinygltf::Model& i_Model, int i_Skin, gltfSkeleton& o_Skeleton);

	const int c_NotInScene = -2;

	// the default scene's parent of each node, sized to the model's nodes:
	// -1 for a scene root, c_NotInScene for a node outside the scene
	std::vector<int> SceneParents(const tinygltf::Model& i_Model);
}
