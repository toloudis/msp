/*****************************************************************************
**  g2dScreenCaptureUtilOptiX.cpp
**
**      See g2dScreenCaptureUtilOptiX.hpp.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GOX_G2DSCREENCAPTUREUTILOPTIX_HPP
#include "GraphicsOptiX/G2d/private/g2dScreenCaptureUtilOptiX.hpp"
#endif

#include <cstddef>

namespace gox
{
	void g2dScreenCaptureUtilOptiX::CaptureWindowToFile(g2dWindow& /*i_Window*/, const fsLocator& /*i_Locator*/, const int /*i_Sampling*/)
	{
	}

	void g2dScreenCaptureUtilOptiX::CaptureImageToFile(g2dImage& /*i_Image*/, const fsLocator& /*i_Locator*/, const int /*i_Sampling*/)
	{
	}

	void g2dScreenCaptureUtilOptiX::CaptureWindowToImage(g2dWindow& /*i_Window*/, g2dImage*& o_Image, bool /*i_bBackBuffer*/)
	{
		o_Image = NULL;
	}

	void g2dScreenCaptureUtilOptiX::CaptureRenderTargetToImage(g2dRenderTarget& /*i_Target*/, g2dImage*& o_Image)
	{
		o_Image = NULL;
	}

	void g2dScreenCaptureUtilOptiX::Capture2DTextureToImage(matTexture* /*i_pTexture*/, g2dImage*& o_Image)
	{
		o_Image = NULL;
	}
}
