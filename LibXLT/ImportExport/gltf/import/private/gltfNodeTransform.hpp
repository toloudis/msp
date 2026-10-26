/****************************************************************************\
**  gltfNodeTransform.hpp
**
**      gltfNodeTransform reads a glTF node's local transform in
**	centimetres, as a matrix or as translation, rotation and scale, and
**	decomposes matrices. Used by the scene and skeleton readers.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GLTF_NODETRANSFORM_HPP
#error gltfNodeTransform.hpp multiply included
#endif
#define GLTF_NODETRANSFORM_HPP

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

namespace gltfNodeTransform
{
	struct Transform
	{
		maVector3d m_Translation;
		maRotation m_Rotation;
		maVector3d m_Scale = maVector3d(1, 1, 1);
	};

	// the node's local matrix (row vectors, as ours; centimetres)
	maMatrix4x4 NodeMatrix(const tinygltf::Node& i_Node);

	// translation, rotation and scale of a matrix: scale is the basis
	// vectors' lengths (one negated for a mirror), rotation the normalized basis
	Transform Decompose(const maMatrix4x4& i_Matrix);

	// the node's transform: TRS as given, else its matrix decomposed
	Transform NodeTransform(const tinygltf::Node& i_Node);
}
