/*****************************************************************************
**  matMipFilter.cpp
**
**      See matMipFilter.hpp. The DirectXTex code is followed operation for
**	operation, in scalar float (DirectXMath's SSE paths round the same
**	way, and MSVC doesn't fuse the multiply-adds), so levels come out as
**	DirectXTex makes them; WIC's Fant scaler, whose code isn't available,
**	is an area average rounded to nearest.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GVK_MATMIPFILTER_HPP
#include "GraphicsVulkan/Mat/matMipFilter.hpp"
#endif
#ifndef GVK_MATIMAGEDECODEINTERNAL_HPP
#include "GraphicsVulkan/Mat/private/matImageDecodeInternal.hpp"
#endif

#include <algorithm>
#include <cmath>
#include <vector>

namespace gvk
{
	namespace
	{
		using namespace matDecode;

		struct V4
		{
			float x, y, z, w;
		};

		V4 operator + (const V4& a, const V4& b)	{ return { a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w }; }
		V4 operator * (const V4& a, float s)		{ return { a.x * s, a.y * s, a.z * s, a.w * s }; }

		//--------------------------------------------------------------------
		//	Formats (LoadScanline / StoreScanline)
		//--------------------------------------------------------------------
		struct Format
		{
			uint32_t	m_Dxgi;
			uint32_t	m_Bytes;		// per pixel
			bool		m_bSrgb;		// filtered in linear space
			bool		m_bWic;			// DirectXTex hands it to WIC's scalers (8 bits per channel, not sRGB)
		};

		const Format c_Formats[] =
		{
			{ e_R8G8B8A8_UNORM, 4, false, true },
			{ e_R8G8B8A8_UNORM_SRGB, 4, true, false },
			{ e_B8G8R8A8_UNORM, 4, false, true },
			{ e_B8G8R8A8_UNORM_SRGB, 4, true, false },
			{ e_B8G8R8X8_UNORM, 4, false, true },
			{ e_B8G8R8X8_UNORM_SRGB, 4, true, false },
			{ e_R8_UNORM, 1, false, true },
			{ e_R16_UNORM, 2, false, false },
			{ e_R16G16B16A16_UNORM, 8, false, false },
			{ e_R16G16B16A16_FLOAT, 8, false, false },
			{ e_R32G32B32A32_FLOAT, 16, false, false },
			{ 54, 2, false, false },		// R16_FLOAT
			{ e_R32_FLOAT, 4, false, false },
		};

		const Format* find_format(uint32_t i_Dxgi)
		{
			for (const Format& f : c_Formats)
			{
				if (f.m_Dxgi == i_Dxgi)
				{
					return &f;
				}
			}
			return nullptr;
		}

		float saturate(float v)
		{
			return std::min(1.0f, std::max(0.0f, v));
		}

		//	XMColorSRGBToRGB / XMColorRGBToSRGB on x, y, z
		float srgb_to_linear(float v)
		{
			v = saturate(v);
			return v > 0.04045f ? std::pow((v + 0.055f) * (1.f / 1.055f), 2.4f) : v * (1.f / 12.92f);
		}

		float linear_to_srgb(float v)
		{
			v = saturate(v);
			return v < 0.0031308f ? v * 12.92f : 1.055f * std::pow(v, 1.0f / 2.4f) - 0.055f;
		}

		//	XMStoreUByteN4 of v + 0.5/255 (g_8BitBias)
		uint8_t store_unorm8(float v)
		{
			return (uint8_t)(saturate(v + 0.5f / 255.f) * 255.f);
		}

		//	XMStoreUShortN4
		uint16_t store_unorm16(float v)
		{
			return (uint16_t)(saturate(v) * 65535.0f + 0.5f);
		}

		uint16_t store_half(float v)
		{
			return FloatToHalf(std::min(65504.f, std::max(-65504.f, v)));
		}

		void load_row(const Format& i_F, const uint8_t* i_pSrc, uint32_t i_Count, V4* o_pRow)
		{
			for (uint32_t i = 0; i < i_Count; i++)
			{
				const uint8_t* s = i_pSrc + (size_t)i * i_F.m_Bytes;
				V4& v = o_pRow[i];
				switch (i_F.m_Dxgi)
				{
					case e_R8G8B8A8_UNORM:
					case e_R8G8B8A8_UNORM_SRGB:
						v = { s[0] * (1.0f / 255.0f), s[1] * (1.0f / 255.0f), s[2] * (1.0f / 255.0f), s[3] * (1.0f / 255.0f) };
						break;
					case e_B8G8R8A8_UNORM:
					case e_B8G8R8A8_UNORM_SRGB:
					case e_B8G8R8X8_UNORM:
					case e_B8G8R8X8_UNORM_SRGB:
						v = { s[2] * (1.0f / 255.0f), s[1] * (1.0f / 255.0f), s[0] * (1.0f / 255.0f), s[3] * (1.0f / 255.0f) };
						if (i_F.m_Dxgi == e_B8G8R8X8_UNORM || i_F.m_Dxgi == e_B8G8R8X8_UNORM_SRGB)
						{
							v.w = 1.0f;
						}
						break;
					case e_R8_UNORM:
						v = { (float)s[0] / 255.f, 0.f, 0.f, 1.f };
						break;
					case e_R16_UNORM:
						v = { (float)Read16(s) / 65535.f, 0.f, 0.f, 1.f };
						break;
					case e_R16G16B16A16_UNORM:
						v = { Read16(s) * (1.0f / 65535.0f), Read16(s + 2) * (1.0f / 65535.0f), Read16(s + 4) * (1.0f / 65535.0f),
							  Read16(s + 6) * (1.0f / 65535.0f) };
						break;
					case e_R16G16B16A16_FLOAT:
						v = { HalfToFloat(Read16(s)), HalfToFloat(Read16(s + 2)), HalfToFloat(Read16(s + 4)), HalfToFloat(Read16(s + 6)) };
						break;
					case e_R32G32B32A32_FLOAT:
						memcpy(&v, s, 16);
						break;
					case 54:
						v = { HalfToFloat(Read16(s)), 0.f, 0.f, 1.f };
						break;
					case e_R32_FLOAT:
						memcpy(&v.x, s, 4);
						v.y = v.z = 0.f;
						v.w = 1.f;
						break;
				}
				if (i_F.m_bSrgb)
				{
					v.x = srgb_to_linear(v.x);
					v.y = srgb_to_linear(v.y);
					v.z = srgb_to_linear(v.z);
				}
			}
		}

		void store_row(const Format& i_F, V4* io_pRow, uint32_t i_Count, uint8_t* o_pDst)
		{
			for (uint32_t i = 0; i < i_Count; i++)
			{
				V4 v = io_pRow[i];
				if (i_F.m_bSrgb)
				{
					v.x = linear_to_srgb(v.x);
					v.y = linear_to_srgb(v.y);
					v.z = linear_to_srgb(v.z);
				}
				uint8_t* d = o_pDst + (size_t)i * i_F.m_Bytes;
				switch (i_F.m_Dxgi)
				{
					case e_R8G8B8A8_UNORM:
					case e_R8G8B8A8_UNORM_SRGB:
						d[0] = store_unorm8(v.x); d[1] = store_unorm8(v.y); d[2] = store_unorm8(v.z); d[3] = store_unorm8(v.w);
						break;
					case e_B8G8R8A8_UNORM:
					case e_B8G8R8A8_UNORM_SRGB:
						d[0] = store_unorm8(v.z); d[1] = store_unorm8(v.y); d[2] = store_unorm8(v.x); d[3] = store_unorm8(v.w);
						break;
					case e_B8G8R8X8_UNORM:
					case e_B8G8R8X8_UNORM_SRGB:
						d[0] = store_unorm8(v.z); d[1] = store_unorm8(v.y); d[2] = store_unorm8(v.x); d[3] = store_unorm8(1.0f);
						break;
					case e_R8_UNORM:
						d[0] = store_unorm8(v.x);		// with the bias since DirectXTex's May 2026 release
						break;
					case e_R16_UNORM:
					{
						const uint16_t s = store_unorm16(v.x);
						memcpy(d, &s, 2);
						break;
					}
					case e_R16G16B16A16_UNORM:
					{
						const uint16_t s[4] = { store_unorm16(v.x), store_unorm16(v.y), store_unorm16(v.z), store_unorm16(v.w) };
						memcpy(d, s, 8);
						break;
					}
					case e_R16G16B16A16_FLOAT:
					{
						const uint16_t s[4] = { store_half(v.x), store_half(v.y), store_half(v.z), store_half(v.w) };
						memcpy(d, s, 8);
						break;
					}
					case e_R32G32B32A32_FLOAT:
						memcpy(d, &v, 16);
						break;
					case 54:
					{
						const uint16_t s = store_half(v.x);
						memcpy(d, &s, 2);
						break;
					}
					case e_R32_FLOAT:
						memcpy(d, &v.x, 4);
						break;
				}
			}
		}

		//--------------------------------------------------------------------
		//	Images
		//--------------------------------------------------------------------
		struct Level
		{
			uint32_t	m_Width, m_Height;
			uint8_t*	m_pPixels;
			size_t		m_RowPitch;
		};

		//	o_Image as an empty chain of i_Levels mips of i_Format
		void make_chain(matDecodedImage& o_Image, uint32_t i_Dxgi, uint32_t i_Bytes, uint32_t i_Width, uint32_t i_Height,
						uint32_t i_Levels)
		{
			o_Image = matDecodedImage();
			o_Image.m_DxgiFormat = i_Dxgi;
			o_Image.m_Width = i_Width;
			o_Image.m_Height = i_Height;
			o_Image.m_Mips = i_Levels;
			size_t size = 0;
			uint32_t w = i_Width, h = i_Height;
			for (uint32_t level = 0; level < i_Levels; level++)
			{
				const size_t row = (size_t)w * i_Bytes;
				o_Image.m_Subs.push_back({ size, row, row * h });
				size += row * h;
				w = w > 1 ? w >> 1 : 1;
				h = h > 1 ? h >> 1 : 1;
			}
			o_Image.m_Bytes.assign(size, 0);
		}

		Level level_of(matDecodedImage& i_Image, uint32_t i_Mip)
		{
			const matDecodedImage::Sub& s = i_Image.m_Subs[i_Mip];
			return { std::max(1u, i_Image.m_Width >> i_Mip), std::max(1u, i_Image.m_Height >> i_Mip),
					 i_Image.m_Bytes.data() + s.m_Offset, s.m_RowPitch };
		}

		//	Mip 0 of i_Image, rows tightly packed into o_Level0's storage
		void copy_level0(const matDecodedImage& i_Image, uint32_t i_Bytes, const Level& o_Level0)
		{
			const matDecodedImage::Sub& s = i_Image.m_Subs[0];
			for (uint32_t y = 0; y < o_Level0.m_Height; y++)
			{
				memcpy(o_Level0.m_pPixels + y * o_Level0.m_RowPitch, i_Image.m_Bytes.data() + s.m_Offset + y * s.m_RowPitch,
					   (size_t)o_Level0.m_Width * i_Bytes);
			}
		}

		//	The image's mip 0 lies within its bytes
		bool valid_level0(const matDecodedImage& i_Image, uint32_t i_Bytes)
		{
			if (i_Image.m_Subs.empty() || i_Image.m_Width == 0 || i_Image.m_Height == 0 || i_Image.m_Depth != 1)
			{
				return false;
			}
			const matDecodedImage::Sub& s = i_Image.m_Subs[0];
			const size_t row = (size_t)i_Image.m_Width * i_Bytes;
			return s.m_RowPitch >= row && s.m_Offset + s.m_RowPitch * (i_Image.m_Height - 1) + row <= i_Image.m_Bytes.size();
		}

		//--------------------------------------------------------------------
		//	DirectXTex filters
		//--------------------------------------------------------------------
		//	filters.h CreateTriangleFilter (no wrap): for each source pixel,
		//	the destination pixels it adds to and the weights
		struct To { size_t m_U; float m_Weight; };
		typedef std::vector<std::vector<To>> TriangleFilter;

		TriangleFilter create_triangle_filter(size_t i_Source, size_t i_Dest)
		{
			const float TF_EPSILON = 0.00001f;
			const float scale = float(i_Dest) / float(i_Source);
			const float scaleInv = 0.5f / scale;
			TriangleFilter filter(i_Source);
			size_t accumU = 0;
			float accumWeight = 0.f;
			for (size_t u = 0; u < i_Source; ++u)
			{
				std::vector<To>& to = filter[u];
				//	Two passes capture the influences from both sides
				for (size_t j = 0; j < 2; ++j)
				{
					const float src = float(u + j) - 0.5f;
					float destMin = src * scale;
					float destMax = destMin + scale;
					if (destMin < 0.f)
						destMin = 0.f;
					if (destMax > float(i_Dest))
						destMax = float(i_Dest);
					for (auto k = static_cast<ptrdiff_t>(floorf(destMin)); float(k) < destMax; ++k)
					{
						float d0 = float(k);
						float d1 = d0 + 1.f;
						const size_t u0 = size_t(k);
						if (u0 != accumU)
						{
							if (accumWeight > TF_EPSILON)
							{
								to.push_back({ accumU, accumWeight });
							}
							accumWeight = 0.f;
							accumU = u0;
						}
						if (d0 < destMin)
							d0 = destMin;
						if (d1 > destMax)
							d1 = destMax;
						float weight;
						if (src < 0.f)
							weight = 1.f;
						else if ((src + 1.f) >= float(i_Source))
							weight = 0.f;
						else
							weight = (d0 + d1) * scaleInv - src;
						accumWeight += (d1 - d0) * (j ? (1.f - weight) : weight);
					}
				}
				if (accumWeight > TF_EPSILON)
				{
					to.push_back({ accumU, accumWeight });
				}
				accumWeight = 0.f;
			}
			return filter;
		}

		//	Generate2DMipsTriangleFilter
		bool triangle_mips(const Format& i_F, matDecodedImage& io_Chain)
		{
			uint32_t width = io_Chain.m_Width, height = io_Chain.m_Height;
			std::vector<V4> row(width);
			std::vector<std::vector<V4>> acc;
			for (uint32_t level = 1; level < io_Chain.m_Mips; ++level)
			{
				const Level src = level_of(io_Chain, level - 1), dest = level_of(io_Chain, level);
				const uint32_t nwidth = width > 1 ? width >> 1 : 1, nheight = height > 1 ? height >> 1 : 1;
				const TriangleFilter tfX = create_triangle_filter(width, nwidth);
				const TriangleFilter tfY = create_triangle_filter(height, nheight);
				acc.assign(nheight, std::vector<V4>(nwidth, V4{ 0, 0, 0, 0 }));
				std::vector<uint32_t> remaining(nheight, 0);
				for (const std::vector<To>& yFrom : tfY)
				{
					for (const To& t : yFrom)
					{
						remaining[t.m_U]++;
					}
				}
				for (uint32_t y = 0; y < height; ++y)
				{
					const std::vector<To>& yFrom = tfY[y];
					load_row(i_F, src.m_pPixels + y * src.m_RowPitch, width, row.data());
					for (uint32_t x = 0; x < width; ++x)
					{
						const std::vector<To>& xFrom = tfX[x];
						for (const To& yt : yFrom)
						{
							std::vector<V4>& a = acc[yt.m_U];
							for (const To& xt : xFrom)
							{
								const float weight = yt.m_Weight * xt.m_Weight;
								a[xt.m_U] = row[x] * weight + a[xt.m_U];
							}
						}
					}
					//	Write completed accumulation rows
					for (const To& yt : yFrom)
					{
						if (--remaining[yt.m_U] == 0)
						{
							store_row(i_F, acc[yt.m_U].data(), dest.m_Width, dest.m_pPixels + yt.m_U * dest.m_RowPitch);
						}
					}
				}
				height = nheight;
				width = nwidth;
			}
			return true;
		}

		bool is_pow2(uint32_t i_X)
		{
			return i_X && !(i_X & (i_X - 1));
		}

		//	Generate2DMipsBoxFilter, with its row pointers (once the height is 1,
		//	the second row is the first, but the pixels to its right still come
		//	from the second row's buffer, as DirectXTex reads them)
		bool box_mips(const Format& i_F, matDecodedImage& io_Chain)
		{
			uint32_t width = io_Chain.m_Width, height = io_Chain.m_Height;
			if (!is_pow2(width) || !is_pow2(height))
			{
				return false;
			}
			std::vector<V4> scanline((size_t)width * 3, V4{ 0, 0, 0, 0 });
			V4* target = scanline.data();
			V4* urow0 = target + width;
			V4* urow1 = target + width * 2;
			const V4* urow2 = urow0 + 1;
			const V4* urow3 = urow1 + 1;
			for (uint32_t level = 1; level < io_Chain.m_Mips; ++level)
			{
				if (height <= 1)
				{
					urow1 = urow0;
				}
				if (width <= 1)
				{
					urow2 = urow0;
					urow3 = urow1;
				}
				const Level src = level_of(io_Chain, level - 1), dest = level_of(io_Chain, level);
				const uint8_t* pSrc = src.m_pPixels;
				uint8_t* pDest = dest.m_pPixels;
				const uint32_t nwidth = width > 1 ? width >> 1 : 1, nheight = height > 1 ? height >> 1 : 1;
				for (uint32_t y = 0; y < nheight; ++y)
				{
					load_row(i_F, pSrc, width, urow0);
					pSrc += src.m_RowPitch;
					if (urow0 != urow1)
					{
						load_row(i_F, pSrc, width, urow1);
						pSrc += src.m_RowPitch;
					}
					for (uint32_t x = 0; x < nwidth; ++x)
					{
						const uint32_t x2 = x << 1;
						target[x] = (((urow0[x2] + urow1[x2]) + urow2[x2]) + urow3[x2]) * 0.25f;
					}
					store_row(i_F, target, nwidth, pDest);
					pDest += dest.m_RowPitch;
				}
				if (height > 1)
					height >>= 1;
				if (width > 1)
					width >>= 1;
			}
			return true;
		}

		//	ResizeBoxFilter (an exact halving)
		void resize_box(const Format& i_F, const Level& i_Src, const Level& o_Dest)
		{
			std::vector<V4> scanline((size_t)i_Src.m_Width * 2 + o_Dest.m_Width);
			V4* target = scanline.data();
			V4* urow0 = target + o_Dest.m_Width;
			V4* urow1 = urow0 + i_Src.m_Width;
			const V4* urow2 = urow0 + 1;
			const V4* urow3 = urow1 + 1;
			const uint8_t* pSrc = i_Src.m_pPixels;
			for (uint32_t y = 0; y < o_Dest.m_Height; ++y)
			{
				load_row(i_F, pSrc, i_Src.m_Width, urow0);
				pSrc += i_Src.m_RowPitch;
				load_row(i_F, pSrc, i_Src.m_Width, urow1);
				pSrc += i_Src.m_RowPitch;
				for (uint32_t x = 0; x < o_Dest.m_Width; ++x)
				{
					const uint32_t x2 = x << 1;
					target[x] = (((urow0[x2] + urow1[x2]) + urow2[x2]) + urow3[x2]) * 0.25f;
				}
				store_row(i_F, target, o_Dest.m_Width, o_Dest.m_pPixels + y * o_Dest.m_RowPitch);
			}
		}

		//	filters.h CreateLinearFilter (no wrap)
		struct LinearFilter { size_t u0; float weight0; size_t u1; float weight1; };

		std::vector<LinearFilter> create_linear_filter(size_t i_Source, size_t i_Dest)
		{
			std::vector<LinearFilter> lf(i_Dest);
			const float scale = float(i_Source) / float(i_Dest);
			for (size_t u = 0; u < i_Dest; ++u)
			{
				const float srcB = (float(u) + 0.5f) * scale + 0.5f;
				ptrdiff_t isrcB = ptrdiff_t(srcB);
				ptrdiff_t isrcA = isrcB - 1;
				const float weight = 1.0f + float(isrcB) - srcB;
				if (isrcA < 0)
				{
					isrcA = 0;
				}
				if (size_t(isrcB) >= i_Source)
				{
					isrcB = ptrdiff_t(i_Source) - 1;
				}
				lf[u] = { size_t(isrcA), weight, size_t(isrcB), 1.0f - weight };
			}
			return lf;
		}

		//	ResizeLinearFilter
		void resize_linear(const Format& i_F, const Level& i_Src, const Level& o_Dest)
		{
			const std::vector<LinearFilter> lfX = create_linear_filter(i_Src.m_Width, o_Dest.m_Width);
			const std::vector<LinearFilter> lfY = create_linear_filter(i_Src.m_Height, o_Dest.m_Height);
			std::vector<V4> rows((size_t)i_Src.m_Width * 2), target(o_Dest.m_Width);
			V4* row0 = rows.data();
			V4* row1 = row0 + i_Src.m_Width;
			size_t u0 = size_t(-1), u1 = size_t(-1);
			for (uint32_t y = 0; y < o_Dest.m_Height; ++y)
			{
				const LinearFilter& toY = lfY[y];
				if (toY.u0 != u0)
				{
					if (toY.u0 != u1)
					{
						u0 = toY.u0;
						load_row(i_F, i_Src.m_pPixels + i_Src.m_RowPitch * u0, i_Src.m_Width, row0);
					}
					else
					{
						u0 = u1;
						u1 = size_t(-1);
						std::swap(row0, row1);
					}
				}
				if (toY.u1 != u1)
				{
					u1 = toY.u1;
					load_row(i_F, i_Src.m_pPixels + i_Src.m_RowPitch * u1, i_Src.m_Width, row1);
				}
				for (uint32_t x = 0; x < o_Dest.m_Width; ++x)
				{
					const LinearFilter& toX = lfX[x];
					target[x] = ((row0[toX.u0] * toX.weight0 + row0[toX.u1] * toX.weight1) * toY.weight0) +
								((row1[toX.u0] * toX.weight0 + row1[toX.u1] * toX.weight1) * toY.weight1);
				}
				store_row(i_F, target.data(), o_Dest.m_Width, o_Dest.m_pPixels + y * o_Dest.m_RowPitch);
			}
		}

		//	WIC's Fant scaler on 8-bit channels, as measured through DirectXTex
		//	(its code isn't available):
		//	- each destination pixel averages the source area it covers, except
		//	  that when that area starts partway into a source pixel (covering a
		//	  fraction f of it), that pixel weighs f^2 and the next one the rest
		//	  (1 + f - f^2); sums round to nearest
		//	- formats with alpha are scaled premultiplied: colours times alpha
		//	  ((c * a + 127) / 255) before, times 0xFF0000 / alpha >> 16 after,
		//	  so even an unscaled copy loses colour where alpha is low
		void resize_fant(const Format& i_F, const Level& i_Src, const Level& o_Dest)
		{
			struct Span { uint32_t m_First; std::vector<double> m_Weights; };
			auto spans = [](uint32_t i_Source, uint32_t i_Dest)
			{
				std::vector<Span> out(i_Dest);
				const double scale = (double)i_Source / i_Dest;
				for (uint32_t d = 0; d < i_Dest; d++)
				{
					const double a = d * scale, b = (d + 1) * scale;
					Span& s = out[d];
					s.m_First = (uint32_t)a;
					for (uint32_t i = s.m_First; i < i_Source && i < b; i++)
					{
						s.m_Weights.push_back(std::min<double>(b, i + 1) - std::max<double>(a, i));
					}
					const double f = s.m_Weights[0];
					if (a > s.m_First && s.m_Weights.size() > 1)
					{
						s.m_Weights[0] = f * f;
						s.m_Weights[1] += f - f * f;
					}
					for (double& w : s.m_Weights)
					{
						w /= scale;
					}
				}
				return out;
			};
			const uint32_t channels = i_F.m_Bytes;
			const bool alpha = channels == 4 && i_F.m_Dxgi != e_B8G8R8X8_UNORM;
			//	The source, premultiplied where it has alpha
			std::vector<uint8_t> source((size_t)i_Src.m_Width * i_Src.m_Height * channels);
			for (uint32_t y = 0; y < i_Src.m_Height; y++)
			{
				memcpy(&source[(size_t)y * i_Src.m_Width * channels], i_Src.m_pPixels + y * i_Src.m_RowPitch, (size_t)i_Src.m_Width * channels);
			}
			if (alpha)
			{
				for (size_t p = 0; p < source.size(); p += 4)
				{
					for (int c = 0; c < 3; c++)
					{
						source[p + c] = (uint8_t)((source[p + c] * source[p + 3] + 127) / 255);
					}
				}
			}
			const std::vector<Span> sx = spans(i_Src.m_Width, o_Dest.m_Width), sy = spans(i_Src.m_Height, o_Dest.m_Height);
			std::vector<double> sum(channels);
			for (uint32_t y = 0; y < o_Dest.m_Height; y++)
			{
				for (uint32_t x = 0; x < o_Dest.m_Width; x++)
				{
					std::fill(sum.begin(), sum.end(), 0.0);
					for (size_t j = 0; j < sy[y].m_Weights.size(); j++)
					{
						const uint8_t* srow = &source[(size_t)(sy[y].m_First + j) * i_Src.m_Width * channels];
						for (size_t i = 0; i < sx[x].m_Weights.size(); i++)
						{
							const double w = sy[y].m_Weights[j] * sx[x].m_Weights[i];
							const uint8_t* p = srow + (size_t)(sx[x].m_First + i) * channels;
							for (uint32_t c = 0; c < channels; c++)
							{
								sum[c] += p[c] * w;
							}
						}
					}
					uint8_t* d = o_Dest.m_pPixels + y * o_Dest.m_RowPitch + (size_t)x * channels;
					for (uint32_t c = 0; c < channels; c++)
					{
						d[c] = (uint8_t)std::min(255.0, std::floor(sum[c] + 0.5));
					}
					if (alpha)
					{
						for (int c = 0; c < 3; c++)
						{
							//	16.16 fixed-point reciprocal, as WIC divides (exact for all 256 x 256 cases)
							d[c] = d[3] ? (uint8_t)std::min(255u, (uint32_t)(((uint64_t)d[c] * (0xFF0000u / d[3])) >> 16)) : 0;
						}
					}
				}
			}
		}

		uint32_t full_chain(uint32_t i_W, uint32_t i_H)
		{
			uint32_t levels = 1;
			while (i_W > 1 || i_H > 1)
			{
				i_W = i_W > 1 ? i_W >> 1 : 1;
				i_H = i_H > 1 ? i_H >> 1 : 1;
				levels++;
			}
			return levels;
		}
	}

	bool matGenerateMips(const matDecodedImage& i_Level0, uint32_t i_Levels, matDecodedImage& o_Chain, matMipFilter i_Filter)
	{
		const Format* f = find_format(i_Level0.m_DxgiFormat);
		if (!f || !valid_level0(i_Level0, f->m_Bytes))
		{
			return false;
		}
		const uint32_t most = full_chain(i_Level0.m_Width, i_Level0.m_Height);
		const uint32_t levels = i_Levels == 0 ? most : i_Levels;
		if (levels > most)
		{
			return false;
		}
		make_chain(o_Chain, f->m_Dxgi, f->m_Bytes, i_Level0.m_Width, i_Level0.m_Height, levels);
		copy_level0(i_Level0, f->m_Bytes, level_of(o_Chain, 0));
		if (i_Filter == matMipFilter::e_Triangle)
		{
			return triangle_mips(*f, o_Chain);
		}
		if (f->m_bWic)
		{
			//	WIC scales the base image to each level
			const Level base = level_of(o_Chain, 0);
			for (uint32_t level = 1; level < levels; level++)
			{
				resize_fant(*f, base, level_of(o_Chain, level));
			}
			return true;
		}
		if (!box_mips(*f, o_Chain))
		{
			o_Chain = matDecodedImage();
			return false;
		}
		return true;
	}

	bool matReduce(const matDecodedImage& i_Image, int i_WidthReduce, int i_HeightReduce, matDecodedImage& o_Image)
	{
		const Format* f = find_format(i_Image.m_DxgiFormat);
		if (!f || !valid_level0(i_Image, f->m_Bytes) || i_WidthReduce < 0 || i_HeightReduce < 0 ||
			i_WidthReduce > 31 || i_HeightReduce > 31)
		{
			return false;
		}
		const uint32_t width = i_Image.m_Width >> i_WidthReduce, height = i_Image.m_Height >> i_HeightReduce;
		if (width == 0 || height == 0)
		{
			return false;
		}
		matDecodedImage source;
		make_chain(source, f->m_Dxgi, f->m_Bytes, i_Image.m_Width, i_Image.m_Height, 1);
		copy_level0(i_Image, f->m_Bytes, level_of(source, 0));
		make_chain(o_Image, f->m_Dxgi, f->m_Bytes, width, height, 1);
		const Level src = level_of(source, 0), dest = level_of(o_Image, 0);
		if (f->m_bWic)
		{
			resize_fant(*f, src, dest);
		}
		else if (dest.m_Width * 2 == src.m_Width && dest.m_Height * 2 == src.m_Height)
		{
			resize_box(*f, src, dest);
		}
		else
		{
			resize_linear(*f, src, dest);
		}
		return true;
	}
}
