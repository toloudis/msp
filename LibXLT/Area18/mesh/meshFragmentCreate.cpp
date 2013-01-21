/*****************************************************************************
**  meshFragmentCreate.cpp
**
**	meshFragmentCreate creates triangle mesh fragments by creating
**	bumpRegFrag objects.
**
** Area17
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "Area18/mesh/meshFragmentCreate.hpp"

#include "Area18/Area18Layer.hpp"
#include "Area18/ogl/oglSystem2D.h"
#include "Area18/mesh/meshTriMeshFrag.hpp"
#include "Area18/ogl/oglBufferUtil.hpp"

namespace
{
}

//--------------------------------------------------------------------
//	Creates fragment with no texture coordinates
//--------------------------------------------------------------------
g3dFragment* meshFragmentCreate::CreateFragment(	const maPoint3d* i_Vertices,
								const maPoint3d* i_Normals,
								int	i_nVertices,
								const g3dIndexPtr i_Indices,
								int	i_nIndices,
								matMaterial* i_pMaterial,
								bool i_bMorphable )
{
	return new meshTriMeshFrag(Area18Layer::GetDevice(0),
		i_Vertices, i_Normals, NULL, NULL, NULL,
		i_nVertices, i_Indices, i_nIndices, i_nIndices, 
		i_pMaterial, i_bMorphable);
}


//--------------------------------------------------------------------
//	Creates fragment with one set of texture coordinates
//--------------------------------------------------------------------
g3dFragment* meshFragmentCreate::CreateFragment(	const maPoint3d* i_Vertices,
								const maPoint3d* i_Normals,
								const maPoint2d* i_UVs,
								int	i_nVertices,
								const g3dIndexPtr i_Indices,
								int	i_nIndices,
								matMaterial* i_pMaterial,
								bool i_bMorphable  )
{
	return new meshTriMeshFrag(Area18Layer::GetDevice(0),
		i_Vertices, i_Normals, i_UVs, NULL, NULL,
		i_nVertices, i_Indices, i_nIndices, i_nIndices, 
		i_pMaterial, i_bMorphable);
}

//--------------------------------------------------------------------
//	Creates line list fragment
//--------------------------------------------------------------------
g3dFragment* meshFragmentCreate::CreateLineList(	const maPoint3d* i_Vertices,
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
	meshTriMeshFrag *frag = new meshTriMeshFrag(Area18Layer::GetDevice(0),
		i_Vertices, i_Normals, NULL, NULL, NULL,
		i_nVertices, i_Indices, i_nIndices, i_nIndices, 
		i_pMaterial, i_bMorphable);
	frag->SetPrimitiveType( meshTriMeshFrag::e_LineList );
	return frag;

//	meshTriMeshFrag *frag = new meshTriMeshFrag(i_Vertices, i_Normals, NULL, NULL, NULL,
//		i_nVertices, i_Indices, i_nIndices, i_nIndices, 
//		i_pMaterial, i_bMorphable, false, meshTriMeshFrag::e_LineList);
//	frag->SetPrimitiveType( meshTriMeshFrag::e_LineList );
//	return frag;
}

//--------------------------------------------------------------------
// Creates new fragment with same values as the old fragment
// sharing vertex and index buffers, if possible.
//--------------------------------------------------------------------
g3dFragment* meshFragmentCreate::CloneFragment(	const g3dFragment* i_pFragment )
{
	const meshTriMeshFrag *bump_frag = dynamic_cast<const meshTriMeshFrag*>( i_pFragment );
	if (bump_frag)
	{
		return new meshTriMeshFrag(*bump_frag);
	}
	return NULL;
}

//--------------------------------------------------------------------
// Set flag to turn off generation of basis vectors (for faster loads)
//--------------------------------------------------------------------
void meshFragmentCreate::SetComputeBasisVectors(bool i_bCompute)
{
	meshTriMeshFrag::SetComputeBasisVectors(i_bCompute);
}

//--------------------------------------------------------------------
// Set flag for whether to store geometry in video or system memory
//--------------------------------------------------------------------
void meshFragmentCreate::StoreGeometryInVideoMemory(bool i_bVideoMem)
{
	oglBufferUtil::StoreGeometryInVideoMemory(i_bVideoMem);
}
