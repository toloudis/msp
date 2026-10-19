/****************************************************************************\
**  g2dScreenCaptureUtilVk.cpp
**
**	See g2dScreenCaptureUtilVk.hpp.
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifndef GVK_G2DSCREENCAPTUREUTILVK_HPP
#include "GraphicsVulkan/G2d/g2dScreenCaptureUtilVk.hpp"
#endif
#ifndef GVK_G2DSTUBSVK_HPP
#include "GraphicsVulkan/stub/g2dStubsVk.hpp"
#endif
#ifndef GVK_G2DVKGLOBAL_HPP
#include "GraphicsVulkan/G2d/g2dVkGlobal.hpp"
#endif
#ifndef GVK_G2DWINDOWVK_HPP
#include "GraphicsVulkan/G2d/g2dWindowVk.hpp"
#endif
#ifndef GVK_G3DVKTEXTUREUTIL_HPP
#include "GraphicsVulkan/G3d/g3dVkTextureUtil.hpp"
#endif
#ifndef GVK_MATRENDERTARGETTEXTURE_HPP
#include "GraphicsVulkan/Mat/matRenderTargetTexture.hpp"
#endif
#ifndef GVK_VKDCONTEXT_HPP
#include "GraphicsVulkan/vkd/vkdContext.hpp"
#endif
#ifndef GVK_VKDFORMAT_HPP
#include "GraphicsVulkan/vkd/vkdFormat.hpp"
#endif
#ifndef GVK_VKDGLOBAL_HPP
#include "GraphicsVulkan/vkd/vkdGlobal.hpp"
#endif
#ifndef GVK_VKDIMAGE_HPP
#include "GraphicsVulkan/vkd/vkdImage.hpp"
#endif
#ifndef DBG_MSG_HPP
#include "Core/Dbg/dbgMsg.hpp"
#endif
#ifndef FS_FILEUTIL_HPP
#include "Core/fs/fsFileUtil.hpp"
#endif
#ifndef G2D_EXCEPTIONX_HPP
#include "Graphics/G2d/g2dExceptionX.hpp"
#endif
#ifndef G2D_IMAGESAVE_HPP
#include "Graphics/G2d/g2dImageSave.hpp"
#endif
#ifndef MAT_TEXTURE_HPP
#include "Graphics/Mat/matTexture.hpp"
#endif

#include <cstring>
#include <utility>
#include <vector>

namespace gvk
{
namespace
{
	//----------------------------------------------------------------------------
	//	DX11's capture_rendertarget: the first mip of i_pSource's first layer,
	//	copied to the CPU, as an image of its format (B8G8R8A8 becomes
	//	R8G8B8A8, as a DX11 back buffer)
	//----------------------------------------------------------------------------
	g2dImageVk* capture_image(vkdImage* i_pSource)
	{
		if (!i_pSource)
		{
			return NULL;
		}
		//	end the frame's rendering and submit it, then copy
		vkdGlobal::Context().Flush();
		std::vector<uint8_t> pixels = i_pSource->ReadBack(0, 0);

		VkFormat format = i_pSource->Format();
		if (format == VK_FORMAT_B8G8R8A8_UNORM || format == VK_FORMAT_B8G8R8A8_SRGB)
		{
			for (size_t i = 0; i + 3 < pixels.size(); i += 4)
			{
				std::swap(pixels[i], pixels[i + 2]);
			}
			format = (format == VK_FORMAT_B8G8R8A8_SRGB) ? VK_FORMAT_R8G8B8A8_SRGB : VK_FORMAT_R8G8B8A8_UNORM;
		}
		g2dPFD pfd;
		g2dVkGlobal::PFDFromDxgiFormat(vkdDxgiFromFormat(format), pfd);

		g2dImageVk* image = new g2dImageVk;
		image->Make(i_pSource->Width(), i_pSource->Height(), pfd, g2dImage::e_SystemMemory);
		const size_t bytes = (size_t)image->GetStride() * image->GetHeight();
		if (pixels.size() != bytes || !image->Lock())
		{
			DBG_ERROR("Couldn't copy surface (" << pixels.size() << " bytes read, " << bytes << " expected)");
			delete image;
			throw g2dCaptureX();
		}
		memcpy(image->Lock(), pixels.data(), bytes);
		image->Release();
		return image;
	}
}

	//------------------------------------------------------------------------
	//	CaptureWindowToFile writes screen to given filename, the
	//	format of this image is up to the implementation.
	//  This should be used for screen captures for PR shots.
	//
	//	the sample rate is how many input pixels per output pixel
	//	(a sampling of 2 on a 640x480 image will produce a 320x240 image)
	//	(a sampling of 4 on a 640x480 image will produce a 160x120 image)
	//------------------------------------------------------------------------
	void g2dScreenCaptureUtilVk::CaptureWindowToFile( g2dWindow& i_Window,
							const fsLocator& i_Locator,
							const int i_Sampling )
	{
		if (fsFileUtil::FileExists(i_Locator))
			fsFileUtil::DeleteFile(i_Locator);

		g2dImageSave::Save( i_Locator, &i_Window );
	}

	void g2dScreenCaptureUtilVk::CaptureImageToFile( g2dImage& i_Image,
							const fsLocator& i_Locator,
							const int i_Sampling )
	{
		g2dImageVk* pImageVk = dynamic_cast<g2dImageVk*>(&i_Image);
		if (pImageVk != NULL)
		{
			if (fsFileUtil::FileExists(i_Locator))
				fsFileUtil::DeleteFile(i_Locator);

			g2dImageSave::Save(i_Locator, &i_Image);
		}
	}

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void g2dScreenCaptureUtilVk::CaptureWindowToImage( g2dWindow& i_Window,
														g2dImage*& o_Image,
														bool backBuffer /*= true*/ )
	{
		g2dWindowVk* window = dynamic_cast<g2dWindowVk*>(&i_Window);
		DBG_ASSERT(window != NULL, "Expected a Vulkan Window");

		vkdImage* buffer = window->GetBackBuffer();
		DBG_ASSERT(buffer, "error getting back buffer");

		// ownership of the image goes to o_Image.
		o_Image = capture_image(buffer);
	}

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void g2dScreenCaptureUtilVk::CaptureRenderTargetToImage( g2dRenderTarget& i_Window,
																g2dImage*& o_Image)
	{
		matRenderTargetTexture* window = dynamic_cast<matRenderTargetTexture*>(&i_Window);
		DBG_ASSERT(window != NULL, "Expected a Vulkan render target texture");

		g2dImageVk* image = window ? capture_image(window->GetTextureSurface()) : NULL;
		if (image != NULL)
		{
			// ownership of the image goes to o_Image.
			o_Image = image;
		}
		else
		{
			DBG_ERROR("Couldn't copy surface");
			throw g2dCaptureX();
		}
	}

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void g2dScreenCaptureUtilVk::Capture2DTextureToImage( matTexture* i_pTexture,
															g2dImage*& o_Image)
	{
		vkdImage* buffer = g3dVkTextureUtil::GetImage(i_pTexture);
		DBG_ASSERT(buffer != NULL, "Expected a Vulkan texture");

		g2dImageVk* image = capture_image(buffer);
		if (image != NULL)
		{
			// ownership of the image goes to o_Image.
			o_Image = image;
		}
		else
		{
			DBG_ERROR("Couldn't copy surface");
			throw g2dCaptureX();
		}
	}
}
