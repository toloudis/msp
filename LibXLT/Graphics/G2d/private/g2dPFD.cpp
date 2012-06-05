/****************************************************************************\
**  g2dPFD.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Graphics/g2d/g2dPFD.hpp"

#include "Core/dbg/dbgMsg.hpp"


//============================================================================
//============================================================================
namespace
{
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void find_bit_span(char& o_FirstBit, char& o_NumBits, envType::UInt32 i_Mask)
{
	if( i_Mask == 0 )
	{
		o_FirstBit = 0;
		o_NumBits = 0;
		return;
	}

	// find first bit
	envType::UInt32 test = 1;
	int i;

	for( i = 0 ; i < 32 ; i++ )
	{
		if( test & i_Mask )
			break;

		test <<= 1;
	}

	o_FirstBit = i;
	i++;
	test <<= 1;

	for( ; i < 32 ; i++ )
	{
		if( (test & i_Mask) == 0 )
			break;

		test <<= 1;
	}

	o_NumBits = i - o_FirstBit;
}

}

//--------------------------------------------------------------------
//	This constructor makes a g2dPFD with all zeroes (invalid).
//--------------------------------------------------------------------
g2dPFD::g2dPFD()
:	m_BitsPerPixel(0),
	m_FirstRed(0),
	m_NumRed(0),
	m_FirstGreen(0),
	m_NumGreen(0),
	m_FirstBlue(0),
	m_NumBlue(0),
	m_FirstAlpha(0),
	m_NumAlpha(0),
	m_bSwizzled(false),
	m_PixelFormat(e_Color)
{
}

//--------------------------------------------------------------------
//	This constructor makes a g2dPFD with the given pixel format.
//--------------------------------------------------------------------
g2dPFD::g2dPFD( PixelFormat i_PixelFormat, int i_BitsPerPixel )
:	m_BitsPerPixel(i_BitsPerPixel),
	m_FirstRed(0),
	m_NumRed(0),
	m_FirstGreen(0),
	m_NumGreen(0),
	m_FirstBlue(0),
	m_NumBlue(0),
	m_FirstAlpha(0),
	m_NumAlpha(0),
	m_bSwizzled(false),
	m_PixelFormat(i_PixelFormat)
{
}

//--------------------------------------------------------------------
//	This constructor deduces the pixel format from the given masks.
//--------------------------------------------------------------------
g2dPFD::g2dPFD(	envType::UInt32 i_RedMask,
				envType::UInt32 i_GreenMask,
				envType::UInt32 i_BlueMask,
				envType::UInt32 i_AlphaMask,
				int i_BitsPerPixel)
:	m_BitsPerPixel(i_BitsPerPixel),
	m_bSwizzled(false),
	m_PixelFormat(e_Color)
{
	find_bit_span(m_FirstRed, m_NumRed, i_RedMask);
	find_bit_span(m_FirstGreen, m_NumGreen, i_GreenMask);
	find_bit_span(m_FirstBlue, m_NumBlue, i_BlueMask);
	find_bit_span(m_FirstAlpha, m_NumAlpha, i_AlphaMask);
}

//--------------------------------------------------------------------
//	This constructor creates the pixel format from the given data
//--------------------------------------------------------------------
g2dPFD::g2dPFD(	int i_FirstRedBit,
				int i_NumRedBits,
				int i_FirstGreenBit,
				int i_NumGreenBits,
				int i_FirstBlueBit,
				int i_NumBlueBits,
				int i_FirstAlphaBit,
				int i_NumAlphaBits,
				int i_BitsPerPixel
		)
:	m_BitsPerPixel(i_BitsPerPixel),
	m_FirstRed(i_FirstRedBit),
	m_NumRed(i_NumRedBits),
	m_FirstGreen(i_FirstGreenBit),
	m_NumGreen(i_NumGreenBits),
	m_FirstBlue(i_FirstBlueBit),
	m_NumBlue(i_NumBlueBits),
	m_FirstAlpha(i_FirstAlphaBit),
	m_NumAlpha(i_NumAlphaBits),
	m_bSwizzled(false),
	m_PixelFormat(e_Color)
{
}

//--------------------------------------------------------------------
//	This Set deduces the pixel format from the given masks
//--------------------------------------------------------------------
void g2dPFD::Set(	envType::UInt32 i_RedMask,
					envType::UInt32 i_GreenMask,
					envType::UInt32 i_BlueMask,
					envType::UInt32 i_AlphaMask,
					int i_BitsPerPixel)
{
	m_BitsPerPixel = i_BitsPerPixel;
	find_bit_span(m_FirstRed, m_NumRed, i_RedMask);
	find_bit_span(m_FirstGreen, m_NumGreen, i_GreenMask);
	find_bit_span(m_FirstBlue, m_NumBlue, i_BlueMask);
	find_bit_span(m_FirstAlpha, m_NumAlpha, i_AlphaMask);
}


//--------------------------------------------------------------------
//	This Set creates the pixel format from the given data
//--------------------------------------------------------------------
void g2dPFD::Set(	int i_FirstRedBit,
					int i_NumRedBits,
					int i_FirstGreenBit,
					int i_NumGreenBits,
					int i_FirstBlueBit,
					int i_NumBlueBits,
					int i_FirstAlphaBit,
					int i_NumAlphaBits,
					int i_BitsPerPixel
			)
{
	m_BitsPerPixel = i_BitsPerPixel;
	m_FirstRed = i_FirstRedBit;
	m_NumRed = i_NumRedBits;
	m_FirstGreen = i_FirstGreenBit;
	m_NumGreen = i_NumGreenBits;
	m_FirstBlue = i_FirstBlueBit;
	m_NumBlue = i_NumBlueBits;
	m_FirstAlpha = i_FirstAlphaBit;
	m_NumAlpha = i_NumAlphaBits;
}

//--------------------------------------------------------------------
//	MakeColor makes a g2dARGBColor from the given pixel
//--------------------------------------------------------------------
g2dARGBColor g2dPFD::MakeColor(envType::UInt32 i_Pixel) const
{
	envType::UInt32 red, green, blue, alpha;

	red = (i_Pixel >> m_FirstRed) & ((0x01 << m_NumRed) - 1);
	green = (i_Pixel >> m_FirstGreen) & ((0x01 << m_NumGreen) - 1);
	blue = (i_Pixel >> m_FirstBlue) & ((0x01 << m_NumBlue) - 1);

	if( m_NumAlpha )
		alpha = (i_Pixel >> m_FirstAlpha) & ((0x01 << m_NumAlpha) - 1);
	else
		alpha = 0;

	return g2dARGBColor(red, green, blue, alpha);
}

//--------------------------------------------------------------------
//	MakeScaledColor makes a g2dARGBColor from the given pixel,
//	in such a way that the maximum value of a color channel in the
//	pixel is 255 in the g2dARGBColor.
//--------------------------------------------------------------------
g2dARGBColor g2dPFD::MakeScaledColor(envType::UInt32 i_Pixel) const
{
	envType::UInt32 red, green, blue, alpha;

	red = (i_Pixel >> m_FirstRed) & ((0x01 << m_NumRed) - 1);
	green = (i_Pixel >> m_FirstGreen) & ((0x01 << m_NumGreen) - 1);
	blue = (i_Pixel >> m_FirstBlue) & ((0x01 << m_NumBlue) - 1);

	if( m_NumAlpha )
		alpha = (i_Pixel >> m_FirstAlpha) & ((0x01 << m_NumAlpha) - 1);
	else
		alpha = 0;

	red <<= 8 - m_NumRed;
	green <<= 8 - m_NumGreen;
	blue <<= 8 - m_NumBlue;
	alpha <<= 8 - m_NumAlpha;

	return g2dARGBColor(red, green, blue, alpha);
}

//--------------------------------------------------------------------
//	TransformPixel reformats a pixel from it's given pixel format
//	to the pixel format of this g2dPFD.  Some color data could
//	be lost in this operation.
//--------------------------------------------------------------------
envType::UInt32 g2dPFD::TransformPixel(envType::UInt32 i_Pixel, const g2dPFD& i_PFD) const
{
	envType::UInt32 red, green, blue, alpha;

	red = (i_Pixel >> i_PFD.m_FirstRed) & ((0x01 << i_PFD.m_NumRed) - 1);
	green = (i_Pixel >> i_PFD.m_FirstGreen) & ((0x01 << i_PFD.m_NumGreen) - 1);
	blue = (i_Pixel >> i_PFD.m_FirstBlue) & ((0x01 << i_PFD.m_NumBlue) - 1);

	if( m_NumAlpha )
		alpha = (i_Pixel >> i_PFD.m_FirstAlpha) & ((0x01 << i_PFD.m_NumAlpha) - 1);
	else
		alpha = 0;

	red >>= 8 - m_NumRed;
	red <<= m_FirstRed;

	green >>= 8 - m_NumGreen;
	green <<= m_FirstGreen;

	blue >>= 8 - m_NumBlue;
	blue <<= m_FirstBlue;

	alpha >>= 8 - m_NumAlpha;
	alpha <<= m_FirstAlpha;

	return red | green | blue | alpha;

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
int g2dPFD::GetBitsPerChannel() const
{
	switch (m_PixelFormat)
	{
		case e_Color:
		case e_RColor:
			return 8;
		case e_Float16:
		case e_RGBA16f:
		case e_RGBA16UInt:
			return 16;
		case e_Float32:
		case e_RGBA32f:
		case e_GR32f:
			return 32;
	};
	DBG_ASSERT(false, "BitsPerChannel unsupported for PixelFormat " << m_PixelFormat);
	return 0;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
int g2dPFD::GetNumChannels() const
{
	switch (m_PixelFormat)
	{
		case e_Color:
		case e_RColor:
		case e_RGBA16f:
		case e_RGBA32f:
		case e_RGBA16UInt:
			return 4;
		case e_Float16:
		case e_Float32:
			return 1;
		case e_GR32f:
			return 2;
	};
	DBG_ASSERT(false, "NumChannels unsupported for PixelFormat " << m_PixelFormat);
	return 0;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
bool g2dPFD::IsFloat() const
{
	switch (m_PixelFormat)
	{
		case e_RGBA16f:
		case e_RGBA32f:
		case e_Float16:
		case e_Float32:
		case e_GR32f:
			return true;
		case e_Color:
		case e_RColor:
		case e_RGBA16UInt:
			return false;
	};
	DBG_ASSERT(false, "IsFloat unsupported for PixelFormat " << m_PixelFormat);
	return false;
}
