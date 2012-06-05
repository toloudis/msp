/****************************************************************************\
**  g2dImageDX11.hpp
**
**      g2dImageDX11.hpp is the D3D implementation of g2dImage.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifdef G2D_IMAGEDX11_HPP
#error g2dImageDX11.hpp multiply included
#endif
#define G2D_IMAGEDX11_HPP

#ifndef G2D_IMAGE_HPP
#include "Graphics/g2d/g2dImage.hpp"
#endif
#ifndef G2D_DX11TYPES_HPP
#include "GraphicsDX11/g2d/g2dDX11Types.hpp"
#endif
#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif


//============================================================================
//============================================================================
class g2dImageDX11 : public g2dImage
{
	public:
		//--------------------------------------------------------------------
		//	This constructor makes an "empty" surface
		//--------------------------------------------------------------------
		g2dImageDX11();

		//--------------------------------------------------------------------
		//	This constructor makes a surface with the given dimensions
		//	and pixel format.  If the pixel format is not supported it will
		//	throw a g2dUnsupportedPixelFormatX.
		//--------------------------------------------------------------------
		g2dImageDX11(int i_Width, int i_Height, const g2dPFD& i_PFD, g2dImage::CreateLoc i_CreateLoc = g2dImage::e_SystemMemory );

		//--------------------------------------------------------------------
		// wrap a surface. the image will own it and release it on dtor.
		//--------------------------------------------------------------------
		g2dImageDX11(g2dD3D11TexturePtr i_surface);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~g2dImageDX11();

		//--------------------------------------------------------------------
		//	Make makes the surface into one with the given dimensions and
		//	pixel format.  If the pixel format is not supported it will
		//	throw a g2dUnsupportedPixelFormatX, and leave the old surface
		//	intact.
		//--------------------------------------------------------------------
		void Make(int i_Width, int i_Height, const g2dPFD& i_PFD, g2dImage::CreateLoc i_CreateLoc = g2dImage::e_SystemMemory );

		//--------------------------------------------------------------------
		//	GetPixelFormat returns the current pixel format of the surface.
		//--------------------------------------------------------------------
		const g2dPFD& GetPixelFormat() const;

		//--------------------------------------------------------------------
		//	The width of the surface, in pixels (not bytes!).
		//	Note that this is not the same as the stride; see below.
		//--------------------------------------------------------------------
		int GetWidth() const;

		//--------------------------------------------------------------------
		//	The height of the surface.
		//--------------------------------------------------------------------
		int GetHeight() const;

		//--------------------------------------------------------------------
		//	The stride of the surface.  The stride is the number of bytes
		//	separating the beginning of one horizontal line from the
		//	beginning of the next horizontal line.  This might _not_ be the
		//	same as the width of the surface multiplied by the bytes per
		//	pixel.  This value is only valid between calls to Lock and
		//	Release.
		//--------------------------------------------------------------------
		int GetStride() const;

		//--------------------------------------------------------------------
		//	Lock returns a pointer to the surface memory, allowing it to be
		//	modified directly.  This should not be done often, as a surface
		//	may be residing in video memory.  Call Release() when you are
		//	done reading or modifying the surface.
		//--------------------------------------------------------------------
		void* Lock();

		//--------------------------------------------------------------------
		//	Release should be called when you are finished with Lock() (or
		//	future operations with the surface may fail).
		//--------------------------------------------------------------------
		void Release();

		//------------------------------------------------------------------------
		//	This CopyImage makes a possibly resized copy of the original g2dImage
		//------------------------------------------------------------------------
		virtual void CopyImage( const g2dImage& i_SrcImage, 
			const maFilter* i_Filter = NULL, 
			float i_FilterWidth = 1, float i_FilterHeight = 1,
			int i_SampleRateX = 1, int i_SampleRateY = 1);

		//------------------------------------------------------------------------
		//	This DrawImage blits the source image to this image.
		//	The source image upper left corner will be located at (i_DX1, i_DY1)
		//	on the destination image.
		//------------------------------------------------------------------------
		void DrawImage(	int i_DX1,
						int i_DY1,
						const g2dImage& i_SrcImage);

		//------------------------------------------------------------------------
		//	This DrawImage blits a rectangular section of the source image to a
		//	location on this image.  The rectangle of the source image
		//	is defined by (i_SX1, i_SY1) - (i_SX2, i_SY2).  The point
		//	(i_SX1, i_SY1) will be located at (i_DX1, i_DY1).
		//------------------------------------------------------------------------
		void DrawImage(	int i_DX1,
						int i_DY1,
						int i_SX1,
						int i_SY1,
						int i_SX2,
						int i_SY2,
						const g2dImage& i_SrcImage);

		//----------------------------------------------------------------------------
		// Copy pixels into buffer. this call allocates a big block of mem!!!!!
		// caller must delete [] o_pBuffer when done using it!!!!
		//----------------------------------------------------------------------------
		virtual void GetPixels(envType::UInt8** o_pBuffer,
			int* o_bufSize,
			int* o_width,
			int* o_height,
			int* o_bpp,
			int i_sampling = 1,
			bool i_bGreyscale = false,
			RECT* i_pRect = NULL);
		virtual void GetPixelsTGA(envType::UInt8 **o_pBuffer,
			int *o_bufSize,
			int *o_width,
			int *o_height,
			int *o_bpp,
			int	i_Sampling = 1,
			const std::string& i_CompressionCode = std::string("None"),
			const RECT* i_pRect = NULL);
		// straight copy, no downsampling. caller must delete.
		virtual void GetPixels(g2dPixelR16G16B16A16** o_pBuffer,
			int* o_width,
			int* o_height);
		// straight copy, no downsampling. caller must delete.
		virtual void GetPixels(g2dPixelR8G8B8A8** o_pBuffer,
			int* o_width,
			int* o_height);
		// straight copy, no downsampling. caller must delete.
		virtual void GetPixels(g2dPixelRGBA32F** o_pBuffer,
			int* o_width,
			int* o_height);

		// fill in buffer as provided
		virtual void GetPixels(g2dPixelRGBA32F* o_pBuffer,
			int i_Width,
			int i_Height);

		//----------------------------------------------------------------------------
		// Get the color of the pixel at an individual position on the image
		//----------------------------------------------------------------------------
		void GetPixelColor(int i_X,
							int i_Y,
							envType::UInt8 &o_Red,
							envType::UInt8 &o_Green,
							envType::UInt8 &o_Blue,
							envType::UInt8 &o_Alpha);

		//----------------------------------------------------------------------------
		// Get the color of the pixel at an individual position on the image
		//----------------------------------------------------------------------------
		void GetPixelColor(int i_X,
							int i_Y,
							envType::Float32 &o_Red,
							envType::Float32 &o_Green,
							envType::Float32 &o_Blue,
							envType::Float32 &o_Alpha);

		//----------------------------------------------------------------------------
		// caller must destroy the HBITMAP!!!
		//----------------------------------------------------------------------------
		virtual HBITMAP GetHBMP(const int i_Sampling );

		//----------------------------------------------------------------------------
		// SwapChannels() - assumes 4 bytes per pixel and non-floats
		//----------------------------------------------------------------------------
		virtual void SwapChannels( int i_ChannelOne, int i_ChannelTwo );

		//--------------------------------------------------------------------
		//	GetSurface returns the DirectDraw surface pointer.
		//--------------------------------------------------------------------
		g2dD3D11TexturePtr GetSurface() const;

		//--------------------------------------------------------------------
		//	This locator will be used for images that were loaded from a file
		//	on disk.  The locator can be used to reload the images if they
		//	get swapped out of video memory.
		//--------------------------------------------------------------------
		const fsLocator& GetLocator() const { return m_Locator; }
		void SetLocator(const fsLocator& i_Locator) { m_Locator = i_Locator; }

	private:

		//--------------------------------------------------------------------
		//	This operator = is not implemented.  It is placed here in private
		//	to prevent it's useage.
		//--------------------------------------------------------------------
		g2dImageDX11& operator = (const g2dImageDX11& i_Image);

		g2dPFD		m_PixelFormat;
		short		m_Width, m_Height;
		int			m_Stride;
		fsLocator	m_Locator;

		g2dD3D11TexturePtr m_pSurface;
};


