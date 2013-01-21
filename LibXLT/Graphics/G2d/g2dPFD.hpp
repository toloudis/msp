/****************************************************************************\
**  g2dPFD.hpp
**
**      g2dPFD describes a color pixel format
**	(PFD == Pixel Format Description).
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#pragma once
#ifdef G2D_PFD_HPP
#error g2dPFD.hpp multiply included
#endif
#define G2D_PFD_HPP

#ifndef ENV_TYPE_HPP
#include "Core/env/envType.hpp"
#endif
#ifndef G2D_ARGBCOLOR_HPP
#include "Graphics/g2d/g2dARGBColor.hpp"
#endif
#ifndef G2D_RGBCOLOR_HPP
#include "Graphics/g2d/g2dRGBColor.hpp"
#endif


//============================================================================
//============================================================================
struct g2dPixelR16G16B16A16
{
	envType::UInt16 r,g,b,a;
};
struct g2dPixelR8G8B8A8
{
	envType::UInt8 r,g,b,a;
};
struct g2dPixelRGBA32F
{
	float r,g,b,a;
};


//============================================================================
//============================================================================
class g2dPFD
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		enum PixelFormat
		{
			e_Unknown,
			e_Color,			   // for normal color with ARGB
			e_BumpMapV8U8,		   // 16-bit bump-map format using 8 bits each for u and v data
			e_BumpMapCxV8U8,	   // 16-bit bump-map format using 8 bits each for u and v data
			e_BumpMapV16U16,	   // 32-bit bump-map format using 16 bits each for u and v data
			e_BumpMapX8L8V8U8,	   // 32-bit bump-map format
			e_Depth24Stencil8,	   // depth map 24 bit with 8 bit of stencil
			e_Depth16,			   // depth map, 16 bit
			e_Depth32f,				// depth buffer, 32bit float
			e_Float16,			   // floating point data, 16-bits
			e_Float32,			   // floating point data, 32-bits
			e_RGBA16f,			   // floating point pixel, 16 bit per channel
			e_RGBA32f,			   // floating point pixel, 32 bit per channel
			e_RGBA16UInt,		   // unsigned int pixel, 16 bit per channel
			e_DXT1,				   // 1-bit alpha compressed texture format, 64 bits for 4X4 block
			e_DXT2,
			e_DXT3,				   // 4-bit alpha compressed texture format, 128 bits for 4X4 block
			e_DXT4,
			e_DXT5,
			e_Luminance8,
			e_GR32f,			   // floating point pixel, 32 bit per channel
			e_RColor,			   // for reversed color with BGRA
		};

		//--------------------------------------------------------------------
		//	This constructor makes a g2dPFD with all zeroes (invalid).
		//--------------------------------------------------------------------
		g2dPFD();

		//--------------------------------------------------------------------
		//	This constructor makes a g2dPFD with the given pixel format.
		//--------------------------------------------------------------------
		g2dPFD( PixelFormat i_PixelFormat, int i_BitsPerPixel );

		//--------------------------------------------------------------------
		//	This constructor deduces the pixel format from the given masks.
		//--------------------------------------------------------------------
		g2dPFD(	envType::UInt32 i_RedMask,
				envType::UInt32 i_GreenMask,
				envType::UInt32 i_BlueMask,
				envType::UInt32 i_AlphaMask,
				int i_BitsPerPixel);

		//--------------------------------------------------------------------
		//	This constructor creates the pixel format from the given data
		//--------------------------------------------------------------------
		g2dPFD(	int i_FirstRedBit,
				int i_NumRedBits,
				int i_FirstGreenBit,
				int i_NumGreenBits,
				int i_FirstBlueBit,
				int i_NumBlueBits,
				int i_FirstAlphaBit,
				int i_NumAlphaBits,
				int i_BitsPerPixel
				);

		//--------------------------------------------------------------------
		//	This Set deduces the pixel format from the given masks
		//--------------------------------------------------------------------
		void Set(	envType::UInt32 i_RedMask,
					envType::UInt32 i_GreenMask,
					envType::UInt32 i_BlueMask,
					envType::UInt32 i_AlphaMask,
					int i_BitsPerPixel);

		//--------------------------------------------------------------------
		//	This Set creates the pixel format from the given data
		//--------------------------------------------------------------------
		void Set(	int i_FirstRedBit,
					int i_NumRedBits,
					int i_FirstGreenBit,
					int i_NumGreenBits,
					int i_FirstBlueBit,
					int i_NumBlueBits,
					int i_FirstAlphaBit,
					int i_NumAlphaBits,
					int i_BitsPerPixel
					);

		//--------------------------------------------------------------------
		//	SetSwizzled()
		//
		//	Sets the flag about whether the texels in the surface/texture have
		//	been reordered for improved locality of reference during the
		//	rendering process.
		//--------------------------------------------------------------------
		inline void SetSwizzled( bool i_bSwizzled );

		//--------------------------------------------------------------------
		//	IsSwizzled()
		//
		//	Returns whether this format is swizzled that is the texels in the
		//	surface/texture have been reordered for improved locality of
		//	reference during the rendering process.
		//--------------------------------------------------------------------
		inline bool IsSwizzled() const;

		//--------------------------------------------------------------------
		//	SetPixelFormat()

		//	Sets pixel format(color, bump map data, etc.)
		//--------------------------------------------------------------------
		inline void SetPixelFormat( PixelFormat i_PixelFormat );

		//--------------------------------------------------------------------
		//	GetPixelFormat()

		//	Returns pixel format(color, bump map data, etc.)
		//--------------------------------------------------------------------
		inline PixelFormat GetPixelFormat() const;

		//--------------------------------------------------------------------
		// SetBitsPerPixel sets the bits per pixel of this format
		//--------------------------------------------------------------------
		inline void SetBitsPerPixel(int i_BitsPerPixel);

		//--------------------------------------------------------------------
		//	BitsPerPixels returns the number of bits in each pixel of this
		//	format.
		//--------------------------------------------------------------------
		int BitsPerPixel() const;

		//--------------------------------------------------------------------
		//	Information about the red bits in the pixel format
		//--------------------------------------------------------------------
		int FirstRedBit() const;
		int NumRedBits() const;

		//--------------------------------------------------------------------
		//	Information about the green bits in the pixel format
		//--------------------------------------------------------------------
		int FirstGreenBit() const;
		int NumGreenBits() const;

		//--------------------------------------------------------------------
		//	Information about the blue bits in the pixel format
		//--------------------------------------------------------------------
		int FirstBlueBit() const;
		int NumBlueBits() const;

		//--------------------------------------------------------------------
		//	Information about the alpha bits in the pixel format
		//--------------------------------------------------------------------
		int FirstAlphaBit() const;
		int NumAlphaBits() const;

		//--------------------------------------------------------------------
		//	MakePixel makes a 32-bit pixel with the format from the given
		//	color.  The alpha section, if present, will be zero.
		//--------------------------------------------------------------------
		envType::UInt32 MakePixel(const g2dRGBColor &i_Color) const;
		envType::UInt32 MakePixel(int i_Red, int i_Green, int i_Blue) const;

		//--------------------------------------------------------------------
		//	MakePixel makes a 32-bit pixel with the format from the given
		//	color.
		//--------------------------------------------------------------------
		envType::UInt32 MakePixel(const g2dARGBColor &i_Color) const;
		envType::UInt32 MakePixel(int i_Red, int i_Green, int i_Blue, int i_Alpha) const;

		//--------------------------------------------------------------------
		//	MakeColor makes a g2dARGBColor from the given pixel
		//--------------------------------------------------------------------
		g2dARGBColor MakeColor(envType::UInt32 i_Pixel) const;

		//--------------------------------------------------------------------
		//	MakeScaledColor makes a g2dARGBColor from the given pixel,
		//	in such a way that the maximum value of a color channel in the
		//	pixel is 255 in the g2dARGBColor.
		//--------------------------------------------------------------------
		g2dARGBColor MakeScaledColor(envType::UInt32 i_Pixel) const;

		//--------------------------------------------------------------------
		//	TransformPixel reformats a pixel from it's given pixel format
		//	to the pixel format of this g2dPFD.  Some color data could
		//	be lost in this operation.
		//--------------------------------------------------------------------
		envType::UInt32 TransformPixel(envType::UInt32 i_Pixel, const g2dPFD& i_PFD) const;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		int GetBitsPerChannel() const;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		int GetNumChannels() const;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		bool IsFloat() const;
	private:

		int m_BitsPerPixel;
		char m_FirstRed, m_NumRed;
		char m_FirstGreen, m_NumGreen;
		char m_FirstBlue, m_NumBlue;
		char m_FirstAlpha, m_NumAlpha;
		PixelFormat m_PixelFormat;
		bool m_bSwizzled;
};

//--------------------------------------------------------------------
//	SetSwizzled()
//
//	Sets the flag about whether the texels in the surface/texture have
//	been reordered for improved locality of reference during the
//	rendering process.
//--------------------------------------------------------------------
inline void g2dPFD::SetSwizzled( bool i_bSwizzled )
{
	m_bSwizzled = i_bSwizzled;
}

//--------------------------------------------------------------------
//	IsSwizzled()
//
//	Returns whether this format is swizzled that is the texels in the
//	surface/texture have been reordered for improved locality of
//	reference during the rendering process.
//--------------------------------------------------------------------
inline bool g2dPFD::IsSwizzled() const
{
	return m_bSwizzled;
}

//--------------------------------------------------------------------
//	SetPixelFormat()

//	Sets pixel format(color, bump map data, etc.)
//--------------------------------------------------------------------
inline void g2dPFD::SetPixelFormat( PixelFormat i_PixelFormat )
{
	m_PixelFormat = i_PixelFormat;
}

//--------------------------------------------------------------------
//	GetPixelFormat()

//	Returns pixel format(color, bump map data, etc.)
//--------------------------------------------------------------------
inline g2dPFD::PixelFormat g2dPFD::GetPixelFormat() const
{
	return m_PixelFormat;
}

//--------------------------------------------------------------------
// SetBitsPerPixel sets the bits per pixel of this format
//--------------------------------------------------------------------
inline void g2dPFD::SetBitsPerPixel(int i_BitsPerPixel)
{
	m_BitsPerPixel = i_BitsPerPixel;
}

//--------------------------------------------------------------------
//	BitsPerPixels returns the number of bits in each pixel of this
//	format.
//--------------------------------------------------------------------
inline int g2dPFD::BitsPerPixel() const
{
	return m_BitsPerPixel;
}

//--------------------------------------------------------------------
//	Information about the red bits in the pixel format
//--------------------------------------------------------------------
inline int g2dPFD::FirstRedBit() const
{
	return m_FirstRed;
}

inline int g2dPFD::NumRedBits() const
{
	return m_NumRed;
}

//--------------------------------------------------------------------
//	Information about the green bits in the pixel format
//--------------------------------------------------------------------
inline int g2dPFD::FirstGreenBit() const
{
	return m_FirstGreen;
}

inline int g2dPFD::NumGreenBits() const
{
	return m_NumGreen;
}

//--------------------------------------------------------------------
//	Information about the blue bits in the pixel format
//--------------------------------------------------------------------
inline int g2dPFD::FirstBlueBit() const
{
	return m_FirstBlue;
}

inline int g2dPFD::NumBlueBits() const
{
	return m_NumBlue;
}

//--------------------------------------------------------------------
//	Information about the alpha bits in the pixel format
//--------------------------------------------------------------------
inline int g2dPFD::FirstAlphaBit() const
{
	return m_FirstAlpha;
}

inline int g2dPFD::NumAlphaBits() const
{
	return m_NumAlpha;
}

//--------------------------------------------------------------------
//	MakePixel makes a 32-bit pixel with the format from the given
//	color.  The alpha section, if present, will be zero.
//--------------------------------------------------------------------
inline envType::UInt32 g2dPFD::MakePixel(int i_Red, int i_Green, int i_Blue) const
{
	envType::UInt32 red, green, blue;

	red = i_Red;
	red >>= 8 - m_NumRed;
	red <<= m_FirstRed;

	green = i_Green;
	green >>= 8 - m_NumGreen;
	green <<= m_FirstGreen;

	blue = i_Blue;
	blue >>= 8 - m_NumBlue;
	blue <<= m_FirstBlue;

	return red | green | blue;
}

inline envType::UInt32 g2dPFD::MakePixel(const g2dRGBColor &i_Color) const
{
	return this->MakePixel(i_Color.GetRed(), i_Color.GetGreen(), i_Color.GetBlue());
}

//--------------------------------------------------------------------
//	MakePixel makes a 32-bit pixel with the format from the given
//	color.
//--------------------------------------------------------------------
inline envType::UInt32 g2dPFD::MakePixel(int i_Red, int i_Green, int i_Blue, int i_Alpha) const
{
	envType::UInt32 red, green, blue, alpha;

	red = i_Red;
	red >>= 8 - m_NumRed;
	red <<= m_FirstRed;

	green = i_Green;
	green >>= 8 - m_NumGreen;
	green <<= m_FirstGreen;

	blue = i_Blue;
	blue >>= 8 - m_NumBlue;
	blue <<= m_FirstBlue;

	alpha = i_Alpha;
	alpha >>= 8 - m_NumAlpha;
	alpha <<= m_FirstAlpha;

	return red | green | blue | alpha;
}

inline envType::UInt32 g2dPFD::MakePixel(const g2dARGBColor &i_Color) const
{
	return this->MakePixel(i_Color.GetRed(), i_Color.GetGreen(), i_Color.GetBlue(), i_Color.GetAlpha());
}

