/*****************************************************************************
**  g2dImageVk.cpp
**
**      See g2dImageVk.hpp. GraphicsDX11's g2dImageDX11.cpp and
**	g2dImageDrawUtilDX11.cpp; the D3D calls (CreateTexture2D, Map,
**	CopyResource, CopySubresourceRegion) become operations on the CPU
**	pixels.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GVK_G2DIMAGEVK_HPP
#include "GraphicsVulkan/G2d/g2dImageVk.hpp"
#endif
#ifndef GVK_G2DSURFACEUTILVK_HPP
#include "GraphicsVulkan/G2d/private/g2dSurfaceUtilVk.hpp"
#endif
#ifndef GVK_G2DVKGLOBAL_HPP
#include "GraphicsVulkan/G2d/g2dVkGlobal.hpp"
#endif

#ifndef DBG_MSG_HPP
#include "Core/Dbg/dbgMsg.hpp"
#endif
#ifndef MA_FLOATRGBA_HPP
#include "Core/Ma/maFloatRGBA.hpp"
#endif
#ifndef MA_INTRECT_HPP
#include "Core/Ma/maIntRect.hpp"
#endif
#ifndef MA_SAMPLING_HPP
#include "Core/Ma/maSampling.hpp"
#endif
#ifndef MA_VECTOR3D_HPP
#include "Core/Ma/maVector3d.hpp"
#endif
#ifndef G2D_EXCEPTIONX_HPP
#include "Graphics/G2d/g2dExceptionX.hpp"
#endif

#include <algorithm>
#include <cmath>
#include <cstring>

namespace gvk
{
namespace
{
	//------------------------------------------------------------------------
	//	D3D11's CopySubresourceRegion of a 2D box between two images: the
	//	formats must match and the box must lie in the source, or D3D11
	//	copies nothing (its debug layer reports it)
	//------------------------------------------------------------------------
	void copy_region(g2dImageVk& o_Dest, int i_DX, int i_DY, const g2dImageVk& i_Src,
					 int i_Left, int i_Top, int i_Right, int i_Bottom)
	{
		if (o_Dest.GetDxgiFormat() != i_Src.GetDxgiFormat())
		{
			DBG_WARNING("g2dImageVk: copy between different formats (" << i_Src.GetDxgiFormat() << " to "
						<< o_Dest.GetDxgiFormat() << ") skipped, as D3D11 does");
			return;
		}
		if (i_Left < 0 || i_Top < 0 || i_Right > i_Src.GetWidth() || i_Bottom > i_Src.GetHeight() ||
			i_Left >= i_Right || i_Top >= i_Bottom ||
			i_DX < 0 || i_DY < 0 || i_DX + (i_Right - i_Left) > o_Dest.GetWidth() || i_DY + (i_Bottom - i_Top) > o_Dest.GetHeight())
		{
			DBG_WARNING("g2dImageVk: copy box outside an image skipped, as D3D11 does");
			return;
		}
		const int pixel_bytes = (int)g2dVkGlobal::BitsPerPixel(i_Src.GetDxgiFormat()) / 8;
		const size_t row_bytes = (size_t)(i_Right - i_Left) * pixel_bytes;
		uint8_t* dest = static_cast<uint8_t*>(o_Dest.Lock());
		const uint8_t* src = i_Src.Pixels();
		for (int y = 0; y < i_Bottom - i_Top; y++)
		{
			memcpy(dest + (size_t)(i_DY + y) * o_Dest.GetStride() + (size_t)i_DX * pixel_bytes,
				   src + (size_t)(i_Top + y) * i_Src.GetStride() + (size_t)i_Left * pixel_bytes, row_bytes);
		}
		o_Dest.Release();
	}

	//------------------------------------------------------------------------
	//	g2dImageDrawUtilDX11::DrawImage: blits a rectangular section of the
	//	source image to a location on the destination image. The rectangle
	//	of the source image is defined by (i_SX1, i_SY1) - (i_SX2, i_SY2).
	//	The point (i_SX1, i_SY1) will be located at (i_DX1, i_DY1).
	//------------------------------------------------------------------------
	void draw_image(g2dImageVk& o_DestSurface,
					int i_DX1,
					int i_DY1,
					int i_DestWidth,
					int i_DestHeight,
					int i_SX1,
					int i_SY1,
					int i_SX2,
					int i_SY2,
					const g2dImageVk& i_SrcSurface)
	{
		DBG_ASSERT(o_DestSurface.Pixels(), "Image has no surface");
		DBG_ASSERT(i_SrcSurface.Pixels(), "Image has no surface");

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

		int width_over_limit = i_DX1 + source_width - i_DestWidth;
		if ( width_over_limit > 0 )
		{
			if( i_DX1 >= i_DestWidth )
				return;	// off the right edge

			i_SX2 -= width_over_limit;
			source_width -= width_over_limit;
		}

		int height_over_limit = i_DY1 + source_height - i_DestHeight;
		if ( height_over_limit > 0 )
		{
			if( i_DY1 >= i_DestHeight )
				return;	// off the right edge

			i_SY2 -= height_over_limit;
			source_height -= height_over_limit;
		}

		DBG_ASSERT(source_width > 0, "Invalid image area");
		DBG_ASSERT(source_height > 0, "Invalid image area");

		copy_region(o_DestSurface, i_DX1, i_DY1, i_SrcSurface, i_SX1, i_SY1, i_SX2, i_SY2);
	}
}	// end of namespace

	//--------------------------------------------------------------------
	//	This constructor makes an "empty" surface
	//--------------------------------------------------------------------
	g2dImageVk::g2dImageVk()
	{
	}

	//--------------------------------------------------------------------
	//	This constructor makes a surface with the given dimensions
	//	and pixel format.
	//--------------------------------------------------------------------
	g2dImageVk::g2dImageVk(int i_Width, int i_Height, const g2dPFD& i_PFD, g2dImage::CreateLoc i_CreateLoc)
	{
		Make(i_Width, i_Height, i_PFD, i_CreateLoc);
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	g2dImageVk::~g2dImageVk()
	{
	}

	void g2dImageVk::allocate(int i_Width, int i_Height, uint32_t i_DxgiFormat)
	{
		const int pixel_bytes = (int)g2dVkGlobal::BitsPerPixel(i_DxgiFormat) / 8;
		if (i_DxgiFormat == g2dVkGlobal::e_DXGI_UNKNOWN || pixel_bytes <= 0 || i_Width <= 0 || i_Height <= 0)
		{
			// DX11's CreateTexture2D fails: the constructor throws g2dGeneralX
			DBG_ERROR("Error allocating surface (w" << i_Width << "-h" << i_Height << "-f" << i_DxgiFormat << ")");
			throw g2dGeneralX();
		}
		m_Format = i_DxgiFormat;
		m_Width = i_Width;
		m_Height = i_Height;
		m_Stride = i_Width * pixel_bytes;
		m_Pixels.assign((size_t)m_Stride * m_Height, 0);
	}

	//--------------------------------------------------------------------
	//	Make makes the surface into one with the given dimensions and
	//	pixel format.  If the pixel format is not supported it will
	//	throw a g2dGeneralX, and leave the old surface intact.
	//--------------------------------------------------------------------
	void g2dImageVk::Make(int i_Width, int i_Height, const g2dPFD& i_PFD, g2dImage::CreateLoc /*i_CreateLoc*/)
	{
		allocate(i_Width, i_Height, g2dVkGlobal::DxgiFormatFromPFD(i_PFD));

		// if we get here, the surface creation succeeded, and we can go
		// ahead and copy the values
		m_PixelFormat = i_PFD;
	}

	void g2dImageVk::MakeFromDxgiFormat(int i_Width, int i_Height, uint32_t i_DxgiFormat)
	{
		allocate(i_Width, i_Height, i_DxgiFormat);
		g2dVkGlobal::PFDFromDxgiFormat(i_DxgiFormat, m_PixelFormat);
	}

	//--------------------------------------------------------------------
	//	GetPixelFormat returns the current pixel format of the surface.
	//--------------------------------------------------------------------
	const g2dPFD& g2dImageVk::GetPixelFormat() const
	{
		return m_PixelFormat;
	}

	//--------------------------------------------------------------------
	//	The width of the surface, in pixels (not bytes!).
	//	Note that this is not the same as the stride see below.
	//--------------------------------------------------------------------
	int g2dImageVk::GetWidth() const
	{
		return m_Width;
	}

	//--------------------------------------------------------------------
	//	The height of the surface.
	//--------------------------------------------------------------------
	int g2dImageVk::GetHeight() const
	{
		return m_Height;
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	int g2dImageVk::GetStride() const
	{
		return m_Stride;
	}

	//--------------------------------------------------------------------
	//	Lock returns a pointer to the surface memory, allowing it to be
	//	modified directly.  Call Release() when you are done reading or
	//	modifying the surface.
	//--------------------------------------------------------------------
	void* g2dImageVk::Lock()
	{
		return m_Pixels.empty() ? NULL : m_Pixels.data();
	}

	//--------------------------------------------------------------------
	//	Release should be called when you are finished with Lock().
	//--------------------------------------------------------------------
	void g2dImageVk::Release()
	{
	}

	//------------------------------------------------------------------------
	//	This CopyImage makes a possibly resized copy of the original g2dImage
	//------------------------------------------------------------------------
	void g2dImageVk::CopyImage( const g2dImage& i_SrcImage,
		const maFilter* i_Filter /*= NULL*/,
		float i_FilterWidth /*= 1*/, float i_FilterHeight /*= 1*/,
		int i_SampleRateX /*= 1*/, int i_SampleRateY /*= 1*/)
	{
		const g2dImageVk *src_image = dynamic_cast<const g2dImageVk*>(&i_SrcImage);
		DBG_ASSERT(src_image, "Image needs to be a Vulkan Image");
		if (!src_image)
			return;

		if (i_Filter == NULL)
		{
			// assert same size and format?
			DBG_ASSERT((src_image->GetWidth() == this->GetWidth()) && (src_image->GetHeight() == this->GetHeight()), "CopyImage images must match in size");

			// CopyResource
			if (src_image->GetWidth() == this->GetWidth() && src_image->GetHeight() == this->GetHeight())
			{
				copy_region(*this, 0, 0, *src_image, 0, 0, src_image->GetWidth(), src_image->GetHeight());
			}
		}
		else
		{
			int srcW = i_SrcImage.GetWidth();
			int srcH = i_SrcImage.GetHeight();
			DBG_ASSERT(srcW == this->GetWidth() * i_SampleRateX, "downsampling inconsistent input widths");
			DBG_ASSERT(srcH == this->GetHeight() * i_SampleRateY, "downsampling inconsistent input heights");

			// make sure there's at least 1 sample
			int filterSamplesX = std::max((int)ceil(i_FilterWidth * i_SampleRateX), 1);
			int filterSamplesY = std::max((int)ceil(i_FilterHeight * i_SampleRateY), 1);

			std::vector<maVector3d> samples;
			// returns samples positions centered on 0.5, spanning half the filterwidth in each direction
			maSampling::GetSamples2D(filterSamplesX, filterSamplesY, samples, 0, i_FilterWidth, i_FilterHeight);
			maSampling::WeightSamples(samples, i_Filter, i_FilterWidth, i_FilterHeight);

			maFloatRGBA sample;
			float r,g,b,a;
			// for each pixel in dest image, apply filter region over pixels in src image.
			SurfaceLocker dst_surf_locker(this, true);
			SurfaceLocker src_surf_locker(src_image);
			if (src_surf_locker.IsLocked() && dst_surf_locker.IsLocked())
			{
				SurfaceIterator& src_surf_it = *(src_surf_locker.GetIterator());
				SurfaceIterator& dst_surf_it = *(dst_surf_locker.GetIterator());

				// loop over every pixel of dest image (this)
				for (int i = 0; i < this->GetHeight(); i++)
				{
					dst_surf_it.SetPosition(i, 0);
					for (int j = 0; j < this->GetWidth(); j++)
					{
						// init to 0
						maFloatRGBA finalPixel;

	// filter is always centered over a destination pixel.

	// 3 samples per pixel (ssx = 3)
	// filter width = 3 pixels (radius=1.5):
	// |***|***|***|***|***|
	//     {-----=-----}
	// note ctr of filter is right on a sample.

	// 4 samples per pixel (ssx = 4)
	// filter width = 3 pixels (radius=1.5):
	// |****|****|****|****|****|
	//      {------==------}
	// note ctr of filter is between samples!

						// accumulate 2d filtered samples from src image:
						// center the filter over the pixel we want, based on sampling rate...
						// We will take filterSamplesX * filterSamplesY actual samples to compute the final pixel.

						// must account for odd and even numbers!!

						// each dst pixel spans sampleratex * sampleratey src samples.
						int filterCenterColumn = j*i_SampleRateX + (i_SampleRateX/2);
						int filterCenterRow = i*i_SampleRateY + (i_SampleRateY/2);

						// edges lack filter support!  positions will be out of bounds for some samples.
						int filterStartColumn = filterCenterColumn - (filterSamplesX/2) ;
						int filterStartRow = filterCenterRow - (filterSamplesY/2);

						for (int k = 0; k < filterSamplesY; k++)
						{
							for (int l = 0; l < filterSamplesX; l++)
							{
								src_surf_it.SetPosition(filterStartRow + k,
									filterStartColumn + l);

								src_surf_it.GetValue(r,g,b,a);
								sample.Set(r,g,b,a);
								// weight
								sample *= samples[l + filterSamplesX*k].m_Z;
								// add to total
								finalPixel += sample;
							}
						}

						// now set the finalPixel into the dest image:
						dst_surf_it.SetValue(finalPixel.GetRed(), finalPixel.GetGreen(), finalPixel.GetBlue(), finalPixel.GetAlpha());

						dst_surf_it.IncCol();
					}
				}
			}
		}
	}

	//------------------------------------------------------------------------
	//	This DrawImage blits the source image to this image.
	//	The source image upper left corner will be located at (i_DX1, i_DY1)
	//	on the destination image.
	//------------------------------------------------------------------------
	void g2dImageVk::DrawImage(	int i_DX1,
								int i_DY1,
								const g2dImage& i_SrcImage)
	{
		const g2dImageVk *src_image = dynamic_cast<const g2dImageVk*>(&i_SrcImage);
		DBG_ASSERT(src_image, "Image needs to be a Vulkan Image");
		if (!src_image)
			return;

		draw_image(	*this,
					i_DX1,
					i_DY1,
					this->GetWidth(),
					this->GetHeight(),
					0,
					0,
					i_SrcImage.GetWidth(),
					i_SrcImage.GetHeight(),
					*src_image);
	}

	//------------------------------------------------------------------------
	//	This DrawImage blits a rectangular section of the source image to a
	//	location on this image.  The rectangle of the source image
	//	is defined by (i_SX1, i_SY1) - (i_SX2, i_SY2).  The point
	//	(i_SX1, i_SY1) will be located at (i_DX1, i_DY1).
	//------------------------------------------------------------------------
	void g2dImageVk::DrawImage(	int i_DX1,
								int i_DY1,
								int i_SX1,
								int i_SY1,
								int i_SX2,
								int i_SY2,
								const g2dImage& i_SrcImage)
	{
		const g2dImageVk *src_image = dynamic_cast<const g2dImageVk*>(&i_SrcImage);
		DBG_ASSERT(src_image, "Image needs to be a Vulkan Image");
		if (!src_image)
			return;

		draw_image(	*this,
					i_DX1,
					i_DY1,
					this->GetWidth(),
					this->GetHeight(),
					i_SX1,
					i_SY1,
					i_SX2,
					i_SY2,
					*src_image);
	}

	//----------------------------------------------------------------------------
	// this call allocates a big block of mem!!!!!
	// caller must delete [] o_pBuffer when done using it!!!!
	//----------------------------------------------------------------------------
	void g2dImageVk::GetPixels(uint8_t** o_pBuffer,
								int* o_bufSize,
								int* o_width,
								int* o_height,
								int* o_bpp,
								int i_sampling /*= 1*/,
								bool i_bGreyscale /*= false*/,
								const maIntRect* i_pRect /*= NULL*/)
	{
		g2dSurfaceUtilVk::GetSurfacePixels(this, o_pBuffer,
			o_bufSize, o_width, o_height, o_bpp, i_sampling, i_bGreyscale, i_pRect);
	}

	void g2dImageVk::GetPixelsTGA(uint8_t **o_pBuffer,
		int *o_bufSize,
		int *o_width,
		int *o_height,
		int *o_bpp,
		int	i_Sampling /*= 1*/,
		const std::string& i_CompressionCode /*= std::string("None")*/,
		const maIntRect* i_pRect /*= NULL*/)
	{
		g2dSurfaceUtilVk::GetSurfacePixelsTGA(this, o_pBuffer,
			o_bufSize, o_width, o_height, o_bpp, i_Sampling, i_CompressionCode, i_pRect);
	}

	//----------------------------------------------------------------------------
	// this call allocates a big block of mem!!!!!
	// caller must delete [] o_pBuffer when done using it!!!!
	//----------------------------------------------------------------------------
	void g2dImageVk::GetPixels(g2dPixelR16G16B16A16** o_pBuffer,
								int* o_width,
								int* o_height)
	{
		g2dSurfaceUtilVk::GetSurfacePixels(this, o_pBuffer, o_width, o_height);
	}

	//----------------------------------------------------------------------------
	// this call allocates a big block of mem!!!!!
	// caller must delete [] o_pBuffer when done using it!!!!
	//----------------------------------------------------------------------------
	void g2dImageVk::GetPixels(g2dPixelR8G8B8A8** o_pBuffer,
								int* o_width,
								int* o_height)
	{
		g2dSurfaceUtilVk::GetSurfacePixels(this, o_pBuffer, o_width, o_height);
	}

	//----------------------------------------------------------------------------
	// this call allocates a big block of mem!!!!!
	// caller must delete [] o_pBuffer when done using it!!!!
	//----------------------------------------------------------------------------
	void g2dImageVk::GetPixels(g2dPixelRGBA32F** o_pBuffer,
		int* o_width,
		int* o_height)
	{
		g2dSurfaceUtilVk::GetSurfacePixels(this, o_pBuffer, o_width, o_height);
	}

	// fill in buffer as provided
	void g2dImageVk::GetPixels(g2dPixelRGBA32F* o_pBuffer,
		int i_Width,
		int i_Height)
	{
		DBG_ASSERT(i_Width <= m_Width, "GetPixels: bad image dimensions");
		DBG_ASSERT(i_Height <= m_Height, "GetPixels: bad image dimensions");
		g2dSurfaceUtilVk::GetSurfacePixels(this, o_pBuffer, i_Width, i_Height);
	}

	//----------------------------------------------------------------------------
	// Get the color of the pixel at an individual position on the image
	//----------------------------------------------------------------------------
	void g2dImageVk::GetPixelColor(int i_X,
									int i_Y,
									uint8_t &o_Red,
									uint8_t &o_Green,
									uint8_t &o_Blue,
									uint8_t &o_Alpha)
	{
		g2dSurfaceUtilVk::GetPixelColor(this, i_X, i_Y, o_Red, o_Green, o_Blue, o_Alpha);
	}

	//----------------------------------------------------------------------------
	// Get the color of the pixel at an individual position on the image
	//----------------------------------------------------------------------------
	void g2dImageVk::GetPixelColor(int i_X,
									int i_Y,
									float &o_Red,
									float &o_Green,
									float &o_Blue,
									float &o_Alpha)
	{
		g2dSurfaceUtilVk::GetPixelColor(this, i_X, i_Y, o_Red, o_Green, o_Blue, o_Alpha);
	}

	//----------------------------------------------------------------------------
	// SwapChannels() - assumes 4 bytes per pixel and non-floats
	//----------------------------------------------------------------------------
	void g2dImageVk::SwapChannels( int i_ChannelOne, int i_ChannelTwo )
	{
		int numBytesPerPixel = 4;

		// Access bits in pImg
		void* imgBits = this->Lock();
		uint8_t* pImgBits = (uint8_t*)imgBits;
		if (!pImgBits)
			return;

		//	DX11 writes past a row of a format under 4 bytes a pixel, into
		//	its mapped row padding; here the pixels outside the image's
		//	memory are left out
		const size_t size = m_Pixels.size();

		//Loop through all the pixels
		int i, j;
		for (i = 0 ; i < m_Height ; i++)
		{
			for (j = 0 ; j < m_Width ; j++)
			{
				int idx = (j*numBytesPerPixel) + (i*this->GetStride());
				if ((size_t)(idx + std::max(i_ChannelOne, i_ChannelTwo)) >= size || i_ChannelOne < 0 || i_ChannelTwo < 0)
					continue;
				pImgBits[idx+i_ChannelOne] ^= pImgBits[idx+i_ChannelTwo];
				pImgBits[idx+i_ChannelTwo] ^= pImgBits[idx+i_ChannelOne];
				pImgBits[idx+i_ChannelOne] ^= pImgBits[idx+i_ChannelTwo];
			}
		}

		// Unlock surfaces
		this->Release();
	}
}
