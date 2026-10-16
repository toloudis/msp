/*****************************************************************************
**  twxAppUtil.hpp
**
**     MainForm when using wxWidgets
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIWx/twx/twxAppUtil.hpp"

#include "ToolUIWx/twx/twxInputFeed.hpp"
#include "ToolUIWx/twx/twxMessaging.hpp"

#ifdef USE_WXWIDGETS

//#include <wx/init.h>

#ifdef __WXMSW__
extern void wxSetInstance(HINSTANCE hInst);
#endif

namespace twxAppUtil
{
	namespace
	{
		// ----------------------------------------------------------------------------
		// Determine if the window with focus wants hot keys
		// ----------------------------------------------------------------------------
		bool window_wants_hotkeys()
		{
			// Start with window that has focus and search up through
			// parent windows and see if any parent has registered a desire
			// to process hot keys.
			wxWindow *pWindow = wxWindow::FindFocus();
			while (pWindow)
			{
				if (twxMessaging::WantsHotKeys(pWindow))
					return true;
				pWindow = pWindow->GetParent();
			}
			return false;
		}

		// ----------------------------------------------------------------------------
		// Define a new application type, but we will not use it directly outside
		//	of this file since we are doing an unorthodox setup of wxWidgets.
		// ----------------------------------------------------------------------------
		class MyApp : public wxApp
		{
		public:
			// ----------------------------------------------------------------------------
			// Usually wxWidgets setup the application and main form in this function.
			//	But, we keep it empty in order to do the setup in WinMain.
			// ----------------------------------------------------------------------------
			virtual bool OnInit();

			//----------------------------------------------------------------------------
			// Catch events in order to feed the input package and process hot keys
			//----------------------------------------------------------------------------
			virtual int FilterEvent(wxEvent& i_Event)
			{
				// First, so it sees the keys that the hot keys use up
				twxInputFeed::FilterEvent(i_Event);

				if ( i_Event.GetEventType() == wxEVT_KEY_DOWN )
				{
					if (twxMessaging::ShouldProcessHotKeys())
					{
						if (window_wants_hotkeys())
						{
							// Handle hot keys here
							return twxMessaging::ProcessKeyEvent(static_cast<wxKeyEvent&>(i_Event));
						}
					}
				}
				return -1;
			}

			//----------------------------------------------------------------------------
			// Catch events in order to process hot keys
			//----------------------------------------------------------------------------
			//virtual bool ProcessEvent(wxEvent& i_Event)
			//{
			//	if ( i_Event.GetEventType() == wxEVT_KEY_DOWN )
			//	{
			//		if (twxMessaging::ShouldProcessHotKeys())
			//		{
			//			if (window_wants_hotkeys())
			//			{
			//				// Handle hot keys here
			//				return twxMessaging::ProcessKeyEvent(static_cast<wxKeyEvent&>(i_Event));
			//			}
			//		}
			//	}
			//	return false;
			//}
		};

		// ----------------------------------------------------------------------------
		// Create a new application object: this macro will allow wxWidgets to create
		// the application object during program execution (it's better than using a
		// static object for many reasons) and also implements the accessor function
		// wxGetApp() which will return the reference of the right type (i.e. MyApp and
		// not wxApp)
		// ----------------------------------------------------------------------------
		//IMPLEMENT_APP(MyApp)
		IMPLEMENT_APP_NO_MAIN(MyApp)

		// ----------------------------------------------------------------------------
		// the application class
		// ----------------------------------------------------------------------------
		bool MyApp::OnInit()
		{
			// call the base class initialization method, currently it only parses a
			// few common command-line options but it could be do more in the future
			if ( !wxApp::OnInit() )
				return false;

			// success: wxApp::OnRun() will be called which will enter the main message
			// loop and the application will run. If we returned false here, the
			// application would exit immediately.
			return true;
		}

	} // local namespace

	//--------------------------------------------------------------------
	// Pass in a few variables from WinMain into wxWidgets
	//--------------------------------------------------------------------
	void SetInstance( void*     i_hInstance,
					  int       i_nCmdShow )
	{
#ifdef __WXMSW__
		// remember the parameters Windows gave us
		wxSetInstance(static_cast<HINSTANCE>(i_hInstance));
		wxApp::m_nCmdShow = i_nCmdShow;
#endif
	}

	//--------------------------------------------------------------------
	// Initializes wxWidgets, returns true if successful.
	//--------------------------------------------------------------------
	bool Init(int i_Argc, wxChar **i_Argv)
	{
//		 DisableAutomaticSETranslator();

		// library initialization
		if ( !wxEntryStart(i_Argc, i_Argv) )
		{
	//#if wxUSE_LOG
	//        // flush any log messages explaining why we failed
	//        delete wxLog::SetActiveTarget(NULL);
	//#endif
			return false;
		}
		
#if wxUSE_LIBPNG
		wxImage::AddHandler( new wxPNGHandler );
#endif

		wxTheApp->OnInit();

		return true;
	}

	//--------------------------------------------------------------------
	// Runs the application loop
	//--------------------------------------------------------------------
	void Run()
	{
		// app execution
		wxTheApp->OnRun();
	}

	//--------------------------------------------------------------------
	// DeInitializes wxWidgets
	//--------------------------------------------------------------------
	void CleanUp()
	{
		wxTheApp->OnExit();

		wxEntryCleanup();
	}
}

#endif // USE_WXWIDGETS

