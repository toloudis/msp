/****************************************************************************\
**  g2dWindowDrawUtilDX11.hpp
**
**      g2dWindowDrawUtilDX11.hpp provides drawing utilities for D3D
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifdef G2D_WINDOWDRAWUTILD3D11_HPP
#error g2dWindowDrawUtilDX11.hpp multiply included
#endif
#define G2D_WINDOWDRAWUTILD3D11_HPP

#ifndef G2D_DX11TYPES_HPP
#include "GraphicsDX11/g2d/g2dDX11Types.hpp"
#endif
#ifndef G2D_FONTHANDLE_HPP
#include "Graphics/g2d/g2dFontHandle.hpp"
#endif
#ifndef MA_FLOATRGBA_HPP
#include "Core/ma/maFloatRGBA.hpp"
#endif


//============================================================================
//============================================================================
class itString;
class g2dImage;
class g2dRGBColor;


//============================================================================
//============================================================================
namespace g2dWindowDrawUtilDX11
{
	//------------------------------------------------------------------------
	//	Clear fills the screen with the given color.
	//------------------------------------------------------------------------
	void Clear(const g2dRGBColor& i_Color);

	//------------------------------------------------------------------------
	//	Clear fills the screen with the given color.
	//------------------------------------------------------------------------
	void Clear(const maFloatRGBA& i_Color, bool i_ClearDepth = false, float i_Depth = 1, bool i_ClearStencil = true, unsigned int i_Stencil = 0);

	//------------------------------------------------------------------------
	//	DrawRect draws a rectangle of the given color between the two locations
	//	given (in client coordinates).  The rectangle will include the
	//	rows at i_X1 and i_Y1, but not i_X2 and i_Y2.  This is convenient
	//	for spacing adjacent rectangles.
	//	The rectangle will be silently clipped or rejected by the current
	//	screen viewing area.
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
	void DrawImage(	g2dD3D11RenderTargetPtr i_DestSurface,
				   int i_ScreenWidth, int i_ScreenHeight,
					int i_DX1,
					int i_DY1,
					const g2dImage& i_Image);

	//------------------------------------------------------------------------
	//	This DrawImage blits a rectangular section of the given image to the
	//	screen.  The rectangle of the source image is defined by
	//	(i_SX1, i_SY1) - (i_SX2, i_SY2).  The point (i_SX1, i_SY1) will be
	//	located at (i_DX1, i_DY1).
	//------------------------------------------------------------------------
	void DrawImage(	g2dD3D11RenderTargetPtr i_DestSurface,
				   int i_ScreenWidth, int i_ScreenHeight,
					int i_DX1,
					int i_DY1,
					int i_SX1,
					int i_SY1,
					int i_SX2,
					int i_SY2,
					const g2dImage& i_Image);
}
