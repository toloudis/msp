/********************************************************************************************\
**	WinMain.cpp
**
**		Windows application launching point
**
**	StudioGPU
**	Copyright(C) 2004-10 - All Rights Reserved
\********************************************************************************************/
#include "stdafx.h"

#include "MainApp/mainInitApplication.hpp"
#include "MainApp/mainInitDocument.hpp"
#include "MainApp/mainInitGraphics.hpp"
#include "MainApp/mainInitLibrary.hpp"
#include "MainApp/mainInitMainWindow.hpp"
#include "MainApp/mainInitSecurity.hpp"

#include "MainApp/Commands/mainCommands.hpp"
#include "MainApp/Data/mainDocumentInterest.hpp"
#include "MainApp/mainCheckAppVersion.hpp"
#include "MainApp/mainConstants.hpp"
#include "MainApp/MainForm.h"
#include "MainApp/mainPython.hpp"
#include "MainApp/mainResolvePath.hpp"
#include "MainApp/mnmApp.hpp"
#include "MainApp/wxGUI/wxMainForm.hpp"
#include "Support/mnm/mnmConstants.hpp"

// Local Project includes
#include "Features/Capture/cptrModeRender.hpp"
#include "Features/Capture/cptrRenderUtil.hpp"
#include "Features/Channels/chnlDialogUtil.hpp"
#ifdef FUSION //#if(SGPU_APP == MS_FUSION)
#include "Features/ObjectManip/mnpPackage.hpp"
#endif
#include "Features/Prefs/PrefsData.hpp"
#include "Features/Prefs/PrefsMgr.hpp"
#include "Features/RenderPanels/rpnRenderPane.hpp"
#include "Features/RenderPanels/wxGUI/rpnRenderPanel.hpp"
#include "Features/RenderPrefs/rndrPrefsMgr.hpp"
#include "Support/mode/modeModeMgr.hpp"
#include "Support/mnm/mnmAppPackageMgr.hpp"
#include "Support/mnm/mnmAppUtil.hpp"
#include "Support/mnm/mnmErrorCodes.hpp"
#include "Support/mnm/mnmPaths.hpp"
#include "Support/mnm/mnmVJoystick.hpp"
#include "Support/pyth/pythModules.hpp"
#include "Support/pyth/pythUtil.hpp"
#include "Support/SupportLayer.hpp"
#include "Support/tmln/tmlnTimeLine.hpp"
#include "Support/vis/visMgr.hpp"
#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"

// library includes
#include "AudioDS/AudioDSLayer.hpp"
#include "AudioDS/sn/snExceptionX.hpp"
#include "AudioDS/sn/snSoundManager.hpp"
#include "AudioDS/sn/snSoundSystem.hpp"
#include "Core/App/appTime.hpp"
#include "Core/App/appTimeUtils.hpp"
#include "Core/CoreLayer.hpp"
#include "Core/dbg/dbgSystemInfo.hpp"
#include "Core/Env/envString.hpp"
#include "Core/Fs/fsFileUtil.hpp"
#include "Core/fs/fsResourceTracker.hpp"
#include "Core/gf/gfPackage.hpp"
#include "Core/it/itStringUtil.hpp"
#include "Graphics/g2d/g2dExceptionX.hpp"
#include "Graphics/g3d/g3dExceptionX.hpp"
#include "Graphics/GraphicsLayer.hpp"
#include "Graphics/mat/matTextureMgr.hpp"
#include "Graphics/mdl/mdlModelingPackageListMgr.hpp"
#include "ImportExport/ImportExportLayer.hpp"
#include "Input/in/inPackage.hpp"
#include "Tool/doc/docSingleDocumentMgr.hpp"
#include "Tool/doc/docSingleTypeMgr.hpp"
#include "Tool/gui/guiMessageBox.hpp"
#include "Tool/gui/guiSingleDocHandler.hpp"
#include "Tool/gui/guiSplashScreen.hpp"
#include "Tool/rstk/rstkDocumentInterest.hpp"
#include "ToolUIWx/pwx/pwxControlMgr.hpp"
#include "ToolUIWx/twx/twxAppUtil.hpp"
#include "ToolUIWx/twx/twxToolbarMgr.hpp"
#include "ToolUIWx/wui/wuiPackage.hpp"

#include "Core/app/private/appApplicationPAC.hpp"	// FIX - if this is sorted with includes, it compiles with errors.

#undef MessageBox	// for guiMessageBox

#include <iostream>
#include <fstream>
#ifdef DEMO_VERSION
#include <wx/config.h>
#endif

#ifdef FUSION //#if(SGPU_APP == MS_FUSION)
#include "FCSupport/fcui/qtGUI/qtfc3d.h"
#include <QtGui/QApplication>
#include <QtGUI/QSplashScreen>
#include <QtGui/QtGui>
#include <QtGUI/QSplashScreen>
#include "FCSupport/fcui/fcuiFormMgr.hpp"
#include "FCSupport/fcui/qtGUI/fcuiSplashScreen.hpp"
#endif

#ifdef BATCH_MODE
#pragma comment(lib,"libpng16.lib")
#endif


//============================================================================
// Time limited version of MachStudio in order to enable minidumps for H&F
//============================================================================
//#define TIME_LIMITED
#ifdef TIME_LIMITED
const int c_TimeLimitedStart	= 2455298;	// Julian date for April 11th, 2010
const int c_TimeLimitedEnd		= 2455532;	// Julian date for December 1st, 2010
#endif


//============================================================================
// Catch SEH exceptions and print a mini dump
//============================================================================
#ifdef BATCH_MODE
#define CATCH_EXCEPTIONS 0
#else
	#ifdef _DEBUG
	#define CATCH_EXCEPTIONS 0
	#else
	#define CATCH_EXCEPTIONS 1
	#endif
#endif

#if (CATCH_EXCEPTIONS)
#include "dbghelp.h"

LONG HandleExceptionFilter(struct _EXCEPTION_POINTERS *data);

typedef BOOL (WINAPI *MINIDUMPWRITEDUMP)(HANDLE hProcess, DWORD dwPid, HANDLE hFile, MINIDUMP_TYPE DumpType,
									CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
									CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
									CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam
									);
#endif // CATCH_EXCEPTIONS


//============================================================================
//============================================================================
HANDLE g_hMutexAppRunning = NULL;


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool app_instance_exists(LPCTSTR i_pName)
{
	// Create a global mutex. Use a unique name, for example
	// incorporating your company and application name.
	g_hMutexAppRunning = CreateMutex( NULL, false, i_pName);

	// Check if the mutex object already exists, indicating an
	// existing application instance
	if (   ( g_hMutexAppRunning != NULL ) 
		&&
		   ( GetLastError() == ERROR_ALREADY_EXISTS))
	{
		// Close the mutex for this application instance. This assumes
		// the application will inform the user that it is
		// about to terminate
#ifdef FUSION //#if(SGPU_APP == MS_FUSION)
		MessageBoxA(NULL, "Another instance of Fusion Cinema3d is running.  Please close that instance first before trying to launch the application again", "Application already running", MB_OK|MB_ICONERROR);
#else
		MessageBoxA(NULL, "Another instance of Mach Studio is running.  Please close that instance first before trying to launch the application again", "Application already running", MB_OK|MB_ICONERROR);
#endif
		CloseHandle( g_hMutexAppRunning );
		g_hMutexAppRunning = NULL;
	}

	// Return False if a new mutex was created,
	// as this means it's the first app instance
	return ( g_hMutexAppRunning == NULL );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool app_instance_exists()
{
	bool bAppRunning = false;

	//	clunky copy of executable
	std::string exename(mnmConstants::c_EXECUTABLE);
	std::wstring exe(exename.length(), L' ');
	std::copy(exename.begin(), exename.end(), exe.begin());
	exe.insert(0,L"Global\\");
	bAppRunning |= app_instance_exists( exe.c_str() );
	//bAppRunning |= app_instance_exists(L"Global\\MachStudio.exe");

	//EnumWindows((WNDENUMPROC)Report, 0); 

	return bAppRunning;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool CALLBACK Report(int hwnd, int lParam)
{
	LPTSTR lpString = new TCHAR[128];
	GetWindowText((HWND)hwnd, lpString, 128);

	LPTSTR lpStringF = new TCHAR[256];
	GetWindowModuleFileName((HWND)hwnd,lpStringF,256);

	DBG_LOG("Window title " << lpString << " filename " << lpStringF);

	return true;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void run_python_startup_scripts()
{
#if defined(PYTHON_ENABLED)
	// Set up python command line arguments just so Tkinter is happy.
	const int fake_argc = 1;
	char* fake_argv[2] = { "", NULL };
	PySys_SetArgv(fake_argc, fake_argv);

	// Add the directory ".\python"  to the python path so that we
	// can source scripts from that directory easily
	fsLocator script_dir = gfPaths::GetPath( mnmPaths::e_Python );
	std::string script_dir_str;
	fsFileUtil::LocatorToANSIFilename(script_dir, script_dir_str);
	std::string append_path_cmd("sys.path.append('");
	append_path_cmd += script_dir_str;
	append_path_cmd += std::string("')");
	pythUtil::ExecuteCommand(append_path_cmd);

#if ( SGPU_APP != MS_CORE )
	//If we cannot find the user's python directory in their user data path, we
	//should add it along with the sample user_python script.  Fixes issue of users
	//installing MSP on an admin account and then when they switch to a new user, the
	//user python directories don't exist.
	fsLocator user_python_dir = gfPaths::GetPath(mnmPaths::e_Python, pythUtil::e_UserPython );
	if ( !fsFileUtil::DirectoryExists(user_python_dir) )
	{
		fsLocator temp_user_dir = script_dir;
		temp_user_dir.Push("UserPython");

		fsFileUtil::CreateDirectory(user_python_dir);
		fsFileUtil::CopyDirectory(temp_user_dir, user_python_dir);
	}
#endif

	// Load a python script file at start, can define functions for later use
	// in command shell
	fsLocator script_loc = script_dir;
	script_loc.Push("StartUp.py");
	if (fsFileUtil::FileExists(script_loc))
		pythUtil::ScriptFile( script_loc );

#if ( SGPU_APP != MS_CORE )
	//the user script allows users to define their personal startup script to 
	//execute custom python scripts in the user data directory
	fsLocator user_script = PrefsMgr::Data().m_UserPyLocation.GetValue();
	std::string user_script_dir_str;
	if (fsFileUtil::FileExists(user_script))
		user_script.Pop();

	//Weird error: if the user has their script in their my docs folder, the sys path
	//loses a slash between "Documents and Settings" and the User name.  To fix this,
	//we insert a second backslash between each directory entry
	fsFileUtil::LocatorToANSIFilename(user_script, user_script_dir_str, true);
	append_path_cmd = "sys.path.append('";
	append_path_cmd += user_script_dir_str;
	append_path_cmd += std::string("')");
	pythUtil::ExecuteCommand(append_path_cmd);

	//load the userStartup script
	user_script = PrefsMgr::Data().m_UserPyLocation.GetValue();
	if (fsFileUtil::FileExists(user_script))
		pythUtil::ScriptFile(user_script);

	//now find each python object script previously created in the user's data path
	fsLocator python_object_dir = gfPaths::GetPath(mnmPaths::e_Python, pythUtil::e_PythonObjects );
	fsFileEnum::fsFileList python_objects;
	itString py_ext("py");
	fsFileEnum::EnumerateFiles(python_object_dir, python_objects, py_ext);

	//after finding each python object file, we need to execute them to add them to the menu
	for(int i = 0; i < python_objects.size(); ++i)
	{
		pythUtil::ScriptFile(python_objects[i]);
	}
#endif

#endif
}


//----------------------------------------------------------------------------
// DoWinMain handles the setup and run of the application. It is designed
// so that any cleanup is organized into objects that go out of scope
// in order when an exception is thrown.
//----------------------------------------------------------------------------
int DoWinMain(HINSTANCE hInstance,
			  LPTSTR lpCmdLine,
			  int nCmdShow)
{
#ifndef USE_WXWIDGETS
	// MOVED TO TOP SO SUSPENDED IS TURNED OFF FOR RENDERNODE EARLY ENOUGH
	//
	// Don't want to have suspend messages so that we can render in the background in batch mode
	appApplicationPAC::SetAllowSuspended(false);
#endif

#ifdef DEMO_VERSION
	bool	lb_valid_demo = true;
#endif

	// Initialization error until we get to App Run
	mnmAppUtil::SetErrorCode(mnmErrorCodes::c_InitializationError);

	//	Set the application name and version so the APPV chunk can be written
	//
	mnmAppPackageMgr::SetRestricted(false);
	envAppVersion user_version( mainConstants::mc_ExecutableVersion );
	itString app_name(mnmConstants::c_PRODUCT);
	mnmAppPackageMgr::SetAppData( app_name, user_version );

	// Begin mainInitLibrary scope
	{
		// Initialize small core of library application here, if exception is thrown after, 
		// that core will be closed automatically.
		mainInitLibrary mainLibrary(hInstance, nCmdShow);

		// Begin mainInitMainWindow scope
		if (mainLibrary.InitSuccessful())
		{
			// Initialize main frame for windowed application here, if exception is thrown after, 
			// that frame will be closed automatically.
			//
			mainInitMainWindow mainWindow;

#ifdef FUSION //#if(SGPU_APP == MS_FUSION)
			//	set-up fusion specific paths
			mnmPaths::SetupMainPaths();

			// Set the Splash screen
			QPixmap pixmap("Media/GUI/splash.jpg");
			QPixmap cursorpixmap("Media/Gui/cursor.png");
			QPixmap waitcursor("Media/Gui/waitcursor.png");
	
			QCursor bitmapcursor(cursorpixmap,0,0);
			//QSplashScreen* splash  = new QSplashScreen(pixmap,Qt::WindowStaysOnTopHint);
			//splash.show();

			fcuiSplashScreen* splash = new fcuiSplashScreen(pixmap, Qt::WindowStaysOnTopHint);
			QApplication::setOverrideCursor(waitcursor);

			splash->ShowScreen();
			//splash->show();
			//QGraphicsOpacityEffect* effect = new QGraphicsOpacityEffect();
			//effect->setOpacity(0.0);
			//splash->setGraphicsEffect(effect);

			//QPropertyAnimation* animation = new QPropertyAnimation(splash, "windowOpacity");

			////set the necessary animation parameters
			//animation->setDuration(5000);
			//animation->setStartValue(1.0);
			//animation->setEndValue(1.0);
			//animation->start(QPropertyAnimation::DeleteWhenStopped);

			//splash->show();
			
			fcuiFormMgr::CreateMainWindow();
			QtFC3D* pWindow = fcuiFormMgr::GetMainWindow();
			
			DBG_ASSERT(pWindow != NULL, "Qt Window not properly initialized");

			itString it_title(mnmConstants::c_PRODUCT);
			QString title( itStringUtil::GetStdString( it_title ).c_str() );
			pWindow->setWindowTitle( title );

			// The application icon, typically displayed in the upper left corner of the application top-level windows,
			// can in Qt be set by using the QWidget::setIcon() method on the top-level widgets. 
			//
			pWindow->setWindowIcon(QIcon(QString("app.ico")));

			// Get the HWND of the window from Qt 
			appApplicationPAC::SetHWND( pWindow->GetWinId() );

			//	Show the window for the first time!
			//
			mnmApp::SetWindowSize(pWindow->ui.Home_CenterWidget->width(), pWindow->ui.Home_CenterWidget->height());
			//tma3dScreenUtil::SetWindowSize( maPoint2d(pWindow->ui.Home_CenterWidget->x(), pWindow->ui.Home_CenterWidget->y()), maPoint2d(pWindow->ui.Home_CenterWidget->width(), pWindow->ui.Home_CenterWidget->height()) );
		//	splash->showMessage("Loading...");
#endif

#ifdef USE_WXWIDGETS
			wxMainForm *main_frame = mainWindow.GetMainFrame();

	#if( SGPU_APP == MS_CORE )
			twxToolbarMgr::SetIconSize(24);

			mdlModelingPackageListMgr::Clear();
		#ifdef MODELINGPACKAGE_RHINO
			mdlModelingPackageListMgr::Add( itString("Rhino") );
		#endif
	#endif	// MSAPP == MS_CORE

#endif // USE_WXWIDGETS
			// Begin mainInitGraphics scope
			{
				// Initialize graphics systems here, if exception is thrown after, 
				// those systems will be closed automatically.
#ifdef USE_WXWIDGETS
				mainInitGraphics mainGraphics(main_frame);
#else
#ifdef FUSION //#if(SGPU_APP == MS_FUSION)
				int width, height;
				width = pWindow->ui.Home_CenterWidget->width();
				height = pWindow->ui.Home_CenterWidget->height();
				//width = (int)(height * 16 / 9);
				mainInitGraphics mainGraphics(width, height, pWindow->ui.Home_CenterWidget->x(), pWindow->ui.Home_CenterWidget->y());
#else
				mainInitGraphics mainGraphics;
#endif
#endif // USE_WXWIDGETS

				// Begin mainInitApplication scope
				if (mainGraphics.InitSuccessful())
				{
					// Initialize application libraries here, if exception is thrown after, 
					// those libraries will be closed automatically.
					mainInitApplication mainApplication(mainGraphics.GetSystem());

					// Begin mainInitDocument scope
					{
						// Initialize document here, if exception is thrown after, 
						// the document will be closed automatically.
						mainInitDocument mainDocument;

						// Now, success condition until we get to start up scripts
						// and command line arguments
						mnmAppUtil::SetErrorCode(mnmErrorCodes::c_Success);

#ifdef BATCH_MODE
						// Clear out the mode so that the program exits unless the
						// command line instructs it to switch to capture mode.
#ifndef FUSION
						if (!modeModeMgr::IsEmpty())
						{
							modeModeMgr::Clear();
						}

						// Don't need viewport rendering when in batch mode
						mnmApp::EnableRender(false);
#endif	// FUSION

#endif	// BATCH_MODE

						//	Show child dialogs (if flags set)
						//
						//	TODO - this would be better if the main form had a "child window interest"
						//	and it could let all the windows know that this is their chance to show
						//	themselves.
						//
						cmmSystemDialogUtil::ShowInitial();

						//	set up python, executing start up scripts
						//
						run_python_startup_scripts();

						mainInitSecurity mainSecurity;

						//
						//	parse the command line and configure the app accordingly
						//
						//bga - This is not good...
						std::string cmdline( envString::WideCharToUTF8(lpCmdLine) );
						mnmApp::ParseCommandLine( cmdline );

#ifdef FUSION //#if(SGPU_APP == MS_FUSION)
						//	don't show the 3D icons
						visMgr::ShowIcons(false);
						sel3dMgr::ClearSelection();

						//	Automatically open an empty application if no command line options
						if (cmdline.size() == 0)
							mnpPackage::LaunchNew();
#endif
						// valid the license initially
						bool license_valid = mainSecurity.ValidateLicense();

						//	set the app active right before the app runs
						mnmApp::SetActive( true );

					//
					//	the "run" loop
					//
					#ifdef USE_WXWIDGETS
						// Add toolbars should be full of buttons by now,
						// so create the toolbars and set their size
						twxToolbarMgr::RealizeAllToolbars();

						// Read in the preferences
						main_frame->prefs_ReadAndApply();

						// Show the main window
						main_frame->Show(true);

						// Extra call to make sure Status bar is in correct pace in Release mode
						main_frame->ConfirmPositionStatusBar();

						// Update interface
						cmmSystemDialogUtil::UpdateDialog();
						chnlDialogUtil::UpdateZoom();

						#ifdef DEMO_VERSION
						//	Write the date to the registry as a quick and crude way to limit the users
						//	to a certain period of time.
						//
							#ifdef ENV_WINDOWS
							const int DAYS_VALID = 32;
							const WCHAR * lc_Reg_StartDate	= L"InitialConfig";
							const WCHAR * lc_Reg_LastDate	= L"LastConfig";
							wxString product_name(mnmConstants::c_PRODUCT, wxConvUTF8);
							wxString company_name(mnmConstants::c_COMPANY, wxConvUTF8);
							wxConfig reg_config(product_name, company_name);

							//	Get the current (Julian) date
							//
							unsigned short yr, mo, dy;
							appTime::GetDate( yr, mo, dy );
							int todays_date = appTimeUtils::DateToJulian( yr, mo, dy );

							//	check the start date entry
							if (reg_config.HasEntry(lc_Reg_StartDate))
							{
								wxString start_date_wxstr;
								if ( reg_config.Read(lc_Reg_StartDate, &start_date_wxstr) ) 
								{
									//	convert the start date string to a date (Julian)
									std::string start_date_str(start_date_wxstr.utf8_str());
									std::istringstream str(start_date_str);
									int start_date = 0; 
									str >> start_date;

									// Read last date from registry
									wxString last_date_wxstr;
									if ( reg_config.Read(lc_Reg_LastDate, &last_date_wxstr) ) 
									{
										//	convert the last date string to a date (Julian)
										std::string last_date_str(last_date_wxstr.utf8_str());
										std::istringstream str(last_date_str);
										int last_date = 0; 
										str >> last_date;

										//  check the dates, if today is before the start or last, the user reset the date/time
										//
										if (   (start_date > todays_date)
											|| (last_date > todays_date)
											|| (last_date < start_date))
										{
											lb_valid_demo = false;
										}
										else
										{
											//  check the dates, if today is before the start or last, the user reset the date/time
											//
											if ((last_date - start_date) > DAYS_VALID)
											{
												lb_valid_demo = false;
											}
											else
											{
												//	valid run, update last date with current date
												wxString current_date;
												current_date << todays_date;
												reg_config.Write(lc_Reg_LastDate, current_date);
											}
										}
									}
									else
									{
										//	the last date should be there...it must have been deleted
										lb_valid_demo = false;
									}
								}
								else
								{
									//	the start date exists, but can't be read...bad data?  abort
									lb_valid_demo = false;
								}
							}
							else
							{
								//	first run, create both
								wxString current_date;
								current_date << todays_date;
								reg_config.Write(lc_Reg_StartDate, current_date);
								reg_config.Write(lc_Reg_LastDate, current_date);
							}
							#endif

							//	display a welcome dialog every time for the demo version
							std::string msg;
							msg = "Welcome.  This is a 30-day trial version of ";
							msg += mnmConstants::c_PRODUCT;
							msg += ".\n\nThis trial version is fully functional except for the following limitations:\n\tThe final render output is restricted to JPEG 852x480\n\tThe final render output will have a watermark\n\nThank you for trying ";
							msg += mnmConstants::c_PRODUCT;
							msg += ".\n\nThe ";
							msg += mnmConstants::c_PRODUCT;
							msg += " Team\nwww.studioGPU.com";
							std::ostringstream oss;
							oss << msg.c_str();
							std::string theText(oss.str());
							msg = mnmConstants::c_PRODUCT;
							msg += " Trial Software Welcome Message";
							int retval = guiMessageBox::Show( theText.c_str(), msg.c_str(), guiMessageBox::e_OKOnly );
						#endif	// DEMO_VERSION

						#ifdef DEMO_VERSION
						//	If the valid is false then tell the user they can't run MS.
						if (lb_valid_demo)
						{
						#endif

						#ifdef TIME_LIMITED
							//	Get the current (Julian) date
							//
							unsigned short yr, mo, dy;
							appTime::GetDate( yr, mo, dy );
							int todays_date = appTimeUtils::DateToJulian( yr, mo, dy );

							//  check the dates, today should be between start and end dates.
							// If today is before the start, the user reset the date/time.
							//
							if (   (c_TimeLimitedStart > todays_date)
								|| (c_TimeLimitedEnd < todays_date) )
							{
								std::string msg;
								msg = "The testing period for this version of ";
								msg += mnmConstants::c_PRODUCT;
								msg += " has expired.  Please revert to the official released version.";
								std::string title;
								title = mnmConstants::c_PRODUCT;
								title += " Testing Software Expiration";
								guiMessageBox::Show( msg.c_str(), 
									title.c_str(), guiMessageBox::e_OKOnly );
								
								main_frame->Show(false);
							}
							else
							{
						#endif

								//if (license_valid)
									twxAppUtil::Run();

								// Write out the preferences
								main_frame->prefs_UpdateAndWrite();

						#ifdef TIME_LIMITED
							}
						#endif

						#ifdef DEMO_VERSION
						}
						else
						{
							//	Invalid system date/time
							//	pop-up a message box for the user
							//
							std::string msg;
							msg = "Your 30 day trial version of ";
							msg += mnmConstants::c_PRODUCT;
							msg += " has expired.  Please go to www.StudioGPU.com to find more info on purchasing ";
							msg += mnmConstants::c_PRODUCT;
							msg += ".  Thank you for trying ";
							msg += mnmConstants::c_PRODUCT;
							msg += ".\n\nThe ";
							msg += mnmConstants::c_PRODUCT;
							msg += " Team\nwww.studioGPU.com\n";

							std::ostringstream oss;
							oss << msg.c_str();
							std::string theText(oss.str());
							msg = mnmConstants::c_PRODUCT;
							msg += " Trial Software Expiration";
							int retval = guiMessageBox::Show( theText.c_str(), msg.c_str(), guiMessageBox::e_OKOnly );
						}
						#endif

					#else // USE_WXWIDGETS	
						// Want to have a click on the "X" button close the application
						appApplicationPAC::SetWantCloseMessage(true);

						// Don't want to have suspend messages so that we can render in the background in batch mode
						//	NOTE: Moved before command line
						//
						//appApplicationPAC::SetAllowSuspended(false);

					#ifdef TIME_LIMITED
						//	Get the current (Julian) date
						//
						unsigned short yr, mo, dy;
						appTime::GetDate( yr, mo, dy );
						int todays_date = appTimeUtils::DateToJulian( yr, mo, dy );

						//  check the dates, today should be between start and end dates.
						// If today is before the start, the user reset the date/time.
						//
						if (   (c_TimeLimitedStart > todays_date)
							|| (c_TimeLimitedEnd < todays_date) )
						{
							std::string msg;
							msg = "The testing period for this version of ";
							msg += mnmConstants::c_PRODUCT;
							msg += " has expired.  Please revert to the official released version.";
							std::cerr << msg.c_str() << std::endl;
							mnmAppUtil::SetErrorCode(mnmErrorCodes::c_InitializationError);
						}
						else
						{
					#endif	// TIME_LIMITED

						#ifdef FUSION //#if(SGPU_APP == MS_FUSION)
							pWindow->show();
							//splash->finish(pWindow);
							splash->HideScreen(pWindow);
							delete splash;
							splash = NULL;
							QApplication::restoreOverrideCursor();
							QApplication::setOverrideCursor(bitmapcursor);
						#endif

							mnmApp::RunApp();

					#ifdef TIME_LIMITED
						}
					#endif	// TIME_LIMITED

					#endif // USE_WXWIDGETS
					} // end mainInitDocument scope
				} // end mainInitApplication successful
				else
				{
					//mnmApp::Exit();
				}
			} // end mainInitGraphics scope
		} // end mainInitMainWindow scope
	} // end mainInitLibrary scope
#ifdef FUSION //#if(SGPU_APP == MS_FUSION)
QApplication::restoreOverrideCursor();
#endif
	return mnmAppUtil::GetErrorCode();
}

//----------------------------------------------------------------------------
// TryWinMain is a wrapper around DoWinMain that catches exceptions
// in order to pop up a window telling the user that the application
// is shutting down.
//----------------------------------------------------------------------------
int TryWinMain(	HINSTANCE hInstance,
				LPTSTR lpCmdLine,
				int nCmdShow)
{
	int ret_val = mnmErrorCodes::c_Success;
#if (CATCH_EXCEPTIONS)
	__try
	{
		ret_val = DoWinMain(hInstance, lpCmdLine, nCmdShow);
	}

	__except(HandleExceptionFilter(GetExceptionInformation()))
	{	
		std::cerr << "A critical error has occurred, the program needs to close." << std::endl;
#ifndef BATCH_MODE
		::MessageBoxA(NULL, "A critical error has occurred, the program needs to close. A dump file has been created.", "Critical error", MB_OK|MB_ICONERROR);
#endif
		ret_val = mnmErrorCodes::c_UnhandledException;
		TerminateProcess( GetCurrentProcess(),0);
	}

#else
	{
		try
		{
			ret_val = DoWinMain(hInstance, lpCmdLine, nCmdShow);
		}
		catch(...)
		{
			std::cerr << "A critical error has occurred, the program needs to close." << std::endl;
#ifndef BATCH_MODE
			::MessageBoxA(NULL, "A critical error has occurred, the program needs to close.", "Critical error", MB_OK|MB_ICONERROR);
#endif
		}
	}
#endif
	return ret_val;
}

static int sc_allocID = 0;

//----------------------------------------------------------------------------
//
// Catch all the exceptions and write it to a dump file
//
//----------------------------------------------------------------------------
#if (CATCH_EXCEPTIONS)
LONG HandleExceptionFilter(struct _EXCEPTION_POINTERS *ExceptionInfo)
{
	HANDLE hFile = NULL;
	HANDLE cProcess = NULL;
	cProcess = GetCurrentProcess();

	DWORD cProcessId = GetCurrentProcessId();
	DBG_LOG("Current Process Id " << cProcessId);

	fsLocator f_dumpFileName;
	std::string s_dumpFileName;
	f_dumpFileName = gfPaths::GetPath(gfPaths::e_UserDocumentsPath);
	f_dumpFileName.Push("CrashDump.dmp");
	fsFileUtil::LocatorToANSIFilename(f_dumpFileName, s_dumpFileName);

	hFile = CreateFileA(LPCSTR(s_dumpFileName.c_str()), GENERIC_WRITE, 0, NULL,
			CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL , NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		std::cerr << "Couldn't create the dump file." << std::endl;
#ifndef BATCH_MODE
		::MessageBoxA(NULL, "Couldn't create the dump file.","Critical Error", MB_OK|MB_ICONERROR);
#endif
	}
	HMODULE hDll = NULL;
	hDll = ::LoadLibrary(TEXT("DBGHELP.DLL"));
	{
		if (hDll)
		{
			MINIDUMPWRITEDUMP pDump = (MINIDUMPWRITEDUMP)::GetProcAddress(hDll, "MiniDumpWriteDump");
			if (pDump)
			{
				/*This enfo is used to get call stack in windbg.*/

				_MINIDUMP_EXCEPTION_INFORMATION eInfo;
				eInfo.ThreadId = GetCurrentThreadId();
				eInfo.ExceptionPointers = ExceptionInfo;
				eInfo.ClientPointers = NULL;
				BOOL bOK = pDump(cProcess, cProcessId,
								 hFile, MiniDumpNormal, ExceptionInfo ? &eInfo : NULL, NULL, NULL);
				if (!bOK)
				{
					std::cerr << "Minidump Failed" << std::endl;
#ifndef BATCH_MODE
					::MessageBoxA(NULL, "Minidump Failed","Critical Error", MB_OK|MB_ICONERROR);
#endif
				}
			}
			else
			{
				std::cerr << "Couldn't write the memory dump" << std::endl;
#ifndef BATCH_MODE
				::MessageBoxA(NULL, "Couldn't write the memory dump","Critical Error", MB_OK|MB_ICONERROR);
#endif
			}
		}
		else
		{
			std::cerr << "Couldn't load DBGHELP.DLL" << std::endl;
#ifndef BATCH_MODE
			::MessageBoxA(NULL, "Couldn't load DBGHELP.DLL","Error", MB_OK|MB_ICONERROR);
#endif
		}
	}
	CloseHandle(hFile);
	return EXCEPTION_EXECUTE_HANDLER;
}
#endif // CATCH_EXCEPTIONS

//----------------------------------------------------------------------------
//
// Main entry point
//
//----------------------------------------------------------------------------
int APIENTRY _tWinMain( HINSTANCE hInstance,
						HINSTANCE hPrevInstance,
						LPTSTR    lpCmdLine,
						int       nCmdShow )
{
	// In Debug wxWidgets non-managed version, the CRT will print out messages when it 
	// detects memory leaks. If the report is repeatable (same id number), then you can
	// put the id code number into the call to _CrtSetBreakAlloc and then the debugger
	// will stop at the line when the memory that is never freed is allocated.
	// This sample report:
	//{55870} normal block at 0x043982A8, 1792 bytes long.
	// would be debugged with this line
//	_CrtSetBreakAlloc(1702677); 
//	_CrtSetBreakAlloc(sc_allocID);

//	int tmpDbgFlag;
//	tmpDbgFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
////	tmpDbgFlag |= _CRTDBG_DELAY_FREE_MEM_DF;
//	tmpDbgFlag |= _CRTDBG_LEAK_CHECK_DF;
////	tmpDbgFlag |= _CRTDBG_CHECK_CRT_DF;
//	tmpDbgFlag |= _CRTDBG_ALLOC_MEM_DF;
//	_CrtSetDbgFlag(tmpDbgFlag);
//
//	_CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_DEBUG );

	//	don't allow MS to launch more than once.
	//
	// NOTE: How does this play out with Fast User Switching? [rjk]

	if (app_instance_exists())
	{
		std::cerr << "An instance of this app is already running" << std::endl;

		HWND hWndOtherInstance;
		hWndOtherInstance = FindWindowA(appApplicationPAC::GetWindowClassName(), NULL);	// window class, title
		if ( hWndOtherInstance != (HWND)NULL )
		{
			// Application is running in current user's session
			if (IsIconic(hWndOtherInstance))
				ShowWindow(hWndOtherInstance, SW_RESTORE);
			SetForegroundWindow(hWndOtherInstance);
		}
		else
		{
			//MessageBoxA(NULL, "An instance of this app is running","Critical error", MB_OK|MB_ICONERROR);
		}
		return mnmErrorCodes::c_AppAlreadyRunning;
	}

#ifdef FUSION //#if(SGPU_APP == MS_FUSION)
	int argc = 0;
	QApplication a(argc,NULL );
	appApplicationPAC::SetShowWindows(false);
	a.addLibraryPath("Media\\Plugins");
#endif

	// Do main work of application setup and run here:
	int ret_val = TryWinMain(hInstance, lpCmdLine, nCmdShow);

	if (g_hMutexAppRunning != NULL )
	{
		CloseHandle(g_hMutexAppRunning);
		g_hMutexAppRunning = NULL;
	}

	return ret_val;
}


#ifdef BATCH_MODE
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
int _tmain(int argc, TCHAR** argv)
{
	std::cout << mnmConstants::c_PRODUCT << " in batch mode" << std::endl;

	// Hide all windows created in batch mode
	appApplicationPAC::SetShowWindows(false);

	// Do main work of application setup and run here:
	HINSTANCE hInstance = ::GetModuleHandle(NULL);
	itString command_line;
	char* arg[1024];
	for (int i=1; i<argc; i++)
	{
//		std::cout << itString(argv[i]) << std::endl;

		if (i>1) command_line += L" ";
		command_line += itString(argv[i]);
		arg[i]= (char*)(argv[i]);
	}

	LPTSTR lpCmdLine = (LPTSTR)command_line.GetString();
#ifdef FUSION //#if(SGPU_APP == MS_FUSION)
	QApplication a(argc,arg );
#endif
	int nCmdShow = 0;
	int ret_val = TryWinMain(hInstance, lpCmdLine, nCmdShow);

	return ret_val;
}
#endif

