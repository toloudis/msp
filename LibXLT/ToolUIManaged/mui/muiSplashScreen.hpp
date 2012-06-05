/*****************************************************************************
**  muiSplashScreen.hpp
**
**      Display loading screen with image
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef MUI_SPLASHSCREEN_HPP
#error muiSplashScreen.hpp multiply included
#endif
#define MUI_SPLASHSCREEN_HPP

#ifndef GUI_SPLASHSCREEN_HPP
#include "Tool/gui/guiSplashScreen.hpp"
#endif

//============================================================================
//============================================================================
class muiSplashScreen : public guiSplashScreenImpl
{
	//--------------------------------------------------------------------
	// StartUp - display splash screen
	//--------------------------------------------------------------------
	void  StartUp(const fsLocator &i_SplashImage,
			      const std::string& i_Message);

	//--------------------------------------------------------------------
	// ShutDown - hide the splash screen
	//--------------------------------------------------------------------
	void  ShutDown();
};

