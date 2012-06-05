/*****************************************************************************
**	g3dFragmentCreate.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Graphics/g3d/g3dFragmentCreate.hpp"


//--------------------------------------------------------------------
//	Creates fragment with no texture coordinates
//--------------------------------------------------------------------
//static
g3dFragment* g3dFragmentCreate::CreateFragment(	const maPoint3d* i_Vertices,
												const maPoint3d* i_Normals,
												int	i_nVertices,
												const g3dIndexPtr i_Indices,
												int	i_nIndices,
												matMaterial* i_pMaterial,
												bool i_bMorphable )
{
	DBG_ASSERT(g3dFragmentCreate::sm_pImplementation, "g3dFragmentCreate: No implementation");
	if (!sm_pImplementation) return NULL;
	return sm_pImplementation->CreateFragment(i_Vertices, i_Normals, i_nVertices,
		i_Indices, i_nIndices, i_pMaterial, i_bMorphable);
}


//--------------------------------------------------------------------
//	Creates fragment with one set of texture coordinates
//--------------------------------------------------------------------
//static
g3dFragment* g3dFragmentCreate::CreateFragment(	const maPoint3d* i_Vertices,
												const maPoint3d* i_Normals,
												const maPoint2d* i_UVs,
												int	i_nVertices,
												const g3dIndexPtr i_Indices,
												int	i_nIndices,
												matMaterial* i_pMaterial,
												bool i_bMorphable  )
{
	DBG_ASSERT(g3dFragmentCreate::sm_pImplementation, "g3dFragmentCreate: No implementation");
	if (!sm_pImplementation) return NULL;
	return sm_pImplementation->CreateFragment(i_Vertices, i_Normals, i_UVs, i_nVertices,
		i_Indices, i_nIndices, i_pMaterial, i_bMorphable);
}

//--------------------------------------------------------------------
//	Creates line list fragment
//--------------------------------------------------------------------
//static
g3dFragment* g3dFragmentCreate::CreateLineList(	const maPoint3d* i_Vertices,
												const maPoint3d* i_Normals,
												int	i_nVertices,
												const g3dIndexPtr i_Indices,
												int	i_nIndices,
												matMaterial* i_pMaterial,
												bool i_bMorphable,
												const maPoint2d* i_UVs,
												const maPoint3d* i_Ss,
												const maPoint3d* i_Ts )
{
	DBG_ASSERT(g3dFragmentCreate::sm_pImplementation, "g3dFragmentCreate: No implementation");
	if (!sm_pImplementation) return NULL;
	return sm_pImplementation->CreateLineList(i_Vertices, i_Normals, i_nVertices,
		i_Indices, i_nIndices, i_pMaterial, i_bMorphable, i_UVs, i_Ss, i_Ts );
}

//--------------------------------------------------------------------
// Creates new fragment with same values as the old fragment
// sharing vertex and index buffers, if possible.
//--------------------------------------------------------------------
//static 
g3dFragment* g3dFragmentCreate::CloneFragment(	const g3dFragment* i_pFragment )
{
	DBG_ASSERT(g3dFragmentCreate::sm_pImplementation, "g3dFragmentCreate: No implementation");
	if (!sm_pImplementation) return NULL;
	return sm_pImplementation->CloneFragment(i_pFragment);
}

//--------------------------------------------------------------------
// Set flag to turn off generation of basis vectors (for faster loads)
//--------------------------------------------------------------------
//static 
void g3dFragmentCreate::SetComputeBasisVectors(bool i_bCompute)
{
	DBG_ASSERT(g3dFragmentCreate::sm_pImplementation, "g3dFragmentCreate: No implementation");
	if (!sm_pImplementation) return;
	return sm_pImplementation->SetComputeBasisVectors(i_bCompute);
}

//--------------------------------------------------------------------
// Set flag for whether to store geometry in video or system memory
//--------------------------------------------------------------------
//static 
void g3dFragmentCreate::StoreGeometryInVideoMemory(bool i_bVideoMem)
{
	DBG_ASSERT(g3dFragmentCreate::sm_pImplementation, "g3dFragmentCreate: No implementation");
	if (!sm_pImplementation) return;
	return sm_pImplementation->StoreGeometryInVideoMemory(i_bVideoMem);
}
