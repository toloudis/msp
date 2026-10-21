/*****************************************************************************
**  g2dImageVk.hpp
**
**      GraphicsDX11's g2dImageDX11 as a CPU image. DX11's image is a
**	staging texture of the DXGI format its g2dPFD maps to
**	(D3DFormatFromPFD), and its operations are CPU code over the mapped
**	pixels (g2dDX11SurfaceUtil), plus CopyResource and
**	CopySubresourceRegion for plain copies and blits. This one keeps the
**	pixels of the same DXGI format in memory and runs a copy of that code
**	(g2dSurfaceUtilVk), so every operation gives DX11's bytes.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GVK_G2DIMAGEVK_HPP
#error g2dImageVk.hpp multiply included
#endif
#define GVK_G2DIMAGEVK_HPP

#ifndef G2D_IMAGE_HPP
#include "Graphics/G2d/g2dImage.hpp"
#endif
#ifndef G2D_PFD_HPP
#include "Graphics/G2d/g2dPFD.hpp"
#endif
#ifndef FS_LOCATOR_HPP
#include "Core/Fs/fsLocator.hpp"
#endif

#include <cstdint>
#include <string>
#include <vector>

namespace gvk
{
	class g2dImageVk : public g2dImage
	{
		public:
			g2dImageVk();
			//	Throws g2dGeneralX for a pixel format with no DXGI format (as
			//	DX11's CreateTexture2D fails for DXGI_FORMAT_UNKNOWN)
			g2dImageVk(int i_Width, int i_Height, const g2dPFD& i_PFD, CreateLoc i_CreateLoc = e_VideoMemory);
			virtual ~g2dImageVk();

			//	DX11's g2dImageDX11(texture): an image of a texture's DXGI
			//	format, its g2dPFD from PFDFromDxgiFormat (frame captures)
			void MakeFromDxgiFormat(int i_Width, int i_Height, uint32_t i_DxgiFormat);

			virtual void Make(int i_Width, int i_Height, const g2dPFD& i_PFD, CreateLoc i_Loc = e_VideoMemory);
			virtual const g2dPFD& GetPixelFormat() const;
			virtual int GetWidth() const;
			virtual int GetHeight() const;
			//	The row size in bytes (DX11's is the mapped RowPitch, valid
			//	between Lock and Release; here the rows aren't padded)
			virtual int GetStride() const;
			virtual void* Lock();
			virtual void Release();
			virtual void CopyImage(const g2dImage& i_SrcImage, const maFilter* i_Filter = NULL, float i_FilterWidth = 1,
								   float i_FilterHeight = 1, int i_SampleRateX = 1, int i_SampleRateY = 1);
			virtual void DrawImage(int i_DX1, int i_DY1, const g2dImage& i_SrcImage);
			virtual void DrawImage(int i_DX1, int i_DY1, int i_SX1, int i_SY1, int i_SX2, int i_SY2, const g2dImage& i_SrcImage);
			virtual void GetPixels(uint8_t** o_pBuffer, int* o_bufSize, int* o_width, int* o_height, int* o_bpp,
								   int i_sampling = 1, bool i_bGreyscale = false, const maIntRect* i_pRect = NULL);
			virtual void GetPixelsTGA(uint8_t** o_pBuffer, int* o_bufSize, int* o_width, int* o_height, int* o_bpp,
									  int i_Sampling = 1, const std::string& i_CompressionCode = std::string("None"),
									  const maIntRect* i_pRect = NULL);
			virtual void GetPixels(g2dPixelR16G16B16A16** o_pBuffer, int* o_width, int* o_height);
			virtual void GetPixels(g2dPixelR8G8B8A8** o_pBuffer, int* o_width, int* o_height);
			virtual void GetPixels(g2dPixelRGBA32F** o_pBuffer, int* o_width, int* o_height);
			virtual void GetPixels(g2dPixelRGBA32F* o_pBuffer, int i_Width, int i_Height);
			virtual void SwapChannels(int i_ChannelOne, int i_ChannelTwo);

			//	The color of one pixel (g2dImageDX11::GetPixelColor)
			void GetPixelColor(int i_X, int i_Y, uint8_t& o_Red, uint8_t& o_Green, uint8_t& o_Blue, uint8_t& o_Alpha);
			void GetPixelColor(int i_X, int i_Y, float& o_Red, float& o_Green, float& o_Blue, float& o_Alpha);

			//	The file the image was loaded from (g2dImageCreate::Load)
			const fsLocator& GetLocator() const				{ return m_Locator; }
			void SetLocator(const fsLocator& i_Locator)		{ m_Locator = i_Locator; }

			//	The pixels' DXGI format (DXGI_FORMAT value)
			uint32_t GetDxgiFormat() const	{ return m_Format; }
			//	The pixels, GetStride() bytes per row (NULL if empty)
			const uint8_t* Pixels() const	{ return m_Pixels.empty() ? NULL : m_Pixels.data(); }

		private:
			g2dImageVk(const g2dImageVk&) = delete;
			g2dImageVk& operator = (const g2dImageVk&) = delete;

			void allocate(int i_Width, int i_Height, uint32_t i_DxgiFormat);

			uint32_t				m_Format = 0;
			g2dPFD					m_PixelFormat;
			int						m_Width = 0;
			int						m_Height = 0;
			int						m_Stride = 0;
			std::vector<uint8_t>	m_Pixels;
			fsLocator				m_Locator;
	};
}
