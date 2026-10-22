/*****************************************************************************
**  matMipFilter.hpp
**
**      The mip generation and size reduction GraphicsDX11's texture loaders
**	do on the CPU with DirectXTex, ported (DirectXTexMipmaps.cpp,
**	DirectXTexResize.cpp, filters.h and the scanline conversions of
**	DirectXTexConvert.cpp; MIT licence, Copyright (c) Microsoft
**	Corporation; the May 2026 release GraphicsDX11 links):
**
**	- matGenerateMips with e_Triangle is GenerateMipMaps(TEX_FILTER_TRIANGLE)
**	  (the WIC formats' loads); e_Box is GenerateMipMaps(TEX_FILTER_FANT)
**	  (the TGA and HDR loads): DirectXTex hands 8-bit formats to WIC's
**	  Fant scaler, emulated here as an area average of the base image, and
**	  box-filters the rest itself, which needs power-of-two sizes
**	- matReduce is Resize(width >> w, height >> h, TEX_FILTER_DEFAULT): WIC's
**	  Fant scaler for 8-bit formats, else DirectXTex's box filter for an
**	  exact halving and its linear filter otherwise
**	- sRGB formats are filtered in linear space, as DirectXTex does
**
**	Formats: R8G8B8A8 and B8G8R8A8/X8 (and their _SRGB), R8, R16,
**	R16G16B16A16 (UNORM and FLOAT), R32G32B32A32_FLOAT, R16_FLOAT,
**	R32_FLOAT. Others fail (return false), as do compressed images.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GVK_MATMIPFILTER_HPP
#error matMipFilter.hpp multiply included
#endif
#define GVK_MATMIPFILTER_HPP

#include <cstdint>

namespace gvk
{
	struct matDecodedImage;

	enum class matMipFilter { e_Triangle, e_Box };

	//	A chain of i_Levels mips (0 = down to 1x1) made from mip 0 of i_Level0
	bool matGenerateMips(const matDecodedImage& i_Level0, uint32_t i_Levels, matDecodedImage& o_Chain,
						 matMipFilter i_Filter = matMipFilter::e_Triangle);

	//	Mip 0 of i_Image resized to width >> i_WidthReduce, height >> i_HeightReduce
	bool matReduce(const matDecodedImage& i_Image, int i_WidthReduce, int i_HeightReduce, matDecodedImage& o_Image);
}
