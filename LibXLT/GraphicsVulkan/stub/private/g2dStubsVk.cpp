/*****************************************************************************
**  g2dStubsVk.cpp
**
**      See g2dStubsVk.hpp.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GVK_G2DSTUBSVK_HPP
#include "GraphicsVulkan/stub/g2dStubsVk.hpp"
#endif
#ifndef GVK_VKDCHECK_HPP
#include "GraphicsVulkan/vkd/vkdCheck.hpp"
#endif

#ifndef DBG_MSG_HPP
#include "Core/Dbg/dbgMsg.hpp"
#endif
#ifndef MA_FUNCTIONS_HPP
#include "Core/Ma/maFunctions.hpp"
#endif

#include <algorithm>
#include <cstring>

namespace gvk
{
	//------------------------------------------------------------------------
	//	g2dResourceCounterVk
	//------------------------------------------------------------------------
	float g2dResourceCounterVk::GetTotalVideoMemory()			{ GVK_NOT_YET("g2dResourceCounterVk"); return 0; }
	float g2dResourceCounterVk::GetTotalSceneTextureMemory()	{ GVK_NOT_YET("g2dResourceCounterVk"); return 0; }
	int g2dResourceCounterVk::GetNumSceneTextures()				{ GVK_NOT_YET("g2dResourceCounterVk"); return 0; }
	float g2dResourceCounterVk::GetTotalFramebufferMemory()		{ GVK_NOT_YET("g2dResourceCounterVk"); return 0; }
	int g2dResourceCounterVk::GetNumFramebufferTextures()		{ GVK_NOT_YET("g2dResourceCounterVk"); return 0; }
	float g2dResourceCounterVk::GetTotalShadowMapMemory()		{ GVK_NOT_YET("g2dResourceCounterVk"); return 0; }
	int g2dResourceCounterVk::GetNumShadowMapTextures()			{ GVK_NOT_YET("g2dResourceCounterVk"); return 0; }
	float g2dResourceCounterVk::GetTotalVertexBufferMemory()	{ GVK_NOT_YET("g2dResourceCounterVk"); return 0; }
	int g2dResourceCounterVk::GetNumVertexBuffers()				{ GVK_NOT_YET("g2dResourceCounterVk"); return 0; }
	float g2dResourceCounterVk::GetTotalIndexBufferMemory()		{ GVK_NOT_YET("g2dResourceCounterVk"); return 0; }
	int g2dResourceCounterVk::GetNumIndexBuffers()				{ GVK_NOT_YET("g2dResourceCounterVk"); return 0; }

	//------------------------------------------------------------------------
	//	g2dImageVk - a CPU image
	//------------------------------------------------------------------------
	int g2dImageVk::bytes_per_pixel() const
	{
		return std::max(1, m_PFD.BitsPerPixel() / 8);
	}

	void g2dImageVk::Make(int i_Width, int i_Height, const g2dPFD& i_PFD, CreateLoc /*i_Loc*/)
	{
		m_PFD = i_PFD;
		m_Width = std::max(0, i_Width);
		m_Height = std::max(0, i_Height);
		m_Pixels.assign(static_cast<size_t>(m_Width) * m_Height * bytes_per_pixel(), 0);
	}

	const g2dPFD& g2dImageVk::GetPixelFormat() const	{ return m_PFD; }
	int g2dImageVk::GetWidth() const					{ return m_Width; }
	int g2dImageVk::GetHeight() const					{ return m_Height; }
	int g2dImageVk::GetStride() const					{ return m_Width * bytes_per_pixel(); }
	void* g2dImageVk::Lock()							{ return m_Pixels.empty() ? NULL : m_Pixels.data(); }
	void g2dImageVk::Release()							{}

	void g2dImageVk::CopyImage(const g2dImage& i_SrcImage, const maFilter*, float, float, int, int)
	{
		const g2dImageVk* source = dynamic_cast<const g2dImageVk*>(&i_SrcImage);
		if (source && source->m_Width == m_Width && source->m_Height == m_Height &&
			source->bytes_per_pixel() == bytes_per_pixel())
		{
			m_Pixels = source->m_Pixels;
			return;
		}
		GVK_NOT_YET("g2dImageVk::CopyImage (resampling or format conversion)");
	}

	void g2dImageVk::DrawImage(int, int, const g2dImage&)						{ GVK_NOT_YET("g2dImageVk::DrawImage"); }
	void g2dImageVk::DrawImage(int, int, int, int, int, int, const g2dImage&)	{ GVK_NOT_YET("g2dImageVk::DrawImage"); }

	void g2dImageVk::GetPixels(uint8_t** o_pBuffer, int* o_bufSize, int* o_width, int* o_height, int* o_bpp,
							   int, bool, const maIntRect*)
	{
		GVK_NOT_YET("g2dImageVk::GetPixels (sampling, greyscale, rectangles)");
		*o_pBuffer = new uint8_t[m_Pixels.size()];
		memcpy(*o_pBuffer, m_Pixels.data(), m_Pixels.size());
		*o_bufSize = static_cast<int>(m_Pixels.size());
		*o_width = m_Width;
		*o_height = m_Height;
		*o_bpp = m_PFD.BitsPerPixel();
	}

	void g2dImageVk::GetPixelsTGA(uint8_t** o_pBuffer, int* o_bufSize, int* o_width, int* o_height, int* o_bpp,
								  int, const std::string&, const maIntRect*)
	{
		GVK_NOT_YET("g2dImageVk::GetPixelsTGA");
		*o_pBuffer = NULL;
		*o_bufSize = 0;
		*o_width = m_Width;
		*o_height = m_Height;
		*o_bpp = m_PFD.BitsPerPixel();
	}

	void g2dImageVk::GetPixels(g2dPixelR16G16B16A16** o_pBuffer, int* o_width, int* o_height)
	{
		GVK_NOT_YET("g2dImageVk::GetPixels (R16G16B16A16)");
		*o_pBuffer = new g2dPixelR16G16B16A16[static_cast<size_t>(m_Width) * m_Height]();
		*o_width = m_Width;
		*o_height = m_Height;
	}

	//	8-bit RGBA from a 32-bit image; red and blue swapped for BGRA layouts.
	void g2dImageVk::GetPixels(g2dPixelR8G8B8A8** o_pBuffer, int* o_width, int* o_height)
	{
		const size_t count = static_cast<size_t>(m_Width) * m_Height;
		*o_pBuffer = new g2dPixelR8G8B8A8[count]();
		*o_width = m_Width;
		*o_height = m_Height;
		if (bytes_per_pixel() != 4)
		{
			GVK_NOT_YET("g2dImageVk::GetPixels (R8G8B8A8 from a non-32-bit image)");
			return;
		}
		memcpy(*o_pBuffer, m_Pixels.data(), count * 4);
		if (m_PFD.FirstRedBit() == 16)
		{
			uint8_t* p = reinterpret_cast<uint8_t*>(*o_pBuffer);
			for (size_t i = 0; i < count; ++i)
			{
				std::swap(p[i * 4], p[i * 4 + 2]);
			}
		}
	}

	//	GraphicsDX11's GetSurfacePixels (g2dDX11SurfaceUtil's copyPixels and
	//	SurfaceIterator::GetValue): bottom row first; 8-bit RGBA / 255, 16- and
	//	32-bit float with 1, 2 or 4 channels (green and blue 0, alpha 1 where
	//	missing). Another format gives zeros, as an iterator DX11 lacks.
	bool g2dImageVk::get_float_pixel(int i_X, int i_Y, g2dPixelRGBA32F& o_Pixel) const
	{
		const int nc = m_PFD.GetNumChannels();
		const uint8_t* data = m_Pixels.data() + (size_t)i_Y * GetStride() + (size_t)i_X * bytes_per_pixel();
		float c[4] = { 0, 0, 0, 1 };
		switch (m_PFD.GetBitsPerChannel())
		{
		case 8:
			if (nc != 4 || m_PFD.IsFloat())
				return false;
			for (int i = 0; i < 4; i++)
				c[i] = data[i] / 255.0f;
			break;
		case 16:
			if (!m_PFD.IsFloat() || (nc != 1 && nc != 2 && nc != 4))
				return false;
			for (int i = 0; i < nc; i++)
				c[i] = maFunctions::HalfToFloat(reinterpret_cast<const uint16_t*>(data)[i]);
			break;
		case 32:
			if (!m_PFD.IsFloat() || (nc != 1 && nc != 2 && nc != 4))
				return false;
			memcpy(c, data, nc * sizeof(float));
			break;
		default:
			return false;
		}
		o_Pixel.r = c[0];
		o_Pixel.g = c[1];
		o_Pixel.b = c[2];
		o_Pixel.a = c[3];
		return true;
	}

	//	this call allocates a big block of mem; the caller deletes [] it
	void g2dImageVk::GetPixels(g2dPixelRGBA32F** o_pBuffer, int* o_width, int* o_height)
	{
		*o_pBuffer = new g2dPixelRGBA32F[static_cast<size_t>(m_Width) * m_Height]();
		*o_width = m_Width;
		*o_height = m_Height;
		GetPixels(*o_pBuffer, m_Width, m_Height);
	}

	void g2dImageVk::GetPixels(g2dPixelRGBA32F* o_pBuffer, int i_Width, int i_Height)
	{
		DBG_ASSERT(i_Width <= m_Width, "GetPixels: bad image dimensions");
		DBG_ASSERT(i_Height <= m_Height, "GetPixels: bad image dimensions");
		g2dPixelRGBA32F* ptr = o_pBuffer;
		for (int r = i_Height - 1; r >= 0; r--)
		{
			for (int c = 0; c < i_Width; c++)
			{
				if (!get_float_pixel(c, r, *ptr))
				{
					GVK_NOT_YET("g2dImageVk::GetPixels (RGBA32F from this pixel format)");
					memset(o_pBuffer, 0, sizeof(g2dPixelRGBA32F) * static_cast<size_t>(i_Width) * i_Height);
					return;
				}
				ptr++;
			}
		}
	}

	void g2dImageVk::SwapChannels(int i_ChannelOne, int i_ChannelTwo)
	{
		const int bpp = bytes_per_pixel();
		if (i_ChannelOne < 0 || i_ChannelTwo < 0 || i_ChannelOne >= bpp || i_ChannelTwo >= bpp)
		{
			return;
		}
		for (size_t i = 0; i < m_Pixels.size(); i += bpp)
		{
			std::swap(m_Pixels[i + i_ChannelOne], m_Pixels[i + i_ChannelTwo]);
		}
	}

	//------------------------------------------------------------------------
	//	g2dImageCreateVk
	//------------------------------------------------------------------------
	g2dImage* g2dImageCreateVk::Make(int i_Width, int i_Height, const g2dPFD& i_PFD, g2dImage::CreateLoc i_Loc)
	{
		g2dImageVk* image = new g2dImageVk;
		image->Make(i_Width, i_Height, i_PFD, i_Loc);
		return image;
	}

	//	Not yet: a 1x1 white image stands in for the file.
	g2dImage* g2dImageCreateVk::Load(const fsLocator& /*i_Locator*/, g2dImage::CreateLoc i_Loc)
	{
		GVK_NOT_YET("g2dImageCreateVk::Load");
		g2dImageVk* image = new g2dImageVk;
		image->Make(1, 1, g2dPFD(0, 8, 8, 8, 16, 8, 24, 8, 32), i_Loc);
		memset(image->Lock(), 0xFF, 4);
		return image;
	}

}
