/*****************************************************************************
**	muiSplashScreen.cpp
**
**	see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIManaged/mui/muiSplashScreen.hpp"

#include "Core/fs/fsFileUtil.hpp"

namespace
{
#ifdef SHOW_SPLASH
	public ref class muiSplashScreenPtr
	{
	public:
		static SplashImage^ m_Splash = nullptr;
	};
#endif
}

//--------------------------------------------------------------------
// StartUp - display splash screen
//--------------------------------------------------------------------
void  muiSplashScreen::StartUp(const fsLocator &i_SplashImage,
		   const std::string& i_Message)
{
#ifdef SHOW_SPLASH	// don't show the splash if in debug mode or not managed
		// display the splash screen
		//
		if (fsFileUtil::FileExists(i_SplashImage))
		{
			std::string sfile;
			fsFileUtil::LocatorToANSIFilename(i_SplashImage,sfile);
			muiSplashScreenPtr::m_Splash = gcnew SplashImage(gcnew System::String(sfile.c_str()));
			muiSplashScreenPtr::m_Splash->SplashFadeInTime = 500;
			muiSplashScreenPtr::m_Splash->SplashFadeOutTime = 500;
			muiSplashScreenPtr::m_Splash->SplashTextX = 48;
			muiSplashScreenPtr::m_Splash->SplashTextY = 98;
			muiSplashScreenPtr::m_Splash->SplashString = gcnew System::String(i_Message.c_str());
			muiSplashScreenPtr::m_Splash->SetTextColor(255,255,255,0,0,0);
			muiSplashScreenPtr::m_Splash->StartUp();
		}
#endif
}

//--------------------------------------------------------------------
// ShutDown - hide the splash screen
//--------------------------------------------------------------------
void  muiSplashScreen::ShutDown()
{
#ifdef SHOW_SPLASH	// don't show the splash if in debug mode or not managed
	if (muiSplashScreenPtr::m_Splash)
		muiSplashScreenPtr::m_Splash->ShutDown();
#endif
}
