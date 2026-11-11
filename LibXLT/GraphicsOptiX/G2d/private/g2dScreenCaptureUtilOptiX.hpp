/*****************************************************************************
**  g2dScreenCaptureUtilOptiX.hpp
**
**      g2dScreenCaptureUtilOptiX reads the OptiX backend's windows, render
**	targets and textures back into CPU images (g2dScreenCaptureUtil).
**
**	Stub: captures nothing (images are NULL; no file is written).
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GOX_G2DSCREENCAPTUREUTILOPTIX_HPP
#error g2dScreenCaptureUtilOptiX.hpp multiply included
#endif
#define GOX_G2DSCREENCAPTUREUTILOPTIX_HPP

#ifndef G2D_SCREENCAPTUREUTIL_HPP
#include "Graphics/G2d/g2dScreenCaptureUtil.hpp"
#endif

namespace gox
{
	class g2dScreenCaptureUtilOptiX : public g2dScreenCaptureImpl
	{
		public:
			virtual void CaptureWindowToFile(g2dWindow& i_Window, const fsLocator& i_Locator, const int i_Sampling);
			virtual void CaptureImageToFile(g2dImage& i_Image, const fsLocator& i_Locator, const int i_Sampling);
			virtual void CaptureWindowToImage(g2dWindow& i_Window, g2dImage*& o_Image, bool i_bBackBuffer = true);
			virtual void CaptureRenderTargetToImage(g2dRenderTarget& i_Target, g2dImage*& o_Image);
			virtual void Capture2DTextureToImage(matTexture* i_pTexture, g2dImage*& o_Image);
	};
}
