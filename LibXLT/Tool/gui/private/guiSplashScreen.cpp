/*****************************************************************************
**	guiSplashScreen.cpp
**
**	see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Tool/gui/guiSplashScreen.hpp"

#include "Core/dbg/dbgMsg.hpp"


//--------------------------------------------------------------------
// StartUp - display splash screen
//--------------------------------------------------------------------
void guiSplashScreen::StartUp(const fsLocator &i_SplashImage,
							  const std::string& i_Message)
{
//	DBG_ASSERT(sm_pImplementation, "guiSplashScreen: No implementation");
	if (sm_pImplementation)
	{
		sm_pImplementation->StartUp(i_SplashImage, i_Message);
	}
}

//--------------------------------------------------------------------
// ShutDown - hide the splash screen
//--------------------------------------------------------------------
void  guiSplashScreen::ShutDown()
{
//	DBG_ASSERT(sm_pImplementation, "guiSplashScreen: No implementation");
	if (sm_pImplementation)
	{
		sm_pImplementation->ShutDown();
	}
}

//--------------------------------------------------------------------
// Set the duration for the splash screen
//--------------------------------------------------------------------
void  guiSplashScreen::SetDoSplashTimeout(bool i_bSplashTimeout)
{
	if (sm_pImplementation)
	{
		sm_pImplementation->SetDoSplashTimeout(i_bSplashTimeout);
	}
}