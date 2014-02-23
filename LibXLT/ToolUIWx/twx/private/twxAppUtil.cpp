/*****************************************************************************
**  twxAppUtil.hpp
**
**     MainForm when using wxWidgets
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIWx/twx/twxAppUtil.hpp"

#include "ToolUIWx/twx/twxMessaging.hpp"

#ifdef USE_WXWIDGETS

#include <wx/cmdline.h>
//#include <wx/init.h>

extern void wxSetInstance(HINSTANCE hInst);

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
			// Catch events in order to process hot keys
			//----------------------------------------------------------------------------
			virtual int FilterEvent(wxEvent& i_Event)
			{
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

		// ----------------------------------------------------------------------------
		// ----------------------------------------------------------------------------
		wxArrayString ConvertStringToArgs(const wxChar *p)
		{
			wxArrayString args;

			wxString arg;
//			arg.reserve(1024);

			bool isInsideQuotes = false;
			for ( ;; )
			{
				// skip white space
				while ( *p == _T(' ') || *p == _T('\t') )
					p++;

				// anything left?
				if ( *p == _T('\0') )
					break;

				// parse this parameter
				bool endParam = false;
				bool lastBS = false;
				for ( arg.clear(); !endParam; p++ )
				{
					switch ( *p )
					{
						case _T('"'):
							if ( !lastBS )
							{
								isInsideQuotes = !isInsideQuotes;

								// don't put quote in arg
								continue;
							}
							//else: quote has no special meaning but the backslash
							//      still remains -- makes no sense but this is what
							//      Windows does
							break;

						case _T(' '):
						case _T('\t'):
							// backslash does *not* quote the space, only quotes do
							if ( isInsideQuotes )
							{
								// skip assignment below
								break;
							}
							// fall through

						case _T('\0'):
							endParam = true;

							break;
					}

					if ( endParam )
					{
						break;
					}

					lastBS = *p == _T('\\');

					arg += *p;
				}

				args.push_back(arg);
			}

			return args;
		}

	} // local namespace

	//--------------------------------------------------------------------
	// Pass in a few variables from WinMain into wxWidgets
	//--------------------------------------------------------------------
	void SetInstance( HINSTANCE i_hInstance,
					  int       i_nCmdShow )
	{
		// remember the parameters Windows gave us
		wxSetInstance(i_hInstance);
		wxApp::m_nCmdShow = i_nCmdShow;
	}

	//--------------------------------------------------------------------
	//	ParseCommandLine fills in the o_Argc and o_Argv variables needed
	//	by wxWidgets setup
	//--------------------------------------------------------------------
	void ParseCommandLine(int &o_Argc,
						  wxChar **&o_Argv)
	{
		// parse the command line: we can't use pCmdLine in Unicode build so it is
		// simpler to never use it at all (this also results in a more correct
		// o_Argv[0])

		// break the command line in words
		wxArrayString args;

		const wxChar *cmdLine = ::GetCommandLine();
		if ( cmdLine )
		{
			//args = wxCmdLineParser::ConvertStringToArgs(cmdLine);
			args = ConvertStringToArgs(cmdLine);
		}

		o_Argc = args.GetCount();

		// +1 here for the terminating NULL
		o_Argv = new wxChar *[o_Argc + 1];
		for ( int i = 0; i < o_Argc; i++ )
		{
			o_Argv[i] = (wxChar*)wxStrdup(args[i]);
		}

		// o_Argv[] must be NULL-terminated
		o_Argv[o_Argc] = NULL;
	}

	//--------------------------------------------------------------------
	// Frees the arguments allocated in ParseCommandLine
	//--------------------------------------------------------------------
	void FreeArgs(int &o_Argc, wxChar **&o_Argv)
	{
		// notice that o_Argv elements are supposed to be allocated using malloc() while
		// o_Argv array itself is allocated with new
		for ( int i = 0; i < o_Argc; i++ )
		{
			free(o_Argv[i]);
		}

		delete [] o_Argv;
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

