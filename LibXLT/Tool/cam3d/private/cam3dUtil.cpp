/****************************************************************************\
**	cam3dUtil.cpp
**
**		cam3dUtil supplies functions useful for camera manipulation.
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Tool/cam3d/cam3dUtil.hpp"

#include "Core/ma/maConstants.hpp"
#include "Core/ma/maFunctions.hpp"


//============================================================================
//============================================================================
namespace cam3dUtil
{
//------------------------------------------------------------------------
//	Relationship between focal length and field of view.
//  Field of view is in degrees, dimension in millimeters.
//------------------------------------------------------------------------
float CalculateFieldOfView( float i_FocalLength, 
						    float i_FilmDimension, 
							float i_Magnification )
{
	return (float) ( maConstants::c_fRadToAngle * ( 2.0f * atan( i_FilmDimension / (2.0f * i_FocalLength * (1.0f + i_Magnification) ) ) ) );
}

float CalculateFocalLength( float i_FieldOfView, 
						    float i_FilmDimension, 
							float i_Magnification )
{
	float fov_rad = maConstants::c_fAngleToRad * i_FieldOfView;
	return  (float) (i_FilmDimension / ( 2.0f * tan( fov_rad / 2.0f ) * (1.0f + i_Magnification) ));
}

}	// end of namespace
