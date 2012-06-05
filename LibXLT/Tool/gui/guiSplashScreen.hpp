/*****************************************************************************
**	guiSplashScreen.hpp
**
**		Display loading screen with image
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef GUI_SPLASHSCREEN_HPP
#error guiSplashScreen.hpp multiply included
#endif
#define GUI_SPLASHSCREEN_HPP

#ifndef ENV_ABSTRACTION_HPP
#include "Core/env/envAbstraction.hpp"
#endif

#include <string>


//============================================================================
// forward declaration
//============================================================================
class fsLocator;
class guiSplashScreenImpl;


//============================================================================
// static functions define API
//============================================================================
class guiSplashScreen : public envAbstraction<guiSplashScreenImpl>
{
public:
	//--------------------------------------------------------------------
	// StartUp - display splash screen
	//--------------------------------------------------------------------
	static void  StartUp(const fsLocator &i_SplashImage,
						 const std::string& i_Message);

	//--------------------------------------------------------------------
	// ShutDown - hide the splash screen
	//--------------------------------------------------------------------
	static void  ShutDown();

	//--------------------------------------------------------------------
	// Set the duration for the splash screen
	//--------------------------------------------------------------------
	static void SetDoSplashTimeout(bool i_bSplashTimeout);
};

//============================================================================
// Implementation class, needs to be derived in implementation library
//============================================================================
class guiSplashScreenImpl
{
public:
	//--------------------------------------------------------------------
	// StartUp - display splash screen
	//--------------------------------------------------------------------
	virtual void  StartUp(const fsLocator &i_SplashImage,
			      const std::string& i_Message) = 0;

	//--------------------------------------------------------------------
	// ShutDown - hide the splash screen
	//--------------------------------------------------------------------
	virtual void  ShutDown() = 0;

	//--------------------------------------------------------------------
	// Set the duration for the splash screen
	//--------------------------------------------------------------------
	virtual void SetDoSplashTimeout(bool i_bSplashTimeout) = 0;
};
