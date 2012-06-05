/****************************************************************************\
**  mnmScreenCaptureUtil.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "Support/mnm/mnmScreenCaptureUtil.hpp"

#include "Core/fs/fsFileStream.hpp"
#include "Core/fs/fsLocator.hpp"
#include "Graphics/g2d/g2dScreenCaptureUtil.hpp"
#include "Graphics/g2d/g2dWindow.hpp"


namespace
{
//
//	variables
//
	g2dWindow* l_pWindow = NULL;	// the capture window


} // end of anonymous namespace




//
//	mnmScreenCaptureUtil
//

//--------------------------------------------------------------------------------
//	WriteBMP writes screen in 24-bit BMP format to given filename.
//  This should be used for screen captures for PR shots.
//
//	the sample rate is how many input pixels per output pixel
//	(a sampling of 2 on a 640x480 image will produce a 320x240 image)
//	(a sampling of 4 on a 640x480 image will produce a 160x120 image)
//--------------------------------------------------------------------------------
void
mnmScreenCaptureUtil::WriteBMP( const fsLocator& i_Locator, const int i_Sampling )
{
	DBG_ASSERT( l_pWindow != NULL, "No window has been set." );
	g2dScreenCaptureUtil::CaptureWindowToFile(*l_pWindow, i_Locator, i_Sampling);
}

//------------------------------------------------------------------------
//	SetCaptureWindow() - set the window to capture from
//------------------------------------------------------------------------
void mnmScreenCaptureUtil::SetCaptureWindow( g2dWindow* i_pWindow )
{
	DBG_ASSERT( i_pWindow != NULL, "cannot capture from a null window" );
	l_pWindow = i_pWindow;
}
