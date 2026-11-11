/*****************************************************************************
**  g3dFragmentCreateOptiX.cpp
**
**      See g3dFragmentCreateOptiX.hpp.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GOX_G3DFRAGMENTCREATEOPTIX_HPP
#include "GraphicsOptiX/G3d/private/g3dFragmentCreateOptiX.hpp"
#endif

#ifndef GOX_G3DFRAGMENTOPTIX_HPP
#include "GraphicsOptiX/G3d/g3dFragmentOptiX.hpp"
#endif

#include <cstddef>

namespace gox
{
	g3dFragment* g3dFragmentCreateOptiX::CreateFragment(const maPoint3d* i_Vertices,
														const maPoint3d* i_Normals,
														int i_nVertices,
														const g3dIndexPtr i_Indices,
														int i_nIndices,
														matMaterial* i_pMaterial,
														bool i_bMorphable)
	{
		return new g3dFragmentOptiX(i_Vertices, i_Normals, NULL, NULL, NULL, i_nVertices, i_Indices, i_nIndices, i_pMaterial, i_bMorphable, g3dFragmentOptiX::e_TriangleList);
	}

	g3dFragment* g3dFragmentCreateOptiX::CreateFragment(const maPoint3d* i_Vertices,
														const maPoint3d* i_Normals,
														const maPoint2d* i_UVs,
														int i_nVertices,
														const g3dIndexPtr i_Indices,
														int i_nIndices,
														matMaterial* i_pMaterial,
														bool i_bMorphable)
	{
		return new g3dFragmentOptiX(i_Vertices, i_Normals, i_UVs, NULL, NULL, i_nVertices, i_Indices, i_nIndices, i_pMaterial, i_bMorphable, g3dFragmentOptiX::e_TriangleList);
	}

	g3dFragment* g3dFragmentCreateOptiX::CreateLineList(const maPoint3d* i_Vertices,
														const maPoint3d* i_Normals,
														int i_nVertices,
														const g3dIndexPtr i_Indices,
														int i_nIndices,
														matMaterial* i_pMaterial,
														bool i_bMorphable,
														const maPoint2d* i_UVs,
														const maPoint3d* i_Ss,
														const maPoint3d* i_Ts)
	{
		return new g3dFragmentOptiX(i_Vertices, i_Normals, i_UVs, i_Ss, i_Ts, i_nVertices, i_Indices, i_nIndices, i_pMaterial, i_bMorphable, g3dFragmentOptiX::e_LineList);
	}

	g3dFragment* g3dFragmentCreateOptiX::CloneFragment(const g3dFragment* i_pFragment)
	{
		const g3dFragmentOptiX* frag = dynamic_cast<const g3dFragmentOptiX*>(i_pFragment);
		return frag ? new g3dFragmentOptiX(*frag) : NULL;
	}

	void g3dFragmentCreateOptiX::SetComputeBasisVectors(bool /*i_bCompute*/)
	{
	}

	void g3dFragmentCreateOptiX::StoreGeometryInVideoMemory(bool /*i_bVideoMem*/)
	{
	}
}
