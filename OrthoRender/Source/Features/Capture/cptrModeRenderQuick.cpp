/*****************************************************************************
**  cptrModeRenderQuick.cpp
**
**      see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include <windows.h>

#include "Features/Capture/cptrModeRenderQuick.hpp"

//#include "Features/Capture/cptrRenderOutputData.hpp"
#include "Features/Capture/cptrRenderOutputDataUtil.hpp"

#include "Core/dbg/dbgLog.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Support/mnm/mnmPaths.hpp"
#include "Support/mode/modeModeMgr.hpp"
#include "Features/Prefs/PrefsMgr.hpp"
#include "Features/RenderPrefs/rndrPrefsMgr.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
cptrModeRenderQuick::cptrModeRenderQuick(modeModeID i_ModeIDCapture)
:	cptrModeRender(),
	m_ModeRenderID( i_ModeIDCapture )
{
	m_RenderPrefsObjectID = rndrPrefsMgr::e_RenderQuickPrefs;

	SetMenuItemName( "Render Quick" );

	//	read in the config file immediately so python commands can override values
	//
	ResetConfigFileName();
	cptrRenderOutputData& data = cptrRenderOutputDataUtil::Data();
	cptrRenderOutputDataUtil::ReadData(data);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
// virtual
cptrModeRenderQuick::~cptrModeRenderQuick()
{
}

//----------------------------------------------------------------------------
//	Initialize will be called before the first call of Think after
//	the object is first created or DeInitialized.  During the
//	lifetime of a mode, Initialize and DeInitialize may be called
//	several times.  Children of appMode should remember to call
//	appMode::Initialize() at the beginning of their Initialize
//	function.
//----------------------------------------------------------------------------
//virtual
void cptrModeRenderQuick::Initialize()
{
	//	reset the config filename so the correct one gets read in
	//
	ResetConfigFileName();

	cptrModeRender::Initialize();

	//	more initializing after modeRender is finished
	//	to set parameters to this modes needs.
	//
	cptrRenderOutputData& data = cptrRenderOutputDataUtil::Data();

	//	only set the current camera
	//
	//data.m_bShowRenderProgressDialog = false;
	data.m_bCaptureAllCameras = false;
	for (int i=0; i < data.m_CameraList.size(); ++i)
	{
		if ( data.m_CameraList[i].m_CameraName.GetValue() == data.m_CurrentCamera.GetString() )
		{
			data.m_CameraList[i].m_bCapture = true;
		}
		else
		{
			data.m_CameraList[i].m_bCapture = false;
		}
	}

	//	set variables
	data.m_bOutputTitleCard = false;
}

//----------------------------------------------------------------------------
//	The object should clean up things that are not needed while the
//	mode is not running in the DeInitialize function.
//----------------------------------------------------------------------------
//virtual
void cptrModeRenderQuick::DeInitialize()
{
	cptrModeRender::DeInitialize();
}

//--------------------------------------------------------------------
//	PostCapture
//--------------------------------------------------------------------
void cptrModeRenderQuick::BeginStatePostCapture()
{
	cptrModeRender::BeginStateWaitToEndMode();

	cptrRenderOutputData& data = cptrRenderOutputDataUtil::Data();
	if (data.m_bCaptureMovie.GetValue())
	{
		//	build the command
		char cmd[2048];
		std::string movie_launch;
		fsFileUtil::LocatorToANSIFilename( PrefsMgr::Data().m_MoviePlayerLocation.GetValue(), movie_launch );

		std::string movie_directory;
		fsFileUtil::LocatorToANSIFilename( data.m_OutputMovieDirectory.GetValue(), movie_directory );
		std::string movie_filename;
		movie_filename = data.m_OutputFileName.GetString();

		sprintf(cmd, "\"%s\" \"%s\\%s\" ", movie_launch.c_str(), movie_directory.c_str(), movie_filename.c_str() );
		DBG_LOG1("playing movie (%s)", cmd);

		//	launch the process
		STARTUPINFO si = { sizeof(STARTUPINFO) };
		si.dwFlags = STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_SHOWNORMAL; //SW_HIDE;
		PROCESS_INFORMATION pi;

		if(CreateProcess(0, cmd, 0, 0, FALSE, 0, 0, 0, &si, &pi))
		{
			// optionally wait for process to finish
			WaitForSingleObject(pi.hProcess, INFINITE);

			// check for errors
			DWORD exit_code;
			GetExitCodeProcess(pi.hProcess, &exit_code);

			// free handles			 		//	Execute!
			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);
		}
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void cptrModeRenderQuick::ResetConfigFileName()
{
	fsLocator cfgdir = gfPaths::GetPath(mnmPaths::e_Configs);
	cfgdir.Push( "RenderQuickOutput.cfg" );

	cptrRenderOutputData& data = cptrRenderOutputDataUtil::Data();
	data.m_RenderOutputDataFile.SetValue( cfgdir );
}
