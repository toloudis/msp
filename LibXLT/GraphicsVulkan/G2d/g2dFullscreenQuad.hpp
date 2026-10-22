/*****************************************************************************
**  g2dFullscreenQuad.hpp
**
**	GraphicsVulkan port of GraphicsDX11's g2dFullscreenQuad: a quad over a
**	viewport rectangle of the current render target, drawn with whatever
**	shaders an effect pass has set (DrawFullScreenQuad), or with its own
**	shaders to copy a texture (DrawTexturedQuad). Its shaders are compiled
**	at build time (G2d/private/Shaders) instead of from strings at run time.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef GVK_G2DFULLSCREENQUAD_HPP
#error g2dFullscreenQuad.hpp multiply included
#endif
#define GVK_G2DFULLSCREENQUAD_HPP

#ifndef MA_VECTOR2D_HPP
#include "Core/ma/maVector2d.hpp"
#endif
#ifndef MA_VECTOR4D_HPP
#include "Core/ma/maVector4d.hpp"
#endif

#include <cstdint>

namespace gvk
{
	class vkdImage;

	namespace g2dFullscreenQuad
	{
		struct SCREEN_VERTEX
		{
			maVector4d pos;
			maVector2d tex;
		};

		void InitFullscreenQuad();

		void CleanUpFullscreenQuad();

		//	The quad over (offsetPixelsX, offsetPixelsY, Width, Height) of the
		//	current render target, with the current shaders; i_InputVtxData
		//	replaces the 4 vertices (triangle strip) for this draw only.
		//	The viewport is restored afterwards.
		void DrawFullScreenQuad( uint32_t Width, uint32_t Height,
								 float offsetPixelsX = 0, float offsetPixelsY = 0,
								 SCREEN_VERTEX* i_InputVtxData = NULL );

		//	Draws i_pTexture stretched over the given viewport rect of the
		//	current render target with linear filtering. Binds its own
		//	shaders and states.
		void DrawTexturedQuad( vkdImage* i_pTexture, uint32_t Width, uint32_t Height,
							   float offsetPixelsX = 0, float offsetPixelsY = 0 );
	}
}
