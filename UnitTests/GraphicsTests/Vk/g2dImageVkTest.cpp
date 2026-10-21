/*****************************************************************************
**  g2dImageVkTest.cpp
**
**      g2dImage, the CPU image MachStudio's captures go through
**      (cptrRenderUtil's supersampling and quadrants, the accumulation
**      buffer, AVI and icon writing): the same operations on images made
**      by g2dImageCreate on each backend, from the same pixels, must give
**      the same bytes. GraphicsDX11's image is a staging texture and its
**      operations are CPU code over the mapped pixels (and CopyResource /
**      CopySubresourceRegion for plain copies); GraphicsVulkan's is a CPU
**      image running a copy of that code, so the results are identical.
**
**      Where DX11 writes only part of a buffer it allocates (the sampled
**      greyscale GetPixels and the uncompressed TGA skip a column or a
**      row), only the written part is compared; the rest is uninitialised
**      on both. The uint8_t paths (GetPixels' 24-bit and greyscale, TGA,
**      R8G8B8A8, R16G16B16A16) read only 16- and 32-bit pixels on DX11
**      (SurfaceIterator::GetValue leaves the colour unset otherwise), so
**      they are compared for those formats only.
**      Skipped without a Vulkan device.
**
**	Extra Large Technology
\****************************************************************************/
#include "gtBackend.hpp"
#include "gtTempDir.hpp"

#ifndef G2D_IMAGE_HPP
#include "Graphics/G2d/g2dImage.hpp"
#endif
#ifndef G2D_IMAGECREATE_HPP
#include "Graphics/G2d/g2dImageCreate.hpp"
#endif
#ifndef G2D_PFD_HPP
#include "Graphics/G2d/g2dPFD.hpp"
#endif
#ifndef G2D_EXCEPTIONX_HPP
#include "Graphics/G2d/g2dExceptionX.hpp"
#endif
#ifndef MA_SAMPLING_HPP
#include "Core/Ma/maSampling.hpp"
#endif
#ifndef MA_INTPOINT2D_HPP
#include "Core/Ma/maIntPoint2d.hpp"
#endif
#ifndef MA_INTRECT_HPP
#include "Core/Ma/maIntRect.hpp"
#endif
#ifndef MA_FUNCTIONS_HPP
#include "Core/Ma/maFunctions.hpp"
#endif
#ifndef FS_FILEX_HPP
#include "Core/fs/fsFileX.hpp"
#endif

#define STB_IMAGE_WRITE_STATIC
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include <gtest/gtest.h>

#include <cstring>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace
{
	typedef std::vector<uint8_t> Bytes;
	//	Named results of one backend's run
	typedef std::map<std::string, Bytes> Results;

	//	The pixel formats a capture or a texture copy gives (and that
	//	DX11's surface iterators read)
	struct Format
	{
		const char*	m_pName;
		g2dPFD		m_PFD;
		bool		m_bBytePaths;	// 16- or 32-bit pixels: the uint8_t paths are defined
	};

	std::vector<Format> formats()
	{
		return
		{
			{ "RGBA8",	 g2dPFD(0, 8, 8, 8, 16, 8, 24, 8, 32), true },
			{ "BGRX8",	 g2dPFD(g2dPFD::e_RColor, 32), true },
			{ "RGBA16F", g2dPFD(g2dPFD::e_RGBA16f, 64), false },
			{ "RGBA32F", g2dPFD(g2dPFD::e_RGBA32f, 128), false },
			{ "RG32F",	 g2dPFD(g2dPFD::e_GR32f, 64), false },
			{ "R16F",	 g2dPFD(g2dPFD::e_Float16, 16), true },
			{ "R32F",	 g2dPFD(g2dPFD::e_Float32, 32), true },
		};
	}

	//	A small deterministic generator (the same sequence on both backends)
	struct Lcg
	{
		uint32_t m_State;
		explicit Lcg(uint32_t i_Seed) : m_State(i_Seed * 2654435761u + 1) {}
		uint32_t Next() { m_State = m_State * 1664525u + 1013904223u; return m_State >> 8; }
		float Unit() { return (Next() & 0xffff) / 65535.0f; }
	};

	//	Fills an image's pixels: bytes for 8-bit channels, floats in
	//	[-0.25, 1.5] (halves for 16-bit) otherwise; i_Block > 1 gives
	//	blocks of one colour (for the TGA run-length paths)
	void fill(g2dImage& io_Image, uint32_t i_Seed, int i_Block = 1)
	{
		const g2dPFD& pfd = io_Image.GetPixelFormat();
		uint8_t* base = static_cast<uint8_t*>(io_Image.Lock());
		const int stride = io_Image.GetStride();
		const int bpp = pfd.BitsPerPixel() / 8;
		const bool is_float = pfd.GetPixelFormat() != g2dPFD::e_Color && pfd.GetPixelFormat() != g2dPFD::e_RColor;
		const int bits = is_float ? pfd.GetBitsPerChannel() : 8;
		for (int y = 0; y < io_Image.GetHeight(); y++)
		{
			uint8_t* row = base + (size_t)y * stride;
			for (int x = 0; x < io_Image.GetWidth(); x++)
			{
				Lcg rng(i_Seed * 7919u + (uint32_t)((y / i_Block) * 1031 + (x / i_Block)));
				uint8_t* pixel = row + (size_t)x * bpp;
				for (int c = 0; c < bpp * 8 / bits; c++)
				{
					if (bits == 8)
					{
						pixel[c] = (uint8_t)(rng.Next() & 0xff);
					}
					else if (bits == 16)
					{
						reinterpret_cast<uint16_t*>(pixel)[c] = maFunctions::FloatToHalf(rng.Unit() * 1.75f - 0.25f);
					}
					else
					{
						reinterpret_cast<float*>(pixel)[c] = rng.Unit() * 1.75f - 0.25f;
					}
				}
			}
		}
		io_Image.Release();
	}

	//	The image's pixels, row by row without the row padding
	Bytes raw(g2dImage& i_Image)
	{
		const uint8_t* base = static_cast<const uint8_t*>(i_Image.Lock());
		const int stride = i_Image.GetStride();
		const size_t row_bytes = (size_t)i_Image.GetWidth() * (i_Image.GetPixelFormat().BitsPerPixel() / 8);
		Bytes out;
		for (int y = 0; y < i_Image.GetHeight(); y++)
		{
			out.insert(out.end(), base + (size_t)y * stride, base + (size_t)y * stride + row_bytes);
		}
		i_Image.Release();
		return out;
	}

	std::unique_ptr<g2dImage> make(int i_Width, int i_Height, const g2dPFD& i_PFD, uint32_t i_Seed, int i_Block = 1)
	{
		std::unique_ptr<g2dImage> image(g2dImageCreate::Make(i_Width, i_Height, i_PFD, g2dImage::e_SystemMemory));
		fill(*image, i_Seed, i_Block);
		return image;
	}

	template <class T>
	Bytes as_bytes(const T* i_pData, size_t i_Count)
	{
		const uint8_t* p = reinterpret_cast<const uint8_t*>(i_pData);
		return Bytes(p, p + i_Count * sizeof(T));
	}

	Bytes ints(std::initializer_list<int> i_Values)
	{
		std::vector<int> v(i_Values);
		return as_bytes(v.data(), v.size());
	}

	//	Runs i_Fill on DX11, then on Vulkan, and expects the same results
	void expect_same(const std::function<void(Results&)>& i_Fill)
	{
		Results dx11, vulkan;
		if (!gtBackend::Run(gtBackend::e_DX11, [&]() { i_Fill(dx11); }))
			return;
		if (!gtBackend::Run(gtBackend::e_Vulkan, [&]() { i_Fill(vulkan); }))
			return;
		ASSERT_FALSE(dx11.empty());
		EXPECT_EQ(dx11.size(), vulkan.size());
		for (const auto& entry : dx11)
		{
			SCOPED_TRACE(entry.first);
			auto found = vulkan.find(entry.first);
			if (found == vulkan.end())
			{
				ADD_FAILURE() << "missing on Vulkan";
				continue;
			}
			const Bytes& a = entry.second;
			const Bytes& b = found->second;
			if (a != b)
			{
				size_t first = 0;
				while (first < a.size() && first < b.size() && a[first] == b[first])
					first++;
				ADD_FAILURE() << "differs: DX11 " << a.size() << " bytes, Vulkan " << b.size()
							  << " bytes, first difference at byte " << first;
			}
		}
	}

	//	What an exception (or none) was: a name to compare
	template <class F>
	std::string outcome(F i_Function)
	{
		try
		{
			i_Function();
			return "ok";
		}
		catch (const g2dUnknownImageFileTypeX&)	{ return "g2dUnknownImageFileTypeX"; }
		catch (const fsFileDoesntExistX&)		{ return "fsFileDoesntExistX"; }
		catch (const g2dGeneralX&)				{ return "g2dGeneralX"; }
		catch (const envExceptionX&)			{ return "envExceptionX"; }
	}

	Bytes text(const std::string& i_Text)
	{
		return Bytes(i_Text.begin(), i_Text.end());
	}

	Bytes describe(const g2dImage& i_Image)
	{
		const g2dPFD& pfd = i_Image.GetPixelFormat();
		return ints({ i_Image.GetWidth(), i_Image.GetHeight(), (int)pfd.GetPixelFormat(), pfd.BitsPerPixel(),
					  pfd.FirstRedBit(), pfd.NumRedBits(), pfd.FirstAlphaBit(), pfd.NumAlphaBits() });
	}

	std::vector<std::pair<const char*, std::function<maFilter*()>>> filters()
	{
		return
		{
			{ "box",		[]() { return new maBoxFilter; } },
			{ "gaussian",	[]() { return new maGaussianFilter; } },
			{ "mitchell",	[]() { return new maMitchellFilter; } },
			{ "triangle",	[]() { return new maTriangleFilter; } },
			{ "sinc",		[]() { return new maSincFilter; } },
			{ "lanczos",	[]() { return new maLanczosFilter; } },
			{ "blackman",	[]() { return new maBlackmanHarrisFilter; } },
			{ "catmullrom",	[]() { return new maCatmullRomFilter; } },
		};
	}
}

//----------------------------------------------------------------------------
//	CopyImage with each filter cptrRenderUtil offers, filter widths under
//	and over a pixel, sample rates 1-4, odd sizes; and plain copies
//----------------------------------------------------------------------------
TEST(g2dImageVkTest, CopyImageFiltersMatchDX11)
{
	expect_same([](Results& o_Results)
	{
		const int width = 33, height = 17;
		for (const Format& format : formats())
		{
			if (format.m_PFD.GetPixelFormat() != g2dPFD::e_Color && format.m_PFD.GetPixelFormat() != g2dPFD::e_RGBA32f)
				continue;
			for (int rate = 1; rate <= 4; rate++)
			{
				std::unique_ptr<g2dImage> source = make(width * rate, height * rate, format.m_PFD, 11 + rate);
				for (const auto& filter : filters())
				{
					for (float filter_width : { 0.5f, 1.0f, 2.5f })
					{
						std::unique_ptr<maFilter> f(filter.second());
						std::unique_ptr<g2dImage> dest = make(width, height, format.m_PFD, 3);
						dest->CopyImage(*source, f.get(), filter_width, filter_width, rate, rate);
						o_Results[std::string(format.m_pName) + "/" + filter.first + "/w" + std::to_string(filter_width) +
								  "/r" + std::to_string(rate)] = raw(*dest);
					}
				}
			}
		}

		//	plain copies: the same format, and across formats
		for (const Format& format : formats())
		{
			std::unique_ptr<g2dImage> source = make(width, height, format.m_PFD, 5);
			std::unique_ptr<g2dImage> dest = make(width, height, format.m_PFD, 6);
			dest->CopyImage(*source);
			o_Results[std::string("plain/") + format.m_pName] = raw(*dest);
		}
		std::unique_ptr<g2dImage> source = make(width, height, formats()[0].m_PFD, 7);
		std::unique_ptr<g2dImage> dest = make(width, height, formats()[3].m_PFD, 8);
		dest->CopyImage(*source);
		o_Results["plain/RGBA8_to_RGBA32F"] = raw(*dest);
	});
}

//----------------------------------------------------------------------------
//	Both DrawImage overloads, inside, clipped on each side and off each edge
//----------------------------------------------------------------------------
TEST(g2dImageVkTest, DrawImageMatchDX11)
{
	expect_same([](Results& o_Results)
	{
		struct Place { const char* m_pName; int m_X, m_Y; };
		const Place places[] =
		{
			{ "inside", 5, 4 }, { "clip_left_top", -6, -3 }, { "clip_right_bottom", 30, 25 },
			{ "off_left", -20, 0 }, { "off_top", 0, -12 }, { "off_right", 40, 0 }, { "off_bottom", 0, 30 },
		};
		for (const Format& format : formats())
		{
			std::unique_ptr<g2dImage> source = make(16, 12, format.m_PFD, 21);
			for (const Place& place : places)
			{
				std::unique_ptr<g2dImage> dest = make(40, 30, format.m_PFD, 22);
				dest->DrawImage(place.m_X, place.m_Y, *source);
				o_Results[std::string(format.m_pName) + "/" + place.m_pName] = raw(*dest);
			}
			std::unique_ptr<g2dImage> dest = make(40, 30, format.m_PFD, 23);
			dest->DrawImage(3, 2, 2, 1, 10, 9, *source);
			o_Results[std::string(format.m_pName) + "/rect"] = raw(*dest);
			dest->DrawImage(-1, 27, 4, 3, 14, 11, *source);
			o_Results[std::string(format.m_pName) + "/rect_clipped"] = raw(*dest);
		}
	});
}

//----------------------------------------------------------------------------
//	DrawImage between different formats converts each pixel through the
//	surface iterators (D3D11's CopySubresourceRegion copies only within a
//	format family, so DX11 used to copy nothing): every pair of formats,
//	inside and clipped; and cptrRenderUtil's quadrant capture of the
//	velocity map, RG32F quadrants into the RGBA32F image GetCapturePFD
//	makes, which must hold (r, g, 0, 1) of each quadrant
//----------------------------------------------------------------------------
TEST(g2dImageVkTest, DrawImageAcrossFormatsMatchDX11)
{
	expect_same([](Results& o_Results)
	{
		for (const Format& from : formats())
		{
			std::unique_ptr<g2dImage> source = make(16, 12, from.m_PFD, 31);
			for (const Format& to : formats())
			{
				if (from.m_PFD.GetPixelFormat() == to.m_PFD.GetPixelFormat())
					continue;
				const std::string name = std::string(from.m_pName) + "_to_" + to.m_pName;
				std::unique_ptr<g2dImage> dest = make(40, 30, to.m_PFD, 32);
				const Bytes before = raw(*dest);
				dest->DrawImage(5, 4, *source);
				const Bytes inside = raw(*dest);
				EXPECT_TRUE(before != inside) << name << ": nothing drawn";
				o_Results[name + "/inside"] = inside;
				dest->DrawImage(30, -3, *source);
				dest->DrawImage(-2, 27, 4, 3, 14, 11, *source);
				o_Results[name + "/clipped"] = raw(*dest);
			}
		}

		//	four 16 x 12 RG32F quadrants into a 32 x 24 RGBA32F image
		const g2dPFD rg32f(g2dPFD::e_GR32f, 64), rgba32f(g2dPFD::e_RGBA32f, 128);
		std::unique_ptr<g2dImage> quad(g2dImageCreate::Make(32, 24, rgba32f, g2dImage::e_SystemMemory));
		std::vector<std::unique_ptr<g2dImage>> quadrants;
		for (int i = 0; i < 4; i++)
		{
			quadrants.push_back(make(16, 12, rg32f, 40 + i));
			quad->DrawImage((i % 2) * 16, (i / 2) * 12, *quadrants.back());
		}
		const Bytes out = raw(*quad);
		int wrong = 0;
		for (int i = 0; i < 4; i++)
		{
			const Bytes in = raw(*quadrants[i]);
			for (int y = 0; y < 12; y++)
			{
				for (int x = 0; x < 16; x++)
				{
					float rg[2], rgba[4];
					std::memcpy(rg, &in[((size_t)y * 16 + x) * 8], sizeof(rg));
					std::memcpy(rgba, &out[(((size_t)(i / 2) * 12 + y) * 32 + (i % 2) * 16 + x) * 16], sizeof(rgba));
					wrong += rgba[0] != rg[0] || rgba[1] != rg[1] || rgba[2] != 0 || rgba[3] != 1;
				}
			}
		}
		EXPECT_EQ(0, wrong) << "quadrant pixels not (r, g, 0, 1)";
		o_Results["quadrants_RG32F_to_RGBA32F"] = out;
	});
}

namespace
{
	//	The bytes DX11's GetPixels writes (see the file comment)
	size_t written_pixels_bytes(int i_Width, int i_Height, int i_Sampling, bool i_bGreyscale)
	{
		if (i_Sampling <= 1)
			return (size_t)i_Width * i_Height * (i_bGreyscale ? 1 : 3);
		const int rows = i_Height >= i_Sampling ? (i_Height - i_Sampling) / i_Sampling + 1 : 0;
		const int cols = i_bGreyscale ? (i_Width > i_Sampling ? (i_Width - i_Sampling + i_Sampling - 1) / i_Sampling : 0)
									  : (i_Width >= i_Sampling ? (i_Width - i_Sampling) / i_Sampling + 1 : 0);
		return (size_t)rows * cols * (i_bGreyscale ? 1 : 3);
	}

	//	The bytes DX11's uncompressed GetPixelsTGA writes
	size_t written_tga_bytes(int i_Width, int i_Height, int i_Sampling)
	{
		if (i_Sampling <= 1)
			return (size_t)i_Width * (i_Height - 1) * 3;
		const int rows = (i_Height - i_Sampling) / i_Sampling + 1;
		const int cols = (i_Width - i_Sampling) / i_Sampling + 1;
		return (size_t)rows * cols * 3;
	}
}

//----------------------------------------------------------------------------
//	GetPixels' variants and GetPixelsTGA
//----------------------------------------------------------------------------
TEST(g2dImageVkTest, GetPixelsVariantsMatchDX11)
{
	expect_same([](Results& o_Results)
	{
		const maIntRect rect(maIntPoint2d(3, 2), maIntPoint2d(20, 15));
		for (const Format& format : formats())
		{
			for (int size = 0; size < 2; size++)
			{
				const int width = size ? 37 : 36, height = size ? 25 : 24;
				for (int block : { 1, 6 })
				{
					std::unique_ptr<g2dImage> image = make(width, height, format.m_PFD, 31 + size, block);
					const std::string name = std::string(format.m_pName) + (size ? "/odd" : "/even") +
											 (block > 1 ? "/blocks" : "/noise");

					if (format.m_bBytePaths)
					{
						for (int sampling = 1; sampling <= 4; sampling++)
						{
							for (int grey = 0; grey < 2; grey++)
							{
								for (int use_rect = 0; use_rect < 2; use_rect++)
								{
									uint8_t* buffer = NULL;
									int buf_size = 0, w = 0, h = 0, bpp = 0;
									image->GetPixels(&buffer, &buf_size, &w, &h, &bpp, sampling, grey != 0, use_rect ? &rect : NULL);
									const size_t n = written_pixels_bytes(w, h, sampling, grey != 0);
									Bytes out = ints({ buf_size, w, h, bpp });
									if (buffer && n <= (size_t)buf_size)
										out.insert(out.end(), buffer, buffer + n);
									delete [] buffer;
									o_Results[name + "/pixels/s" + std::to_string(sampling) + (grey ? "/grey" : "") +
											  (use_rect ? "/rect" : "")] = out;
								}
							}
							for (const char* code : { "None", "RLE" })
							{
								uint8_t* buffer = NULL;
								int buf_size = 0, w = 0, h = 0, bpp = 0;
								image->GetPixelsTGA(&buffer, &buf_size, &w, &h, &bpp, sampling, code);
								const size_t full = (size_t)w * h * 3 / sampling / sampling;
								const bool compressed = strcmp(code, "RLE") == 0 && (size_t)buf_size != full;
								const size_t n = compressed ? (size_t)buf_size : written_tga_bytes(w, h, sampling);
								Bytes out = ints({ buf_size, w, h, bpp });
								if (buffer)
									out.insert(out.end(), buffer, buffer + n);
								delete [] buffer;
								o_Results[name + "/tga/s" + std::to_string(sampling) + "/" + code] = out;
							}
						}

						g2dPixelR16G16B16A16* p16 = NULL;
						int w = 0, h = 0;
						image->GetPixels(&p16, &w, &h);
						o_Results[name + "/r16g16b16a16"] = as_bytes(p16, (size_t)w * h);
						delete [] p16;

						g2dPixelR8G8B8A8* p8 = NULL;
						image->GetPixels(&p8, &w, &h);
						o_Results[name + "/r8g8b8a8"] = as_bytes(p8, (size_t)w * h);
						delete [] p8;
					}

					g2dPixelRGBA32F* pf = NULL;
					int w = 0, h = 0;
					image->GetPixels(&pf, &w, &h);
					o_Results[name + "/rgba32f"] = as_bytes(pf, (size_t)w * h);
					delete [] pf;

					std::vector<g2dPixelRGBA32F> part((size_t)(width - 5) * (height - 3));
					image->GetPixels(part.data(), width - 5, height - 3);
					o_Results[name + "/rgba32f_part"] = as_bytes(part.data(), part.size());
				}
			}
		}
	});
}

//----------------------------------------------------------------------------
//	SwapChannels (DX11 assumes 4 bytes a pixel: on a 2-byte format it
//	writes past each row into its driver's row padding, so R16F isn't
//	compared)
//----------------------------------------------------------------------------
TEST(g2dImageVkTest, SwapChannelsMatchesDX11)
{
	expect_same([](Results& o_Results)
	{
		for (const Format& format : formats())
		{
			if (format.m_PFD.BitsPerPixel() < 32)
				continue;
			for (auto channels : { std::make_pair(0, 2), std::make_pair(1, 3) })
			{
				std::unique_ptr<g2dImage> image = make(19, 7, format.m_PFD, 41);
				image->SwapChannels(channels.first, channels.second);
				o_Results[std::string(format.m_pName) + "/" + std::to_string(channels.first) + std::to_string(channels.second)] =
					raw(*image);
			}
		}
	});
}

//----------------------------------------------------------------------------
//	Making images of formats a capture can't have
//----------------------------------------------------------------------------
TEST(g2dImageVkTest, MakeOtherFormatsMatchesDX11)
{
	expect_same([](Results& o_Results)
	{
		const std::pair<const char*, g2dPFD> cases[] =
		{
			{ "Luminance8", g2dPFD(g2dPFD::e_Luminance8, 8) },
			{ "RGBA16UInt", g2dPFD(g2dPFD::e_RGBA16UInt, 64) },
		};
		for (const auto& c : cases)
		{
			std::unique_ptr<g2dImage> image;
			const std::string result = outcome([&]() { image.reset(g2dImageCreate::Make(9, 5, c.second, g2dImage::e_SystemMemory)); });
			Bytes out = text(result);
			if (image)
			{
				Bytes d = describe(*image);
				out.insert(out.end(), d.begin(), d.end());
			}
			o_Results[c.first] = out;
		}
	});
}

//----------------------------------------------------------------------------
//	A pixel format with no DXGI format (24-bit RGB): DX11's CreateTexture2D
//	fails and its constructor throws g2dGeneralX (measured; it isn't run on
//	DX11 here because DX11 then logs "DirectX error : E_INVALIDARG", which
//	MSBuild takes for an error in the post-build test run)
//----------------------------------------------------------------------------
TEST(g2dImageVkTest, MakeUnknownFormatThrows)
{
	gtBackend::Run(gtBackend::e_Vulkan, []()
	{
		std::unique_ptr<g2dImage> image;
		EXPECT_EQ("g2dGeneralX", outcome([&]()
		{
			image.reset(g2dImageCreate::Make(9, 5, g2dPFD(0, 8, 8, 8, 16, 8, 0, 0, 24), g2dImage::e_SystemMemory));
		}));
		EXPECT_EQ(nullptr, image.get());
	});
}

//----------------------------------------------------------------------------
//	g2dImageCreate::Load: BMP and PNG files; other files throw
//----------------------------------------------------------------------------
TEST(g2dImageVkTest, LoadMatchesDX11)
{
	gtTempDir dir;
	struct File { const char* m_pName; int m_Width, m_Height, m_Channels; };
	const File files[] =
	{
		{ "rgba.png", 23, 9, 4 }, { "rgb.png", 23, 9, 3 },
		{ "rgb32.bmp", 32, 9, 3 }, { "rgb33.bmp", 33, 9, 3 }, { "rgba.bmp", 16, 9, 4 },
		{ "rgb.jpg", 16, 8, 3 }, { "rgb.tga", 16, 8, 3 },
	};
	for (const File& file : files)
	{
		std::vector<uint8_t> pixels((size_t)file.m_Width * file.m_Height * file.m_Channels);
		Lcg rng(51);
		for (uint8_t& p : pixels)
			p = (uint8_t)(rng.Next() & 0xff);
		const std::string path = (dir.GetPath() / file.m_pName).string();
		const std::string ext = path.substr(path.size() - 3);
		int ok = 0;
		if (ext == "png")
			ok = stbi_write_png(path.c_str(), file.m_Width, file.m_Height, file.m_Channels, pixels.data(), file.m_Width * file.m_Channels);
		else if (ext == "bmp")
			ok = stbi_write_bmp(path.c_str(), file.m_Width, file.m_Height, file.m_Channels, pixels.data());
		else if (ext == "jpg")
			ok = stbi_write_jpg(path.c_str(), file.m_Width, file.m_Height, file.m_Channels, pixels.data(), 90);
		else
			ok = stbi_write_tga(path.c_str(), file.m_Width, file.m_Height, file.m_Channels, pixels.data());
		ASSERT_TRUE(ok) << path;
	}

	expect_same([&](Results& o_Results)
	{
		for (const File& file : files)
		{
			std::unique_ptr<g2dImage> image;
			const std::string result = outcome([&]() { image.reset(g2dImageCreate::Load(dir.GetLocator(file.m_pName))); });
			Bytes out = text(result);
			if (image)
			{
				Bytes d = describe(*image);
				out.insert(out.end(), d.begin(), d.end());
				Bytes r = raw(*image);
				out.insert(out.end(), r.begin(), r.end());
			}
			o_Results[file.m_pName] = out;
		}
		o_Results["missing"] = text(outcome([&]() { delete g2dImageCreate::Load(dir.GetLocator("missing.png")); }));
	});
}
