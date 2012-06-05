/*****************************************************************************
**	captStereoUtil.hpp
**
**		Stereoscopy utility functions
**
**	Studio GPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef CAPT_STEREOUTIL_HPP
#error captStereoUtil.hpp multiply included
#endif
#define CAPT_STEREOUTIL_HPP

#ifndef CAM_CAMERA_HPP
#include "Graphics/cam/camCamera.hpp"
#endif
#ifndef G2D_PFD_HPP
#include "Graphics/g2d/g2dPFD.hpp"
#endif


//============================================================================
//	Forward References
//============================================================================
class maFloatRGBA;
class fsLocator;
class itString;
class g2dImage;
class maVector3d;


//============================================================================
//============================================================================
#define STEREO_OFF			0
#define STEREO_ANAGLYPH		1
#define STEREO_DUALCAM		2
#define STEREO_LEFTCAM		3
#define STEREO_RIGHTCAM		4

#define PI_OVER_ONE_EIGHTY	0.0174532925

#define R_OFFSET_RGBAxxf	0
#define G_OFFSET_RGBAxxf	1
#define B_OFFSET_RGBAxxf	2
#define A_OFFSET_RGBAxxf	3

#define R_OFFSET_E_COLOR	0
#define G_OFFSET_E_COLOR	1
#define B_OFFSET_E_COLOR	2
#define A_OFFSET_E_COLOR	3

//#define R_OFFSET_E_COLOR	2
//#define G_OFFSET_E_COLOR	1
//#define B_OFFSET_E_COLOR	0
//#define A_OFFSET_E_COLOR	3

#define TOE_IN		0
#define OFF_AXIS	1


//============================================================================
//============================================================================
namespace captStereoUtil
{
	//--------------------------------------------------------------------
	// GetPixelType()
	//--------------------------------------------------------------------
	int GetNumBytesPerPixel(g2dPFD::PixelFormat rendererPFD);

	//--------------------------------------------------------------------
	// setChannelOffsets()
	//--------------------------------------------------------------------
	void setChannelOffsets(g2dPFD::PixelFormat rendererPFD, int * offsets);

	//--------------------------------------------------------------------
	// GetLeftColorFilter()
	//--------------------------------------------------------------------
	void SetLeftColorFilter(int selection , maFloatRGBA * filter);

	//--------------------------------------------------------------------
	// GetRightColorFilter()
	//--------------------------------------------------------------------
	void SetRightColorFilter(int selection , maFloatRGBA * filter);

	//--------------------------------------------------------------------------------
	//	MergeLeftRight()
	//--------------------------------------------------------------------------------
	void MergeLeftRight(g2dImage* ana, g2dImage* left, g2dImage* right, g2dPFD::PixelFormat rendererPFD );

	//--------------------------------------------------------------------
	// ApplyAnaglyphColorFilter()
	//--------------------------------------------------------------------
	void ApplyAnaglyphColorFilter( g2dImage* orig, const maFloatRGBA& filter, g2dPFD::PixelFormat rendererPFD  );

	//--------------------------------------------------------------------
	// SetStereoCamPos()
	//--------------------------------------------------------------------
	float SetStereoCamPos(const maVector3d& origLeftVec, const maPoint3d& origPos, const maPoint3d& origTarget, 
						 const maPoint3d& origUp, camCamera * pCamera, int currentEye);
};
