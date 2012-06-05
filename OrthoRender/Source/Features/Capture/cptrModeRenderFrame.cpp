/*****************************************************************************
**  cptrModeRenderFrame.cpp
**
**      see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Features/Capture/cptrModeRenderFrame.hpp"

//#include "cptrCaptureFrameData.hpp"
//#include "cptrCaptureFrameDataUtil.hpp"
//#include "cptrCaptureFrameDialogUtil.hpp"
#include "Features/Capture/cptrRenderOptionsDialogUtil.hpp"
//#include "Features/Capture/cptrRenderStatsDataUtil.hpp"
#include "Features/Capture/cptrRenderProgressDialogUtil.hpp"
#include "Features/Capture/cptrModeRender.hpp"
#include "Features/Capture/cptrPostRenderUtil.hpp"
#include "Features/Capture/cptrRenderOutputDataUtil.hpp"
#include "Features/Capture/cptrRenderUtil.hpp"

#include "Support/cams/camsCameraMgr.hpp"
#include "Support/cams/camsFollowUtil.hpp"
#include "Features/Channels/chnlDialogUtil.hpp"
#include "Support/mnm/mnmConstants.hpp"
#include "Support/mnm/mnmPaths.hpp"
#include "Support/mode/modeModeMgr.hpp"

//	library
#include "Tool/cam3d/cam3dMgr.hpp"
#include "Tool/doc/docSingleDocumentMgr.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Core/fs/fsFileUtil.hpp"
//#include "Core/it/itStringUtil.hpp"
#include "Tool/gui/guiMessageBox.hpp"
#include "Tool/gui/guiSingleDocHandler.hpp"
#include "Tool/gui/guiStatusBarMgr.hpp"
#include "Support/tmln/tmlnTimeLine.hpp"
#include "Drivers/Attach/tmlnDriverAttachUtil.hpp"
#include "Support/vis/visMgr.hpp"



//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
cptrModeRenderFrame::cptrModeRenderFrame( modeModeID i_ModeIDCapture )
:	m_bInitialized( false ),
	m_bAborted( false ),
	m_bProcessing( false ),
	m_ModeRenderID( i_ModeIDCapture ),
	m_Stage( e_Begin ),
	m_fRestoreTime(0.0f)
{
	SetMenuItemName( "Frame Render" );

	//	read in the config file immediately so python commands can override values
	//
	ResetConfigFileName();
	cptrRenderOutputData& data = cptrRenderOutputDataUtil::Data();
	cptrRenderOutputDataUtil::ReadData(data);
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

	//	Set-up the cameras
	//
	camCamera &camera = cam3dMgr::GetCamera();
	int cam_index = camsCameraMgr::GetIndexForCamera(&camera);
	if (   (cam_index < 0)
		||  (cam_index >= camsCameraMgr::GetNumCameras()))
	{
		guiMessageBox::Show("Must use a camera that isn't the Edit Camera", "Capture Error", guiMessageBox::e_OKOnly);
		m_bAborted = true;

		//DBG_LOG1("CaptureFrame Restore Time (%6.3f) SAVE!", m_fRestoreTime);
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
		ResetConfigFileName();
	}

	guiStatusBarMgr::SetText( mnmConstants::e_SBPanel_Mode, "Frame Render" );
	visMgr::ShowIcons(false);
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

			//	set the current scene and camera names
			//
			if ( docSingleDocumentMgr::GetFilename().GetNumNames() > 0 )
			{
				itString itPrefix = docSingleDocumentMgr::GetFilename().GetLastName();
				itPrefix.StripExtension();
				cptrRenderOutputDataUtil::SetCurrentScene(itPrefix);
			}

			nameString theName;
			camsFollowUtil::GetCurrentCameraName( theName );
			cptrRenderOutputDataUtil::SetCurrentCamera(theName);

			//	fill in more capture data
			cptrRenderOutputData& capData = cptrRenderOutputDataUtil::Data();
			capData.m_bUseSceneFilenameInFilename = true;
			capData.m_NumberOfScenes = 1;
			capData.m_CurrentSceneNumber = 0;
			capData.m_fStartTime = tmlnTimeLine::GetValue();
			capData.m_fEndTime = tmlnTimeLine::GetValue(); // + (1.0f / capData.m_fCaptureFPS);
			capData.m_bCaptureAllCameras = false;
			capData.m_bUseMarkerTimes = false;
			capData.m_bOutputTitleCard = false;
			capData.m_bKeepFrameOpenAfterRender = true;
			capData.m_bShowRenderProgressDialog = false;

			capData.m_RenderPosTime = -1.0f;
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
			//cptrRenderOutputData& theCapData	= cptrRenderOutputDataUtil::Data();

			//cptrRenderOutputDataUtil::Debug();

			//	set the prefix to be the scene name
			//itString current_scenename;
			//current_scenename = theData.m_Scenes[m_SceneIndex].m_Filename.GetLastName();
			//current_scenename.StripExtension();
			//cptrRenderOutputDataUtil::SetCurrentScene( current_scenename );

			//	set the max capture frames
			//cptrRenderOutputDataUtil::SetMaxTime( 0 );
			//chnlDialogUtil::UpdateMarkersData();
			//theCapData.m_fMarkerInTime	= chnlMarkerMgr::GetMarkerInTime();
			//theCapData.m_fMarkerOutTime	= chnlMarkerMgr::GetMarkerOutTime();

			// process
			cptrModeRender* pModeRender = dynamic_cast<cptrModeRender*>(modeModeMgr::GetMode( m_ModeRenderID ));
			DBG_ASSERT0( pModeRender != 0, "cannot find the capture mode or it's index has changed" );

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

			//cptrRenderOutputData& theCapData = cptrRenderOutputDataUtil::Data();

			m_Stage = e_EndMode;
			break;
		}
		case e_EndMode:
		{
			//this->SetTerminateCondition(appMode::e_TerminateAndRemove);
			modeModeMgr::Pop();

			// If there is balance between Push and Pop, then there would be
			// no need to Push ObjectManip back on.
			//modeModeMgr::Push( this->m_ModeReturnID );
			cptrModeRender* pModeRender = dynamic_cast<cptrModeRender*>(modeModeMgr::GetMode( m_ModeRenderID ));
			DBG_ASSERT0( pModeRender != 0, "cannot find the capture mode or it's index has changed" );
			pModeRender->SetUseOnlyValidCaptureTimes( true );

			m_bProcessing = false;
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
	cfgdir.Push( "RenderFrameOutput.cfg" );

	cptrRenderOutputData& data = cptrRenderOutputDataUtil::Data();
	data.m_RenderOutputDataFile.SetValue( cfgdir );
}

