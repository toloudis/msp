/****************************************************************************\
**	hairFragmentCreate.hpp
**
**	hairFragmentCreate contains functions for creating a hair fragment from
**	vertices and materials.  It must be configured to create fragments
**	of a certain type by implementing its Creator class.
**
**	StudioGPU
**	Copyright(C) 2009. - All Rights Reserved
\****************************************************************************/

#ifdef HAIR_FRAGMENTCREATE_HPP
#error hairFragmentCreate.hpp multiply included
#endif
#define HAIR_FRAGMENTCREATE_HPP

#ifndef ENV_ABSTRACTION_HPP
#include "Core/env/envAbstraction.hpp"
#endif

#ifndef MA_POINT2D_HPP
#include "Core/ma/maPoint2d.hpp"
#endif
#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif
#ifndef MA_POINT4D_HPP
#include "Core/ma/maPoint4d.hpp"
#endif
#ifndef G3D_INDEXPTR_HPP
#include "Graphics/G3d/g3dIndexPtr.hpp"
#endif

//forward declarations
class g3dFragment;
class matMaterial;

namespace hair
{
	class HairInfo;
}

class hairFragmentCreateImpl
{
public:
/*
	virtual g3dFragment* CreateFragment( const maPoint4d* i_Vertices,
										 const maPoint3d* i_Tangents,
										 const unsigned int* i_Colors,
										 const maPoint2d* i_UVs,
										 int              i_nVertices,
										 matMaterial*     i_pMaterial ) = 0;
*/
	virtual g3dFragment* CreateHairFragment( const hair::HairInfo* i_Hair,
		                                     matMaterial*          i_pMaterial ) = 0;

	//--------------------------------------------------------------------
	// Creates new fragment with same values as the old fragment
	// sharing vertex and index buffers, if possible.
	//--------------------------------------------------------------------
	virtual g3dFragment* CloneFragment( const g3dFragment* i_pFragment ) = 0;
};

class hairFragmentCreate : public envAbstraction<hairFragmentCreateImpl>
{
public:
	//--------------------------------------------------------------------
	//	Creates fragment with no texture coordinates
	//--------------------------------------------------------------------
/*
	static g3dFragment* CreateFragment( const maPoint4d* i_Vertices,
										const maPoint3d* i_Tangents,
										const unsigned int* i_Colors,
										const maPoint2d* i_UVs,
										int              i_nVertices,
										matMaterial*     i_pMaterial );
*/
	static g3dFragment* CreateHairFragment( hair::HairInfo* i_Hair,
		                                    matMaterial*    i_pMaterial );

	//--------------------------------------------------------------------
	// Creates new fragment with same values as the old fragment
	// sharing vertex and index buffers, if possible.
	//--------------------------------------------------------------------
	static g3dFragment* CloneFragment( const g3dFragment* i_pFragment );
};


