/****************************************************************************\
**  g2dScreenCaptureUtilDX11.hpp
**
**      Supplies routines for saving the current scene into files or a movie
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifdef G2D_SCREENCAPTUREUTILD3D11_HPP
#error g2dScreenCaptureUtilDX11.hpp multiply included
#endif
#define G2D_SCREENCAPTUREUTILD3D11_HPP

#ifndef G2D_SCREENCAPTUREUTIL_HPP
#include "Graphics/g2d/g2dScreenCaptureUtil.hpp"
#endif

//============================================================================
//	forward references
//============================================================================
class fsLocator;
class g2dWindow;

//============================================================================
//============================================================================
class g2dScreenCaptureUtilDX11 : public g2dScreenCaptureImpl
{

public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	g2dScreenCaptureUtilDX11();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual ~g2dScreenCaptureUtilDX11();

	//------------------------------------------------------------------------
	//	CaptureWindowToFile writes screen to given filename, the
	//	format of this image is up to the implementation.
	//  This should be used for screen captures for PR shots.
	//
	//	the sample rate is how many input pixels per output pixel
	//	(a sampling of 2 on a 640x480 image will produce a 320x240 image)
	//	(a sampling of 4 on a 640x480 image will produce a 160x120 image)
	//------------------------------------------------------------------------
	virtual void CaptureWindowToFile(  g2dWindow& i_Window,
								const fsLocator& i_Locator, 
								const int i_Sampling );
	virtual void CaptureImageToFile(  g2dImage& i_Image,
								const fsLocator& i_Locator, 
								const int i_Sampling );

	// this CREATES the g2dimage. caller must delete.
	virtual void CaptureWindowToImage(  g2dWindow& i_Window,
								g2dImage*& o_Image, bool backBuffer = true );

	// this CREATES the g2dimage. caller must delete.
	virtual void CaptureRenderTargetToImage(  g2dRenderTarget& i_Window,
								g2dImage*& o_Image);

	// this CREATES the g2dimage. caller must delete.
	virtual void Capture2DTextureToImage(  matTexture* i_pTexture,
								g2dImage*& o_Image);
};

