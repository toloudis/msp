/*****************************************************************************
**  g2dWindowPAC.hpp
**
**      g2dWindowPAC.hpp forwards the native-window calls g2dWindowOptiX
**	makes to the correct PAC component.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GOX_G2DWINDOWPAC_HPP
#error g2dWindowPAC.hpp multiply included
#endif
#define GOX_G2DWINDOWPAC_HPP

#ifndef ENV_PLATFORM_HPP
#include "Core/env/envPlatform.hpp"
#endif

// PAC components should support the following interface:
//
/*namespace gox { namespace g2dWindowPAC
{
	//====================================================================
	//	GetClientSize gives the native window's client area in pixels;
	//	false (outputs unchanged) if there is no such window.
	//====================================================================
	bool GetClientSize(void* i_hWnd, int& o_Width, int& o_Height);

	//====================================================================
	//	FillClient fills the native window's whole client area with the
	//	given color (8 bits per channel). Does nothing without a window.
	//====================================================================
	void FillClient(void* i_hWnd, unsigned char i_Red, unsigned char i_Green, unsigned char i_Blue);
}}*/

#if ENV_WINDOWS
	#include "GraphicsOptiX/G2d/private/g2dWindowPACWin.hpp"
#else
	#error g2dWindowPAC not defined for this platform
#endif
