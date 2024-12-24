/*****************************************************************************
**	mnmApp.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003-10 - All Rights Reserved
\****************************************************************************/
#include "stdafx.h"
#include "MainApp/mnmApp.hpp"

#include "MainApp/mainConstants.hpp"
#include "MainApp/wxGUI/wxMainForm.hpp"

#ifdef FUSION
#include "FCSupport/fcmd/fcmdModeMgr.hpp"
#include "FCSupport/fcui/fcuiFormMgr.hpp"
#endif
#include "Features/Capture/cptrPackage.hpp"
#include "Features/Capture/cptrRenderUtil.hpp"
#include "Features/ObjectManip/mnpPackage.hpp"
#include "Features/Prefs/PrefsData.hpp"
#include "Features/Prefs/PrefsMgr.hpp"
#include "Features/RenderPanels/rpnPanelViewer.hpp"
#include "Features/RenderPanels/wxGUI/rpnRenderPanel.hpp"
#include "Features/RenderPanels/wxGUI/rpnPanelGrid.hpp"
#include "Features/SceneSetup/Data/SceneSetupData.hpp"
#include "Support/capt/captRenderOutputDataUtil.hpp"
#include "Support/fgmt/fgmtHighlight.hpp"
#include "Support/mnm/mnmConstants.hpp"
#include "Support/mnm/mnmDebugInfo.hpp"
#include "Support/mnm/mnmPaths.hpp"
#include "Support/mnm/mnmScreenCaptureUtil.hpp"
#include "Support/mnm/mnmSecurityMgr.hpp"
#include "Support/mnm/mnmTimeCodeUtil.hpp"
#include "Support/mnm/mnmVJoystick.hpp"
#include "Support/mode/modeModeMgr.hpp"
#include "Support/mtrl/mtrlHighlight.hpp"
#include "Support/tmln/tmlnTimeLine.hpp"
#include "Support/xfrm/xfrmTransformMgr.hpp"
#include "Systems/Character/GUI/chtrDialogInterest.hpp"

//	library
#include "Core/app/appCharEvent.hpp"
#include "Core/app/appCharEventHandler.hpp"
#include "Core/app/appFlowEvent.hpp"
#include "Core/app/appMouseEvent.hpp"
#include "Core/app/appMouseEventHandler.hpp"
#include "Core/app/appPackage.hpp"
#include "Core/app/appSimTime.hpp"
#include "Core/app/appTime.hpp"
#include "Core/app/private/appApplicationPAC.hpp"
#include "Core/dbg/dbgPackage.hpp"
#include "Core/env/envPackage.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Core/Env/envThread.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/fs/fsPackage.hpp"
#include "Core/gf/gfPackage.hpp"
#include "Core/it/itPackage.hpp"
#include "Core/It/itStringUtil.hpp"
#include "Core/Ma/maFunctions.hpp"
#include "Graphics/GraphicsLayer.hpp"
#include "Graphics/g2d/g2dExceptionX.hpp"
#include "Graphics/g2d/g2dFontUtil.hpp"
#include "Graphics/g2d/g2dPackage.hpp"
#include "Graphics/g2d/g2dResourceCounter.hpp"
#include "Graphics/g2d/g2dSystem.hpp"
#include "Graphics/g2d/g2dWindow.hpp"
#include "Graphics/g3d/g3dConstants.hpp"
#include "Graphics/g3d/g3dDirectionalLight.hpp"
#include "Graphics/g3d/g3dExceptionX.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dPackage.hpp"
#include "Graphics/g3d/g3dPrefs.hpp"
#include "Graphics/g3d/g3dPrimitiveFragmentUtil.hpp"
#include "Graphics/g3d/g3dSystem.hpp"
#include "Graphics/G3d/g3dThreadControl.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mat/matShaderMgr.hpp"
#include "Graphics/mdl/mdlReader.hpp"
#include "Input/in/inDeviceMgr.hpp"
#include "Input/in/inPackage.hpp"
#include "Support/pyth/pythUtil.hpp"
#include "Tool/api3d/api3dLightMgr.hpp"
#include "Tool/api3d/api3dScene.hpp"
#include "Tool/api3d/api3dTargetRendererMgr.hpp"
#include "Tool/cam3d/cam3dMgr.hpp"
#include "Tool/doc/docSingleTypeMgr.hpp"
#include "Tool/gpx/gpxProxyMgr.hpp"
#include "Tool/gpx/gpxRenderControl.hpp"
#include "Tool/gui/guiMenuMgr.hpp"
#include "Tool/gui/guiMessageBox.hpp"
#include "Tool/gui/guiSingleDocHandler.hpp"
#include "Tool/gui/guiStatusBarMgr.hpp"
#include "Tool/tma3d/tma3dCursorMgr.hpp"
#include "Tool/tma3d/tma3dRenderView.hpp"
#include "Tool/tma3d/tma3dScreenUtil.hpp"
#include "Tool/tma3d/tma3dViewerMgr.hpp"
#include "ToolUIWx/wui/wuiSplashScreen.hpp"

#include <assert.h>
#include <vector>


//============================================================================
// Compiler define which turns on rendering in a separate thread.
// Multi-threaded rendering requires that graphical proxies are used.
//============================================================================
#if USE_PROXIES
	#ifdef BATCH_MODE
		const bool c_USE_RENDER_THREAD = false;
	#else
		const bool c_USE_RENDER_THREAD = true;
	#endif
#else
	const bool c_USE_RENDER_THREAD = false;
#endif


//============================================================================
//============================================================================
bool mnmApp::sm_bThreadingEnabled = true; // default needs to match PrefsData constructor
maRunningAverage mnmApp::sm_FPSAverage;


//============================================================================
//	namespace
//============================================================================
namespace
{
	//g3dRenderState* l_pRootRenderState = NULL;
	//g3dDirectionalLight *l_pDirLight = NULL; // temp until lights system

	mnmApp * l_pAppInstance = NULL;
	bool	l_bRenderEnabled = true;
	bool	l_bDebugDisplay = false;
	bool	l_bActive = false;
	bool	l_bDidUpdateDeferredTime = false;
	int		l_nScreenSpaceIndex = 0;
	//int		l_nIconsLayerIndex = 0;
	//int		l_nManipulatorsLayerIndex = 0;

#if ENV_BUILD == ENV_DEBUGBUILD
	const int	lc_SECURITY_CHECK_DELAY_NORMAL	= 60.0f * 0.5f;		// 30 seconds (for testing only)
	const int	lc_SECURITY_CHECK_DELAY_INVALID	= 60.0f * 1.0f;		// 
	const int	lc_SECURITY_VALID_DELAY_NORMAL	= 60.0f * 0.125f;	// less than "checks"
	const int	lc_SECURITY_VALID_DELAY_INVALID	= 60.0f * 0.25f;	// 
	const UINT	lc_SECURITY_INVALID_COUNTER_MAX	= 2;
#else
	const int	lc_SECURITY_CHECK_DELAY_NORMAL	= 60.0f * 30.0f;	// 30 minutes
	const int	lc_SECURITY_CHECK_DELAY_INVALID	= 60.0f * 5.0f;		// 5 minutes
	const int	lc_SECURITY_VALID_DELAY_NORMAL	= 60.0f * 20.0f;	// 20 minutes
	const int	lc_SECURITY_VALID_DELAY_INVALID	= 60.0f * 3.0f;	// 10 minutes
	const UINT	lc_SECURITY_INVALID_COUNTER_MAX	= 5;
#endif
	const int	lc_SECURITY_VALID_TIME_RESET	= -1;
	//const UINT	lc_SECURITY_INVALID_COUNTER_MAX	= 1;				// 1 (for testing only)

	bool	l_bSecurityValid = true;
	UINT	l_SecurityInvalidCounter	= 0;
	float	l_SecurityLastCheck			= 0;
	float	l_SecurityValidTime			= lc_SECURITY_VALID_TIME_RESET;
	float	l_SecurityValidTimeDelay	= lc_SECURITY_VALID_DELAY_NORMAL;
	float	l_SecurityLastCheckDelay	= lc_SECURITY_CHECK_DELAY_NORMAL;

	// in KB
	float	l_MaxVideoMem = 0;
	// in KB
	float	l_CurrentVideoMem = 0;

	// The Qt window size
	int l_WindowWidth = 0;
	int l_WindowHeight = 0;

	// moved to mnmConstants
	//const char* c_ViewerFontName = "font-lucd00.png";
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
void render_3dWindow()
{
	if (l_pAppInstance)
	{
		float begin_time = appTime::GetTime();

		l_pAppInstance->Render();

		float duration = appTime::GetTime() - begin_time;
		if (duration > 1.0f)
		{
			//DBG_LOG("Long render " << duration);
		}
	}
}


//
//	mnmApp functions
//


//--------------------------------------------------------------------
//	RunApp() should be called to start the application.  When control
//	returns from RunApp(), the application is finished.
//--------------------------------------------------------------------
//static 
void mnmApp::RunApp()
{
	DBG_ASSERT( l_pAppInstance,"The app has not been created");

	l_pAppInstance->Run();
}

//--------------------------------------------------------------------
//	Thinks the app instance
//--------------------------------------------------------------------
//static
void mnmApp::ThinkApp()
{
	DBG_ASSERT( l_pAppInstance,"The app has not been created");

	l_pAppInstance->TryThink();
}

//--------------------------------------------------------------------
//	Thinks the app instance
//--------------------------------------------------------------------
//static
bool mnmApp::IsActive()
{
	return l_bActive;

	//if (l_pAppInstance)
	//{
	//	return true;
	//}

	//return false;
}


//--------------------------------------------------------------------
//	set this app as "active"
//--------------------------------------------------------------------
//static 
void mnmApp::SetActive( bool i_bActive )
{
	l_bActive = i_bActive;
}

//--------------------------------------------------------------------
// Enable/Disable rendering in main window
//--------------------------------------------------------------------
//static
void mnmApp::EnableRender(bool i_bVal)
{
	l_bRenderEnabled = i_bVal;
}
//static
bool mnmApp::IsRenderEnabled()
{
	return l_bRenderEnabled;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//static 
int mnmApp::GetScreenSpaceIndex()
{
	return l_nScreenSpaceIndex;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//static 
//int mnmApp::GetIconsLayerIndex()
//{
//	return l_nIconsLayerIndex;
//}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//static 
//int mnmApp::GetManipulatorsLayerIndex()
//{
//	return l_nManipulatorsLayerIndex;
//}

//--------------------------------------------------------------------
//	parse the command line and configure the app accordingly
//--------------------------------------------------------------------
//static
void mnmApp::ParseCommandLine(std::string& i_lpCmdLine)
{
	if ( i_lpCmdLine.length() == 0 )
		return;

	DBG_TRACE("Command Line: " << i_lpCmdLine.c_str());

	bool bParsedAnyData	= false;
	char seps[]		= " \t\n";
	char seps_tok1[]	= "\"";
	char seps_tok2[]	= " \"";
	char seps3[]	= "\n";
	char *token;

	char pLine[1024];
	strcpy( pLine, i_lpCmdLine.c_str() );
	token = strtok( pLine, seps );
	while( token != NULL )
	{
		//DBG_TRACE( "   " << token );

		if (   ( strcmp(token,"-file") == 0 )
			|| ( strcmp(token,"/file") == 0 ))
		{
			token = strtok( NULL, seps_tok1 );
			DBG_LOG( "   Command-line FILE " << token );
			fsLocator scenefile;
			fsFileUtil::ANSIFilenameToLocator(std::string(token), scenefile );

			mnpPackage::LaunchFile( scenefile );
			bParsedAnyData	= true;
		}
		else if (   ( strcmp(token, "-lastfile") == 0 )
				 || ( strcmp(token, "/lastfile") == 0 ))
		{
			std::vector<fsLocator>	mru_list;
			docSingleTypeMgr::GetMRUList( mru_list );
			if ((mru_list.size()) > 0 && (mru_list[0].GetNumNames() > 0))
			{
				fsLocator lastfile = mru_list[0];
				mnpPackage::LaunchFile( lastfile );
			}
			bParsedAnyData	= true;
		}
		else if (   ( strcmp(token,"-help") == 0 )
				 || ( strcmp(token,"/help") == 0 ))
		{
			// TODO - Implement a list of available commands on the command line
			std::cout << "Command Line Parameters";
			std::cout << "-----------------------";
			std::cout << "-file";
			std::cout << "-lastfile";
		}
		else if (   ( strcmp(token,"-show") == 0 )
				 || ( strcmp(token,"/show") == 0 ))
		{
			// Batch mode usually hides render windows, use /show
			// to display them.
			appApplicationPAC::SetShowWindows(true);
		}
		else if (   ( strcmp(token,"-batch") == 0 )
				 || ( strcmp(token,"/batch") == 0 ))
		{
			token = strtok( NULL, seps_tok1 );
			DBG_LOG( "   Command-line BATCH-RENDER " << token );

			fsLocator scenefile;
			fsFileUtil::ANSIFilenameToLocator(std::string(token), scenefile );

			cptrPackage::LaunchBatchRender( scenefile );
			bParsedAnyData	= true;
		}
		else if (   ( strcmp(token,"-render") == 0 )
				 || ( strcmp(token,"/render") == 0 ))
		{
			token = strtok( NULL, seps_tok1 );
			DBG_LOG( "   Command-line RENDER " << token );

			fsLocator scenefile;
			fsFileUtil::ANSIFilenameToLocator(std::string(token), scenefile );

			cptrPackage::LaunchRender( scenefile );
			bParsedAnyData	= true;
		}
		else if (   ( strcmp(token,"-script") == 0 )
				 || ( strcmp(token,"/script") == 0 ))
		{
			size_t startpos = i_lpCmdLine.find("script");
			DBG_ASSERT( startpos != std::string::npos, "SCRIPT command not found when accessing script prompt" );
			
			std::string command_line_string = i_lpCmdLine.substr(startpos);

			startpos = command_line_string.find_first_not_of("script");
			size_t endpos = command_line_string.find_last_not_of("\"");
			size_t endpos2 = command_line_string.find_last_of("\"");
			if( endpos2 != std::string::npos &&
				endpos2 < endpos &&
				endpos2 > startpos )
				endpos = endpos2;
			if( endpos == std::string::npos )
				endpos = command_line_string.length();

			std::string script_command = command_line_string.substr( startpos+1, endpos-startpos );
			startpos = script_command.find_first_not_of(" ");
			if( startpos != std::string::npos )
				script_command = script_command.substr( startpos );

			DBG_LOG( "   Script Line " << script_command );

			std::string command_string;
			std::string script_string;
			std::string args_string;

			//grab just the .py
			startpos = script_command.find_first_not_of("\"");
			endpos = script_command.find_last_not_of("\"");
			if( startpos == std::string::npos )
				startpos = 0;
			if( endpos == std::string::npos )
				endpos = script_command.length();

			command_string = script_command.substr(startpos, endpos+1);
			std::string::size_type ext = command_string.find(".py", 0);
			ext += 3;
			if ( ext != std::string::npos)
			{
				script_string = command_string.substr(0, ext);
				args_string = command_string.substr(ext);
				startpos = args_string.find_first_not_of(" ");
				if( startpos != std::string::npos )
					args_string = args_string.substr(startpos);
			}

			fsLocator script_file;
			fsFileUtil::ANSIFilenameToLocator(script_string, script_file );
			
			if (fsFileUtil::FileExists( script_file ))
			{
				if (args_string != "")
					pythUtil::ScriptFile( script_file, args_string );
				else
					pythUtil::ScriptFile( script_file );
			}
			bParsedAnyData	= true;
		}
		else if (   ( strcmp(token,"-capFormat") == 0 )
				 || ( strcmp(token,"/capFormat") == 0 ))
		{
			token = strtok( NULL, seps_tok2 );
			DBG_LOG( "   Command-line CAPTURE-FORMAT " << token );
			itString format(token);
			captRenderOutputDataUtil::SetCaptureFormat( format );
			bParsedAnyData	= true;
		}
		else if (   ( strcmp(token,"-capCompress") == 0 )
				 || ( strcmp(token,"/capCompress") == 0 ))
		{
			token = strtok( NULL, seps_tok2 );
			DBG_LOG( "   Command-line CAPTURE-COMPRESS " << token );
			itString code(token);
			captRenderOutputDataUtil::SetCompressCode( code );
			bParsedAnyData	= true;
		}
		else if (   ( strcmp(token,"-capCamera") == 0 )
				 || ( strcmp(token,"/capCamera") == 0 ))
		{
			token = strtok( NULL, seps_tok1 );
			DBG_LOG( "   Command-line CAPTURE-CAMERA " << token );
			itString cam(token);
			captRenderOutputDataUtil::SetActiveCamera( cam );
			bParsedAnyData	= true;
		}
		else if (   ( strcmp(token,"-capWidth") == 0 )
				 || ( strcmp(token,"/capWidth") == 0 ))
		{
			token = strtok( NULL, seps_tok2 );
			DBG_LOG( "   Command-line CAPTURE-WIDTH " << token );
			itString width(token);
			captRenderOutputDataUtil::SetCaptureWidth(itStringUtil::GetInt(width));
			bParsedAnyData	= true;
		}
		else if (   ( strcmp(token,"-capHeight") == 0 )
				 || ( strcmp(token,"/capHeight") == 0 ))
		{
			token = strtok( NULL, seps_tok2 );
			DBG_LOG( "   Command-line CAPTURE-HEIGHT " << token );
			itString height(token);
			captRenderOutputDataUtil::SetCaptureHeight(itStringUtil::GetInt(height));
			bParsedAnyData	= true;
		}
		else if (   ( strcmp(token,"-capOutputFile") == 0 )
				 || ( strcmp(token,"/capOutputFile") == 0 ))
		{
			token = strtok( NULL, seps_tok1 );
			DBG_LOG( "   Command-line CAPTURE-OUTPUTFILE " << token );
			itString file(token);
			captRenderOutputDataUtil::SetCaptureOutputFile( file );
			bParsedAnyData	= true;
		}
		else if (   ( strcmp(token,"-capOutputPath") == 0 )
				 || ( strcmp(token,"/capOutputPath") == 0 ))
		{
			token = strtok( NULL, seps_tok1 );
			DBG_LOG( "   Command-line CAPTURE-OUTPUTPATH " << token );
			itString path(token);
			captRenderOutputDataUtil::SetCaptureOutputDirectory( path );
			bParsedAnyData	= true;
		}
		else if (   ( strcmp(token,"-capOutputPathRoot") == 0 )
				 || ( strcmp(token,"/capOutputPathRoot") == 0 ))
		{
			token = strtok( NULL, seps_tok1 );
			DBG_LOG( "   Command-line CAPTURE-OUTPUTPATHROOT " << token );
			itString path(token);
			captRenderOutputDataUtil::SetCaptureOutputDirectoryRoot( path );
			bParsedAnyData	= true;
		}
		else if (   ( strcmp(token,"-NoSplash") == 0 )
				 || ( strcmp(token,"/NoSplash") == 0 ))
		{
			splashscreen::SetSplashFlag(false);
			bParsedAnyData = true;
		}
		else if (   ( strcmp(token,"-new") == 0 )
				 || ( strcmp(token,"/new") == 0 ))
		{
			DBG_LOG( "   Command-line NEW" );

			mnpPackage::LaunchNew();
			bParsedAnyData	= true;
		}
		else if (   ( strcmp(token,"-license") == 0 )
				 || ( strcmp(token,"/license") == 0 ))
		{
			token = strtok( NULL, seps_tok1 );
			DBG_LOG( "   Command-line license " << token );

			std::string license_str(token);
			mnmSecurityMgr::ValidateLicense(license_str);
			bParsedAnyData	= true;
		}
		else if (   ( strcmp(token,"-version") == 0 )
				 || ( strcmp(token,"/version") == 0 ))
		{
			itString helpMessage = itString(mnmConstants::c_PRODUCT_FOR_DISPLAY);
			helpMessage += L" ";
		#ifdef WIN64
			helpMessage += itString(mnmConstants::cw_64BIT);
		#else
			#ifdef WIN32
			helpMessage += itString(mnmConstants::cw_32BIT);
			#endif
		#endif
			helpMessage += L" (";
			helpMessage += itString(mainConstants::mc_ExecutableVersion);
			helpMessage += L")";
			helpMessage += L"\r\n";
			helpMessage += L"\r\n";
			helpMessage += mnmConstants::cw_COPYRIGHT;
			helpMessage += L"\r\n";
			helpMessage += mnmConstants::cw_RIGHTS;
			helpMessage += L"\r\n";
			std::string outputstr = itStringUtil::GetStdString(helpMessage);
			std::cout << outputstr.c_str();
		}
		else
		{
			if (!bParsedAnyData)
			{
				//	If the code gets to this case then we assume the user
				//	has double-clicked on an MAB and launched the app via
				//	file type association.
				//
				//	Ignore the token and use the whole line.
				//
				size_t startpos = i_lpCmdLine.find_first_not_of("\"");
				size_t endpos = i_lpCmdLine.find_last_not_of("\"");
				i_lpCmdLine = i_lpCmdLine.substr( startpos, endpos-startpos+1 );
				DBG_LOG( "   Open File from command line = " << i_lpCmdLine.c_str() );

				fsLocator scenefile;
				std::string typeCheck;
				fsFileUtil::ANSIFilenameToLocator(i_lpCmdLine, scenefile );
				if ( fsFileUtil::FileExists( scenefile ) )
				{	
					fsFileUtil::LocatorToANSIFilename(scenefile, typeCheck);
					if (typeCheck.find(".mab") != std::string::npos)
					{
						//DBG_TRACE( "   Opening Scene " << token );
						mnpPackage::LaunchFile( scenefile );
					}
					else if ((typeCheck.find(".chx") != std::string::npos)
						  || (typeCheck.find(".gxb") != std::string::npos) )
					{
						SceneSetupData SceneData;
						std::string chtrFileName;

						//get the project directory for this character file
						fsLocator tempLocator = scenefile;
						tempLocator.RemoveAfter(itString("Data"));
						tempLocator.Pop();
						std::string tempString;
						fsFileUtil::LocatorToANSIFilename(tempLocator, tempString);
						//DBG_TRACE( "   projectDirectory " << tempString.c_str());

						SceneData.m_Data.m_ProjectDirectory = tempString;

						//Get the project name
						tempString = itStringUtil::GetStdString(tempLocator.GetLastName());
						//DBG_TRACE( "   projectName " << tempString.c_str());
						SceneData.m_Data.m_ProjectName = tempString;

						//Create a new scene file name with tempScene appended after the character's file name
						tempString = itStringUtil::GetStdString(scenefile.GetLastName());
						chtrFileName = tempString;
						tempString = tempString.substr(0, tempString.find("."));
						tempString += "_tempScene";
						//DBG_TRACE( "   New Scene File name " << tempString.c_str() );
						SceneData.m_Data.m_SceneName = tempString;

						//scene info is complete
						SceneData.m_Data.m_bFinished = true;

						//	prepare the initial scene directory based on project settings
						//
						itString scene_name(SceneData.m_Data.m_SceneName.c_str());
						fsLocator project_dir;
						fsFileUtil::ANSIFilenameToLocator( SceneData.m_Data.m_ProjectDirectory, project_dir );
						mnmPaths::SetupPaths( project_dir, scene_name );
						guiSingleDocHandler::SetInitialDirectory( gfPaths::GetPath( mnmPaths::e_SaveShots ) );

						chtrDialogInterest newCharacter;
						newCharacter.AddObject(itString(chtrFileName.c_str()), scenefile);
					}
				}

				return;
			}
		}

		token = strtok( NULL, seps );
	}
}

//--------------------------------------------------------------------
//static 
// return value in KB
//--------------------------------------------------------------------
float mnmApp::QueryVideoMemory()
{
	if ( l_pAppInstance && l_pAppInstance->GetSystem3D() )
	{
		// *1000 to turn KB into bytes
		l_CurrentVideoMem = g2dResourceCounter::GetTotalVideoMemory();
		//l_CurrentVideoMem = l_pAppInstance->GetSystem3D()->GetVideoMemory();
		return l_CurrentVideoMem;
	}
	return -1;
}

//--------------------------------------------------------------------
// return value in KB
//--------------------------------------------------------------------
float mnmApp::QueryMaxVideoMemory()
{
	return l_MaxVideoMem;
}

//------------------------------------------------------------------------
// Enable viewport multithreading from preferences
//------------------------------------------------------------------------
void mnmApp::SetThreadingEnabled(bool i_bThreading)
{
	sm_bThreadingEnabled = i_bThreading;
}

void mnmApp::SetWindowSize(int i_width, int i_height)
{
	l_WindowWidth = i_width;
	l_WindowHeight = i_height;
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
mnmApp::mnmApp()
:	m_pSystem(NULL), 
	m_pSystem3D(NULL)
{
	l_pAppInstance = this;

	mnmApp::sm_FPSAverage.SetHistorySize(100);
	mnmApp::SetActive( false );
	
	std::vector<fsLocator> iconPathList;
	gfPaths::GetPathList(mnmPaths::e_ExeArt, iconPathList);

	guiMenuMgr::SetIconDirectory( iconPathList );

	l_SecurityInvalidCounter	= 0;
	l_SecurityValidTimeDelay	= lc_SECURITY_VALID_DELAY_NORMAL;
	l_SecurityLastCheckDelay	= lc_SECURITY_CHECK_DELAY_NORMAL;

	//modeModeMgr::Initialize();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
mnmApp::~mnmApp()
{
	//modeModeMgr::DestroyModes();
	//modeModeMgr::DeInitialize();

	if (l_pAppInstance == this)
	{
		l_pAppInstance = NULL;
	}
}

//--------------------------------------------------------------------
//
//--------------------------------------------------------------------
void mnmApp::InitializeRender(void *i_HwndFake, void *i_Hwnd, int i_Width, int i_Height)
{
	m_pSystem = GraphicsLayer::GetSystem2D();

	// This window is owned by the system
	g2dWindow *app_window = NULL;
	if (!i_HwndFake && !i_Hwnd)
	{
		app_window = m_pSystem->CreateSubWindow(appApplication::GetMainWindowHandle());
	}
	else
	{
		app_window = m_pSystem->CreateSubWindow(i_Hwnd);
	}

	// Start with effShaderArray enabled,
	// need to do this before the 3D system is created
	matShaderMgr::SetUseShaderArray(true);

	m_pSystem3D = GraphicsLayer::GetSystem3D();
	l_MaxVideoMem = m_pSystem3D->GetVideoMemory() / 1024;
	//float vidmem = l_MaxVideoMem;

#ifdef DEMO_VERSION
	//	limit the memory for the demo
	const int lc_MINIMUM_VIDEO_MEMORY = 1000000;	// 1gb = 1073741824 4gb = 4021288960 (divide by 1024 to get constant)
	//const envType::UInt64 lc_MINIMUM_VIDEO_MEMORY = 8000000000;	// 1gb = 1073741824 4gb = 4021288960
	if (l_MaxVideoMem < lc_MINIMUM_VIDEO_MEMORY)
	{
		std::string msg = "Unfortunately, your graphics card does not have a sufficient amount of memory to run this trial version of ";
		msg += mnmConstants::c_PRODUCT;
		msg += ".  To run the trial software, you need a graphics card with at least 1 GB of memory.  Please go to www.StudioGPU.com/trial to see detailed recommendations on configurations for this software.";

		DBG_ERROR(msg);
		guiMessageBox::Show(msg.c_str(), "Insufficient Video Card Memory", guiMessageBox::e_OKOnly);
		this->Exit();
		return;
	}
#endif	// DEMO_VERSION

	//	Get the user-set framerate early
	float fps = PrefsMgr::Data().m_FrameRate.GetValue();
	//	if the frame rate is valid, set it
	//
	if (fps > 0.0f)		// should there be an upper bound?
		g3dConstants::c_fDefaultFrameRate = fps;

	//
	api3dLightMgr::Initialize();
	//l_pDirLight = api3dLightMgr::CreateDirectionalLight();
	//l_pDirLight->SetDirection(maVector3d(0.2f, -0.7f, 0.1f));
	cam3dMgr::Initialize();
	cam3dMgr::SetManipFastRender(PrefsMgr::Data().m_bManipFastRender.GetValue());
	api3dScene::Initialize();
	//api3dBakeScene::Initialize();

	// Create main render view's window
	//g2dWindow *window = m_pSystem->CreateSubWindow(i_Hwnd);
	cptrRenderUtil::SetAppWindow( app_window );
	cptrRenderUtil::SetCaptureWindow( app_window );
	mnmScreenCaptureUtil::SetCaptureWindow( app_window );
	mnmDebugInfo::SetWindow( app_window );
	mnmTimeCodeUtil::SetMainWindow( app_window );
	mnmTimeCodeUtil::SetWindow( app_window );

	// Create render view class for the main view
	rpnPanelViewer *pPanelViewer = new rpnPanelViewer();
	m_PanelViewers.push_back(pPanelViewer);
	pPanelViewer->SetAllowShadows(true); // Allow shadows in the main window
	tma3dRenderView *pMainView = new tma3dRenderView(app_window, pPanelViewer, pPanelViewer->GetLayers());
	//#if(SGPU_APP == MS_FUSION)
	//	pMainView->ResizeWindow(l_WindowWidth,l_WindowHeight);
	//#endif
	pMainView->SetRenderer(tma3dRenderView::e_Default);
	this->m_RenderViews.push_back(pMainView);
	tma3dRenderView::SetActiveRenderView(pMainView);

	std::vector<dbgStream*> _osList = dbgMsg::GetStreamList(); 
	std::vector<bool> doThreadList(_osList.size());
	std::vector<bool> doFileList(_osList.size());

	for(int i = 0; i < _osList.size(); ++i)
	{
		doThreadList[i] = _osList[i]->GetData()->m_WriteThreadID;
		doFileList[i] = _osList[i]->GetData()->m_WriteFileName_Line;
		dbgMsg::enableThreadStamp(_osList[i]->GetName(), false);
		dbgMsg::enableFileNameStamp(_osList[i]->GetName(), false);
	}
	DBG_LOG("Video Hardware Name: " << m_pSystem3D->GetVideoAdapterName());
	for(int i = 0; i < _osList.size(); ++i)
	{
		if (doThreadList[i])
			dbgMsg::enableThreadStamp(_osList[i]->GetName(), true);
		if (doFileList[i])
			dbgMsg::enableFileNameStamp(_osList[i]->GetName(), true);
	}
	// Force ambient color in materials to be full white
	mdlReader::SetAlwaysFullAmbient(true);

	//	Create a screen-space layer
	//
	l_nScreenSpaceIndex = api3dScene::AddLayer( g3dLayer::e_ZBuffer,
												g3dLayer::e_Screen, g3dLayer::e_Additive,
												false, false, true, false );

	//bga - Icons and manipulators are now in layers that are owned
	//	by the render panels...
	////	Create a world space layer for icons only:
	//l_nIconsLayerIndex = api3dScene::AddLayer( g3dLayer::e_ZBuffer,
	//	g3dLayer::e_World, g3dLayer::e_Additive,
	//	false, false, false, false);
	////	Create a world space layer for compass manipulators only:
	//// (note that this layer clears the z buffer in order to always draw manipulators on top!)
	//l_nManipulatorsLayerIndex = api3dScene::AddLayer( g3dLayer::e_ZBuffer,
	//	g3dLayer::e_World, g3dLayer::e_Additive,
	//	false, false, false, true);

	
	// Now we can create the text label for the main render window
	// after the shaders are enabled and the screen layer has been created.
	fsLocator font_loc;
	font_loc.Push( gfPaths::GetPath(mnmPaths::e_ExeArt) );
	font_loc.Push( mnmConstants::c_ViewerFontName );
	try
	{
		pPanelViewer->CreateTextLabel( font_loc );
	}
	catch ( const envExceptionX& i_Ex )
	{
		std::string msg = "Error creating text label, " + i_Ex.GetErrorMessage();
		DBG_ERROR(msg);
		guiMessageBox::Show(msg.c_str(), "Font Error", guiMessageBox::e_OKOnly);
	}
}

//--------------------------------------------------------------------
// Create an additional render view using the given HWND
//--------------------------------------------------------------------
void mnmApp::CreateRenderView(void *i_Hwnd)
{
	// RenderView class takes ownership of renderer
	g2dWindow *window = m_pSystem->CreateSubWindow(i_Hwnd);
	rpnPanelViewer *pPanelViewer = new rpnPanelViewer();
	m_PanelViewers.push_back(pPanelViewer);
	tma3dRenderView *pView = new tma3dRenderView(window, pPanelViewer, pPanelViewer->GetLayers());
	pView->EnableViewer(false);	// Starting with single pane, this view will be invisible

	// Tried using simple renderer, but that did not do camera space correctly?
	//pView->SetRenderer(tma3dRenderView::e_Simple);

	fsLocator font_loc;
	font_loc.Push( gfPaths::GetPath(mnmPaths::e_ExeArt) );
	font_loc.Push( mnmConstants::c_ViewerFontName );
	pPanelViewer->CreateTextLabel( font_loc );

	this->m_RenderViews.push_back(pView);
}

//--------------------------------------------------------------------
// GetRenderView
//--------------------------------------------------------------------
tma3dRenderView* mnmApp::GetRenderView(int i_Index)
{
	DBG_ASSERT((i_Index>=0 && i_Index<m_RenderViews.size()), "Render View " << i_Index << " out of range " << m_RenderViews.size());
	return this->m_RenderViews[i_Index];
}

//--------------------------------------------------------------------
// GetNumberOfRenderViews - return the number of render views 
//--------------------------------------------------------------------
int mnmApp::GetNumberOfRenderViews()
{
	return this->m_RenderViews.size();
}

//--------------------------------------------------------------------
//
//--------------------------------------------------------------------
void mnmApp::DeInitializeRender()
{
	envSTLHelpers::DeleteContainer(m_RenderViews);

	//api3dBakeScene::DeInitialize();
	api3dScene::DeInitialize();
	cam3dMgr::DeInitialize();

	api3dLightMgr::DeInitialize();
}

//--------------------------------------------------------------------
// Wrapper around Think() that handles exceptions
//--------------------------------------------------------------------
void mnmApp::TryThink()
{
	//bga - I would like to handle exceptions here, but I am not sure what we could
	// do about them. I don't want to continually pop up messages when the rendering fails
	// because the user will need to be able to click other menu items in order to 
	// fix the problem. 
	//
	try
	{
		this->Think();
	}
	catch ( const envExceptionX& i_Ex)
	{
		DBG_ERROR("Problem occurred during render: " << i_Ex.GetErrorMessage());
		guiMessageBox::Show(i_Ex.GetErrorMessage().c_str(), "Error");
		this->Exit();
	}
	catch (const std::bad_alloc&)
	{
		std::string msg = "Out of system memory, could not render";
		DBG_ERROR(msg);
		guiMessageBox::Show(msg.c_str(), "Error");
		this->Exit();
	}
	catch (const std::exception& i_Ex)
	{
		std::string msg = "Problem occurred during render: " + std::string(i_Ex.what());
		DBG_ERROR(msg);
		guiMessageBox::Show(msg.c_str(), "Error");
		this->Exit();
	}
	catch(...)
	{
		std::string msg = "General exception error during render.";
		DBG_ERROR(msg);
		guiMessageBox::Show(msg.c_str(), "Error");
		this->Exit();
	}
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
void mnmApp::Think()
{
	//	poll devices
	//
	inDeviceMgr::Think();

	// think the cursor manager
	tma3dCursorMgr::Think();

	// think the camera manager
	//mnmCameraMgr::Think();

	// Look for tilde hotkey here instead of in 
	// the window code so that we can turn on 
	// a tighter render loop, even when not currently rendering.
	if ( inDeviceMgr::GetKeyboard() &&
		inDeviceMgr::GetKeyboard()->IsReleased(inKeys::e_TILDE) )
	{
		l_bDebugDisplay = !l_bDebugDisplay;
		gpxRenderControl::SetNeedsNewRender(); // need one more render to turn off the display
	}

#ifdef FUSION
	if (fcmdModeMgr::Instance != NULL)
	{
		fcmdModeMgr::Instance->Think();
	}
	fcuiFormMgr::Think();
#endif

	//	Think the running mode
	//
	if ( modeModeMgr::IsEmpty() )
	{
		// stop any render threads
		gpxRenderControl::ConfirmSingleThread();

		this->Exit();

		// Don't want to execute any code down below because we are exiting
		return;
	}
	else
	{
		// Multi-threaded sets to the timeline from the user interface
		// will set the time through a "deferred" function. Update that here
		// if the render thread has finished.
		if (!g3dThreadControl::IsRenderThreadActive())
		{
			l_bDidUpdateDeferredTime = tmlnTimeLine::UpdateDeferredTime();
		}

		modeModeMgr::Think();
	}

	if (mnmApp::IsRenderEnabled())
	{
		// Our panel viewers need to do some thinking to setup the 
		// overlay planes for their rendering. Do this before the proxy manager
		// update, and not in Render() anymore in order to better multithread it.
		float timeline_time = tmlnTimeLine::GetTimeInSeconds();
		for (int i=0; i<m_PanelViewers.size(); i++)
			m_PanelViewers[i]->Think(timeline_time);

		// Render views
		this->Render();
	}

	//
	//	security check
	//
	//	First, wait a certain amount of time and check for the security.
	//		if the security is invalid, set another timer to delay the
	//		reporting of the invalid security.
	//
	if ((l_SecurityLastCheck + l_SecurityLastCheckDelay) < appTime::GetTime())
	{
		int func_num = maFunctions::IntRand(1,5);
		//func_num = l_SecurityInvalidCounter+1;		// for debugging only
		switch (func_num)
		{
			case 1:
				l_bSecurityValid = mnmSecurityMgr::CheckSecurity1();
				break;
			case 2:
				l_bSecurityValid = mnmSecurityMgr::CheckSecurity2();
				break;
			case 3:
				l_bSecurityValid = mnmSecurityMgr::CheckSecurity3();
				break;
			case 4:
				l_bSecurityValid = mnmSecurityMgr::CheckSecurity4();
				break;
			case 5:
			default:
				l_bSecurityValid = mnmSecurityMgr::CheckSecurity5();
				break;
		}

		if (l_bSecurityValid)
		{
			//	security is valid, reset variables to normal values
			l_SecurityLastCheck			= appTime::GetTime();
			l_SecurityLastCheckDelay	= lc_SECURITY_CHECK_DELAY_NORMAL;
			l_SecurityValidTime			= lc_SECURITY_VALID_TIME_RESET;		// valid security
			l_SecurityValidTimeDelay	= lc_SECURITY_VALID_DELAY_NORMAL;
		}
		else
		{
			l_SecurityInvalidCounter++;		// increment this value.  if an invalid security happens too many times, exit

			//	reset the checking time to something short so we check for the security again quickly
			l_SecurityLastCheck			= appTime::GetTime();
			l_SecurityLastCheckDelay	= lc_SECURITY_CHECK_DELAY_INVALID;

			//	set the time for reporting the missing security
			l_SecurityValidTime			= appTime::GetTime();
		}
	}
	//	security check 2
	//
	//	If this if statement is true then the security check failed,
	//	report it.
	if ((l_SecurityValidTime > 0) && (l_SecurityValidTime + l_SecurityValidTimeDelay < appTime::GetTime()))
	{
		l_SecurityValidTime = appTime::GetTime();	//	reset the time

		mnmSecurityMgr::AnnounceNoSecurity();

		l_SecurityValidTimeDelay = lc_SECURITY_VALID_DELAY_INVALID;

		if (l_SecurityInvalidCounter >= lc_SECURITY_INVALID_COUNTER_MAX)
			this->Exit();
	}
}

//--------------------------------------------------------------------
//	Run() should be called to start the application.  When control
//	returns from Run(), the application is finished.
//--------------------------------------------------------------------
void mnmApp::Run()
{
	//	write custom run code here

	appApplication::Run();
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
void mnmApp::Render()
{
	// In capture mode, the render of the main windows is disabled and
	// the rendering is controlled by the mode itself.
	if (mnmApp::IsRenderEnabled())
	{
		// If there is already a render thread active, have to 
		// wait until it finishes
		if (!g3dThreadControl::IsRenderThreadActive())
		{
			// The deferred time set is checked earlier in the Think() function,
			// and that is preferred because it gives the mode a chance to react to the
			// time change. So, if we have a deferred time change, don't render just yet.
			if (l_bDidUpdateDeferredTime || !tmlnTimeLine::HasDeferredTime())
			{
				// Clear flag when rendering
				l_bDidUpdateDeferredTime = false;

				// Push through hierarchical transformations into proxies
				xfrmTransformMgr::UpdateTransforms();

				// Push through all of the buffered changes in the graphical proxies
				bool bHadProxyUpdates = gpxProxyMgr::Update();

				// Special case that requires a new render, check to see if
				// the surface or material highlight is turned on. If it is
				// then we need to keep rendering because there is a color animation
				// in the highlighting.
				bool bHighlightIsOn = (mtrlHighlight::AreMaterialsHighlighted() ||
									   fgmtHighlight::AreSurfacesHighlighted());
			
				// Note: debug display forces re-renders in order to measure 
				// the frame rate correctly.
				m_RenderViews[0]->GetWindow()->EnableDebugOverlay(l_bDebugDisplay);

				// Were there any changes made by the proxies? If not,
				// then we don´t have to update the render views.
				if ( bHadProxyUpdates || gpxRenderControl::IsRenderNeeded() 
					|| bHighlightIsOn ||  l_bDebugDisplay)
				{
					// Clear dirty bit now
					gpxRenderControl::RenderStarted();

					// Whether to render in a thread depends on compiler define
					// and boolean flag from the preferences
					if (c_USE_RENDER_THREAD && mnmApp::sm_bThreadingEnabled)
					{
						guiStatusBarMgr::SetText( mnmConstants::e_SBPanel_RenderThread, "*" );
						guiStatusBarMgr::SetToolTip( mnmConstants::e_SBPanel_RenderThread, "Render thread active");

						envThread render_thread(DoRenderThread);
						g3dThreadControl::WaitForRenderThreadStart();
						render_thread.detach();//?
					}
					else
					{
						// Call function directly when not multithreaded
						DoRenderThread();
					}
				}
				else
				{
					guiStatusBarMgr::SetText( mnmConstants::e_SBPanel_RenderThread, "-" );
					guiStatusBarMgr::SetToolTip( mnmConstants::e_SBPanel_RenderThread, "Render thread not active");
				}
			}
		}
	}
}

//--------------------------------------------------------------------
// Part of render that runs in separate thread
//--------------------------------------------------------------------
void mnmApp::DoRenderThread()
{
	// Exception-safe way to track start and end of render thread
	g3dThreadControl::RenderThreadWrapper render_state;

	mnmApp::DoRender();
}

//--------------------------------------------------------------------
// Actual rendering function, may or may not be in a thread.
//--------------------------------------------------------------------
void mnmApp::DoRender()
{
	//	keep start time of render
	float render_time = appTime::GetTime();

	// Scene's Think() was pulled out of the mode´s Think in order to 
	// refactor it into the render thread.
	float timeline_time = tmlnTimeLine::GetTimeInSeconds();
	//DBG_ASSERT(timeline_time == appSimTime::GetTime(), "Timeline and simulation time don´t match");
	//DBG_LOG("Timeline: " << timeline_time << " Sim time: " << appSimTime::GetTime());
	api3dScene::Think( timeline_time );

	api3dTargetRendererMgr::RenderTargets( timeline_time );
	// All render panels are in the tma3dViewerMgr now
	tma3dViewerMgr::RenderViews( timeline_time );

	//	mark actual render time (this ignores UI, file I/O, and other things to slow down the times)
	mnmApp::sm_FPSAverage.Push( (appTime::GetTime() - render_time) );
	tma3dViewerMgr::SetFrameRateRunningAverage( mnmApp::sm_FPSAverage.GetAverage() );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void mnmApp::SetRenderer(int i_Renderer)
{
	//l_pAppInstance->m_pViewer->SetRenderer(l_pAppInstance->m_pRenderer[i_Renderer]);

	if (!l_pAppInstance->m_RenderViews.empty())
	{
		// enum type conversion
		tma3dRenderView::RendererType rType;
		switch(i_Renderer)
		{
		case g3dSceneRendererTypes::e_Default:
			rType = tma3dRenderView::e_Default;
			break;
		case g3dSceneRendererTypes::e_HDR:
			rType = tma3dRenderView::e_HDR;
			break;
		case g3dSceneRendererTypes::e_AmbientOcclusion:
			rType = tma3dRenderView::e_AmbientOcclusion;
			break;
		case g3dSceneRendererTypes::e_Depth:
			rType = tma3dRenderView::e_Depth;
			break;
		case g3dSceneRendererTypes::e_ShadowMask:
			rType = tma3dRenderView::e_ShadowMask;
			break;
		case g3dSceneRendererTypes::e_Normals:
			rType = tma3dRenderView::e_Normals;
			break;
		case g3dSceneRendererTypes::e_VelocityMap:
			rType = tma3dRenderView::e_VelocityMap;
			break;
		case g3dSceneRendererTypes::e_Materials:
			rType = tma3dRenderView::e_Materials;
			break;
		case g3dSceneRendererTypes::e_IlluminationOnly:
			rType = tma3dRenderView::e_IlluminationOnly;
			break;
		case g3dSceneRendererTypes::e_ReflectionOnly:
			rType = tma3dRenderView::e_ReflectionOnly;
			break;
		case g3dSceneRendererTypes::e_GlobalIllumination:
			rType = tma3dRenderView::e_GlobalIllumination;
			break;
		case g3dSceneRendererTypes::e_Glow:
			rType = tma3dRenderView::e_Glow;
			break;
		default:
			rType = tma3dRenderView::e_Default;
			break;
		};
		// Set renderer for main render pane only
		l_pAppInstance->m_RenderViews[0]->SetRenderer( rType );
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void mnmApp::SetRenderPass(int i_RenderPassType, std::string& i_RenderPassName)
{
	//l_pAppInstance->m_pViewer->SetRenderer(l_pAppInstance->m_pRenderer[i_Renderer]);

	if (!l_pAppInstance->m_RenderViews.empty())
	{
		// copy current prefs.
//		g3dPrefs::g3dRenderPrefs prefs = g3dPrefs::CurrentPrefs();
		// set up prefs for renderpass
//		rlyrPassesObject::AdjustPrefs((rlyrPassesObject::ePassType)i_RenderPassType, prefs);
		
		// enum type conversion
		tma3dRenderView::RendererType rType;
		switch (i_RenderPassType)
		{
		case rlyrPassesObject::e_Beauty:
		case rlyrPassesObject::e_Diffuse:
		case rlyrPassesObject::e_Specular:
		case rlyrPassesObject::e_Bloom:
		case rlyrPassesObject::e_Star:
		case rlyrPassesObject::e_CameraDOF:
		case rlyrPassesObject::e_Preview:
		case rlyrPassesObject::e_DiffEnv:
		case rlyrPassesObject::e_DiffLit:
		case rlyrPassesObject::e_SpecEnv:
		case rlyrPassesObject::e_SpecLit:
		case rlyrPassesObject::e_Emissive:
		case rlyrPassesObject::e_DirtyMatte:
		case rlyrPassesObject::e_Wireframe:
			rType = tma3dRenderView::e_HDR;
			break;
		case rlyrPassesObject::e_AOOnly:
			rType = tma3dRenderView::e_AmbientOcclusion;
			break;
		case rlyrPassesObject::e_Depth:
			rType = tma3dRenderView::e_Depth;
			break;
		case rlyrPassesObject::e_ShadowMask:
			rType = tma3dRenderView::e_ShadowMask;
			break;
		case rlyrPassesObject::e_IlluminationOnly:
			rType = tma3dRenderView::e_IlluminationOnly;
			break;
		case rlyrPassesObject::e_Normals:
			rType = tma3dRenderView::e_Normals;
			break;
		case rlyrPassesObject::e_Materials:
			rType = tma3dRenderView::e_Materials;
			break;
		case rlyrPassesObject::e_ReflectionsOnly:
			rType = tma3dRenderView::e_ReflectionOnly;
			break;
		case rlyrPassesObject::e_Velocity:
			rType = tma3dRenderView::e_VelocityMap;
			break;
		case rlyrPassesObject::e_GlobalIllumination:
			rType = tma3dRenderView::e_GlobalIllumination;
			break;
		case rlyrPassesObject::e_Glow:
			rType = tma3dRenderView::e_Glow;
			break;
		default:
			rType = tma3dRenderView::e_HDR;
			break;
		}

		// Set renderer for main render pane only
		l_pAppInstance->m_RenderViews[0]->SetRenderer( rType );//, &prefs );

		// set the pass string.
		//
#ifdef USE_WXWIDGETS
		rpnRenderPanel *pRenderPane = rpnPanelGrid::Instance->GetRenderPanel(0);
		if (pRenderPane != NULL && pRenderPane->IsActiveView())
		{
			pRenderPane->SetPass(i_RenderPassName);
		}
#endif
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void mnmApp::ReceiveStartEvent(appStartEvent& i_Event)
{
//	g2dScreen::InitializeWindow(640, 480, 0, 0);
//	g3dScene::Initialize();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void mnmApp::ReceiveStopEvent(appStopEvent& i_Event)
{
	//g3dScene::DeInitialize();
	//g2dFontUtil::ReleaseAllFonts();
	//g2dScreen::DeInitialize();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void mnmApp::ReceiveSuspendEvent(appSuspendEvent& i_Event)
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void mnmApp::ReceiveResumeEvent(appResumeEvent& i_Event)
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void mnmApp::ReceiveQuitRequestEvent(appQuitRequestEvent& i_Event)
{
	this->Exit();
}

//--------------------------------------------------------------------
//	Exit() should be called by the client when it decides it is
//	ready to end the program.
//--------------------------------------------------------------------
void mnmApp::Exit()
{
	this->SetActive(false);
	this->EnableRender(false);

#ifdef USE_WXWIDGETS
	wxMainForm::Exit();
#else
	appApplication::Exit();
#endif
}
