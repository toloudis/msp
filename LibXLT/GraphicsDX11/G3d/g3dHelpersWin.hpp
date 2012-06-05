/****************************************************************************\
**  g3dHelpersWin contains some useful functions for interfacing with
**	D3D stuff.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef G3D_HELPERSWIN_HPP
#error g3dHelpersWin.hpp multiply included
#endif
#define G3D_HELPERSWIN_HPP

#ifndef G2D_DX11GLOBALWIN_HPP
#include "GraphicsDX11/g2d/g2dDX11GlobalWin.hpp"
#endif
#ifndef MA_FLOATRGBA_HPP
#include "Core/ma/maFloatRGBA.hpp"
#endif
#ifndef MA_MATRIX3X3_HPP
#include "Core/ma/maMatrix3x3.hpp"
#endif

namespace g3dHelpersWin
{
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
/*	DX11 no D3DCOLORVALUE
	inline void make_d3d_color(const maFloatRGBA& i_Color, D3DCOLORVALUE& o_Color)
	{
		o_Color.r = i_Color.GetRed();
		o_Color.g = i_Color.GetGreen();
		o_Color.b = i_Color.GetBlue();
		o_Color.a = i_Color.GetAlpha();
	}
*/
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	inline void make_d3d_color(const maFloatRGBA& i_Color, unsigned long& o_Color)
	{
		o_Color = int(i_Color.GetBlue() * 255);
		o_Color |= int(i_Color.GetGreen() * 255) << 8;
		o_Color |= int(i_Color.GetRed() * 255) << 16;
		o_Color |= int(i_Color.GetAlpha() * 255) << 24;
	}

/*
	//DX11 doesn't support D3DMATRIX type
	inline void get_d3d_matrix(const maMatrix3x3& i_Matrix, D3DMATRIX& o_Matrix)
	{
		o_Matrix._11 = i_Matrix(0, 0);
		o_Matrix._12 = i_Matrix(0, 1);
		o_Matrix._13 = i_Matrix(0, 2);
		o_Matrix._14 = 0.0f;
		o_Matrix._21 = i_Matrix(1, 0);
		o_Matrix._22 = i_Matrix(1, 1);
		o_Matrix._23 = i_Matrix(1, 2);
		o_Matrix._24 = 0.0f;
		o_Matrix._31 = i_Matrix(2, 0);
		o_Matrix._32 = i_Matrix(2, 1);
		o_Matrix._33 = i_Matrix(2, 2);
		o_Matrix._34 = 0.0f;
		o_Matrix._41 = 0;
		o_Matrix._42 = 0;
		o_Matrix._43 = 0;
		o_Matrix._44 = 1.0f;
	}
*/
}
