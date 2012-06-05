/*****************************************************************************
**	quiSplashScreen.cpp
**
**	see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/qui/quiSplashScreen.hpp"

#include "ToolUIQt/tqt/tqtSystem.hpp"

#include "Core/dbg/dbgMsg.hpp"
#include "Core/Fs/fsFileUtil.hpp"
#include "Core/It/itString.hpp"

#include <string>

#ifdef QT_FINISH_PORT
#include "wx/image.h"
#include "wx/splash.h" 
#include <wx/richtext/richtextctrl.h>
#endif

namespace 
{
#ifdef QT_FINISH_PORT
	wxSplashScreen *l_Splash = NULL;
#endif
bool m_bisSplashOn = true;
}



//----------------------------------------------------------------------------
// StartUp - display splash screen
//----------------------------------------------------------------------------
void  quiSplashScreen::StartUp(const fsLocator &i_SplashImage,
							   const std::string& i_Message)
{
	// Check if Splash Screen loading is disabled
	if(!splashscreen::GetSplashFlag()) return;


#ifdef QT_FINISH_PORT
	wxInitAllImageHandlers();
	if (fsFileUtil::FileExists(i_SplashImage))
	{
		//grab the splash image location
		itString img_location;
		fsFileUtil::LocatorToUnicodeString(i_SplashImage, img_location);
		
		//load the splash image and convert to bitmap type for splashscreen constructor
		wxImage image(img_location.GetString(), wxBITMAP_TYPE_PNG);
		wxBitmap bmp(image);

		if( bmp.Ok())
		{
			//wxSplashScreen *splash;
		
			if (m_bSplashTimeout)
			{
				l_Splash = new wxSplashScreen(bmp,
				wxSPLASH_CENTRE_ON_SCREEN|wxSPLASH_TIMEOUT,
				5000, tqtSystem::g_pMainForm, -1, wxDefaultPosition, wxDefaultSize,
				wxNO_BORDER|wxFRAME_NO_TASKBAR|wxFRAME_SHAPED|wxFRAME_FLOAT_ON_PARENT );
			}
			else
			{
				l_Splash = new wxSplashScreen(bmp,
				wxSPLASH_CENTRE_ON_SCREEN|wxSPLASH_NO_TIMEOUT,
				0, tqtSystem::g_pMainForm, -1, wxDefaultPosition, wxDefaultSize,
				wxNO_BORDER|wxFRAME_NO_TASKBAR|wxFRAME_SHAPED|wxFRAME_FLOAT_ON_PARENT );
			}
			
			wxStaticText* m_staticText1;
			itString msg(i_Message.c_str());
			m_staticText1 = new wxStaticText( l_Splash,wxID_ANY, msg.GetString(), wxPoint(198,134), wxDefaultSize, 0 );
			m_staticText1->Wrap( -1 );
			m_staticText1->SetForegroundColour( wxColour(206,206,206,0));
			m_staticText1->SetBackgroundColour( wxColour(97,97,97,0) );
			
			//for images with transparent edges, we need to define a new shape for the frame
			wxRegion region(bmp);
			l_Splash->SetShape(region);
		}
	}
	wxYield();
#endif
}

//----------------------------------------------------------------------------
// ShutDown - hide the splash screen
//----------------------------------------------------------------------------
void  quiSplashScreen::ShutDown()
{
if(!splashscreen::GetSplashFlag()) return;
#ifdef QT_FINISH_PORT
	if(l_Splash != NULL && !m_bSplashTimeout)
		if(l_Splash->IsShown())
			l_Splash->Show(false);
	
	l_Splash = NULL;
#endif
}

//----------------------------------------------------------------------------
// Set the duration for the splash screen
//----------------------------------------------------------------------------
void quiSplashScreen::SetDoSplashTimeout(bool i_bSplashTimeout)
{
	m_bSplashTimeout = i_bSplashTimeout;
}

//--------------------------------------------------------------------
// Check the flag to see if splash screen is enabled
//--------------------------------------------------------------------
bool splashscreen::GetSplashFlag()
{
	return m_bisSplashOn;
}
//--------------------------------------------------------------------------------
// Set the flag for splash screen. This is currently being set only by '/NoSplash'
// command line option.
//---------------------------------------------------------------------------------
	
void splashscreen::SetSplashFlag(bool i_bisSplashOn)
{
	m_bisSplashOn = i_bisSplashOn;
}
