/****************************************************************************\
**  g2dWindowDrawUtilVk.hpp
**
**      g2dWindowDrawUtilVk provides drawing utilities for windows: the port
**	of GraphicsDX11's g2dWindowDrawUtilDX11 (its Clear functions are
**	g2dVkGlobal::ClearCurrent).
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GVK_G2DWINDOWDRAWUTILVK_HPP
#error g2dWindowDrawUtilVk.hpp multiply included
#endif
#define GVK_G2DWINDOWDRAWUTILVK_HPP

#ifndef G2D_FONTHANDLE_HPP
#include "Graphics/G2d/g2dFontHandle.hpp"
#endif

#undef DrawText

class g2dImage;
class g2dRGBColor;
class itString;

namespace gvk
{
	class vkdImage;

	namespace g2dWindowDrawUtilVk
	{
		//------------------------------------------------------------------------
		//	DrawRect draws a rectangle of the given color between the two locations
		//	given (in client coordinates).  The rectangle will include the
		//	rows at i_X1 and i_Y1, but not i_X2 and i_Y2.  This is convenient
		//	for spacing adjacent rectangles.
		//	The rectangle will be silently clipped or rejected by the current
		//	screen viewing area.
		//	(Not implemented, as in DX11: logs an error and draws nothing.)
		//------------------------------------------------------------------------
		void DrawRect(	int i_X1,
						int i_Y1,
						int i_X2,
						int i_Y2,
						const g2dRGBColor& i_Color);

		//------------------------------------------------------------------------
		//	DrawText draws the given text such that it's upper left corner is
		//	at (i_X1, i_Y1).  It will be silently clipped or rejected by the
		//	current viewing area.
		//	(As in DX11, the colour's 0..255 components go to the shader as
		//	they are, not divided by 255: g2dWindow::DrawText divides them.)
		//------------------------------------------------------------------------
		void DrawText(	int i_X,
						int i_Y,
						g2dFontHandle i_Font,
						const itString& i_Text,
						const g2dRGBColor& i_Color);

		//------------------------------------------------------------------------
		//	This DrawImage blits the given image to the screen so that it's upper left
		//	corner will be located at (i_DX1, i_DY1).
		//------------------------------------------------------------------------
		void DrawImage(	vkdImage* i_pDestSurface,
						int i_ScreenWidth, int i_ScreenHeight,
						int i_DX1,
						int i_DY1,
						const g2dImage& i_Image);

		//------------------------------------------------------------------------
		//	This DrawImage blits a rectangular section of the given image to the
		//	screen.  The rectangle of the source image is defined by
		//	(i_SX1, i_SY1) - (i_SX2, i_SY2).  The point (i_SX1, i_SY1) will be
		//	located at (i_DX1, i_DY1).
		//	The image's pixels are copied as they are (D3D11's
		//	CopySubresourceRegion), so its format must be the surface's; an
		//	8-bit RGBA image goes to a BGRA swap chain with red and blue
		//	swapped, as the DX11 window is always RGBA.
		//------------------------------------------------------------------------
		void DrawImage(	vkdImage* i_pDestSurface,
						int i_ScreenWidth, int i_ScreenHeight,
						int i_DX1,
						int i_DY1,
						int i_SX1,
						int i_SY1,
						int i_SX2,
						int i_SY2,
						const g2dImage& i_Image);
	}
}
