/****************************************************************************\
**  g2dWindowDrawUtilVk.cpp
**
**      See g2dWindowDrawUtilVk.hpp.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GVK_G2DWINDOWDRAWUTILVK_HPP
#include "GraphicsVulkan/G2d/private/g2dWindowDrawUtilVk.hpp"
#endif
#ifndef GVK_G2DVKGLOBAL_HPP
#include "GraphicsVulkan/G2d/g2dVkGlobal.hpp"
#endif
#ifndef GVK_G2DSTUBSVK_HPP
#include "GraphicsVulkan/stub/g2dStubsVk.hpp"
#endif
#ifndef GVK_G2DFONTUTILVK_HPP
#include "GraphicsVulkan/G2d/private/g2dFontUtilVk.hpp"
#endif
#ifndef G2D_RGBCOLOR_HPP
#include "Graphics/G2d/g2dRGBColor.hpp"
#endif
#ifndef GVK_VKDCONTEXT_HPP
#include "GraphicsVulkan/vkd/vkdContext.hpp"
#endif
#ifndef GVK_VKDDEVICE_HPP
#include "GraphicsVulkan/vkd/vkdDevice.hpp"
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
#include "Core/dbg/dbgMsg.hpp"
#endif

#include <cstring>
#include <mutex>

namespace gvk
{
namespace g2dWindowDrawUtilVk
{

//----------------------------------------------------------------------------
//	DrawRect draws a rectangle of the given color between the two locations
//	given (in client coordinates).  The rectangle will include the
//	rows at i_X1 and i_Y1, but not i_X2 and i_Y2.  This is convenient
//	for spacing adjacent rectangles.
//	The rectangle will be silently clipped or rejected by the current
//	screen viewing area.
//----------------------------------------------------------------------------
void DrawRect(	int /*i_X1*/,
				int /*i_Y1*/,
				int /*i_X2*/,
				int /*i_Y2*/,
				const g2dRGBColor& /*i_Color*/)
{
	// should draw a screen aligned quad here.
	DBG_ERROR("g2dWindowDrawUtilVk::DrawRect not implemented!");
}

//----------------------------------------------------------------------------
//	DrawText draws the given text such that it's upper left corner is
//	at (i_X1, i_Y1).  It will be silently clipped or rejected by the
//	current viewing area.
//----------------------------------------------------------------------------
void DrawText(	int i_X,
				int i_Y,
				g2dFontHandle i_Font,
				const itString& i_Text,
				const g2dRGBColor& i_Color)
{
	g2dFontObjectVk* font = reinterpret_cast<g2dFontObjectVk*>(i_Font);
	if (font)
		font->DrawText(i_Text, maFloatRGBA(i_Color.GetRed(), i_Color.GetGreen(), i_Color.GetBlue(), 1.0f), i_X, i_Y);
}

//----------------------------------------------------------------------------
//	This DrawImage blits the given image to the screen so that it's upper left
//	corner will be located at (i_DX1, i_DY1).
//----------------------------------------------------------------------------
void DrawImage(	vkdImage* i_pDestSurface,
			   int i_ScreenWidth, int i_ScreenHeight,
			    int i_DX1,
				int i_DY1,
				const g2dImage& i_Image)
{
	DrawImage(i_pDestSurface, i_ScreenWidth, i_ScreenHeight,
		i_DX1, i_DY1, 0, 0, i_Image.GetWidth(), i_Image.GetHeight(), i_Image);
}

//----------------------------------------------------------------------------
//	This DrawImage blits a rectangular section of the given image to the
//	screen.  The rectangle of the source image is defined by
//	(i_SX1, i_SY1) - (i_SX2, i_SY2).  The point (i_SX1, i_SY1) will be
//	located at (i_DX1, i_DY1).
//----------------------------------------------------------------------------
void DrawImage(	vkdImage* i_pDestSurface,
			   int i_ScreenWidth, int i_ScreenHeight,
			    int i_DX1,
				int i_DY1,
				int i_SX1,
				int i_SY1,
				int i_SX2,
				int i_SY2,
				const g2dImage& i_Image)
{
	const g2dImageVk *src_image = dynamic_cast<const g2dImageVk*>(&i_Image);
	DBG_ASSERT(src_image, "Image needs to be a Vulkan Image");
	const uint8_t* source_surface = src_image ? src_image->Pixels() : NULL;
	DBG_ASSERT(source_surface, "Image has no surface");
	if (!source_surface || !i_pDestSurface)
		return;

	int screen_width = i_ScreenWidth;
	int screen_height = i_ScreenHeight;

	int source_width = i_SX2 - i_SX1;
	int source_height = i_SY2 - i_SY1;

	// do some clipping and rejection
	if( i_DX1 < 0 )
	{
		if( i_DX1 <= -source_width )
			return; // off the left edge

		i_SX1 -= i_DX1;
		source_width += i_DX1;
		i_DX1 = 0;
	}

	if ( i_DY1 < 0 )
	{
		if( i_DY1 <= -source_height )
			return; // off the top edge

		i_SY1 -= i_DY1;
		source_height += i_DY1;
		i_DY1 = 0;
	}

	int width_over_limit = i_DX1 + source_width - screen_width;
	if ( width_over_limit > 0 )
	{
		if( i_DX1 >= screen_width )
			return;	// off the right edge

		i_SX2 -= width_over_limit;
		source_width -= width_over_limit;
	}

	int height_over_limit = i_DY1 + source_height - screen_height;
	if ( height_over_limit > 0 )
	{
		if( i_DY1 >= screen_height )
			return;	// off the right edge

		i_SY2 -= height_over_limit;
		source_height -= height_over_limit;
	}

	DBG_ASSERT(source_width > 0, "Invalid image area");
	DBG_ASSERT(source_height > 0, "Invalid image area");

	// CopySubresourceRegion: the formats must match (an RGBA8 image may go
	// to a BGRA8 swap chain, red and blue swapped)
	const VkFormat source_format = vkdFormatFromDxgi(g2dVkGlobal::DxgiFormatFromPFD(src_image->GetPixelFormat()));
	const VkFormat dest_format = i_pDestSurface->Format();
	const bool swap_red_blue = source_format == VK_FORMAT_R8G8B8A8_UNORM && dest_format == VK_FORMAT_B8G8R8A8_UNORM;
	if (source_format != dest_format && !swap_red_blue)
	{
		DBG_ERROR("g2dWindowDrawUtilVk::DrawImage: the image's format (" << source_format
				  << ") isn't the surface's (" << dest_format << ")");
		return;
	}
	const uint32_t pixel_bytes = vkdFormatSize(dest_format);
	if (source_width <= 0 || source_height <= 0 || pixel_bytes == 0)
		return;

	vkdContext& context = vkdGlobal::Context();
	std::lock_guard<std::recursive_mutex> lock(vkdGlobal::Device().StreamLock());
	const size_t row_bytes = (size_t)source_width * pixel_bytes;
	vkdRingBuffer::Allocation staging = context.Allocate(row_bytes * source_height, 16);
	const int source_stride = src_image->GetStride();
	for (int y = 0; y < source_height; y++)
	{
		const uint8_t* src = source_surface + (size_t)(i_SY1 + y) * source_stride + (size_t)i_SX1 * pixel_bytes;
		uint8_t* dst = static_cast<uint8_t*>(staging.m_pCpu) + y * row_bytes;
		memcpy(dst, src, row_bytes);
		if (swap_red_blue)
		{
			for (size_t i = 0; i < row_bytes; i += 4)
				std::swap(dst[i], dst[i + 2]);
		}
	}

	VkBufferImageCopy region = {};
	region.bufferOffset = staging.m_Offset;
	region.imageSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 };
	region.imageOffset = { i_DX1, i_DY1, 0 };
	region.imageExtent = { (uint32_t)source_width, (uint32_t)source_height, 1 };
	vkCmdCopyBufferToImage(context.Commands(), staging.m_Buffer, i_pDestSurface->Image(), VK_IMAGE_LAYOUT_GENERAL, 1, &region);
}

}
}
