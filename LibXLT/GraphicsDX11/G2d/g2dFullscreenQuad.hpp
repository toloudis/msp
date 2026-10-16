/*****************************************************************************
**  g2dFullscreenQuad.hpp
**
**
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifndef G2D_DX11TYPES_HPP
#include "GraphicsDX11/g2d/g2dDX11Types.hpp"
#endif

#ifndef MA_VECTOR2D_HPP
#include "Core/ma/maVector2d.hpp"
#endif

#ifndef MA_VECTOR4D_HPP
#include "Core/ma/maVector4d.hpp"
#endif

namespace g2dFullscreenQuad
{
	struct SCREEN_VERTEX
	{
		maVector4d pos;
		maVector2d tex;
	};

	void InitFullscreenQuad();

	void CleanUpFullscreenQuad();
/*
	void DrawFullScreenQuad11( ID3D11PixelShader* pPS,
							   UINT Width, UINT Height,
							   float offsetPixelsX = 0, float offsetPixelsY = 0,
							   SCREEN_VERTEX* i_InputVtxData = NULL);
*/
	void DrawFullScreenQuad11( UINT Width, UINT Height,
		float offsetPixelsX = 0, float offsetPixelsY = 0,
		SCREEN_VERTEX* i_InputVtxData = NULL );

	// Draws i_pTexture stretched over the given viewport rect of the current
	// render target with linear filtering. Binds its own shaders and states.
	void DrawTexturedQuad11( ID3D11ShaderResourceView* i_pTexture,
		UINT Width, UINT Height,
		float offsetPixelsX = 0, float offsetPixelsY = 0 );
};
