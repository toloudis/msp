/*****************************************************************************
**  hairFragmentCreate.cpp
**
**	hairFragmentCreate contains functions for creating hair fragments from
**	vertices and materials.  It must be configured to create fragments
**	of a certain type by implementing its Creator class.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Graphics/hair/hairFragmentCreate.hpp"

#include "Graphics/hair/HairReader.h"

//--------------------------------------------------------------------
//	Creates fragment with no texture coordinates
//--------------------------------------------------------------------
//static
/*
g3dFragment* hairFragmentCreate::CreateFragment( const maPoint4d* i_Vertices,
												 const maPoint3d* i_Tangents,
												 const unsigned int* i_Colors,
												 const maPoint2d* i_UVs,
												 int              i_nVertices,
												 matMaterial*     i_pMaterial )
{
	DBG_ASSERT(hairFragmentCreate::sm_pImplementation, "hairFragmentCreate: No implementation");
	if (!sm_pImplementation) return NULL;
	return sm_pImplementation->CreateFragment( i_Vertices, i_Tangents, i_Colors, i_UVs, i_nVertices, i_pMaterial );
}
*/
//static
g3dFragment* hairFragmentCreate::CreateHairFragment( hair::HairInfo* i_Hair,
									   matMaterial*     i_pMaterial )
{
	DBG_ASSERT(hairFragmentCreate::sm_pImplementation, "hairFragmentCreate: No implementation");
	if (!sm_pImplementation) return NULL;
	return sm_pImplementation->CreateHairFragment( i_Hair, i_pMaterial );
}

//--------------------------------------------------------------------
// Creates new fragment with same values as the old fragment
// sharing vertex and index buffers, if possible.
//--------------------------------------------------------------------
g3dFragment* hairFragmentCreate::CloneFragment(	const g3dFragment* i_pFragment )
{
	DBG_ASSERT(hairFragmentCreate::sm_pImplementation, "hairFragmentCreate: No implementation");
	if (!sm_pImplementation) return NULL;
	return sm_pImplementation->CloneFragment( i_pFragment );
}
