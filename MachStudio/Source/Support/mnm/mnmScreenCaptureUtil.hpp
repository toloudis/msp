/****************************************************************************\
**  mnmScreenCaptureUtil.hpp
**
**      Supplies routines for saving the current scene into files
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef MNM_SCREENCAPTUREUTIL_HPP
#error mnmScreenCaptureUtil.hpp multiply included
#endif
#define MNM_SCREENCAPTUREUTIL_HPP


//============================================================================
//	forward references
//============================================================================
class fsLocator;
class g2dWindow;

//============================================================================
//============================================================================
namespace mnmScreenCaptureUtil
{
	//------------------------------------------------------------------------
	//	WriteBMP writes screen in 24-bit BMP format to given filename.
	//  This should be used for screen captures for PR shots.
	//
	//	the sample rate is how many input pixels per output pixel
	//	(a sampling of 2 on a 640x480 image will produce a 320x240 image)
	//	(a sampling of 4 on a 640x480 image will produce a 160x120 image)
	//------------------------------------------------------------------------
	void WriteBMP( const fsLocator& i_Locator, const int i_Sampling = 1 );

	//------------------------------------------------------------------------
	//	SetCaptureWindow() - set the window to capture from
	//------------------------------------------------------------------------
	void SetCaptureWindow( g2dWindow* i_pWindow );
}

