/****************************************************************************\
**	cam3dUtil.hpp
**
**		cam3dUtil supplies functions useful for camera manipulation.
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/

#ifdef CAM3D_UTIL_HPP
#error cam3dUtil.hpp multiply included
#endif
#define CAM3D_UTIL_HPP


//============================================================================
//============================================================================
namespace cam3dUtil
{
	// This value (43.27) is base on the diagonal of 35mm film (36mm x 24mm)
	//const float c_35mmFilmDimension = 43.27f;	// 35mm
	// In Maya and our engine, the field of view only affects the width, so
	// we only need to use the 36mm part of the film dimension
	const float c_35mmFilmDimension = 36.0f;	// 35mm

	//------------------------------------------------------------------------
	//	Relationship between focal length and field of view.
	//  Field of view is in degrees, dimension in millimeters.
	//------------------------------------------------------------------------
	float CalculateFieldOfView( float i_FocalLength, 
								float i_FilmDimension = c_35mmFilmDimension, 
								float i_Magnification = 0.0f );
	float CalculateFocalLength( float i_FieldOfView, 
								float i_FilmDimension = c_35mmFilmDimension, 
								float i_Magnification = 0.0f);

}
