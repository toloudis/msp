/*****************************************************************************
**	twcColorUtil.cpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Support/rmp/private/rmpTextureUtil.hpp"

#include "Core/dbg/dbgMsg.hpp"
#include "Core/ma/maConstants.hpp"

#include <math.h>

namespace rmpTextureUtil 
{
	// According to the definition from wikipedia
	// H is from 0-1, S and L are from 0-1,
	// R,G,B are all 0-1

	//--------------------------------------------------------------------
	// Converts RGB to HSL
	//--------------------------------------------------------------------
	void RGB_to_HSL(double i_Rgb_R, double i_Rgb_G, double i_Rgb_B,
					double &o_Hsl_H, double &o_Hsl_S, double &o_Hsl_L) 
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

		const double lightness = 0.5f * (maximumRGB + minimumRGB) ;

		double hue = 0.0, saturation;
		const double deltaRGB = maximumRGB - minimumRGB;
		if ( maximumRGB == minimumRGB )
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
					DBG_ASSERT(false, "RGB_to_HSL: hue not specified");
					break;
			}

			hue /= 6.0;

			if ( hue < 0.0 )
				hue += 1.0;

			if (lightness <= 0.5f)
			{
				saturation = deltaRGB / (2.0f * lightness);
			}
			else
			{
				saturation = deltaRGB / (2.0f - 2.0f * lightness);
			}
		}

		// Return values as doubles
		o_Hsl_H = hue;
		o_Hsl_S = saturation;
		o_Hsl_L = lightness;
	}

	//--------------------------------------------------------------------
	// Converts a colour from HSL to RGB 
	// H is from 0-1, S and V are from 0-1,
	// R,G,B are all 0-1
	//--------------------------------------------------------------------
	void HSL_to_RGB(double i_Hsl_H, double i_Hsl_S, double i_Hsl_L,
					double &o_Rgb_R, double &o_Rgb_G, double &o_Rgb_B) 
	{
		double color[3]; // R, G, B 

		double hsl_hue = i_Hsl_H;
		double hsl_saturation = i_Hsl_S;
		double hsl_lightness = i_Hsl_L;

		if ( i_Hsl_S == 0 )
		{
			// Grey
			color[0] = color[1] = color[2] = hsl_lightness;
		}
		else // not grey
		{
			double p, q;
			if (hsl_lightness < 0.5f)
			{
				q = hsl_lightness * (hsl_lightness + hsl_saturation);
			}
			else
			{
				q = hsl_lightness + hsl_saturation - (hsl_lightness * hsl_saturation);
			}
			p = 2 * hsl_lightness - q;
			
			color[0] = hsl_hue + (1.0f / 3.0f); 
			color[1] = hsl_hue;
			color[2] = hsl_hue - (1.0f / 3.0f); 

			color[0] = fmod(color[0], 1.0);
			color[1] = fmod(color[1], 1.0);
			color[2] = fmod(color[2], 1.0);

			for (int i = 0; i < 3; i++)
			{
				if (color[i] < (1.0f / 6.0f))
				{
					color[i] = p + ((q - p) * 6 * color[i]);
				}else if (color[i] >= (1.0f / 6.0f) && color[i] < 0.5f )
				{
					color[i] = q;
				}else if (color[i] >= 0.5f && color[i] < (2.0f / 3.0f))
				{
					color[i] = p + ((q - p) * 6 * (2.0f/3.0f - color[i]));
				}else
				{
					color[i] = p;
				}
			}
		}

		o_Rgb_R = color[0];
		o_Rgb_G = color[1];
		o_Rgb_B = color[2];
	}


}	// end of namespace

