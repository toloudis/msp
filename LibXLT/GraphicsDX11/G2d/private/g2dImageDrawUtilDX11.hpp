/****************************************************************************\
**  g2dImageDrawUtilDX11.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifdef G2D_IMAGEDRAWUTILD3D11_HPP
#error g2dImageDrawUtilDX11.hpp multiply included
#endif
#define G2D_IMAGEDRAWUTILD3D11_HPP

#ifndef G2D_DX11TYPES_HPP
#include "GraphicsDX11/g2d/g2dDX11Types.hpp"
#endif


//============================================================================
//============================================================================
namespace g2dImageDrawUtilDX11
{
	//------------------------------------------------------------------------
	//	This DrawImage blits a rectangular section of the source image to a
	//	location on the destination image.  The rectangle of the source image
	//	is defined by (i_SX1, i_SY1) - (i_SX2, i_SY2).  The point
	//	(i_SX1, i_SY1) will be located at (i_DX1, i_DY1).
	//------------------------------------------------------------------------
	void DrawImage(	g2dD3D11TexturePtr o_DestImage,
					int i_DX1,
					int i_DY1,
					int i_DestWidth,
					int i_DestHeight,
					int i_SX1,
					int i_SY1,
					int i_SX2,
					int i_SY2,
					g2dD3D11TexturePtr i_SrcImage);

	//------------------------------------------------------------------------
	//	UpdateSurface() - copy the source surface to the destination
	//------------------------------------------------------------------------
	HRESULT UpdateSurface(	g2dD3D11TexturePtr i_pSource,
							CONST RECT* pSourceRect,
							g2dD3D11TexturePtr i_pDest,
							CONST POINT* pDestPoint );
}
