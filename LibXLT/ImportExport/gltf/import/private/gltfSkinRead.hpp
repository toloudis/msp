/****************************************************************************\
**  gltfSkinRead.hpp
**
**      gltfSkinRead reads what a skinned or morphing glTF primitive adds
**	to its vertices: bone influences (every JOINTS_n/WEIGHTS_n set) and
**	morph target position deltas in centimetres; and a mesh's default
**	morph weights. It needs no app or GPU, so it is tested on its own.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GLTF_SKINREAD_HPP
#error gltfSkinRead.hpp multiply included
#endif
#define GLTF_SKINREAD_HPP

#ifndef MA_VECTOR3D_HPP
#include "Core/ma/maVector3d.hpp"
#endif

#include "tiny_gltf.h"

#include <cstdint>
#include <string>
#include <vector>

namespace gltfSkinRead
{
	struct gltfInfluence
	{
		uint32_t m_Bone;
		float m_Weight;
	};

	// One list per vertex from every JOINTS_n/WEIGHTS_n pair (n = 0, 1, ...
	// while both exist), joint indices mapped through i_BoneOfJoint. Zero
	// weights and joints out of range are dropped; a vertex whose weights
	// are off 1 by more than 1e-3 is renormalized (o_bRenormalized); a
	// vertex left with nothing gets { 0, 1 }. There is no cap on the
	// number of influences: MachStudio's skins take any number. False when
	// JOINTS_0/WEIGHTS_0 are missing or their counts don't match the positions.
	bool ReadInfluences(const tinygltf::Model& i_Model, const tinygltf::Primitive& i_Prim,
						const std::vector<int>& i_BoneOfJoint,
						std::vector<std::vector<gltfInfluence>>& o_Vertices, bool& o_bRenormalized);

	// One list per prim.targets entry: its POSITION deltas in centimetres,
	// zeros when the target has no POSITION. False on a count mismatch.
	bool ReadMorphDeltas(const tinygltf::Model& i_Model, const tinygltf::Primitive& i_Prim, size_t i_NumVertices,
						 std::vector<std::vector<maVector3d>>& o_Targets);

	// The largest number of morph targets of the mesh's primitives
	size_t NumMorphTargets(const tinygltf::Mesh& i_Mesh);

	// node.weights, else mesh.weights, else zeros; sized to NumMorphTargets
	std::vector<float> DefaultWeights(const tinygltf::Mesh& i_Mesh, const tinygltf::Node& i_Node);

	// "#<i_Target>": the name a morph target and its animation keys share
	std::string MorphTargetKey(int i_Target);
}
