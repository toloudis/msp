/****************************************************************************\
**  g2dScreenCaptureUtilVk.hpp
**
**	GraphicsVulkan port of GraphicsDX11's g2dScreenCaptureUtilDX11:
**	copies a window's back buffer (the frame being drawn), a render
**	target or a texture's first mip into a CPU image (g2dImageVk), and
**	saves windows and images to files through g2dImageSave. Where DX11
**	copies into a staging texture, Vulkan reads the image back.
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef GVK_G2DSCREENCAPTUREUTILVK_HPP
#error g2dScreenCaptureUtilVk.hpp multiply included
#endif
#define GVK_G2DSCREENCAPTUREUTILVK_HPP

#ifndef G2D_SCREENCAPTUREUTIL_HPP
#include "Graphics/G2d/g2dScreenCaptureUtil.hpp"
#endif

namespace gvk
{
	class g2dScreenCaptureUtilVk : public g2dScreenCaptureImpl
	{
		public:
			//------------------------------------------------------------------------
			//	CaptureWindowToFile writes screen to given filename, the
			//	format of this image is up to the implementation.
			//------------------------------------------------------------------------
			virtual void CaptureWindowToFile(g2dWindow& i_Window, const fsLocator& i_Locator, const int i_Sampling);
			virtual void CaptureImageToFile(g2dImage& i_Image, const fsLocator& i_Locator, const int i_Sampling);

			//------------------------------------------------------------------------
			//	The window's back buffer (the offscreen image when it renders at
			//	another resolution) as an image; the caller deletes it. An RGBA8
			//	image, as DX11's back buffer is, if the swap chain is BGRA.
			//------------------------------------------------------------------------
			virtual void CaptureWindowToImage(g2dWindow& i_Window, g2dImage*& o_Image, bool backBuffer = true);
			virtual void CaptureRenderTargetToImage(g2dRenderTarget& i_Window, g2dImage*& o_Image);
			virtual void Capture2DTextureToImage(matTexture* i_pTexture, g2dImage*& o_Image);
	};
}
