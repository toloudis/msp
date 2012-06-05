/*****************************************************************************
**  cptrModeRender.cpp
**
**      see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2003-8 - All Rights Reserved
\****************************************************************************/
#include "Features/Capture/cptrModeRender.hpp"

#include "Features/Capture/cptrAccumulationBuffer.hpp"
#include "Features/Capture/cptrMBlurMotionSampler.hpp"
#include "Features/Capture/cptrPostRenderUtil.hpp"
#include "Features/Capture/cptrRenderOptionsDialogUtil.hpp"
#include "Features/Capture/cptrRenderOutputDataUtil.hpp"
#include "Features/Capture/cptrRenderProgressDialogUtil.hpp"
#include "Features/Capture/cptrRenderStatsDataUtil.hpp"
#include "Features/Capture/cptrRenderStatsDialogUtil.hpp"
#include "Features/Capture/cptrRenderUtil.hpp"
#include "Features/Channels/Markers/chnlMarkerMgr.hpp"
#include "Features/FilmGates/fgtFrameMgr.hpp"
#include "Features/RenderPrefs/rndrPrefsMgr.hpp"
#include "MainApp/mnmApp.hpp"
#include "Support/cams/camsCameraMgr.hpp"
#include "Support/cams/camsFollowUtil.hpp"
#include "Support/cmps/cmpsCompassMgr.hpp"
#include "Support/mnm/mnmConstants.hpp"
#include "Support/mnm/mnmDebugInfo.hpp"
#include "Support/mnm/mnmPaths.hpp"
#include "Support/mnm/mnmTimeCodeMgr.hpp"
#include "Support/mnm/mnmTimeCodeUtil.hpp"
#include "Support/mtrl/mtrlScriptObject.hpp"
#include "Support/tmln/tmlnTimeInOutMgr.hpp"
#include "Support/tmln/tmlnTimeLine.hpp"
#include "Support/tmln/tmlnTimeUtil.hpp"
#include "Support/vis/visMgr.hpp"

#include <windows.h>
#undef CreateDirectory

//	library
#include "AudioDS/sn/snSoundManager.hpp"
#include "Core/app/appSimTime.hpp"
#include "Core/app/appTime.hpp"
#include "Core/dbg/dbgLog.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/it/itStringUtil.hpp"
#include "Graphics/g2d/g2dExceptionX.hpp"
#include "Graphics/g2d/g2dFontUtil.hpp"
#include "Graphics/g2d/g2dSystem.hpp"
#include "Graphics/g2d/g2dWindow.hpp"
#include "Graphics/g3d/g3dConstants.hpp"
#include "Graphics/g3d/g3dSceneRenderer.hpp"
#include "Graphics/g3d/g3dViewer.hpp"
#include "InputDI/in/inDeviceMgr.hpp"
#include "InputDI/in/inVirtualJoystick.hpp"
#include "Tool/api3d/api3dScene.hpp"
#include "Tool/api3d/api3dSubdiv.hpp"
#include "Tool/api3d/api3dTargetRendererMgr.hpp"
#include "Tool/cam3d/cam3dMgr.hpp"
#include "Tool/doc/docSingleDocumentMgr.hpp"
#include "Tool/gui/guiMessageBox.hpp"
#include "Tool/gui/guiStatusBarMgr.hpp"
#include "Tool/tma3d/tma3dScreenUtil.hpp"

#include <assert.h>
#include <string>


// TODO remove the systems from cptr.  (chtr, cmra, prtcl,...)


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool cptrModeRender::sm_bUseAppWindow = false;

//------------------------------------------------------------------------
//	SetUseApplicationWindow() - if true, capture mode should use
//	the application window to render into. Otherwise, it should
//	create a subwindow of the correct size to render into.
//------------------------------------------------------------------------
//static 
void cptrModeRender::SetUseApplicationWindow(bool i_bUseAppWindow)
{
	sm_bUseAppWindow = i_bUseAppWindow;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
cptrModeRender::cptrModeRender()
:	m_bInitialized( false ),
	m_bCaptureStarted( false ),
	m_bCaptureFinished( false ),
	m_bSwitchToCameraRenderRange( true ),
	m_bAborted( false ),
	m_bFirstRenderFrame( true ),
	m_bSkipCaptureOptions( false ),
	m_fSimTimeInc( 0.0f ),
	m_fLeadTimeDelta( 0.0f ),
	m_fEndTime(-1.0f),
	m_CurrentCameraIndex( 0 ),
	m_LastCameraIndex( 0 ),
	m_pSystem( NULL ),
	m_pViewer( NULL ),
	m_pWindow( NULL ),
	m_pRenderer( NULL ),
	m_MotionSampler(NULL),
	m_bAudioMuteState(false),
	m_bPaused(false),
	m_CurrentCameraNumber(0),
	m_AccBuf(NULL),
	m_bUseOnlyValidCaptureTimes(true),
	m_fRenderStartTime(0.0f),
	m_RenderPrefsObjectID(rndrPrefsMgr::e_RenderFullPrefs)
{
	SetMenuItemName( "Render" );

	//	set-up the state call-backs
	m_StateRenderConfig.SetState( this, &cptrModeRender::BeginStateRenderConfig, &cptrModeRender::OnStateRenderConfig, &cptrModeRender::EndStateRenderConfig );
	m_StateInitCaptures.SetState( this, &cptrModeRender::BeginStateInitCaptures, &cptrModeRender::OnStateInitCaptures, &cptrModeRender::EndStateInitCaptures );
	m_StateBeginCapture.SetState( this, &cptrModeRender::BeginStateBeginCapture, &cptrModeRender::OnStateBeginCapture, &cptrModeRender::EndStateBeginCapture );
	m_StateLeadIn.SetState( this, &cptrModeRender::BeginStateLeadIn, &cptrModeRender::OnStateLeadIn, &cptrModeRender::EndStateLeadIn );
	m_StateCapture.SetState( this, &cptrModeRender::BeginStateCapture, &cptrModeRender::OnStateCapture, &cptrModeRender::EndStateCapture );
	m_StateLeadOut.SetState( this, &cptrModeRender::BeginStateLeadOut, &cptrModeRender::OnStateLeadOut, &cptrModeRender::EndStateLeadOut );
	m_StateEndCapture.SetState( this, &cptrModeRender::BeginStateEndCapture, &cptrModeRender::OnStateEndCapture, &cptrModeRender::EndStateEndCapture );
	m_StateDeInitCaptures.SetState( this, &cptrModeRender::BeginStateDeInitCaptures, &cptrModeRender::OnStateDeInitCaptures, &cptrModeRender::EndStateDeInitCaptures );
	m_StatePostCapture.SetState( this, &cptrModeRender::BeginStatePostCapture, &cptrModeRender::OnStatePostCapture, &cptrModeRender::EndStatePostCapture );
	m_StateWaitToEndMode.SetState( this, &cptrModeRender::BeginStateWaitToEndMode, &cptrModeRender::OnStateWaitToEndMode, &cptrModeRender::EndStateWaitToEndMode );

	//	hack? put this here so the data for render output gets read in at the start of MS launching
	//	and not on showing the dialog so that python scripting works.
	//
	ResetConfigFileName();
	ReadConfig();

	//
	cptrRenderOutputData& data = cptrRenderOutputDataUtil::Data();
	data.m_bKeepFrameOpenAfterRender = false;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
// virtual
cptrModeRender::~cptrModeRender()
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
void cptrModeRender::Initialize()
{
	cptrRenderOutputData& data = cptrRenderOutputDataUtil::Data();

	if ( !m_bInitialized )
	{
		m_bInitialized = true;
		m_OldSubdivLevel = -1;

		//	set the mute state
		m_bAudioMuteState = snSoundManager::IsMuted();
		snSoundManager::Mute( true );
	}

	this->SetTerminateCondition( appMode::e_Continue );

	//	abort if no document is loaded
	//
	if (docSingleDocumentMgr::GetFilename().GetNumNames() == 0)
	{
		this->m_bAborted = true;
		GotoState( m_StateWaitToEndMode );
		return;
	}

	//	abort if no cameras defined
	//
	if (camsCameraMgr::GetNumCameras() == 0)
	{
		if (data.m_bBatchMode.GetValue())
		{
			//	batch mode -- abort current scene and continue
			DBG_ERROR1("Cannot render the scene %s.  There are no cameras created.", data.m_CurrentSceneFilename.GetString().c_str());
		}
		else
		{
			guiMessageBox::Show("Cannot render this scene yet.  There are no cameras created.", "Error", guiMessageBox::e_OKOnly);
		}

		this->m_bAborted = true;
		GotoState( m_StateWaitToEndMode );
		return;
	}

	//
	guiStatusBarMgr::SetText( mnmConstants::e_SBPanel_Mode, "Render" );

	cptrRenderUtil::SetCapture( false );

	//DBG_LOG2( "Init MODE CAPTURE (%6.2f) max(%6.2f)", tmlnTimeLine::GetValue(), tmlnTimeLine::GetMaximum() );

	//	Read in the configuration file
	//	NOTE: do not read in the config here, so python scripting can set values
	//
	//ReadConfig();

	//	show the options dialog first
	//
	if ( !m_bSkipCaptureOptions )
	{
		cptrRenderOutputData& data = cptrRenderOutputDataUtil::Data();

		//	fill-in the data
		//
		//data.m_bKeepFrameOpenAfterRender = false;
		data.m_fMarkerInTime = 0.0f;
		data.m_fMarkerOutTime = -1.0f;

		//	launch the first state
		//
		GotoState( m_StateRenderConfig );
	}
	else
	{
		m_bSkipCaptureOptions = false;
		cptrRenderUtil::SetCapture( true );

		//	launch the first state
		//
		GotoState( m_StateInitCaptures );
	}
}

//----------------------------------------------------------------------------
//	The object should clean up things that are not needed while the
//	mode is not running in the DeInitialize function.
//----------------------------------------------------------------------------
//virtual
void cptrModeRender::DeInitialize()
{
	// Clean up our sub window
	if (m_pSystem && m_bCaptureStarted)
	{
		mnmApp::EnableRender(true);
		if (!sm_bUseAppWindow)
			m_pSystem->DestroyWindow(m_pWindow);
		delete m_pRenderer;
		delete m_pViewer;
		m_pRenderer = NULL;
		m_pViewer	= NULL;
		m_pWindow	= NULL;
	}

	if ( m_bInitialized )
	{
		snSoundManager::Mute( m_bAudioMuteState );
	}

	//	hide the dialog and write out the capture preferences
	//
	cptrRenderProgressDialogUtil::Hide();

	//	reset the filename for this mode, next time it is called.
	//
	ResetConfigFileName();

	cptrRenderOutputData& data = cptrRenderOutputDataUtil::Data();
	data.m_bKeepFrameOpenAfterRender = false;

	m_bInitialized		= false;
	m_bCaptureStarted	= false;
}

//----------------------------------------------------------------------------
//	Quit level
//----------------------------------------------------------------------------
void cptrModeRender::ExitMode()
{
	cptrRenderStatsDataUtil::WriteRenderStats( cptrRenderStatsDataUtil::Data() );

	//	reset the lod level
	if ( m_OldSubdivLevel != -1 )
	{
		api3dSubdiv::SetSubdivLevel( m_OldSubdivLevel );
	}

//	prtclObjectMgr::Pause(m_bParticlesActive);

	cptrRenderOutputDataUtil::RestoreShadows();

	// terminate this mode
	this->SetTerminateCondition(appMode::e_TerminateAndRemove);
}

//----------------------------------------------------------------------------
//	The mode should do it's per frame "work" in the Think function.
//----------------------------------------------------------------------------
// virtual
void cptrModeRender::Think()
{
	//	abort!
	inKeyboard* pKeyboard = inDeviceMgr::GetKeyboard();
	if ( pKeyboard->IsPressed(inKeys::e_ESC) )
	{
		AbortRender();
	}

	//	update the state
	//
	UpdateState();
}

//------------------------------------------------------------------------
//	SetSystem() - set the g2d system for creating new windows
//------------------------------------------------------------------------
void cptrModeRender::SetSystem( g2dSystem* i_pSystem )
{
	m_pSystem = i_pSystem;
}

//------------------------------------------------------------------------
//	SetSkipCaptureOptions - if this is set, this mode won't call the
//	capture options dialog.  it will assume that it has already been
//	called by something else.
//
//	Note:  this value will reset with each call to this mode.
//------------------------------------------------------------------------
void cptrModeRender::SetSkipCaptureOptions( bool i_bSkip )
{
	m_bSkipCaptureOptions = i_bSkip;
}

//------------------------------------------------------------------------
//	SetUseOnlyValidCaptureTimes - set this to false to ignore all user
//	capture times so any time slice is a valid one.
//------------------------------------------------------------------------
void cptrModeRender::SetUseOnlyValidCaptureTimes( bool i_bUseValid )
{
	m_bUseOnlyValidCaptureTimes = i_bUseValid;
}

//------------------------------------------------------------------------
//	Allow another mode (like batch render) to set the start time
//------------------------------------------------------------------------
void cptrModeRender::SetRenderStartTime( float i_fRenderStartTime )
{
	m_fRenderStartTime = i_fRenderStartTime;
}

//----------------------------------------------------------------------------
//	IsModal() - signals whether the mode should be pushed onto the mode stack
//	ON TOP of the current mode, or replace the current mode (via Pop)
//
//	true - the mode will be pushed on top of the current mode.
//	false- the mode will replace the current mode.
//----------------------------------------------------------------------------
//virtual
bool cptrModeRender::IsModal()
{
	return true;
}

//--------------------------------------------------------------------
//	Abort all the render
//--------------------------------------------------------------------
void cptrModeRender::AbortRender()
{
	m_bAborted = true;
}

//--------------------------------------------------------------------
//	Pause the render
//--------------------------------------------------------------------
void cptrModeRender::PauseRender(bool i_bPause)
{
	m_bPaused = i_bPause;
}

//--------------------------------------------------------------------
//	Is the render paused
//--------------------------------------------------------------------
bool cptrModeRender::IsRenderPaused()
{
	return m_bPaused;
}

//--------------------------------------------------------------------
//	SetCaptureData() - set the capture data
//--------------------------------------------------------------------
void cptrModeRender::SetCaptureData()
{
	m_fSimTimeInc = cptrRenderUtil::GetSimTimeIncrement();
}



//
//	state functions
//
//	BeginState - is executed ONCE at the start of the state
//	OnState - is executed every frame while in the state
//	EndState - is executed on ending the state
//
//

//----------------------------------------------------------------------------
//	RenderConfig
//----------------------------------------------------------------------------
void cptrModeRender::BeginStateRenderConfig()
{
	DBG_WARNING0("state: Begin RenderConfig");

	//cptrRenderOutputData& data = cptrRenderOutputDataUtil::Data();
	//DBG_LOG0("BeginStateRenderConfig");
	//int num_cameras = data.m_Cameras.GetNumberOfItems();
	//for (int i = 0; i < num_cameras; ++i)
	//{
	//	int b1, b2;
	//	b1 = data.m_CameraList[i].m_bCapture.GetValue();
	//	b2 = data.m_Cameras.GetValueFlag(i);
	//	DBG_LOG3("%02d %s vs %s", i, b1?"true":"false", b2?"true":"false");
	//}

	//	now show the dialog
	//
	cptrRenderOptionsDialogUtil::Show( cptrRenderOutputDataUtil::Data() );
}
//----------------------------------------------------------------------------
void cptrModeRender::OnStateRenderConfig()
{
	//	launch the first state
	//
	GotoState( m_StateInitCaptures );
}
//----------------------------------------------------------------------------
void cptrModeRender::EndStateRenderConfig()
{
	//DBG_LOG0("END STATE RENDER CONFIG");
	//cptrRenderOutputData& data = cptrRenderOutputDataUtil::Data();
	//int num_cameras = data.m_Cameras.GetNumberOfItems();
	//for (int i = 0; i < num_cameras; ++i)
	//{
	//	int b1, b2;
	//	b1 = data.m_CameraList[i].m_bCapture.GetValue();
	//	b2 = data.m_Cameras.GetValueFlag(i);
	//	DBG_LOG3("%02d %s vs %s", i, b1?"true":"false", b2?"true":"false");
	//}
}

//----------------------------------------------------------------------------
//	InitCaptures
//----------------------------------------------------------------------------
void cptrModeRender::BeginStateInitCaptures()
{
	DBG_WARNING0("state: Begin StateInitCaptures");
	//DBG_LOG0( "state: initcaptures" );

	m_bAborted = false;
	m_OldSubdivLevel = -1;

	cptrRenderOutputData& data = cptrRenderOutputDataUtil::Data();
	tmlnTimeLine::SetFPS(data.m_fCaptureFPS.GetValue());
}
//----------------------------------------------------------------------------
void cptrModeRender::OnStateInitCaptures()
{
	if ( cptrRenderUtil::GetCapture() )
	{
		GotoState( m_StateBeginCapture );
	}
	else if ( cptrRenderOptionsDialogUtil::IsExitting() )
	{
		// this occurs here is the user hit the "X" on the capture dialog (for instance)
		//
		m_bAborted = true;
		GotoState( m_StateDeInitCaptures );
	}
}
//----------------------------------------------------------------------------
void cptrModeRender::EndStateInitCaptures()
{
	//	clear the statistics data
	//
	cptrRenderStatsDataUtil::ClearAllData();

	//	read in the stats data and see if the render stats dialog should
	//	automatically pop up.
	//
	cptrRenderStatsDataUtil::ReadRenderStats( cptrRenderStatsDataUtil::Data() );
	if ( cptrRenderStatsDataUtil::IsRenderStatsVisible() )
	{
		cptrRenderStatsDialogUtil::Show();
	}

	//
	SetCaptureData();

	cptrRenderOutputData& data = cptrRenderOutputDataUtil::Data();

	//DBG_LOG0("Cameras");
	//int count = data.m_Cameras.GetNumberOfItems();
	//for (int k=0; k < count; ++k)
	//{
	//	DBG_LOG3("%02d - %s (%s)", k, data.m_Cameras.GetValueText(k).c_str(), (data.m_Cameras.GetValueFlag(k) ? "true":"false") );
	//}
	//DBG_LOG0("CameraList");
	//count = data.m_CameraList.size();
	//for (int k=0; k < data.m_CameraList.size(); ++k)
	//{
	//	DBG_LOG3("%02d - %s (%s)", k, data.m_CameraList[k].m_CameraName.GetValue().c_str(), (data.m_CameraList[k].m_bCapture.GetValue() ? "true":"false") );
	//}

	visMgr::ShowIcons(false);
	if (!rndrPrefsMgr::ActualData((rndrPrefsMgr::rndr_Object)m_RenderPrefsObjectID).m_bRenderMatte)
		visMgr::ConfirmGeometryVisible();
	cmpsCompassMgr::SetRenderable( false );	// set all compasses invisible
	fgtFrameMgr::Show(false);

	if ( data.m_bCaptureAllCameras.GetValue() )
	{
		int startcam_index = 0;

		//	if there has been a saved render state, then start the time
		//
		if (data.m_RenderPosTime != -1.0f)
		{
			nameString camname( itStringUtil::GetStdString(data.m_RenderPosCamera.GetValue()) );
			startcam_index = camsCameraMgr::GetIndexForName(camname);
			if (startcam_index == -1)
				startcam_index = 0;
		}

		m_CurrentCameraIndex = startcam_index;
		// Even with Director's Cuts, make this checkbox means only the real *cameras*
		m_LastCameraIndex = camsCameraMgr::GetNumCameras();
	}
	else
	{
		// Here, include the director's cuts in the camera count
		//m_LastCameraIndex		= camsCameraMgr::GetNumCameras();
		m_LastCameraIndex		= camsFollowUtil::GetTotalCount();

		//	if there has been a saved render state, then start the time
		//
		if (data.m_RenderPosTime != -1.0f)
		{
			nameString camname( itStringUtil::GetStdString(data.m_RenderPosCamera.GetValue()) );
			int startcam_index = camsCameraMgr::GetIndexForName(camname);
			if (startcam_index == -1)
				startcam_index = 0;
			m_CurrentCameraIndex	= startcam_index;
		}
		else
		{
			m_CurrentCameraIndex	= -1;
			m_CurrentCameraIndex	= this->get_next_camera_index();
		}
	}
	m_CurrentCameraNumber = 0;
	calculate_number_of_renderable_cameras();

	//DBG_LOG2("first camera to render (index=%d) out of %d cameras", m_CurrentCameraIndex, m_NumberOfRenderableCameras);

	//	set the start + end time based on marker in/out
	//
	if (data.m_bUseMarkerTimes.GetValue())
	{
		//if ( !data.m_bBatchMode )
		{
			data.m_fMarkerInTime.SetValue( chnlMarkerMgr::GetMarkerInTime() );
			data.m_fMarkerOutTime.SetValue( chnlMarkerMgr::GetMarkerOutTime() );
		}

		data.m_fStartTime.SetValue( data.m_fMarkerInTime.GetValue() );
		data.m_fEndTime.SetValue( data.m_fMarkerOutTime.GetValue() );

		// fix a bug where the user creates a marker then shortens the end time of the scene to before marker
		if (data.m_fEndTime.GetValue() > tmlnTimeLine::GetMaximum()) 
			data.m_fEndTime.SetValue( tmlnTimeLine::GetMaximum() );
	}

	//	if there has been a saved render state, then start the time
	//
	if (data.m_RenderPosTime.GetValue() != -1.0f)
	{
		data.m_fStartTime.SetValue( data.m_RenderPosTime.GetValue() );
		data.m_RenderPosTime.SetValue( -1.0f );	// reset it so it doesn't trigger anymore
	}

	//DBG_LOG2("Capture Start(%6.3f) End(%6.3f)", data.m_fStartTime, data.m_fEndTime );

	//	build the in/out lists for this scene
	tmlnTimeInOutMgr::ClearLists();
	tmlnTimeInOutMgr::BuildTimeInOutLists();

	//	set the number of frames to capture
	//
	if ( data.m_fEndTime.GetValue() < 0 )
	{
		float capturemax = (data.m_fCaptureFPS.GetValue() * (tmlnTimeLine::GetMaximum() - data.m_fStartTime.GetValue()));
		cptrRenderUtil::SetCaptureMax( (int)(capturemax) );
		cptrRenderUtil::SetCaptureTimeMax( tmlnTimeLine::GetMaximum() );
		m_fEndTime = tmlnTimeLine::GetMaximum();

		//DBG_LOG2( "ModeRender Capture Max- = %6.3f %d", capturemax, (int)capturemax );
	}
	else
	{
		float capturemax = (data.m_fCaptureFPS.GetValue() * (data.m_fEndTime.GetValue() - data.m_fStartTime.GetValue()));
		cptrRenderUtil::SetCaptureMax( (int)(capturemax) );
		cptrRenderUtil::SetCaptureTimeMax( data.m_fEndTime.GetValue() );
		m_fEndTime = data.m_fEndTime.GetValue();

		//DBG_LOG2( "ModeRender Capture Max = %6.3f %d", capturemax, (int)capturemax );
	}

	//	show the Capture Status dialog
	//
	if (data.m_bShowRenderProgressDialog.GetValue())
	{
		cptrRenderProgressDialogUtil::Show();
		cptrRenderProgressDialogUtil::DisablePauseButton();
	}

	//DBG_LOG2( "cam indexes %d -> %d", m_CurrentCameraIndex, m_LastCameraIndex );

	//cptrRenderOutputData& data = cptrRenderOutputDataUtil::Data();

	if ( !data.m_bBatchMode.GetValue() )
	{
		data.m_NumberOfScenes.SetValue( 1 );
		data.m_CurrentSceneNumber.SetValue( 1 );
		data.m_OutputFiles.ClearList();
	}

	std::string filename;
	docSingleDocumentMgr::GetFilenameOnly(filename);
	data.m_CurrentSceneFilename.SetValue( itString(filename.c_str()) );
	data.m_RenderPosScene.SetValue( data.m_CurrentSceneFilename.GetValue() );		// for saving render position
	itString sfname = data.m_CurrentSceneFilename.GetValue();
	sfname.StripExtension();
	data.m_CurrentSceneFilename.SetValue(sfname);

	cptrRenderStatsDataUtil::StartScene( itStringUtil::GetStdString(sfname) );
	cptrRenderStatsDialogUtil::UpdateStatsDialog();

	//Log_Settings();
}

//----------------------------------------------------------------------------
//	BeginCapture
//----------------------------------------------------------------------------
void cptrModeRender::BeginStateBeginCapture()
{
	DBG_WARNING0("Begin BeginCapture");
	//DBG_LOG0( "state: begincapture" );

	m_bCaptureStarted = false;
	m_bCaptureFinished = false;
	m_bSwitchToCameraRenderRange = true;

	cptrRenderOutputData& data = cptrRenderOutputDataUtil::Data();
	data.m_OutputDirectory.SetValue( gfPaths::GetPath( mnmPaths::e_SaveFootage ) );

	//fsFileUtil::LocatorToANSIFilename( data.m_OutputDirectory, filename );
	//DBG_LOG1( "Mode Capture: setting footage directory to (%s)", filename.c_str() );

	//cptrRenderOutputDataUtil::Debug();

	cptrRenderUtil::Initialize();

	camsFollowUtil::SetFollowIndex( m_CurrentCameraIndex );

	// TODO make an interest that gets called so we don't have to have
	//	system specific stuff in here.
	//

	//	set the lod level
	m_OldSubdivLevel = api3dSubdiv::GetSubdivLevel();
	api3dSubdiv::SetSubdivLevel( data.m_nSubdivLevel.GetValue() );

	//
//	m_bParticlesActive = prtclObjectMgr::Paused();

	// TODO [rjk] currently this assumes there will only be 26 capture drivers for a single camera.
	//	currently they have 8 at most.
	//
	m_cCurrentRenderDriverLetter = 'A';

	//	update the render status dialog
	//
	update_progress_percentages(false);

	//	set the start time if not in batch
	if ( !(data.m_bBatchMode.GetValue()) )
		SetRenderStartTime(appTime::GetTime());
}

//----------------------------------------------------------------------------
//virtual
void cptrModeRender::OnStateBeginCapture()
{
	if ( cptrRenderUtil::GetCapture() )
	{
		//	check to see if this is the first frame of capture
		//
		if ( m_bCaptureStarted == false )
		{
			m_bCaptureStarted	= true;
			m_bFirstRenderFrame	= true;

			tmlnTimeLine::SetValue( tmlnTimeLine::GetMinimum() );

			// This has to go after the timeline is updated
			// and before the frame is captured
			modeModeTime::Think();

			cptrRenderOutputData& data = cptrRenderOutputDataUtil::Data();

			cptrRenderOutputDataUtil::EnableShadows( rndrPrefsMgr::ActualData((rndrPrefsMgr::rndr_Object)m_RenderPrefsObjectID).m_bShadowsOn );

			// Create sub window for capturing
			//
			if (m_pSystem)
			{
				if ( m_pWindow == NULL )
				{
					if (sm_bUseAppWindow)
					{
						m_pWindow = cptrRenderUtil::GetAppWindow();
					}
					else
					{
						// Create sub window of correct size
				
						int width	= data.m_nWidth.GetValue() * (data.m_bJitteredSampling.GetValue() ? 1 : data.m_nCaptureSampling.GetValue());
						int height	= data.m_nHeight.GetValue() * (data.m_bJitteredSampling.GetValue() ? 1 : data.m_nCaptureSampling.GetValue());

						if (cptrRenderUtil::GetCaptureQuadrants())
						{
							int quad_div = cptrRenderUtil::GetQuadrantDivision();
							width /= quad_div;
							height /= quad_div;
						}

						// Create the (void*)main_form->GetRenderPane(0)->Handle to render into
						//
						try
						{
							// TODO - allow subwindow to remember it's location
							// TODO - allow "X" button to be pressed.  Get message back to main window.
							//
							m_pWindow = m_pSystem->CreateSubWindow(width, height, 80, 80);
						}
						catch(const g2dOutOfVideoMemoryX&)
						{
							guiMessageBox::Show("Out of video memory creating capture window. Aborting Capture", "Error", guiMessageBox::e_OKOnly);
							m_bAborted = true;
							m_bCaptureStarted = false;
							m_pWindow = NULL;
							GotoState( m_StateEndCapture );
							return;
						}
						catch( const g2dScreenInitX& )
						{
							std::string msg = "Invalid capture window. Aborting Capture";
							guiMessageBox::Show(msg.c_str(), "Error");
							DBG_ERROR1("g2dScreenInitX (%s)", msg.c_str());

							m_bAborted = true;
							m_bCaptureStarted = false;
							m_pWindow = NULL;
							GotoState( m_StateEndCapture );
							return;
						}
					}

					camCamera &render_camera = cam3dMgr::GetCamera();

					// This isn't necessary because the viewer will
					// do it before rendering when we set SetMatchAspectToWindow(true)
					//render_camera.SetAspect(data.m_nWidth, data.m_nHeight);

					m_pRenderer = g3dSceneRendererCreate::CreateRenderer(rndrPrefsMgr::ActualData((rndrPrefsMgr::rndr_Object)m_RenderPrefsObjectID).m_RendererType);
					m_pViewer = new g3dViewer(m_pWindow, m_pRenderer);
					m_pViewer->SetBackgroundColor( g2dRGBColor(0x00, 0x00, 0x00) );
					m_pViewer->SetCamera(&render_camera);
					m_pViewer->SetScene(api3dScene::GetScene());
					//m_pViewer->SetMatchAspectToWindow(true); // camera has this flag now, and it is set to true for scripted cameras
					mnmApp::EnableRender(false);
				}

				cptrRenderUtil::SetCaptureWindow(m_pWindow);

				if ( data.m_bDisplayTimeCode.GetValue() )
				{
					mnmTimeCodeUtil::SetWindow( this->m_pWindow );
					mnmTimeCodeMgr::Initialize();
					mnmTimeCodeMgr::SetShowTimeCode( true );
					mnmTimeCodeMgr::SetFrameRate( data.m_fCaptureFPS.GetValue() );
				}
			}

			itString cam_name;
			nameString theName;
			std::string cam_desc;
			camsFollowUtil::GetCurrentCameraName(theName);
			camsFollowUtil::GetCurrentCameraDescription(cam_desc);
			cam_name = theName.GetString().c_str();
			data.m_RenderPosCamera.SetValue( cam_name );			// for saving render position

			// stats: start the camera
			if ( m_cCurrentRenderDriverLetter == 'A' )
			{
				cptrRenderStatsDataUtil::StartCamera( theName.GetString(), cam_desc );
				cptrRenderStatsDialogUtil::UpdateStatsDialog();
			}

			//	Append letter on the end of the filename if the flag is set.
			//
			if (   (data.m_bUseRenderDriverAsDirAndFname.GetValue())
				|| (data.m_bMoviePerDriver.GetValue()))
			{
				cam_name += m_cCurrentRenderDriverLetter;

				//	stats: start the render block
				//cptrRenderStatsDataUtil::StartCameraRenderBlock( itStringUtil::GetStdString(cam_name) );
			}

			if ( data.m_bUseCameraNameInFilename.GetValue() )
			{
				cptrRenderOutputDataUtil::SetCurrentCamera( cam_name );
			}

			//	Get the in/out times for this camera
			m_CurrentInOutList.Clear();
			tmlnTimeInOutMgr::GetDataList( theName.GetString(), m_CurrentInOutList );
			m_CurrentInOutList.SetCompletedFlag();

			build_directory();

			// must call ApplyRenderPrefs BEFORE BeginCapture
			ApplyRenderPrefs();
			cptrRenderUtil::BeginCapture();

			// After cptrRenderUtil::BeginCapture, the mtrlScriptObject rendertargets 
			// may have been rebuilt for HDR/LDR renderer change.  
			// So now it is safe to add them to this viewer.
			mtrlScriptObject::AddTargetsToViewer(m_pViewer);

			//	Reset the particle generators
			//
			// TODO: is this the best place for this?
			//
//			prtclObjectMgr::ResetGenerators();

			GotoState( m_StateLeadIn );
		}

		if (m_bAborted)
		{
			GotoState( m_StateEndCapture );
		}

	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void cptrModeRender::ApplyRenderPrefs()
{
	rndrPrefsMgr::ApplyPrefs((rndrPrefsMgr::rndr_Object)m_RenderPrefsObjectID);
}

//----------------------------------------------------------------------------
//virtual
void cptrModeRender::EndStateBeginCapture()
{
	cptrRenderOutputData& data = cptrRenderOutputDataUtil::Data();
	std::string fname = itStringUtil::GetStdString( data.m_CurrentSceneFilename.GetValue() );
	char buffer[64];
	sprintf(buffer, "Scene - %s", fname.c_str());
	std::string scenelabel(buffer);
	cptrRenderProgressDialogUtil::SetSceneLabel(scenelabel);

	//DBG_LOG1("Begin Capture (end) %s", (data.m_bJitteredSampling.GetValue() ? "true":"false") );
}


//----------------------------------------------------------------------------
//	LeadIn
//----------------------------------------------------------------------------
void cptrModeRender::BeginStateLeadIn()
{
	DBG_WARNING0("Begin LeadIn");
	//DBG_LOG0( "state: leadin" );

	cptrRenderOutputData& data = cptrRenderOutputDataUtil::Data();
	m_fLeadTimeDelta = data.m_fLeadIn.GetValue();
}
//----------------------------------------------------------------------------
void cptrModeRender::OnStateLeadIn()
{
	if ( m_fLeadTimeDelta <= 0.0f )
	{
		GotoState( m_StateCapture );
		return;
	}

	//if ( cptrRenderUtil::GetCapture() )
	//{
	//	if ( m_bFirstRenderFrame )
	//	{
	//		m_bFirstRenderFrame = false;
	//		return;
	//	}
	//}

	// This has to go after the timeline is updated
	// and before the frame is captured
	modeModeTime::Think();

	// render and capture frame
	do_lead_render();

	m_fLeadTimeDelta -= m_fSimTimeInc;

	if (m_bAborted)
	{
		GotoState( m_StateEndCapture );
	}
}
//----------------------------------------------------------------------------
void cptrModeRender::EndStateLeadIn()
{
}

//----------------------------------------------------------------------------
//	Capture
//----------------------------------------------------------------------------
void cptrModeRender::BeginStateCapture()
{
	DBG_WARNING0("Begin Capture");
	//DBG_LOG0( "state: capture" );

	cptrRenderProgressDialogUtil::EnablePauseButton();
}
//----------------------------------------------------------------------------
void cptrModeRender::OnStateCapture()
{
	if (m_bCaptureFinished) return;
	if (m_bPaused) return;

	bool bStartingNewCamera = false;
	//	update the sim time if we are capturing
	//
	if ( cptrRenderUtil::GetCapture() )
	{
		cptrRenderUtil::UpdateSimTime();

		set_timeline(appSimTime::GetTime());

		//	check if the current time is a capture time.  If not, find out
		//	the next capture time.
		//

		//	if using only valid times, check for driver times, otherwise all time is valid
		//
		bool bCapture = (m_bUseOnlyValidCaptureTimes ? m_CurrentInOutList.IsTimeWithin( tmlnTimeLine::GetValue(), true ) : true );
		if (!bCapture)
		{
			//	if this isn't a capture time, find the next valid time
			//
			cptrRenderOutputData& data = cptrRenderOutputDataUtil::Data();
			float nexttime = m_CurrentInOutList.GetNextCaptureTime( tmlnTimeLine::GetValue() );
			if (   (nexttime == -1.0f)
				|| (nexttime >= m_fEndTime))
			{
				set_timeline( m_fEndTime, true );
			}
			else
			{
				//	snap the start time to a valid frame time.
				tmlnTimeUtil::AdjustTimeToFrame(nexttime);

				//	set the time
				set_timeline( nexttime, true );

				//
				if ( !m_bSwitchToCameraRenderRange )
					m_cCurrentRenderDriverLetter++;
				m_bSwitchToCameraRenderRange = true;
			}
		}

		if ( m_bSwitchToCameraRenderRange )
		{
			//	set a new directory based on the cam name + letter
			build_directory();

			do_titlecard_render();

			bStartingNewCamera = true;

			m_bSwitchToCameraRenderRange = false;
		}
	}

	//	set the timeline
	//
	//tmlnTimeLine::SetValue( (int)(appSimTime::GetTime() * tmlnTimeLine::GetMaximum() ) );

	// This has to go after the timeline is updated
	// and before the frame is captured
	modeModeTime::Think();

	mnmTimeCodeMgr::Update();

	if (bStartingNewCamera)
		reset_renderer_for_camera();

	//	update the progress dialog
	update_progress_percentages(false);

	//
	float start_time = 0.0;
	float end_time = 0.0;
	bool bCapture = (m_bUseOnlyValidCaptureTimes ? 
						m_CurrentInOutList.GetTimesIfWithin( tmlnTimeLine::GetValue(), start_time, end_time, true ) 
						: true ); // true for return true if no drivers
	if (bCapture)
	{
		//	set the render window title
		//
		std::string window_title;
		tmlnTimeUtil::GetTimeString( tmlnTimeLine::GetValue(), window_title );
		m_pWindow->SetTitle( window_title.c_str() );

		//	set a time code anytime we have a new camera
		//
		if (bStartingNewCamera)
		{
			cptrRenderOutputData& data = cptrRenderOutputDataUtil::Data();

			//	if the drive letter change it means a new driver has started, so stop the first video
			//	and start a new one
			//
			if (data.m_bMoviePerDriver.GetValue())
			{
				if (m_cCurrentRenderDriverLetter != 'A')
				{
					cptrRenderUtil::CloseOutput();
					cptrRenderUtil::OpenOutput();
				}
			}

			//	write a timecode at the beginning of the file
			cptrRenderUtil::CaptureTimeCode( start_time, end_time );
		}

		// render and capture frame
		//
		try
		{
			do_render_capture();
		}
		catch(const g2dOutOfVideoMemoryX&)
		{
			std::string msg = "Out of video memory rendering frame. Aborting Capture";
			guiMessageBox::Show(msg.c_str(), "Error");
			DBG_ERROR1("g2dOutOfVideoMemoryX (%s)", msg.c_str());

			m_bAborted = true;
			GotoState( m_StateEndCapture );
		}
		catch(const g2dOutOfSystemMemoryX&)
		{
			std::string msg = "Out of system memory rendering frame. Aborting Capture";
			guiMessageBox::Show(msg.c_str(), "Error");
			DBG_ERROR1("g2dOutOfSystemMemoryX (%s)", msg.c_str());

			m_bAborted = true;
			GotoState( m_StateEndCapture );
		}
	}

	// done capturing this camera, go to lead out
	//
	if (cptrRenderUtil::GetCaptureFinished())
	{
		if ( !m_bCaptureFinished )
		{
			m_bCaptureFinished = true;
		
			GotoState( m_StateLeadOut );
		}
		return;
	}

	if (m_bAborted)
	{
		GotoState( m_StateEndCapture );
	}
}


//----------------------------------------------------------------------------
void cptrModeRender::EndStateCapture()
{
}

//----------------------------------------------------------------------------
//	LeadOut
//----------------------------------------------------------------------------
void cptrModeRender::BeginStateLeadOut()
{
	DBG_WARNING0("Begin LeadOut");
	//DBG_LOG0( "state: leadout" );

	cptrRenderOutputData& data = cptrRenderOutputDataUtil::Data();
	m_fLeadTimeDelta = data.m_fLeadOut.GetValue();
}
//----------------------------------------------------------------------------
void cptrModeRender::OnStateLeadOut()
{
	if ( m_fLeadTimeDelta <= 0.0f )
	{
		GotoState( m_StateEndCapture );
		return;
	}

	// This has to go after the timeline is updated
	// and before the frame is captured
	modeModeTime::Think();

	try
	{
		do_render_capture();
	}
	catch(const g2dOutOfVideoMemoryX&)
	{
		std::string msg = "Out of video memory rendering lead out frame. Aborting Capture";
		guiMessageBox::Show(msg.c_str(), "Error");
		DBG_ERROR1("g2dOutOfVideoMemoryX (%s)", msg.c_str());

		m_bAborted = true;
		GotoState( m_StateEndCapture );
	}
	catch(const g2dOutOfSystemMemoryX&)
	{
		std::string msg = "Out of system memory rendering frame. Aborting Capture";
		guiMessageBox::Show(msg.c_str(), "Error");
		DBG_ERROR1("g2dOutOfSystemMemoryX (%s)", msg.c_str());

		m_bAborted = true;
		GotoState( m_StateEndCapture );
	}

	m_fLeadTimeDelta -= m_fSimTimeInc;

	if (m_bAborted)
	{
		GotoState( m_StateEndCapture );
	}
}
//----------------------------------------------------------------------------
void cptrModeRender::EndStateLeadOut()
{
}

//----------------------------------------------------------------------------
//	EndCapture
//----------------------------------------------------------------------------
void cptrModeRender::BeginStateEndCapture()
{
	DBG_WARNING0("Begin EndCapture");
	//DBG_LOG0( "state: endcapture" );

	cptrRenderUtil::EndCapture();
	// ASSUME we will not try to render the m_pViewer after cptrRenderUtil::EndCapture
	// otherwise, we need to clean up the m_pViewer's target renderers, since EndCapture
	// may have invalidated them.
	tmlnTimeLine::SetValue( tmlnTimeLine::GetMinimum() );
	if (m_MotionSampler != NULL)
	{
		delete m_MotionSampler;
		m_MotionSampler = NULL;
	}
	if (m_AccBuf != NULL)
	{
		delete m_AccBuf;
		m_AccBuf = NULL;
	}

	GotoState( m_StateDeInitCaptures );
}
//----------------------------------------------------------------------------
void cptrModeRender::OnStateEndCapture()
{
}
//----------------------------------------------------------------------------
void cptrModeRender::EndStateEndCapture()
{
}

//----------------------------------------------------------------------------
//	DeInitCaptures
//----------------------------------------------------------------------------
void cptrModeRender::BeginStateDeInitCaptures()
{
	DBG_WARNING0("Begin DeInitCaptures");
	//DBG_LOG0( "state: deinitcaptures" );

	if (m_MotionSampler != NULL)
	{
		delete m_MotionSampler;
		m_MotionSampler = NULL;
	}
	if (m_AccBuf != NULL)
	{
		delete m_AccBuf;
		m_AccBuf = NULL;
	}

	mnmTimeCodeMgr::DeInitialize();

	tmlnTimeLine::SetValue( 0 );

	if ( m_bAborted )
	{
		ExitMode();
		return;
	}

	cptrRenderOutputData& data = cptrRenderOutputDataUtil::Data();

	//	get the next camera in the list if there is one
	//
	m_CurrentCameraIndex = get_next_camera_index();
	update_progress_percentages(true);

	if ( m_CurrentCameraIndex >= m_LastCameraIndex )
	{
		//	if not in batch mode then call the post capture functionality
		//
		if ( !data.m_bBatchMode.GetValue() )
		{
			//	stats: end the scene
			cptrRenderStatsDataUtil::EndScene();
			cptrRenderStatsDialogUtil::UpdateStatsDialog();

			//	if finished the scene (not in batch) then delete the saved render position file.
			//
			if (data.m_RenderPosSaveFile.GetValue().GetNumNames() > 0)
			{
				//	if single frame, don't delete RP
				if (!data.m_bKeepFrameOpenAfterRender.GetValue())
					cptrRenderProgressDialogUtil::DeleteRenderPosition();
			}
		}

		GotoState(m_StatePostCapture);
	}
	else
	{
		GotoState( m_StateBeginCapture );
	}
}
//----------------------------------------------------------------------------
void cptrModeRender::OnStateDeInitCaptures()
{
}
//----------------------------------------------------------------------------
void cptrModeRender::EndStateDeInitCaptures()
{
}

//----------------------------------------------------------------------------
//	PostCapture
//----------------------------------------------------------------------------
void cptrModeRender::BeginStatePostCapture()
{
	DBG_WARNING0("Begin PostCapture");

	cptrRenderOutputData& data = cptrRenderOutputDataUtil::Data();
	if ( !data.m_bBatchMode.GetValue() )
	{
		//	do post capture things
		cptrPostRenderUtil::Execute();
	}
}
//----------------------------------------------------------------------------
void cptrModeRender::OnStatePostCapture()
{
	//	decide where to go
	cptrRenderOutputData& data = cptrRenderOutputDataUtil::Data();
	if (data.m_bKeepFrameOpenAfterRender.GetValue())
		GotoState(m_StateWaitToEndMode);
	else
		ExitMode();
}
//----------------------------------------------------------------------------
void cptrModeRender::EndStatePostCapture()
{
	tmlnTimeLine::SetFPS(g3dConstants::c_fDefaultFrameRate);	// set to a default value again.
}

//----------------------------------------------------------------------------
//	WaitToEndMode
//----------------------------------------------------------------------------
void cptrModeRender::BeginStateWaitToEndMode()
{
}
//----------------------------------------------------------------------------
void cptrModeRender::OnStateWaitToEndMode()
{
	if (m_bAborted)
	{
		ExitMode();
	}
}
//----------------------------------------------------------------------------
void cptrModeRender::EndStateWaitToEndMode()
{
}


//----------------------------------------------------------------------------
// do title card (slate) render
//----------------------------------------------------------------------------
void cptrModeRender::do_titlecard_render()
{
	cptrRenderOutputData& data = cptrRenderOutputDataUtil::Data();

	//	if flag set, skip rendering the title cards
	//
	if (!data.m_bOutputTitleCard.GetValue())
	{
		return;
	}

	//	clear the surface
	//
	m_pWindow->Clear(g2dRGBColor(0,0,0));
	m_pWindow->BeginScene();

	//	gather the data
	//
	g2dFontHandle font = g2dFontUtil::LoadFont(itString("Arial"), 20);
	itString text;

	char buffer[256];
	float starttime, endtime;
	if (!(this->m_CurrentInOutList.GetTimesIfWithin( tmlnTimeLine::GetValue(), starttime, endtime )))
	{
		starttime	= data.m_fStartTime.GetValue();
		endtime		= m_fEndTime;
	}
	else
	{
		// adjust the displayed time to take into consideration markers
		if (starttime < data.m_fStartTime.GetValue())
			starttime = data.m_fStartTime.GetValue();
		if (endtime > m_fEndTime)
			endtime = m_fEndTime;
	}

	float simtime = starttime;
	int hrs = 0, min = 0, sec = 0, msec = 0;
	int framenum = 0;

	//	build the titlecard
	//
	const g2dRGBColor color(255,255,255);
	const int c_XLOC = 70;
	const int c_YLOC = 50;
	const int c_YLOC_INC = 30;
	float fps = data.m_fCaptureFPS.GetValue();
	//float mintime = tmlnTimeLine::GetMinimum();

	::sprintf(buffer,"Scene: %s", itStringUtil::GetStdString(data.m_CurrentSceneFilename.GetValue()).c_str() );
	text = buffer;
	m_pWindow->DrawText(c_XLOC,  c_YLOC+c_YLOC_INC*0, font, text, color);

	::sprintf(buffer,"Camera: %s", itStringUtil::GetStdString(data.m_CurrentCamera.GetValue()).c_str() );
	text = buffer;
	m_pWindow->DrawText(c_XLOC,  c_YLOC+c_YLOC_INC*1, font, text, color);

	std::string timestr;
	tmlnTimeUtil::GetTimeStringInHMSMAndFrames( starttime, fps, timestr );
	::sprintf(buffer,"Start Time: %s", timestr.c_str());
	text = buffer;
	m_pWindow->DrawText(c_XLOC,  c_YLOC+c_YLOC_INC*2, font, text, color);

	tmlnTimeUtil::GetTimeStringInHMSMAndFrames( endtime, fps, timestr );
	::sprintf(buffer,"End Time: %s", timestr.c_str());
	text = buffer;
	m_pWindow->DrawText(c_XLOC, c_YLOC+c_YLOC_INC*3, font, text, color);

	tmlnTimeUtil::GetTimeStringInHMSMAndFrames( (endtime-starttime), fps, timestr );
	::sprintf(buffer,"Duration: %s", timestr.c_str());
	text = buffer;
	m_pWindow->DrawText(c_XLOC, c_YLOC+c_YLOC_INC*4, font, text, color);

	m_pWindow->EndScene();

	//	render out 1/2 second of the titlecard
	for (int i = 0; i < (int)(fps/2); ++i)
	{
		cptrRenderUtil::CaptureFrame();
	}
	m_pWindow->Present();
}

//----------------------------------------------------------------------------
// do render in secondary window and capture the frame
//----------------------------------------------------------------------------
void cptrModeRender::do_render_capture()
{
	if (!cptrRenderUtil::ReadyToCapture())
		return;

	camCamera *pCamera = camsFollowUtil::GetFollowCamera();
	cptrRenderOutputData& data = cptrRenderOutputDataUtil::Data();

	//	Motion Samples
	//
	if (data.m_nMotionSamplesPerFrame.GetValue() > 1)
	{
		if (m_MotionSampler == NULL)
			m_MotionSampler = new cptrSupersampleMotionSampler(data.m_nMotionSamplesPerFrame.GetValue(), m_pViewer);

		// give the scene a chance to store what it needs
		// in order to record a sub-frame motion blur sample
		try
		{
			m_MotionSampler->CaptureMotionSample(*pCamera, tmlnTimeLine::GetValue());
		}
		catch (const g2dOutOfVideoMemoryX&)
		{
			std::string msg = "Out of video memory capturing motion. Aborting Capture";
			DBG_ERROR1("g2dOutOfVideoMemoryXX (%s)", msg.c_str());

			delete m_MotionSampler;
			m_MotionSampler = NULL;
			throw;
		}
		catch(const g2dOutOfSystemMemoryX&)
		{
			std::string msg = "Out of system memory capturing motion. Aborting Capture";
			DBG_ERROR1("g2dOutOfSystemMemoryX (%s)", msg.c_str());

			delete m_MotionSampler;
			m_MotionSampler = NULL;
			throw;
		}

		// if we are ready, then capture a frame!
		if (m_MotionSampler->DidShutterClose())
			m_MotionSampler->CaptureFrame(*pCamera);

		return;
	}

	api3dTargetRendererMgr::RenderTargets( appSimTime::GetTime() );

	//	render out either quadrants or the whole image
	//
	if (cptrRenderUtil::GetCaptureQuadrants())
	{
		// Render large resolution in 4 quadrant renders
		float timeline_time = tmlnTimeLine::GetValue();

		if (m_pViewer && cptrRenderUtil::GetCapture())
		{
			m_pViewer->SetCamera(pCamera);

			cptrRenderUtil::BeginQuadrants();

			// store state of timecode in order to restore later
			bool show_timecode = mnmTimeCodeMgr::IsShowTimeCode();

			const int num_quadrants = cptrRenderUtil::GetNumQuadrants();
			for (int i=0; i<num_quadrants; i++)
			{
				// Do Quadrant
				cptrRenderUtil::ConfigureViewport(i, *pCamera);
				m_pViewer->Render(timeline_time);
				cptrRenderUtil::CaptureQuadrant(i);
				m_pViewer->Present();

				// only show time code on one quadrant 
				// (i.e. turn it off after first quadrant)
				if (i==0)
				{
					mnmTimeCodeMgr::SetShowTimeCode( false );
				}
			}
			cptrRenderUtil::EndQuadrants();

			// restore camera
			pCamera->SetSubViewport(-1,1, -1,1);

			if ( show_timecode )
				mnmTimeCodeMgr::SetShowTimeCode( true );
		}
	}
	else
	{
		// Render capture window normally
		if (m_pViewer)
		{
			if (data.m_bJitteredSampling.GetValue())
			{
				RenderJitteredFrame();
			}
			else
			{
				float timeline_time = tmlnTimeLine::GetValue();
				m_pViewer->SetCamera(pCamera);
				m_pViewer->Render(timeline_time);
			}
		}

		//	capture the current frame
		//
		if ( cptrRenderUtil::GetCapture() )
		{
			try
			{
				if (data.m_bJitteredSampling.GetValue())
				{
					DBG_ASSERT0(m_AccBuf, "Accumulation buffer not initialized");
					cptrRenderUtil::CaptureImage(m_AccBuf->GetAsImage());
				}
				else
				{
					cptrRenderUtil::CaptureFrame();
				}
			}
			catch( const fsDiskFullX& i_Ex )
			{
				std::string filename;
				fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);
				DBG_ERROR1("fsDiskFullX: %s", filename.c_str());

				std::string msg = "Out of disk space or the disk is corrupt.  Could not write " + filename;
				DBG_ERROR1("%s", msg.c_str() );
				guiMessageBox::Show(msg.c_str(), "Error", guiMessageBox::e_OKOnly);
				assert(false);
			}
		}

		//	
		if (m_pViewer)
		{
			m_pViewer->Present();
		}
	}
}

//----------------------------------------------------------------------------
//	lead in/out render output
//----------------------------------------------------------------------------
void cptrModeRender::do_lead_render()
{
	try
	{
		do_render_capture();
	}
	catch(const g2dOutOfVideoMemoryX&)
	{
		std::string msg = "Out of video memory rendering lead in frame. Aborting Capture";
		guiMessageBox::Show(msg.c_str(), "Error");
		DBG_ERROR1("g2dOutOfVideoMemoryX (%s)", msg.c_str());

		m_bAborted = true;
		GotoState( m_StateEndCapture );
	}
	catch(const g2dOutOfSystemMemoryX&)
	{
		std::string msg = "Out of system memory rendering frame. Aborting Capture";
		guiMessageBox::Show(msg.c_str(), "Error");
		DBG_ERROR1("g2dOutOfSystemMemoryX (%s)", msg.c_str());
		m_bAborted = true;
		GotoState( m_StateEndCapture );
	}
}

//----------------------------------------------------------------------------
//	set the timeline value to the passed in value.  if the time
//----------------------------------------------------------------------------
void cptrModeRender::set_timeline(float i_value, bool i_bSetAppSimTimeAlso)
{
	//cptrRenderOutputData& data = cptrRenderOutputDataUtil::Data();

	//	set the timeline
	//
	if ( i_value <= m_fEndTime )
	{
		tmlnTimeLine::SetValue( i_value );
	}
	else
	{
		tmlnTimeLine::SetValue( m_fEndTime );

		//	if the time has been altered from something different than the passed in value
		//	set the sim time also.
		i_bSetAppSimTimeAlso = true;	
	}

	if (i_bSetAppSimTimeAlso)
	{
		cptrRenderUtil::SetSimTime( tmlnTimeLine::GetValue() );
	}
}


//--------------------------------------------------------------------
//	get the next camera index based on the camera list (or all cams)
//--------------------------------------------------------------------
int cptrModeRender::get_next_camera_index()
{
	cptrRenderOutputData& data = cptrRenderOutputDataUtil::Data();

	//DBG_LOG1("cameras = %d", data.m_CameraList.size());

	for (int i = m_CurrentCameraIndex+1; i < m_LastCameraIndex; ++i)
	{
		if (data.m_bCaptureAllCameras.GetValue() || data.m_CameraList[i].m_bCapture.GetValue())
		{
			m_CurrentCameraIndex = i;
			m_CurrentCameraNumber++;
			return m_CurrentCameraIndex;
		}
	}

	m_CurrentCameraIndex = m_LastCameraIndex + 1;
	return m_CurrentCameraIndex;
}

//--------------------------------------------------------------------
//	Calculate the number of renderable cameras
//--------------------------------------------------------------------
void cptrModeRender::calculate_number_of_renderable_cameras()
{
	m_NumberOfRenderableCameras = 0;
	cptrRenderOutputData& data = cptrRenderOutputDataUtil::Data();

	if ( data.m_bBatchMode.GetValue() )
	{
		m_NumberOfRenderableCameras = m_LastCameraIndex;
	}
	else
	{
		DBG_ASSERT2( (data.m_CameraList.size() >= m_LastCameraIndex), "Invalid number of cameras, %d from %d", m_LastCameraIndex, data.m_CameraList.size());

		for (int i = 0; i < m_LastCameraIndex; ++i)
		{
			if (data.m_CameraList[i].m_bCapture.GetValue())
			{
				//DBG_LOG2("%d - (%s) renderable camera", i, data.m_CameraList[i].m_CameraName.GetValue().c_str());

				m_NumberOfRenderableCameras++;
			}
			else
			{
				//DBG_LOG2("%d - (%s)", i, data.m_CameraList[i].m_CameraName.GetValue().c_str());
			}
		}
	}
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cptrModeRender::build_directory()
{
	cptrRenderOutputData& data = cptrRenderOutputDataUtil::Data();

	data.m_OutputDirectory.SetValue( gfPaths::GetPath( mnmPaths::e_SaveFootage ) );

	//if ( data.m_bAutoGenerateDirectory )
	//{
	//	data.m_OutputDirectory = gfPaths::GetPath(mnmPaths::e_SaveFootage);
	//
	//	DBG_ASSERT0( data.m_OutputDirectory.GetNumNames() > 0, "Footage path is empty" );
	//
	//	std::string StrLoc;
	//	fsFileUtil::LocatorToANSIFilename(data.m_OutputDirectory, StrLoc);
	//	DBG_LOG1( "generated directory (%s)", StrLoc.c_str() );
	//}

	if ( data.m_bUseSceneFilenameAsDirectory.GetValue() )
	{
		fsLocator dir = data.m_OutputDirectory.GetValue();
		dir.Push( data.m_CurrentSceneFilename.GetValue() );
		data.m_OutputDirectory.SetValue(dir);
	}

	//	is the output movie or image?
	if ( data.m_bCaptureMovie.GetValue() )
	{
		//	output is a movie, so determine whether there is a movie
		//	per driver or all in one.  If seperate, set the camera name
		//
		if (data.m_bMoviePerDriver.GetValue())
		{
			itString cam_dir_name;
			nameString theName;
			camsFollowUtil::GetCurrentCameraName( theName );
			cam_dir_name = theName.GetString().c_str();

			//	Append letter on the end of the folder if the flag is set.
			//
			//if ( data.m_bUseRenderDriverAsDirAndFname.GetValue() )
			{
				cam_dir_name += m_cCurrentRenderDriverLetter;
			}

			cptrRenderOutputDataUtil::SetCurrentCamera(cam_dir_name);
		}
	}
	else
	{
		//
		if ( data.m_bUseCameraNameAsDirectory.GetValue() )
		{
			itString cam_dir_name;
			nameString theName;
			camsFollowUtil::GetCurrentCameraName( theName );
			cam_dir_name = theName.GetString().c_str();

			//	Append letter on the end of the folder if the flag is set.
			//
			if ( data.m_bUseRenderDriverAsDirAndFname.GetValue() )
			{
				cam_dir_name += m_cCurrentRenderDriverLetter;
			}

			fsLocator dir = data.m_OutputDirectory.GetValue();
			dir.Push( cam_dir_name );
			data.m_OutputDirectory.SetValue(dir);

			cptrRenderOutputDataUtil::SetCurrentCamera(cam_dir_name);

			//	stats: start the render block
			cptrRenderStatsDataUtil::StartCameraRenderBlock( itStringUtil::GetStdString(cam_dir_name) );
			cptrRenderStatsDialogUtil::UpdateStatsDialog();
		}

		//
		if (data.m_bUseResolutionAsDirectory.GetValue())
		{
			itString resolution;
			char res[28];
			sprintf(res,"%dx%d", data.m_nWidth.GetValue(), data.m_nHeight.GetValue());
			resolution = res;

			fsLocator dir = data.m_OutputDirectory.GetValue();
			dir.Push( resolution );
			data.m_OutputDirectory.SetValue(dir);
		}
	}

	fsLocator dir = data.m_OutputDirectory.GetValue();
	cptrRenderOutputDataUtil::SetDirectory( dir );

	//std::string StrLoc;
	//fsFileUtil::LocatorToANSIFilename(data.m_OutputDirectory, StrLoc);
	//DBG_LOG1( "output directory (%s)", StrLoc.c_str() );

	//	check if the directory exists, if not, create it.
	//
	if ( !fsFileUtil::DirectoryExists(data.m_OutputDirectory.GetValue()) )
	{
		fsFileUtil::CreateDirectory(data.m_OutputDirectory.GetValue());
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
float cptrModeRender::get_camera_percentage()
{
	float timeline_time = tmlnTimeLine::GetValue();
	float camera_percentage = this->m_CurrentInOutList.GetPercentage( timeline_time );
	return camera_percentage;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void cptrModeRender::update_camera_percentage()
{
	char buffer[64];
	nameString theName;
	camsFollowUtil::GetCurrentCameraName(theName);

	sprintf(buffer, "Camera - %s", theName.GetString().c_str());
	std::string camera_label(buffer);
	float percentage = get_camera_percentage();
	cptrRenderProgressDialogUtil::SetCameraRenderPercentage( 100.0f*percentage, camera_label );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
float cptrModeRender::get_cameras_percentage()
{
	float timeline_time = tmlnTimeLine::GetValue();
	//float cameras_percentage = tmlnTimeInOutMgr::GetPercentageComplete(timeline_time);
	float cameras_percentage = 0.0f;
	if (m_NumberOfRenderableCameras > 0)
	{
		cameras_percentage = get_camera_percentage() * (1.0f/(float)m_NumberOfRenderableCameras);
		cameras_percentage += ((float)m_CurrentCameraNumber / (float)m_NumberOfRenderableCameras);
	}
	if (cameras_percentage > 1.0f) cameras_percentage = 1.0f;
	return cameras_percentage;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void cptrModeRender::update_cameras_percentage()
{
	char buffer[64];
	sprintf(buffer, "Camera %d of %d", m_CurrentCameraNumber+1, m_NumberOfRenderableCameras);
	std::string cameras_label(buffer);
	cptrRenderProgressDialogUtil::SetCamerasRenderPercentage( 100.0f*get_cameras_percentage(), cameras_label );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void cptrModeRender::update_scenes_percentage(bool i_bCompleted)
{
	cptrRenderOutputData& data = cptrRenderOutputDataUtil::Data();

	std::string filename;
	filename = itStringUtil::GetStdString( data.m_CurrentSceneFilename.GetValue() );
	char buffer[64];
	sprintf(buffer, "Scene %d of %d", data.m_CurrentSceneNumber.GetValue(), data.m_NumberOfScenes.GetValue() );
	std::string scenes_label(buffer);

	float percentage = 0.0f;
	percentage = ((float)(data.m_CurrentSceneNumber.GetValue()-1) / (float)data.m_NumberOfScenes.GetValue());
	if (percentage < 0.0f) percentage = 0.0f;
	percentage += get_cameras_percentage() * (1.0f / (float)data.m_NumberOfScenes.GetValue());
	cptrRenderProgressDialogUtil::SetScenesRenderPercentage( 100.0f*percentage, scenes_label );
	//if (bCompleted)
	//	percentage = 100.0f;

	//DBG_LOG3( "scene %d of %d (%6.3f)", data.m_CurrentSceneNumber, data.m_NumberOfScenes, percentage );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void cptrModeRender::update_progress_timeleft()
{
	cptrRenderProgressDialogUtil::SetTimeElapsed( appTime::GetTime() - m_fRenderStartTime );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void cptrModeRender::update_progress_percentages(bool i_bCompleted)
{
	update_camera_percentage();
	update_cameras_percentage();
	update_scenes_percentage(i_bCompleted);
	update_progress_timeleft();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void cptrModeRender::reset_renderer_for_camera()
{
	// called after do_titlecard_render
	// tell renderer that we are starting a new camera
	// renderer can draw an initial frame for deltas.

	if (!cptrRenderUtil::GetCaptureQuadrants())
	{
		camCamera *pCamera = camsFollowUtil::GetFollowCamera();
		if (m_pViewer && 
			(rndrPrefsMgr::ActualData((rndrPrefsMgr::rndr_Object)m_RenderPrefsObjectID).m_RendererType == g3dSceneRendererCreate::e_HDR))
		{
			camHDRData hdrOrig;
			pCamera->GetHDRParams(hdrOrig);

			// Render a frame to calibrate the adaptation.
			// This frame will not be captured.
			// We could do this only conditionally if hdrOrig.m_bAdaptiveLuminance is true.
			// However, the same behavior applies to the first frame drawn by this renderer instance.

			float timeline_time = tmlnTimeLine::GetValue();

			camHDRData hdrData = hdrOrig;
			// change any params for calibration here.

			pCamera->SetHDRParams(hdrData);

			m_pViewer->SetCamera(pCamera);
			m_pViewer->Render(timeline_time);
//				m_pViewer->Present();// don't really need to present, since this frame is for HDR exposure calibration only.

			m_pViewer->SetCamera(pCamera);
			m_pViewer->Render(timeline_time);
//				m_pViewer->Present();// don't really need to present, since this frame is for HDR exposure calibration only.

			// restore to original settings
			pCamera->SetHDRParams(hdrOrig);

		}
	}

}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void cptrModeRender::RenderJitteredFrame()
{
	cptrRenderOutputData& data = cptrRenderOutputDataUtil::Data();
	// first time (lazy) init the buffer
	if (m_AccBuf == NULL)
		m_AccBuf = new cptrAccumulationBuffer(data.m_nWidth.GetValue(), data.m_nHeight.GetValue(), data.m_nCaptureSampling.GetValue()*data.m_nCaptureSampling.GetValue());
	else
		m_AccBuf->Clear();

	float timeline_time = tmlnTimeLine::GetValue();
	camCamera *pCamera = camsFollowUtil::GetFollowCamera();

	/////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////
	// draw each jittered sample,
	// and then put the accum buffer back in the backbuffer,
	// to be captured?
	/////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////
	int i;
	for (i = 0; i < data.m_nCaptureSampling.GetValue()*data.m_nCaptureSampling.GetValue(); i++)
	{
		float dx = (cptrRenderUtil::GetSamplePos(i).GetX() - 0.5f)*2.0f/(float)data.m_nWidth.GetValue();
		float dy = (cptrRenderUtil::GetSamplePos(i).GetY() - 0.5f)*2.0f/(float)data.m_nHeight.GetValue();

		pCamera->SetSubViewport(-1+dx, 1+dx, -1+dy, 1+dy);
		m_pViewer->SetCamera(pCamera);
		m_pViewer->Render(timeline_time);
		pCamera->SetSubViewport(-1, 1, -1, 1);

		g2dPixelR8G8B8* currentFrameBuffer = NULL;
		int w,h;
		cptrRenderUtil::CaptureFrame(&currentFrameBuffer, w, h);
		data.m_nWidth.SetValue(w);
		data.m_nHeight.SetValue(h);
		m_AccBuf->Add(currentFrameBuffer);
		delete [] currentFrameBuffer;

		// we can now draw the frame sample into the window.
		m_pViewer->Present();

	}
	m_AccBuf->Capture();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void cptrModeRender::ResetConfigFileName()
{
	fsLocator cfgdir = gfPaths::GetPath(mnmPaths::e_Configs);
	cfgdir.Push( "RenderOutput.cfg" );

	cptrRenderOutputData& data = cptrRenderOutputDataUtil::Data();
	data.m_RenderOutputDataFile.SetValue( cfgdir );

	//std::string dbgfile;
	//fsFileUtil::LocatorToANSIFilename( data.m_RenderOutputDataFile.GetValue(), dbgfile );
	//DBG_LOG1("Reset ConfigFile to (%s)", dbgfile.c_str());
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void cptrModeRender::ReadConfig()
{
	cptrRenderOutputData& data = cptrRenderOutputDataUtil::Data();

	//std::string dbgfile;
	//fsFileUtil::LocatorToANSIFilename( data.m_RenderOutputDataFile.GetValue(), dbgfile );
	//DBG_LOG1("Pre-Read ConfigFile to (%s)", dbgfile.c_str());

	cptrRenderOutputDataUtil::ReadData(data);

	//std::string dbgfile2;
	//fsFileUtil::LocatorToANSIFilename( data.m_RenderOutputDataFile.GetValue(), dbgfile2 );
	//DBG_LOG1("Post-Read ConfigFile to (%s)", dbgfile2.c_str());
}

//----------------------------------------------------------------------------
//	Used for debugging only
//----------------------------------------------------------------------------
void cptrModeRender::Log_Settings()
{
}

