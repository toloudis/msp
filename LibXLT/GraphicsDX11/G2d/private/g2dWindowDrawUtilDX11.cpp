/****************************************************************************\
**  g2dWindowDrawUtilDX11.cpp
**
**      g2dWindowDrawUtilDX11.cpp provides drawing utilities for D3D
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#include "GraphicsDX11/g2d/private/g2dWindowDrawUtilDX11.hpp"

#include "Core/dbg/dbgMsg.hpp"
#include "Graphics/g2d/g2dExceptionX.hpp"
#include "GraphicsDX11/g2d/g2dDX11GlobalWin.hpp"
#include "GraphicsDX11/g2d/private/g2dFontUtilDX11.hpp"
#include "GraphicsDX11/g2d/g2dImageDX11.hpp"
#include "GraphicsDX11/g2d/private/g2dImageDrawUtilDX11.hpp"
#include "GraphicsDX11/g3d/g3dStateMgr.hpp"


namespace g2dWindowDrawUtilDX11
{

//------------------------------------------------------------------------
//	Clear fills the screen with the given color.
//------------------------------------------------------------------------
void Clear(const g2dRGBColor& i_Color)
{
	// assume g2dDX11Global knows the current target to clear

	float rgba[4] = {i_Color.GetRed()/255.0f, i_Color.GetGreen()/255.0f, i_Color.GetBlue()/255.0f,0};
	g2dDX11Global::g_pDeviceContext->ClearRenderTargetView(
		g2dDX11Global::GetColorTarget(), rgba);
}


//------------------------------------------------------------------------
//	Clear fills the screen with the given color.
//
//	NOTE: the 3 multiples are inefficient.
//
// TODO: fix the float to int conversion of the color
//------------------------------------------------------------------------
void Clear(const maFloatRGBA& i_Color, bool i_ClearDepth /*=false*/, float i_Depth /*= 1*/, bool i_ClearStencil /*= true*/, unsigned int i_Stencil /*= 0*/)
{
	// assume g2dDX11Global knows the current target to clear

	float rgba[4] = {i_Color.GetRed(), i_Color.GetGreen(), i_Color.GetBlue(), i_Color.GetAlpha()};
	g2dDX11Global::g_pDeviceContext->ClearRenderTargetView(
		g2dDX11Global::GetColorTarget(), rgba);

	if( i_ClearDepth )
	{
		g2dDX11Global::g_pDeviceContext->ClearDepthStencilView(
			g2dDX11Global::GetDepthTarget(), 
			D3D11_CLEAR_DEPTH | ( (i_ClearStencil && g2dDX11Global::g_bHasStencil) ? D3D11_CLEAR_STENCIL : 0),
			i_Depth, (UINT8)i_Stencil);
	}
}


//----------------------------------------------------------------------------
//	DrawRect draws a rectangle of the given color between the two locations
//	given (in client coordinates).  The rectangle will include the
//	rows at i_X1 and i_Y1, but not i_X2 and i_Y2.  This is convenient
//	for spacing adjacent rectangles.
//	The rectangle will be silently clipped or rejected by the current
//	screen viewing area.
//----------------------------------------------------------------------------
void DrawRect(	int i_X1,
				int i_Y1,
				int i_X2,
				int i_Y2,
				const g2dRGBColor& i_Color)
{
	// should draw a screen aligned quad here.
	DBG_ERROR("g2dWindowDrawUtilDX11::DrawRect not implemented!");
/*
D3DRECT rect;
	rect.x1 = i_X1;
	rect.x2 = i_X2;
	rect.y1 = i_Y1;
	rect.y2 = i_Y2;

	HRESULT op_result = g2dDX11Global::g_pDevice->Clear(	1,
														&rect,
														D3DCLEAR_TARGET,
														D3DCOLOR_XRGB(i_Color.GetRed(), i_Color.GetGreen(), i_Color.GetBlue()),
														0.0f,
														0);
	if( op_result != D3D_OK )
	{
		g2dDX11Global::PrintDXError(op_result);
		DBG_ASSERT0(op_result == D3D_OK, "Clear Failed");
		return;
	}
*/
}

//----------------------------------------------------------------------------
//	DrawText draws the given text such that it's upper left corner is
//	at (i_X1, i_Y1).  It will be silently clipped or rejected by the
//	current viewing area.
//----------------------------------------------------------------------------
void DrawText(	int i_X,
				int i_Y,
				g2dFontHandle i_Font,
				const itString& i_Text,
				const g2dRGBColor& i_Color)
{
	//DBG_ASSERT(false, "DrawText not implemented");

	g2dFontObjectDX11* font = reinterpret_cast<g2dFontObjectDX11*>(i_Font);
	if (font)
		font->DrawText(i_Text, maFloatRGBA(i_Color.GetRed(), i_Color.GetGreen(), i_Color.GetBlue(), 1.0f), i_X, i_Y);
#if 0
	float dpiScaleX, dpiScaleY;
	m_pBackBuffer2D->GetDpi(&dpiScaleX, &dpiScaleY);

    // Create a D2D rect that is the same size as the window.
	int width, height;
	g2dFontUtil::GetTextSizeInPixels(i_Font, i_Text, width, height);
	D2D1_RECT_F layoutRect = D2D1::RectF(
		static_cast<FLOAT>(i_X) / dpiScaleX,
		static_cast<FLOAT>(i_Y) / dpiScaleY,
		static_cast<FLOAT>(i_X + width) / dpiScaleX,
		static_cast<FLOAT>(i_Y + height) / dpiScaleY
		);

	m_pSolidBrush2D->SetColor(D2D1::ColorF(i_Color.GetRed()/255.0f, 
		i_Color.GetGreen()/255.0f, i_Color.GetBlue()/255.0f));

    // Use the DrawText method of the D2D render target interface to draw.

    m_pBackBuffer2D->DrawText(
        i_Text.GetString(),        // The string to render.
        i_Text.GetLength(),    // The string's length.
        reinterpret_cast<IDWriteTextFormat*>(i_Font),    // The text format.
        layoutRect,       // The region of the window where the text will be rendered.
        m_pSolidBrush2D     // The brush used to draw the text.
	);
#endif
}

//----------------------------------------------------------------------------
//	This DrawImage blits the given image to the screen so that it's upper left
//	corner will be located at (i_DX1, i_DY1).
//----------------------------------------------------------------------------
void DrawImage(	g2dD3D11RenderTargetPtr i_DestSurface,
			   int i_ScreenWidth, int i_ScreenHeight,
			    int i_DX1,
				int i_DY1,
				const g2dImage& i_Image)
{
	DrawImage(i_DestSurface, i_ScreenWidth, i_ScreenHeight, 
		i_DX1, i_DY1, 0, 0, i_Image.GetWidth(), i_Image.GetHeight(), i_Image);
}

//----------------------------------------------------------------------------
//	This DrawImage blits a rectangular section of the given image to the
//	screen.  The rectangle of the source image is defined by
//	(i_SX1, i_SY1) - (i_SX2, i_SY2).  The point (i_SX1, i_SY1) will be
//	located at (i_DX1, i_DY1).
//----------------------------------------------------------------------------
void DrawImage(	g2dD3D11RenderTargetPtr i_DestSurface,
			   int i_ScreenWidth, int i_ScreenHeight,
			    int i_DX1,
				int i_DY1,
				int i_SX1,
				int i_SY1,
				int i_SX2,
				int i_SY2,
				const g2dImage& i_Image)
{
	const g2dImageDX11 *src_image = dynamic_cast<const g2dImageDX11*>(&i_Image);
	DBG_ASSERT(src_image, "Image needs to be D3D Image");
	g2dD3D11TexturePtr source_surface = src_image->GetSurface();
	DBG_ASSERT(source_surface, "Image has no surface");

	int screen_width = i_ScreenWidth;
	int screen_height = i_ScreenHeight;

	int source_width = i_SX2 - i_SX1;
	int source_height = i_SY2 - i_SY1;

	// do some clipping and rejection
	if( i_DX1 < 0 )
	{
		if( i_DX1 <= -source_width )
			return; // off the left edge

		i_SX1 -= i_DX1;
		source_width += i_DX1;
		i_DX1 = 0;
	}

	if ( i_DY1 < 0 )
	{
		if( i_DY1 <= -source_height )
			return; // off the top edge

		i_SY1 -= i_DY1;
		source_height += i_DY1;
		i_DY1 = 0;
	}

	int width_over_limit = i_DX1 + source_width - screen_width;
	if ( width_over_limit > 0 )
	{
		if( i_DX1 >= screen_width )
			return;	// off the right edge

		i_SX2 -= width_over_limit;
		source_width -= width_over_limit;
	}

	int height_over_limit = i_DY1 + source_height - screen_height;
	if ( height_over_limit > 0 )
	{
		if( i_DY1 >= screen_height )
			return;	// off the right edge

		i_SY2 -= height_over_limit;
		source_height -= height_over_limit;
	}

	DBG_ASSERT(source_width > 0, "Invalid image area");
	DBG_ASSERT(source_height > 0, "Invalid image area");

	D3D11_BOX source_rect;
	source_rect.left = i_SX1;
	source_rect.top = i_SY1;
	source_rect.right = i_SX2;
	source_rect.bottom = i_SY2;
	source_rect.front = 0;
	source_rect.back = 1;

	ID3D11Resource* pDest = NULL;
	i_DestSurface->GetResource(&pDest);
	g2dDX11Global::g_pDeviceContext->CopySubresourceRegion(pDest, 
	  D3D11CalcSubresource(0,0,1),
	  i_DX1, i_DY1, 0,
	  source_surface,
	  D3D11CalcSubresource(0,0,1),
	  &source_rect
	);
	pDest->Release();
}

}