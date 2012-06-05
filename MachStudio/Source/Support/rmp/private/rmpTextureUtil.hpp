/*****************************************************************************
**	rmpTextureUtil.hpp
**
**	Utility for color conversion.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef RMP_TEXTUREUTIL_HPP
#error rmpTextureUtil.hpp multiply included
#endif
#define RMP_TEXTUREUTIL_HPP


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
namespace rmpTextureUtil 
{	
	//--------------------------------------------------------------------
	// Converts RGB to HSL 
	// H is from 0-1, S and V are from 0-1,
	// R,G,B are all 0-1
	//--------------------------------------------------------------------
	void RGB_to_HSL(double i_Rgb_R, double i_Rgb_G, double i_Rgb_B,
					double &o_Hsl_H, double &o_Hsl_S, double &o_Hsl_L);

	//--------------------------------------------------------------------
	// Converts a colour from HSL to RGB 
	// H is from 0-1, S and V are from 0-1,
	// R,G,B are all 0-1
	//--------------------------------------------------------------------
	void HSL_to_RGB(double i_Hsl_H, double i_Hsl_S, double i_Hsl_L,
					double &o_Rgb_R, double &o_Rgb_G, double &o_Rgb_B);
}

