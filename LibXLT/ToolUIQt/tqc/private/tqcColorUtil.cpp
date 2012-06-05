/*****************************************************************************
**	tqcColorUtil.cpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/tqc/private/tqcColorUtil.hpp"

#include "Core/dbg/dbgMsg.hpp"
#include "Core/ma/maConstants.hpp"

#include <math.h>

namespace tqcColorUtil
{
	// These color conversion algorithms are from wxImage:
	// H is from 0-1, S and V are from 0-1,
	// R,G,B are all 0-1

	//--------------------------------------------------------------------
	// Converts RGB to HSV 
	//--------------------------------------------------------------------
	void RGB_to_HSV(double i_Rgb_R, double i_Rgb_G, double i_Rgb_B,
					double &o_Hsv_H, double &o_Hsv_S, double &o_Hsv_V) 
	{
		const double red = i_Rgb_R, green = i_Rgb_G, blue = i_Rgb_B;

		// find the min and max intensity (and remember which one was it for the
		// latter)
		double minimumRGB = red;
		if ( green < minimumRGB )
			minimumRGB = green;
		if ( blue < minimumRGB )
			minimumRGB = blue;

		enum { RED, GREEN, BLUE } chMax = RED;
		double maximumRGB = red;
		if ( green > maximumRGB )
		{
			chMax = GREEN;
			maximumRGB = green;
		}
		if ( blue > maximumRGB )
		{
			chMax = BLUE;
			maximumRGB = blue;
		}

		const double value = maximumRGB;

		double hue = 0.0, saturation;
		const double deltaRGB = maximumRGB - minimumRGB;
		if ( abs(deltaRGB) < maConstants::c_dEpsilon )
		{
			// Gray has no color
			hue = 0.0;
			saturation = 0.0;
		}
		else
		{
			switch ( chMax )
			{
				case RED:
					hue = (green - blue) / deltaRGB;
					break;

				case GREEN:
					hue = 2.0 + (blue - red) / deltaRGB;
					break;

				case BLUE:
					hue = 4.0 + (red - green) / deltaRGB;
					break;

				default:
					DBG_ASSERT(false, "RGB_to_HSV: hue not specified");
					break;
			}

			hue /= 6.0;

			if ( hue < 0.0 )
				hue += 1.0;

			saturation = deltaRGB / maximumRGB;
		}

		// Return values as doubles
		o_Hsv_H = hue;
		o_Hsv_S = saturation;
		o_Hsv_V = value;
	}

	//--------------------------------------------------------------------
	// Converts a colour from HSV to RGB 
	// H is from 0-1, S and V are from 0-1,
	// R,G,B are all 0-1
	//--------------------------------------------------------------------
	void HSV_to_RGB(double i_Hsv_H, double i_Hsv_S, double i_Hsv_V,
					double &o_Rgb_R, double &o_Rgb_G, double &o_Rgb_B) 
	{
		double red, green, blue;

		double hsv_hue = i_Hsv_H;
		double hsv_saturation = i_Hsv_S;
		double hsv_value = i_Hsv_V;

		if ( i_Hsv_S == 0 )
		{
			// Grey
			red = green = blue = hsv_value;
		}
		else // not grey
		{
			double hue = hsv_hue * 6.0;      // sector 0 to 5
			int i = (int)floor(hue);
			double f = hue - i;          // fractional part of h
			double p = hsv_value * (1.0 - hsv_saturation);

			switch (i)
			{
				case 0:
					red = hsv_value;
					green = hsv_value * (1.0 - hsv_saturation * (1.0 - f));
					blue = p;
					break;

				case 1:
					red = hsv_value * (1.0 - hsv_saturation * f);
					green = hsv_value;
					blue = p;
					break;

				case 2:
					red = p;
					green = hsv_value;
					blue = hsv_value * (1.0 - hsv_saturation * (1.0 - f));
					break;

				case 3:
					red = p;
					green = hsv_value * (1.0 - hsv_saturation * f);
					blue = hsv_value;
					break;

				case 4:
					red = hsv_value * (1.0 - hsv_saturation * (1.0 - f));
					green = p;
					blue = hsv_value;
					break;

				default:    // case 5:
					red = hsv_value;
					green = p;
					blue = hsv_value * (1.0 - hsv_saturation * f);
					break;
			}
		}

		//o_Rgb_R = (int)(red * 255);
		//o_Rgb_G = (int)(green * 255);
		//o_Rgb_B = (int)(blue* 255);
		o_Rgb_R = red;
		o_Rgb_G = green;
		o_Rgb_B = blue;
	}


}	// end of namespace

