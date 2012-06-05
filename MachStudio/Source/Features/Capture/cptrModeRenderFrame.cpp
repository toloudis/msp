/*****************************************************************************
**	cptrModeRenderFrame.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Features/Capture/cptrModeRenderFrame.hpp"

#include "Drivers/Attach/tmlnDriverAttachUtil.hpp"
#include "Features/Capture/cptrModeRender.hpp"
#include "Features/Capture/cptrPostRenderUtil.hpp"
#include "Features/Capture/cptrRenderOptionsDialogUtil.hpp"
#include "Features/Capture/cptrRenderProgressDialogUtil.hpp"
#include "Features/Capture/cptrRenderUtil.hpp"
#include "Features/Capture/wxGUI/cptrToPhotoshopdialog.hpp"
#include "Features/Channels/chnlDialogUtil.hpp"
#include "Support/cams/camsCameraMgr.hpp"
#include "Support/cams/camsFollowUtil.hpp"
#include "Support/capt/captRenderOutputDataUtil.hpp"
#include "Support/mnm/mnmAppUtil.hpp"
#include "Support/mnm/mnmConstants.hpp"
#include "Support/mnm/mnmPaths.hpp"
#include "Support/mnm/mnmSecurityMgr.hpp"
#include "Support/mode/modeModeMgr.hpp"
#include "Support/tmln/tmlnTimeLine.hpp"
#include "Support/vis/visMgr.hpp"

//	library
#include "Core/env/envSTLHelpers.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/it/itStringUtil.hpp"
#include "Tool/cam3d/cam3dMgr.hpp"
#include "Tool/doc/docSingleDocumentMgr.hpp"
#include "Tool/gui/guiMessageBox.hpp"
#include "Tool/gui/guiSingleDocHandler.hpp"
#include "Tool/gui/guiStatusBarMgr.hpp"
#ifndef TWX_WIDGETS_HPP
#include "ToolUIWx/twx/twxWidgets.hpp"
#endif

#undef CreateDirectory


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
cptrModeRenderFrame::cptrModeRenderFrame( modeModeID i_ModeIDCapture )
:	m_bInitialized( false ),
	m_bAborted( false ),
	m_bProcessing( false ),
	m_ModeRenderID( i_ModeIDCapture ),
	m_Stage( e_Begin )
{
	SetMenuItemName( "Frame Render" );
	
	//	read in the config file immediately so python commands can override values
	//
	//ResetConfigFileName();
	//captRenderOutputData& data = captRenderOutputDataUtil::Data();
	//captRenderOutputDataUtil::ReadData(data);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
// virtual
cptrModeRenderFrame::~cptrModeRenderFrame()
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
void cptrModeRenderFrame::Initialize()
{
	if ( !m_bInitialized )
	{
		// get current mode id in order to return to it when finished
	}

	//	fill in more capture data
	captRenderOutputData& capData = captRenderOutputDataUtil::Data();
	m_SavedData = capData; // save the current state

	//	Set-up the cameras
	//
	camCamera &camera = cam3dMgr::GetCamera();
	int cam_index = camsCameraMgr::GetIndexForCamera(&camera);
	if (   (cam_index < 0)
		|| (cam_index >= camsCameraMgr::GetNumCameras()))
	{
		guiMessageBox::Show("Must use a camera that isn't the Edit Camera", "Capture Error", guiMessageBox::e_OKOnly);
		m_bAborted = true;
		if (cptrRenderUtil::GetPSflag())
			cptrRenderUtil::SetPSflag(false);

		//DBG_LOG1("CaptureFrame Restore Time (%6.3f) SAVE!", m_fRestoreTime);
		//save start and end times to restore them at the end of the capture
		m_PrevStart = capData.m_fStartTime.GetValue();
		m_PrevEnd = capData.m_fEndTime.GetValue();
		m_fRestoreTime = tmlnTimeLine::GetValue();
		return;
	}
	camsFollowUtil::SetFollowIndex( cam_index );

	//	initialize variables
	//
	m_bInitialized = true;
	m_bAborted = false;

	if ( m_bProcessing )
	{
		m_Stage = e_EndProcessing; //e_EndMode;
	}
	else
	{
		m_Stage = e_Begin;

		//DBG_LOG1("CaptureFrame Restore Time (%6.3f) SAVE!", m_fRestoreTime);
		m_fRestoreTime = tmlnTimeLine::GetValue();

		//	reset the config filename so the correct one gets read in
		//
		//ResetConfigFileName();

		captRenderOutputData& data = captRenderOutputDataUtil::Data();
		//captRenderOutputDataUtil::ReadData(data);
	}

	guiStatusBarMgr::SetText( mnmConstants::e_SBPanel_Mode, "Frame Render" );
	//visMgr::ShowIcons(false);
	visMgr::ConfirmGeometryVisible();
	tmlnDriverAttachUtil::SetAllowDialogs(false); // should be more general way to do this
}

//----------------------------------------------------------------------------
//	The object should clean up things that are not needed while the
//	mode is not running in the DeInitialize function.
//----------------------------------------------------------------------------
//virtual
void cptrModeRenderFrame::DeInitialize()
{
	//if ( m_bInitialized )
	//{
	//}

	m_bInitialized = false;
	m_bAborted = false;
	m_bProcessing = false;

	//DBG_LOG1("CaptureFrame Restore Time (%6.3f) RESET", m_fRestoreTime);
	tmlnTimeLine::SetValue(m_fRestoreTime);
}


//----------------------------------------------------------------------------
//	The mode should do it's per frame "work" in the Think function.
//----------------------------------------------------------------------------
// virtual
void cptrModeRenderFrame::Think()
{
	if ( m_bAborted )
	{
		m_Stage = e_EndProcessing;
		m_bAborted = false;
	}

	modeMode::Think();

	switch ( m_Stage )
	{
		case e_Begin:
		{
			m_Stage = e_Processing;

			//	if there is no filename, then make them save it.
			//
			if (docSingleDocumentMgr::GetFilename().GetNumNames() == 0)
			{
				//	only perform if the security is present
				if (!mnmSecurityMgr::CheckSecurity3())
					return;
				guiSingleDocHandler::Save();
				mnmAppUtil::UpdateTitleBar( false );
			}

			//	if they cancelled the save, don't let them continue.
			//
			if (docSingleDocumentMgr::GetFilename().GetNumNames() == 0)
			{
				this->m_bAborted = true;
				m_Stage = e_EndMode;
				return;
			}

			//	set the current scene and camera names
			//
			if ( docSingleDocumentMgr::GetFilename().GetNumNames() > 0 )
			{
				itString itPrefix = docSingleDocumentMgr::GetFilename().GetLastName();
				itPrefix.StripExtension();
				captRenderOutputDataUtil::SetCurrentScene(itPrefix);
			}

			nameString theName;
			camsFollowUtil::GetCurrentCameraName( theName );
			captRenderOutputDataUtil::SetCurrentCamera(theName);

			//	fill in more capture data
			captRenderOutputData& capData = captRenderOutputDataUtil::Data();

			//m_SavedData = capData;	// save the current state

			//save start and end times to restore them at the end of the capture
			m_PrevStart = capData.m_fStartTime.GetValue();
			m_PrevEnd = capData.m_fEndTime.GetValue();
			
			capData.m_bUseSceneFilenameInFilename = true;
			capData.m_NumberOfScenes = 1;
			capData.m_CurrentSceneNumber = 0;
			capData.m_fStartTime = tmlnTimeLine::GetValue();
			capData.m_fEndTime = tmlnTimeLine::GetValue(); // + (1.0f / tmlnTimeLine::GetFPS());
			capData.m_RenderFrameRange.SetValue( captRenderOutputData::eSpecifyRange );
			capData.m_bCaptureAllCameras = false;
			//capData.m_bUseMarkerTimes = false;
			//capData.m_bUseCaptureDrivers = true;
			capData.m_bOutputTitleCard = false;
			capData.m_bKeepFrameOpenAfterRender = true;
			capData.m_bShowRenderProgressDialog = false;

			if (cptrRenderUtil::GetPSflag())
			{
				fsLocator rootpath = gfPaths::GetPath(mnmPaths::e_PhotoshopExport);
				capData.m_CaptureFormat.SetValue("PNG");
				if ( !fsFileUtil::DirectoryExists( rootpath ) )
					fsFileUtil::CreateDirectory( rootpath );
				bool result = fsFileUtil::DeleteDirectoryRecursively(rootpath);
				capData.m_OutputDirectoryRoot.SetValue(gfPaths::GetPath(mnmPaths::e_PhotoshopExport));
				capData.m_OutputDirectory.SetValue(gfPaths::GetPath(mnmPaths::e_PhotoshopExport));
				capData.m_bUseCameraNameAsDirectory = false;
				capData.m_bUseLayerNameAsDirectory = false;
				capData.m_bUseRenderPassAsDirectory = false;
				capData.m_bUseResolutionAsDirectory = false;
				capData.m_bUseSceneFilenameAsDirectory = false;
				//capData.m_bUseCaptureDrivers = false;
			}
			
			//capData.m_Resolution.SetValue("640x480");
			

			capData.m_RenderPosTime = captRenderOutputData::c_InitRenderPosTime;
			nameString name;
			camsCameraMgr::GetCameraName( camsFollowUtil::GetFollowIndex(), name );

			capData.m_CameraList.resize( camsCameraMgr::GetNumCameras()	);
			for (int j=0; j < capData.m_CameraList.size(); ++j)
			{
				capData.m_CameraList[j].m_bCapture = false;
			}
			capData.m_CameraList[camsFollowUtil::GetFollowIndex()].m_CameraName = name.GetString();
			capData.m_CameraList[camsFollowUtil::GetFollowIndex()].m_bCapture = true;

			m_bProcessing = true;
			break;
		}
		case e_Processing:
		{
			//captRenderOutputData& theCapData	= captRenderOutputDataUtil::Data();

			//captRenderOutputDataUtil::Debug();

			//	set the prefix to be the scene name
			//itString current_scenename;
			//current_scenename = theData.m_Scenes[m_SceneIndex].m_Filename.GetLastName();
			//current_scenename.StripExtension();
			//captRenderOutputDataUtil::SetCurrentScene( current_scenename );

			//	set the max capture frames
			//captRenderOutputDataUtil::SetMaxTime( 0 );
			//chnlDialogUtil::UpdateMarkersData();
			//theCapData.m_fMarkerInTime	= chnlMarkerMgr::GetMarkerInTime();
			//theCapData.m_fMarkerOutTime	= chnlMarkerMgr::GetMarkerOutTime();

			// process
			cptrModeRender* pModeRender = dynamic_cast<cptrModeRender*>(modeModeMgr::GetMode( m_ModeRenderID ));
			DBG_ASSERT( pModeRender != 0, "cannot find the capture mode or it's index has changed" );

			pModeRender->SetSkipCaptureOptions( false );
			pModeRender->SetUseOnlyValidCaptureTimes( false );
			modeModeMgr::Push( this->m_ModeRenderID );

			m_Stage = e_EndProcessing;
			break;
		}
		case e_EndProcessing:
		{
			//	execute the post capture stuff
			//
			cptrPostRenderUtil::Execute();

			m_Stage = e_EndMode;
			break;
		}
		case e_EndMode:
		{
			captRenderOutputData& capData = captRenderOutputDataUtil::Data();
			capData = m_SavedData;

			//this->SetTerminateCondition(appMode::e_TerminateAndRemove);
			modeModeMgr::Pop();

			// If there is balance between Push and Pop, then there would be
			// no need to Push ObjectManip back on.
			//modeModeMgr::Push( this->m_ModeReturnID );
			cptrModeRender* pModeRender = dynamic_cast<cptrModeRender*>(modeModeMgr::GetMode( m_ModeRenderID ));
			DBG_ASSERT( pModeRender != 0, "cannot find the capture mode or it's index has changed" );
			pModeRender->SetUseOnlyValidCaptureTimes( true );
			
			capData.m_fStartTime.SetValue(m_PrevStart);
			capData.m_fEndTime.SetValue(m_PrevEnd);
			m_bProcessing = false;
			if (cptrRenderUtil::GetPSflag())
			{
#ifdef USE_WXWIDGETS
				fsLocator scriptPath = gfPaths::GetPath(gfPaths::e_ExePath);
				DBG_LOG("Script Path" << scriptPath);
				scriptPath.Push("Data");
				scriptPath.Push("PhotoshopExport.vbs");
				std::string s_scriptPath;
				itString uniStr, fsStr;
				fsFileUtil::LocatorToUnicodeString(scriptPath, fsStr);
				uniStr = L"cscript.exe \"";
				uniStr += fsStr;
				uniStr += L"\"";
				DBG_LOG( "Unicode string" << uniStr );
				DBG_LOG( "Std string" << itStringUtil::GetStdString(uniStr) );
				wxString string (itStringUtil::GetStdString(uniStr).c_str(), wxConvUTF8);
				int temp = ::wxExecute(string);
#endif
				cptrRenderUtil::SetPSflag(false);
			}
			break;
		}
	}
}

//----------------------------------------------------------------------------
//	IsModal() - signals whether the mode should be pushed onto the mode stack
//	ON TOP of the current mode, or replace the current mode (via Pop)
//
//	true - the mode will be pushed on top of the current mode.
//	false- the mode will replace the current mode.
//----------------------------------------------------------------------------
//virtual
bool cptrModeRenderFrame::IsModal()
{
	return true;
}

//--------------------------------------------------------------------
//	Abort all the renders
//--------------------------------------------------------------------
void cptrModeRenderFrame::AbortRenders()
{
	m_bAborted = true;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void cptrModeRenderFrame::ResetConfigFileName()
{
	fsLocator cfgdir = gfPaths::GetPath(mnmPaths::e_Configs);
	captRenderOutputData& data = captRenderOutputDataUtil::Data();

	cfgdir.Push( "RenderFrameOutput.cfg" );
	data.m_RenderOutputDataFile.SetValue( cfgdir );
	cfgdir.Clear();
	cfgdir.Push( gfPaths::GetPath(mnmPaths::e_DefaultConfigs) );
	cfgdir.Push( "RenderFrameOutput-Default.cfg" );
	data.m_RenderOutputDataDefaultFile.SetValue( cfgdir );
}
