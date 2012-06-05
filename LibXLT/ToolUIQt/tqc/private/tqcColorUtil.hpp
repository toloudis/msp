/*****************************************************************************
**	tqcColorUtil.hpp
**
**	Utility for color conversion.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef TQC_COLORUTIL_HPP
#error tqcColorUtil.hpp multiply included
#endif
#define TQC_COLORUTIL_HPP


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
namespace tqcColorUtil 
{	
	//--------------------------------------------------------------------
	// Converts RGB to HSV 
	// H is from 0-1, S and V are from 0-1,
	// R,G,B are all 0-1
	//--------------------------------------------------------------------
	void RGB_to_HSV(double i_Rgb_R, double i_Rgb_G, double i_Rgb_B,
					double &o_Hsv_H, double &o_Hsv_S, double &o_Hsv_V);

	//--------------------------------------------------------------------
	// Converts a colour from HSV to RGB 
	// H is from 0-1, S and V are from 0-1,
	// R,G,B are all 0-1
	//--------------------------------------------------------------------
	void HSV_to_RGB(double i_Hsv_H, double i_Hsv_S, double i_Hsv_V,
					double &o_Rgb_R, double &o_Rgb_G, double &o_Rgb_B);
}

