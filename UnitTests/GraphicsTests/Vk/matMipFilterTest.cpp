/*****************************************************************************
**  matMipFilterTest.cpp
**
**      matMipFilter, the port of DirectXTex's mip generation and Resize,
**      against DirectXTex itself: the triangle filter GraphicsDX11's WIC
**      loads use (sRGB images filtered in linear space), the box filter its
**      TGA and HDR loads use (TEX_FILTER_FANT), and the width/height
**      reduction (Resize with TEX_FILTER_DEFAULT). 8-bit levels within
**      1/255, float levels within 1e-3 (relative). CPU only.
**
**	Extra Large Technology
\****************************************************************************/
#include "gtImageFiles.hpp"

#ifndef GVK_MATMIPFILTER_HPP
#include "GraphicsVulkan/Mat/matMipFilter.hpp"
#endif
#ifndef GVK_MATIMAGEDECODE_HPP
#include "GraphicsVulkan/Mat/matImageDecode.hpp"
#endif

#include <DirectXPackedVector.h>
#include <gtest/gtest.h>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <string>

using namespace gvk;

namespace
{
	struct Size { uint32_t m_W, m_H; };

	uint32_t hash(uint32_t i_X)
	{
		i_X ^= i_X >> 16; i_X *= 0x7feb352d; i_X ^= i_X >> 15; i_X *= 0x846ca68b; i_X ^= i_X >> 16;
		return i_X;
	}

	//	A gradient with noise in i_Format (one of the formats the decoders
	//	make), as DirectXTex sees it
	bool make_image(DXGI_FORMAT i_Format, Size i_Size, DirectX::ScratchImage& o_Image)
	{
		if (FAILED(o_Image.Initialize2D(i_Format, i_Size.m_W, i_Size.m_H, 1, 1)))
		{
			return false;
		}
		const DirectX::Image* img = o_Image.GetImage(0, 0, 0);
		const size_t channels = (i_Format == DXGI_FORMAT_R8_UNORM || i_Format == DXGI_FORMAT_R16_UNORM) ? 1 : 4;
		for (uint32_t y = 0; y < i_Size.m_H; y++)
		{
			for (uint32_t x = 0; x < i_Size.m_W; x++)
			{
				for (size_t c = 0; c < channels; c++)
				{
					const float v = std::min(1.0f, (x * (c == 0) + y * (c == 1) + (x ^ y) * (c == 2) + 40 * (c == 3)) / 64.0f +
										(hash(x * 131 + y * 7 + (uint32_t)c) & 255) / 1024.0f);
					uint8_t* p = img->pixels + y * img->rowPitch;
					switch (i_Format)
					{
						case DXGI_FORMAT_R16G16B16A16_FLOAT:
							((DirectX::PackedVector::HALF*)p)[x * 4 + c] = DirectX::PackedVector::XMConvertFloatToHalf(v * 6 - 1);
							break;
						case DXGI_FORMAT_R32G32B32A32_FLOAT:
							((float*)p)[x * 4 + c] = v * 6 - 1;
							break;
						case DXGI_FORMAT_R16G16B16A16_UNORM:
						case DXGI_FORMAT_R16_UNORM:
							((uint16_t*)p)[x * channels + c] = (uint16_t)(v * 65535);
							break;
						default:
							p[x * channels + c] = (uint8_t)(v * 255);
							break;
					}
				}
			}
		}
		return true;
	}

	//	The same image as a matDecodedImage
	matDecodedImage to_decoded(const DirectX::ScratchImage& i_Image)
	{
		const DirectX::Image* img = i_Image.GetImage(0, 0, 0);
		matDecodedImage d;
		d.m_DxgiFormat = (uint32_t)img->format;
		d.m_Width = (uint32_t)img->width;
		d.m_Height = (uint32_t)img->height;
		const size_t row = DirectX::BitsPerPixel(img->format) / 8 * img->width;
		matDecodedImage::Sub sub = { 0, row, row * img->height };
		d.m_Subs.push_back(sub);
		for (size_t y = 0; y < img->height; y++)
		{
			d.m_Bytes.insert(d.m_Bytes.end(), img->pixels + y * img->rowPitch, img->pixels + y * img->rowPitch + row);
		}
		return d;
	}

	float channel(DXGI_FORMAT i_Format, const uint8_t* i_pRow, size_t i_Index)
	{
		switch (i_Format)
		{
			case DXGI_FORMAT_R16G16B16A16_FLOAT:	return DirectX::PackedVector::XMConvertHalfToFloat(((const DirectX::PackedVector::HALF*)i_pRow)[i_Index]);
			case DXGI_FORMAT_R32G32B32A32_FLOAT:	return ((const float*)i_pRow)[i_Index];
			case DXGI_FORMAT_R16G16B16A16_UNORM:
			case DXGI_FORMAT_R16_UNORM:				return ((const uint16_t*)i_pRow)[i_Index] / 257.0f;
			default:								return i_pRow[i_Index];
		}
	}

	bool is_float(DXGI_FORMAT i_Format)
	{
		return i_Format == DXGI_FORMAT_R16G16B16A16_FLOAT || i_Format == DXGI_FORMAT_R32G32B32A32_FLOAT;
	}

	//	Compares level i_Level of i_Got with i_Expected; returns the largest
	//	error (8-bit units, or relative for floats)
	float compare_level(const DirectX::Image& i_Expected, const matDecodedImage& i_Got, uint32_t i_Level, const std::string& i_What)
	{
		const DXGI_FORMAT format = i_Expected.format;
		if (i_Level >= i_Got.m_Subs.size())
		{
			ADD_FAILURE() << i_What << ": no level " << i_Level;
			return 1e9f;
		}
		const matDecodedImage::Sub& sub = i_Got.m_Subs[i_Level];
		const uint32_t w = std::max(1u, i_Got.m_Width >> i_Level), h = std::max(1u, i_Got.m_Height >> i_Level);
		EXPECT_EQ(i_Expected.width, w) << i_What << " level " << i_Level;
		EXPECT_EQ(i_Expected.height, h) << i_What << " level " << i_Level;
		const size_t values = (DirectX::BitsPerPixel(format) / DirectX::BitsPerColor(format)) * w;
		float worst = 0;
		for (uint32_t y = 0; y < h && y < i_Expected.height; y++)
		{
			const uint8_t* e = i_Expected.pixels + y * i_Expected.rowPitch;
			const uint8_t* g = i_Got.m_Bytes.data() + sub.m_Offset + y * sub.m_RowPitch;
			for (size_t i = 0; i < values; i++)
			{
				const float a = channel(format, e, i), b = channel(format, g, i);
				const float err = is_float(format) ? std::fabs(a - b) / std::max(1.0f, std::fabs(a)) : std::fabs(a - b);
				worst = std::max(worst, err);
			}
		}
		return worst;
	}

	const char* name(DXGI_FORMAT i_Format)
	{
		switch (i_Format)
		{
			case DXGI_FORMAT_R8G8B8A8_UNORM:		return "RGBA8";
			case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:	return "RGBA8 sRGB";
			case DXGI_FORMAT_B8G8R8A8_UNORM:		return "BGRA8";
			case DXGI_FORMAT_R8_UNORM:				return "R8";
			case DXGI_FORMAT_R16_UNORM:				return "R16";
			case DXGI_FORMAT_R16G16B16A16_UNORM:	return "RGBA16";
			case DXGI_FORMAT_R16G16B16A16_FLOAT:	return "RGBA16F";
			case DXGI_FORMAT_R32G32B32A32_FLOAT:	return "RGBA32F";
			default:								return "?";
		}
	}

	void check_mips(DXGI_FORMAT i_Format, Size i_Size, DirectX::TEX_FILTER_FLAGS i_DxFilter, matMipFilter i_Filter, float i_Tolerance)
	{
		const std::string what = std::string(name(i_Format)) + " " + std::to_string(i_Size.m_W) + "x" + std::to_string(i_Size.m_H);
		DirectX::ScratchImage base;
		ASSERT_TRUE(make_image(i_Format, i_Size, base)) << what;
		DirectX::ScratchImage expected;
		ASSERT_TRUE(SUCCEEDED(DirectX::GenerateMipMaps(*base.GetImage(0, 0, 0), i_DxFilter, 0, expected))) << what;
		matDecodedImage chain;
		ASSERT_TRUE(matGenerateMips(to_decoded(base), 0, chain, i_Filter)) << what;
		EXPECT_EQ(expected.GetMetadata().mipLevels, chain.m_Mips) << what;
		EXPECT_EQ((uint32_t)i_Format, chain.m_DxgiFormat) << what;
		float worst = 0;
		for (uint32_t level = 0; level < expected.GetMetadata().mipLevels; level++)
		{
			const float err = compare_level(*expected.GetImage(level, 0, 0), chain, level, what);
			EXPECT_LE(err, i_Tolerance) << what << " level " << level;
			worst = std::max(worst, err);
		}
		std::printf("%s mips (%s): max error %g%s\n", what.c_str(), i_Filter == matMipFilter::e_Triangle ? "triangle" : "box",
					worst, is_float(i_Format) ? " (relative)" : "/255");
	}

	const Size c_Sizes[] = { { 64, 64 }, { 64, 16 }, { 37, 23 }, { 5, 1 } };
	const DXGI_FORMAT c_Formats[] = { DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, DXGI_FORMAT_B8G8R8A8_UNORM,
									  DXGI_FORMAT_R8_UNORM, DXGI_FORMAT_R16_UNORM, DXGI_FORMAT_R16G16B16A16_UNORM,
									  DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R32G32B32A32_FLOAT };
}

TEST(matMipFilter, TriangleMipsMatchDirectXTex)
{
	for (DXGI_FORMAT format : c_Formats)
	{
		for (Size size : c_Sizes)
		{
			check_mips(format, size, DirectX::TEX_FILTER_TRIANGLE, matMipFilter::e_Triangle, is_float(format) ? 1e-3f : 1.0f);
		}
	}
}

//	TEX_FILTER_FANT (GraphicsDX11's TGA and HDR loads): DirectXTex hands
//	8-bit formats to WIC's Fant scaler and filters the others itself
TEST(matMipFilter, BoxMipsMatchDirectXTex)
{
	const Size c_Pow2[] = { { 64, 64 }, { 64, 16 }, { 1, 8 } };
	for (DXGI_FORMAT format : { DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R16G16B16A16_FLOAT })
	{
		for (Size size : c_Pow2)
		{
			check_mips(format, size, DirectX::TEX_FILTER_FANT, matMipFilter::e_Box, is_float(format) ? 1e-3f : 1.0f);
		}
	}
	//	The custom box filter needs powers of two: like DirectXTex, it fails otherwise
	DirectX::ScratchImage base;
	ASSERT_TRUE(make_image(DXGI_FORMAT_R16G16B16A16_FLOAT, { 37, 23 }, base));
	matDecodedImage chain;
	EXPECT_FALSE(matGenerateMips(to_decoded(base), 0, chain, matMipFilter::e_Box));
}

TEST(matMipFilter, ReduceMatchesDirectXTex)
{
	for (DXGI_FORMAT format : { DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8_UNORM, DXGI_FORMAT_R16G16B16A16_FLOAT })
	{
		for (Size size : { Size{ 64, 64 }, Size{ 37, 23 } })
		{
			for (int reduce = 0; reduce <= 2; reduce++)
			{
				const std::string what = std::string(name(format)) + " " + std::to_string(size.m_W) + "x" + std::to_string(size.m_H) +
										 " >> " + std::to_string(reduce);
				DirectX::ScratchImage base;
				ASSERT_TRUE(make_image(format, size, base)) << what;
				DirectX::ScratchImage expected;
				ASSERT_TRUE(SUCCEEDED(DirectX::Resize(*base.GetImage(0, 0, 0), size.m_W >> reduce, size.m_H >> reduce,
													  DirectX::TEX_FILTER_DEFAULT, expected))) << what;
				matDecodedImage reduced;
				ASSERT_TRUE(matReduce(to_decoded(base), reduce, reduce, reduced)) << what;
				const float err = compare_level(*expected.GetImage(0, 0, 0), reduced, 0, what);
				EXPECT_LE(err, is_float(format) ? 1e-3f : 1.0f) << what;
				if (reduce == 0)
				{
					EXPECT_EQ(0, err) << what << ": no reduction must copy";
				}
				std::printf("%s: max error %g%s\n", what.c_str(), err, is_float(format) ? " (relative)" : "/255");
			}
		}
	}
}
