/*****************************************************************************
**  gtScene.cpp
**
**      See gtScene.hpp.
**
**	Extra Large Technology
\****************************************************************************/
#include "gtScene.hpp"

#ifndef G3D_FRAGMENTCREATE_HPP
#include "Graphics/G3d/g3dFragmentCreate.hpp"
#endif
#ifndef G3D_INDEXPTR_HPP
#include "Graphics/G3d/g3dIndexPtr.hpp"
#endif

#include <cmath>

g3dFragment* gtMesh::CreateFragment(matMaterial* i_pMaterial, bool i_bMorphable) const
{
	if (g3dIndexPtr::Needs32Bit(VertexCount()))
	{
		return g3dFragmentCreate::CreateFragment(m_Positions.data(), m_Normals.data(), m_UVs.data(), VertexCount(),
												 g3dIndexPtr(m_Indices.data()), IndexCount(), i_pMaterial, i_bMorphable);
	}
	std::vector<uint16_t> indices(m_Indices.begin(), m_Indices.end());
	return g3dFragmentCreate::CreateFragment(m_Positions.data(), m_Normals.data(), m_UVs.data(), VertexCount(),
											 g3dIndexPtr(indices.data()), IndexCount(), i_pMaterial, i_bMorphable);
}

namespace
{
	//	A (i_Columns + 1) x (i_Rows + 1) vertex lattice, two triangles per cell
	void lattice_indices(int i_Columns, int i_Rows, std::vector<uint32_t>& o_Indices)
	{
		const int stride = i_Columns + 1;
		for (int r = 0; r < i_Rows; r++)
		{
			for (int c = 0; c < i_Columns; c++)
			{
				const uint32_t a = r * stride + c;
				const uint32_t b = a + 1;
				const uint32_t d = a + stride;
				const uint32_t e = d + 1;
				o_Indices.insert(o_Indices.end(), { a, d, b, b, d, e });
			}
		}
	}
}

gtMesh gtScene::Sphere(int i_Rings, int i_Segments)
{
	gtMesh mesh;
	const double pi = 3.14159265358979323846;
	for (int r = 0; r <= i_Rings; r++)
	{
		const double theta = pi * r / i_Rings;			// from +Z down
		for (int s = 0; s <= i_Segments; s++)
		{
			const double phi = 2 * pi * s / i_Segments;
			const float x = (float)(std::sin(theta) * std::cos(phi));
			const float y = (float)(std::sin(theta) * std::sin(phi));
			const float z = (float)std::cos(theta);
			mesh.m_Positions.push_back(maPoint3d(x, y, z));
			mesh.m_Normals.push_back(maPoint3d(x, y, z));
			mesh.m_UVs.push_back(maPoint2d((float)s / i_Segments, (float)r / i_Rings));
		}
	}
	lattice_indices(i_Segments, i_Rings, mesh.m_Indices);
	return mesh;
}

gtMesh gtScene::Grid(int i_N)
{
	gtMesh mesh;
	for (int r = 0; r <= i_N; r++)
	{
		for (int c = 0; c <= i_N; c++)
		{
			const float u = (float)c / i_N;
			const float v = (float)r / i_N;
			mesh.m_Positions.push_back(maPoint3d(-1 + 2 * u, 1 - 2 * v, 0));
			mesh.m_Normals.push_back(maPoint3d(0, 0, 1));
			mesh.m_UVs.push_back(maPoint2d(u, v));
		}
	}
	lattice_indices(i_N, i_N, mesh.m_Indices);
	return mesh;
}

gtMesh gtScene::Plane()
{
	return Grid(1);
}
