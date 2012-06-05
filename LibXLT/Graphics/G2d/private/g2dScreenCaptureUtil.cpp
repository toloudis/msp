/****************************************************************************\
**  g2dScreenCaptureUtil.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Graphics/g2d/g2dScreenCaptureUtil.hpp"

#include "Core/dbg/dbgMsg.hpp"


//============================================================================
//============================================================================
g2dScreenCaptureImpl* g2dScreenCaptureUtil::sm_pImplementation = NULL;


//------------------------------------------------------------------------
//	CaptureWindowToFile writes screen to given filename, the
//	format of this image is up to the implementation.
//  This should be used for screen captures for PR shots.
//
//	the sample rate is how many input pixels per output pixel
//	(a sampling of 2 on a 640x480 image will produce a 320x240 image)
//	(a sampling of 4 on a 640x480 image will produce a 160x120 image)
//------------------------------------------------------------------------
//static 
void g2dScreenCaptureUtil::CaptureWindowToFile( g2dWindow& i_Window,
												const fsLocator& i_Locator, 
												const int i_Sampling )
{
	DBG_ASSERT(g2dScreenCaptureUtil::sm_pImplementation, "g2dScreenCaptureUtil: No implementation");
	if (!g2dScreenCaptureUtil::sm_pImplementation)
		return;
	sm_pImplementation->CaptureWindowToFile(i_Window, i_Locator, i_Sampling);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void g2dScreenCaptureUtil::CaptureImageToFile( g2dImage& i_Image,
							const fsLocator& i_Locator, 
							const int i_Sampling )
{
	DBG_ASSERT(g2dScreenCaptureUtil::sm_pImplementation, "g2dScreenCaptureUtil: No implementation");
	if (!g2dScreenCaptureUtil::sm_pImplementation)
		return;
	sm_pImplementation->CaptureImageToFile(i_Image, i_Locator, i_Sampling);
}

//----------------------------------------------------------------------------
// This function creates the g2dImage. Caller is responsible to delete.
//----------------------------------------------------------------------------
void g2dScreenCaptureUtil::CaptureWindowToImage(  g2dWindow& i_Window,
													g2dImage*& o_Image, bool backBuffer /*= true*/ )
{
	DBG_ASSERT(g2dScreenCaptureUtil::sm_pImplementation, "g2dScreenCaptureUtil: No implementation");
	if (!g2dScreenCaptureUtil::sm_pImplementation)
		return;
	sm_pImplementation->CaptureWindowToImage(i_Window, o_Image, backBuffer);
}

//----------------------------------------------------------------------------
// This function creates the g2dImage. Caller is responsible to delete.
//----------------------------------------------------------------------------
void g2dScreenCaptureUtil::CaptureRenderTargetToImage(  g2dRenderTarget& i_Window,
														g2dImage*& o_Image)
{
	DBG_ASSERT(g2dScreenCaptureUtil::sm_pImplementation, "g2dScreenCaptureUtil: No implementation");
	if (!g2dScreenCaptureUtil::sm_pImplementation)
		return;
	sm_pImplementation->CaptureRenderTargetToImage(i_Window, o_Image);
}

//----------------------------------------------------------------------------
// This function creates the g2dImage. Caller is responsible to delete.
//----------------------------------------------------------------------------
void g2dScreenCaptureUtil::Capture2DTextureToImage(  matTexture* i_pTexture,
														g2dImage*& o_Image)
{
	DBG_ASSERT(g2dScreenCaptureUtil::sm_pImplementation, "g2dScreenCaptureUtil: No implementation");
	if (!g2dScreenCaptureUtil::sm_pImplementation)
		return;
	sm_pImplementation->Capture2DTextureToImage(i_pTexture, o_Image);
}


//--------------------------------------------------------------------
// Set new implementation method, returns pointer to last one
// that was being used.  Both can be NULL.
// Ownership for the pointer remains with the caller.
//--------------------------------------------------------------------
//static
g2dScreenCaptureImpl* g2dScreenCaptureUtil::SetImplementation(g2dScreenCaptureImpl* i_Impl)
{
	g2dScreenCaptureImpl* old_impl = sm_pImplementation;
	sm_pImplementation = i_Impl;
	return old_impl;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
//virtual 
g2dScreenCaptureImpl::~g2dScreenCaptureImpl()
{
}
