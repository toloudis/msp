/*****************************************************************************
**	api3dShape.hpp
**
**		Creates objects that consist of primitive shapes
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef API3D_SHAPE_HPP
#error api3dShape.hpp multiply included
#endif
#define API3D_SHAPE_HPP

#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif
#ifndef MA_FLOATRGBA_HPP
#include "Core/ma/maFloatRGBA.hpp"
#endif
#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif


//============================================================================
//============================================================================
class api3dObjectSimple;


//============================================================================
//============================================================================
namespace api3dShape
{
	//--------------------------------------------------------------------
	//	CreateCube makes a fragment which is a cube with sides of the
	//	given length.
	//--------------------------------------------------------------------
	api3dObjectSimple* CreateCube( const maFloatRGBA &i_Color, float i_fSide, bool i_bMorphable = false);

	//--------------------------------------------------------------------
	// 	CreateTexturedRectangle makes a Rectangle with the given width,	
	//	height, and texture.  It will be XY planar centered at 0,0,0.
	//--------------------------------------------------------------------
	api3dObjectSimple* CreateTexturedRectangle(	const fsLocator& i_TexturePath,
											const maFloatRGBA &i_Color, 
											float i_fWidth,
											float i_fHeight,
											int i_nWidthDivisions,
											int i_nHeightDivisions,
											bool i_bMorphable = false );

	//--------------------------------------------------------------------
	// 	CreateRectangle makes a Rectangle with the given width and height.  
	//	It will be XY planar centered at 0,0,0.
	//--------------------------------------------------------------------
	api3dObjectSimple* CreateRectangle(	const maFloatRGBA &i_Color, 
									float i_fWidth,
									float i_fHeight,
									int i_nWidthDivisions,
									int i_nHeightDivisions,
									bool i_bMorphable = false);

	//--------------------------------------------------------------------
	// 	CreateCone makes a cone object which has the
	// given radius, latitude divisions, and longitude divisions.
	//--------------------------------------------------------------------
	api3dObjectSimple* CreateCone(const maFloatRGBA &i_Color, float i_fRadius,
							float i_fHeight, int i_nDivisions );

	//--------------------------------------------------------------------
	// 	CreateCylinder makes a cone object which has the
	// given radius, latitude divisions, and longitude divisions.
	//--------------------------------------------------------------------
	api3dObjectSimple* CreateCylinder(const maFloatRGBA &i_Color, float i_fRadius,
							float i_fBottomRadius, float i_fHeight, int i_nDivisions );

	//--------------------------------------------------------------------
	// 	CreateSphere makes a sphere-like object which has the
	// given radius, latitude divisions, and longitude divisions.
	//--------------------------------------------------------------------
	api3dObjectSimple* CreateSphere(const maFloatRGBA &i_Color, float i_fRadius,
							  int i_nLatDiv, int i_nLongDiv );

	//--------------------------------------------------------------------
	// 	CreateLineBlock makes a fragment which is a block with
	// sides of the given lengths, drawn with lines.
	//--------------------------------------------------------------------
	api3dObjectSimple* CreateLineBlock(const maFloatRGBA &i_Color, float i_fXSide,
								 float i_fYSide ,float i_fZSide, bool i_bMorphable = false);

	//--------------------------------------------------------------------
	// 	CreateLineList makes a line that passes through the
	//	vertices in the array.
	//--------------------------------------------------------------------
	api3dObjectSimple* CreateLineList(const maFloatRGBA &i_Color,
								maPoint3d* i_pVertices, int i_NumVerts,
								bool i_bClosed = false,
								bool i_bMorphable = false);

}	// end of namespace
