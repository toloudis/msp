/****************************************************************************\
**  g2dScreenCaptureUtil.hpp
**
**      Supplies routines for saving the current scene into files or a movie
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#ifdef G2D_SCREENCAPTUREUTIL_HPP
#error g2dScreenCaptureUtil.hpp multiply included
#endif
#define G2D_SCREENCAPTUREUTIL_HPP


//============================================================================
//	forward references
//============================================================================
class fsLocator;
class g2dImage;
class g2dScreenCaptureImpl;
class g2dRenderTarget;
class g2dWindow;
class matTexture;

//============================================================================
//============================================================================
class g2dScreenCaptureUtil
{
public:
	//------------------------------------------------------------------------
	//	CaptureWindowToFile writes screen to given filename, the
	//	format of this image is up to the implementation.
	//  This should be used for screen captures for PR shots.
	//
	//	the sample rate is how many input pixels per output pixel
	//	(a sampling of 2 on a 640x480 image will produce a 320x240 image)
	//	(a sampling of 4 on a 640x480 image will produce a 160x120 image)
	//------------------------------------------------------------------------
	static void CaptureWindowToFile( g2dWindow& i_Window,
								const fsLocator& i_Locator, 
								const int i_Sampling = 1 );

	static void CaptureImageToFile( g2dImage& i_Image,
								const fsLocator& i_Locator, 
								const int i_Sampling = 1 );

	static void CaptureWindowToImage(  g2dWindow& i_Window,
								g2dImage*& o_Image, bool backBuffer = true );

	static void CaptureRenderTargetToImage(  g2dRenderTarget& i_Window,
								g2dImage*& o_Image);

	static void Capture2DTextureToImage(  matTexture* i_pTexture,
								g2dImage*& o_Image);

//--------------------------------------------------------------------
// Methods for defining implementation
//--------------------------------------------------------------------

	//--------------------------------------------------------------------
	// Set new implementation method, returns pointer to last one
	// that was being used.  Both can be NULL.
	// Ownership for the pointer remains with the caller.
	//--------------------------------------------------------------------
	static g2dScreenCaptureImpl* SetImplementation(g2dScreenCaptureImpl* i_pImpl);

private:
	static g2dScreenCaptureImpl* sm_pImplementation;
};


//============================================================================
//============================================================================
class g2dScreenCaptureImpl
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual ~g2dScreenCaptureImpl();

	//------------------------------------------------------------------------
	//	CaptureWindowToFile writes screen to given filename, the
	//	format of this image is up to the implementation.
	//  This should be used for screen captures for PR shots.
	//
	//	the sample rate is how many input pixels per output pixel
	//	(a sampling of 2 on a 640x480 image will produce a 320x240 image)
	//	(a sampling of 4 on a 640x480 image will produce a 160x120 image)
	//------------------------------------------------------------------------
	virtual void CaptureWindowToFile( g2dWindow& i_Window,
								const fsLocator& i_Locator, 
								const int i_Sampling ) = 0;
	
	virtual void CaptureImageToFile( g2dImage& i_Image,
								const fsLocator& i_Locator, 
								const int i_Sampling ) = 0;

	// This function creates the g2dImage. Caller is responsible to delete.
	virtual void CaptureWindowToImage(  g2dWindow& i_Window,
								g2dImage*& img, bool backBuffer = true ) = 0;

	// This function creates the g2dImage. Caller is responsible to delete.
	virtual void CaptureRenderTargetToImage(  g2dRenderTarget& i_Window,
								g2dImage*& img) = 0;

	// This function creates the g2dImage. Caller is responsible to delete.
	virtual void Capture2DTextureToImage(  matTexture* i_pTexture,
								g2dImage*& o_Image) = 0;

};
