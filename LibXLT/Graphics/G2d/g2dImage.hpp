/****************************************************************************\
**  g2dImage.hpp
**
**      g2dImage.hpp is the basic Terawatt 2D image type.  It exists to
**	hide speedy platform-specific rendering & other manipulations.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef G2D_IMAGE_HPP
#error g2dImage.hpp multiply included
#endif
#define G2D_IMAGE_HPP

#ifndef G2D_PFD_HPP
#include "Graphics/g2d/g2dPFD.hpp"
#endif

#include <string>
#include <windows.h> // for RECT


//============================================================================
//============================================================================
class maFilter;


//============================================================================
//============================================================================
class g2dImage
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		enum CreateLoc
		{
			e_SystemMemory,
			e_VideoMemory,
			e_ScratchMemory,
			e_ManagedMemory,
		};

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~g2dImage() {};

		//--------------------------------------------------------------------
		//	Make makes the surface into one with the given dimensions and
		//	pixel format.  If the pixel format is not supported it will
		//	throw a g2dUnsupportedPixelFormatX, and leave the old surface
		//	intact.
		//--------------------------------------------------------------------
		virtual void Make(int i_Width, int i_Height, const g2dPFD& i_PFD, CreateLoc i_Loc = e_VideoMemory ) = 0;

		//--------------------------------------------------------------------
		//	GetPixelFormat returns the current pixel format of the surface.
		//--------------------------------------------------------------------
		virtual const g2dPFD& GetPixelFormat() const = 0;

		//--------------------------------------------------------------------
		//	The width of the surface, in pixels (not bytes!).
		//	Note that this is not the same as the stride; see below.
		//--------------------------------------------------------------------
		virtual int GetWidth() const = 0;

		//--------------------------------------------------------------------
		//	The height of the surface.
		//--------------------------------------------------------------------
		virtual int GetHeight() const = 0;

		//--------------------------------------------------------------------
		//	The stride of the surface.  The stride is the number of bytes
		//	separating the beginning of one horizontal line from the
		//	beginning of the next horizontal line.  This might _not_ be the
		//	same as the width of the surface multiplied by the bytes per
		//	pixel.  This value is only valid between calls to Lock and
		//	Release.
		//--------------------------------------------------------------------
		virtual int GetStride() const = 0;

		//--------------------------------------------------------------------
		//	Lock returns a pointer to the surface memory, allowing it to be
		//	modified directly.  This should not be done often, as a surface
		//	may be residing in video memory.  Call Release() when you are
		//	done reading or modifying the surface.
		//--------------------------------------------------------------------
		virtual void* Lock() = 0;

		//--------------------------------------------------------------------
		//	Release should be called when you are finished with Lock() (or
		//	future operations with the surface may fail).
		//--------------------------------------------------------------------
		virtual void Release() = 0;

		//------------------------------------------------------------------------
		//	This CopyImage makes a possibly resized copy of the original g2dImage
		//------------------------------------------------------------------------
		virtual void CopyImage( const g2dImage& i_SrcImage, 
			const maFilter* i_Filter = NULL, 
			float i_FilterWidth = 1, float i_FilterHeight = 1,
			int i_SampleRateX = 1, int i_SampleRateY = 1) = 0;

		//------------------------------------------------------------------------
		//	This DrawImage blits the source image to this image.
		//	The source image upper left corner will be located at (i_DX1, i_DY1)
		//	on the destination image.
		//------------------------------------------------------------------------
		virtual void DrawImage(	int i_DX1,
								int i_DY1,
								const g2dImage& i_SrcImage) = 0;

		//------------------------------------------------------------------------
		//	This DrawImage blits a rectangular section of the source image to a
		//	location on this image.  The rectangle of the source image
		//	is defined by (i_SX1, i_SY1) - (i_SX2, i_SY2).  The point
		//	(i_SX1, i_SY1) will be located at (i_DX1, i_DY1).
		//------------------------------------------------------------------------
		virtual void DrawImage(	int i_DX1,
								int i_DY1,
								int i_SX1,
								int i_SY1,
								int i_SX2,
								int i_SY2,
								const g2dImage& i_SrcImage) = 0;


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
			RECT* i_pRect = NULL) = 0;
		virtual void GetPixelsTGA(envType::UInt8 **o_pBuffer,
			int *o_bufSize,
			int *o_width,
			int *o_height,
			int *o_bpp,
			int	i_Sampling = 1,
			const std::string& i_CompressionCode = std::string("None"),
			const RECT* i_pRect = NULL) = 0;
		// straight copy, no downsampling. caller must delete.
		virtual void GetPixels(g2dPixelR16G16B16A16** o_pBuffer,
			int* o_width,
			int* o_height) = 0;
		// straight copy, no downsampling. caller must delete.
		virtual void GetPixels(g2dPixelR8G8B8A8** o_pBuffer,
			int* o_width,
			int* o_height) = 0;
		// straight copy, no downsampling. caller must delete.
		virtual void GetPixels(g2dPixelRGBA32F** o_pBuffer,
			int* o_width,
			int* o_height) = 0;

		// fill in buffer as provided
		virtual void GetPixels(g2dPixelRGBA32F* o_pBuffer,
			int i_Width,
			int i_Height) = 0;

		//----------------------------------------------------------------------------
		// Copy pixels into HBITMAP. this call allocates a GDI Object (the HBITMAP)!
		// caller must destroy the HBITMAP!!!
		//----------------------------------------------------------------------------
		virtual HBITMAP GetHBMP(const int i_Sampling ) = 0;

		//----------------------------------------------------------------------------
		// SwapChannels() - assumes 4 bytes per pixel and non-floats
		//----------------------------------------------------------------------------
		virtual void SwapChannels( int i_ChannelOne, int i_ChannelTwo ) = 0;

	private:
		//--------------------------------------------------------------------
		//	This operator = is not implemented.  It is placed here in private
		//	to prevent it's useage.
		//--------------------------------------------------------------------
		g2dImage& operator = (const g2dImage& i_Image);
};
