/*****************************************************************************
**  quiSplashScreen.hpp
**
**      Display loading screen with image
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef QUI_SPLASHSCREEN_HPP
#error quiSplashScreen.hpp multiply included
#endif
#define QUI_SPLASHSCREEN_HPP

#ifndef GUI_SPLASHSCREEN_HPP
#include "Tool/gui/guiSplashScreen.hpp"
#endif

//============================================================================
//============================================================================
namespace splashscreen
{
	
	//--------------------------------------------------------------------
	// Check the flag to see if splash screen is enabled
	//--------------------------------------------------------------------
	bool GetSplashFlag();

	//--------------------------------------------------------------------------------
	// Set the flag for splash screen. This is currently being set only by '/NoSplash'
	// command line option.
	//---------------------------------------------------------------------------------
	void SetSplashFlag(bool i_bisSplashOn);
}

//============================================================================
//============================================================================
class quiSplashScreen : public guiSplashScreenImpl
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

	//--------------------------------------------------------------------
	// Set the duration for the splash screen
	//--------------------------------------------------------------------
	void SetDoSplashTimeout(bool i_bSplashTimeout);
	
	

private:
	bool m_bSplashTimeout;
	
};


