/*****************************************************************************
**  gtScene.hpp
**
**      Synthetic scene pieces for the backend parity tests, built through
**      the Graphics API only, so the same code runs under either backend
**      (gtBackend): meshes (a UV sphere, a plane, a large grid) made into
**      fragments with g3dFragmentCreate.
**
**	Extra Large Technology
\****************************************************************************/
#pragma once

#ifndef MA_POINT2D_HPP
#include "Core/Ma/maPoint2d.hpp"
#endif
#ifndef MA_POINT3D_HPP
#include "Core/Ma/maPoint3d.hpp"
#endif

#include <cstdint>
#include <vector>

class g3dFragment;
class matMaterial;

//	A triangle mesh: positions, normals, UVs, and a triangle list
struct gtMesh
{
	std::vector<maPoint3d>	m_Positions;
	std::vector<maPoint3d>	m_Normals;
	std::vector<maPoint2d>	m_UVs;
	std::vector<uint32_t>	m_Indices;

	int VertexCount() const		{ return (int)m_Positions.size(); }
	int IndexCount() const		{ return (int)m_Indices.size(); }

	//	A fragment through g3dFragmentCreate (the current backend's), with
	//	UVs; 16-bit indices are passed when they fit, as the importers do
	g3dFragment* CreateFragment(matMaterial* i_pMaterial = NULL, bool i_bMorphable = false) const;
};

namespace gtScene
{
	//	Unit sphere at the origin: i_Rings bands from pole to pole,
	//	i_Segments around; (i_Rings + 1) * (i_Segments + 1) vertices
	gtMesh Sphere(int i_Rings, int i_Segments);

	//	The square -1..1 in X and Y at Z = 0, facing +Z, UVs 0..1
	gtMesh Plane();

	//	The square -1..1 in X and Y at Z = 0, facing +Z, cut into i_N x i_N
	//	quads: (i_N + 1)^2 vertices (Grid(300) needs 32-bit indices)
	gtMesh Grid(int i_N);
}
