/****************************************************************************\
**  gltfNodeTransform.cpp
**
**      see gltfNodeTransform.hpp
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GLTF_NODETRANSFORM_HPP
#include "ImportExport/gltf/import/private/gltfNodeTransform.hpp"
#endif
#ifndef GLTF_MESHIMPORT_HPP
#include "ImportExport/gltf/import/private/gltfMeshImport.hpp"
#endif

#ifndef MA_MATRIX3X3_HPP
#include "Core/ma/maMatrix3x3.hpp"
#endif

namespace gltfNodeTransform
{
	maMatrix4x4 NodeMatrix(const tinygltf::Node& i_Node)
	{
		maMatrix4x4 m;
		if (i_Node.matrix.size() == 16)
		{
			// glTF matrices are column-major with column vectors, which has the
			// same memory layout as our row-major, row-vector matrices
			for (int i = 0; i < 16; ++i)
				m.Ptr()[i] = (float)i_Node.matrix[i];
			for (int i = 12; i < 15; ++i)
				m.Ptr()[i] *= gltfMeshImport::c_UnitScale;
			return m;
		}

		const Transform t = NodeTransform(i_Node);
		m.MakeScale(t.m_Scale.m_X, t.m_Scale.m_Y, t.m_Scale.m_Z);
		m *= t.m_Rotation.GetMatrix();
		m.TranslateBy(t.m_Translation.m_X, t.m_Translation.m_Y, t.m_Translation.m_Z);
		return m;
	}

	Transform Decompose(const maMatrix4x4& i_Matrix)
	{
		Transform t;
		const float* p = i_Matrix.GetPtr();
		t.m_Translation = maVector3d(p[12], p[13], p[14]);

		maVector3d rows[3];
		float scale[3];
		for (int r = 0; r < 3; r++)
		{
			rows[r] = maVector3d(p[r * 4], p[r * 4 + 1], p[r * 4 + 2]);
			scale[r] = rows[r].Length();
		}
		const float det = rows[0].m_X * (rows[1].m_Y * rows[2].m_Z - rows[1].m_Z * rows[2].m_Y)
						- rows[0].m_Y * (rows[1].m_X * rows[2].m_Z - rows[1].m_Z * rows[2].m_X)
						+ rows[0].m_Z * (rows[1].m_X * rows[2].m_Y - rows[1].m_Y * rows[2].m_X);
		if (det < 0)
			scale[0] = -scale[0];

		maMatrix3x3 rot;
		for (int r = 0; r < 3; r++)
		{
			const maVector3d axis = (scale[r] != 0) ? rows[r] * (1.0f / scale[r]) : maVector3d();
			rot(r, 0) = axis.m_X;
			rot(r, 1) = axis.m_Y;
			rot(r, 2) = axis.m_Z;
		}
		t.m_Rotation.SetValue(rot);
		t.m_Scale = maVector3d(scale[0], scale[1], scale[2]);
		return t;
	}

	Transform NodeTransform(const tinygltf::Node& i_Node)
	{
		if (i_Node.matrix.size() == 16)
			return Decompose(NodeMatrix(i_Node));

		Transform t;
		if (i_Node.scale.size() == 3)
			t.m_Scale = maVector3d((float)i_Node.scale[0], (float)i_Node.scale[1], (float)i_Node.scale[2]);
		if (i_Node.rotation.size() == 4)
			t.m_Rotation = maRotation((float)i_Node.rotation[0], (float)i_Node.rotation[1], (float)i_Node.rotation[2], (float)i_Node.rotation[3]);
		if (i_Node.translation.size() == 3)
			t.m_Translation = maVector3d((float)i_Node.translation[0], (float)i_Node.translation[1], (float)i_Node.translation[2]) * gltfMeshImport::c_UnitScale;
		return t;
	}
}
