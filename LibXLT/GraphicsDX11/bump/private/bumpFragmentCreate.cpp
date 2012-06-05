/*****************************************************************************
**  bumpFragmentCreate.cpp
**
**	bumpFragmentCreate creates triangle mesh fragments by creating
**	bumpRegFrag objects.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "GraphicsDX11/bump/bumpFragmentCreate.hpp"

#include "GraphicsDX11/bump/bumpTriMeshBumpFrag.hpp"
#include "GraphicsDX11/g3d/g3dDX11BufferUtil.hpp"

namespace
{
}

//--------------------------------------------------------------------
//	Creates fragment with no texture coordinates
//--------------------------------------------------------------------
g3dFragment* bumpFragmentCreate::CreateFragment(	const maPoint3d* i_Vertices,
								const maPoint3d* i_Normals,
								int	i_nVertices,
								const g3dIndexPtr i_Indices,
								int	i_nIndices,
								matMaterial* i_pMaterial,
								bool i_bMorphable )
{
	return new bumpTriMeshBumpFrag(i_Vertices, i_Normals, NULL, NULL, NULL,
		i_nVertices, i_Indices, i_nIndices, i_nIndices, 
		i_pMaterial, i_bMorphable);
}


//--------------------------------------------------------------------
//	Creates fragment with one set of texture coordinates
//--------------------------------------------------------------------
g3dFragment* bumpFragmentCreate::CreateFragment(	const maPoint3d* i_Vertices,
								const maPoint3d* i_Normals,
								const maPoint2d* i_UVs,
								int	i_nVertices,
								const g3dIndexPtr i_Indices,
								int	i_nIndices,
								matMaterial* i_pMaterial,
								bool i_bMorphable  )
{
	return new bumpTriMeshBumpFrag(i_Vertices, i_Normals, i_UVs, NULL, NULL,
		i_nVertices, i_Indices, i_nIndices, i_nIndices, 
		i_pMaterial, i_bMorphable);
}

//--------------------------------------------------------------------
//	Creates line list fragment
//--------------------------------------------------------------------
g3dFragment* bumpFragmentCreate::CreateLineList(	const maPoint3d* i_Vertices,
												const maPoint3d* i_Normals,
												int	i_nVertices,
												const g3dIndexPtr i_Indices,
												int	i_nIndices,
												matMaterial* i_pMaterial,
												bool i_bMorphable,
												const maPoint2d* i_UVs = NULL,
												const maPoint3d* i_Ss = NULL,
												const maPoint3d* i_Ts = NULL  )
{
	bumpTriMeshBumpFrag *frag = new bumpTriMeshBumpFrag(i_Vertices, i_Normals, NULL, NULL, NULL,
		i_nVertices, i_Indices, i_nIndices, i_nIndices, 
		i_pMaterial, i_bMorphable);
	frag->SetPrimitiveType( bumpTriMeshBumpFrag::e_LineList );
	return frag;

//	bumpTriMeshBumpFrag *frag = new bumpTriMeshBumpFrag(i_Vertices, i_Normals, NULL, NULL, NULL,
//		i_nVertices, i_Indices, i_nIndices, i_nIndices, 
//		i_pMaterial, i_bMorphable, false, bumpTriMeshBumpFrag::e_LineList);
//	frag->SetPrimitiveType( bumpTriMeshBumpFrag::e_LineList );
//	return frag;
}

//--------------------------------------------------------------------
// Creates new fragment with same values as the old fragment
// sharing vertex and index buffers, if possible.
//--------------------------------------------------------------------
g3dFragment* bumpFragmentCreate::CloneFragment(	const g3dFragment* i_pFragment )
{
	const bumpTriMeshBumpFrag *bump_frag = dynamic_cast<const bumpTriMeshBumpFrag*>( i_pFragment );
	if (bump_frag)
	{
		return new bumpTriMeshBumpFrag(*bump_frag);
	}
	return NULL;
}

//--------------------------------------------------------------------
// Set flag to turn off generation of basis vectors (for faster loads)
//--------------------------------------------------------------------
void bumpFragmentCreate::SetComputeBasisVectors(bool i_bCompute)
{
	bumpTriMeshBumpFrag::SetComputeBasisVectors(i_bCompute);
}

//--------------------------------------------------------------------
// Set flag for whether to store geometry in video or system memory
//--------------------------------------------------------------------
void bumpFragmentCreate::StoreGeometryInVideoMemory(bool i_bVideoMem)
{
	g3dDX11BufferUtil::StoreGeometryInVideoMemory(i_bVideoMem);
}
