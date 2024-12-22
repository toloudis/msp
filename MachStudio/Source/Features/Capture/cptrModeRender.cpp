//*****************************************************************************
//	cptrModeRender.cpp
//
//		see .hpp
//
//	StudioGPU
//	Copyright(C) 2003-8 - All Rights Reserved
//****************************************************************************
#include "Features/Capture/cptrModeRender.hpp"

#include "Features/Capture/cptrAccumulationBuffer.hpp"
#include "Features/Capture/cptrMBlurMotionSampler.hpp"
#include "Features/Capture/cptrPostRenderUtil.hpp"
#include "Features/Capture/cptrRenderOptionsDialogUtil.hpp"
#include "Features/Capture/cptrRenderProgressDialogUtil.hpp"
#include "Features/Capture/cptrRenderStateUtil.hpp"
#include "Features/Capture/cptrRenderStatsDataUtil.hpp"
#include "Features/Capture/cptrRenderStatsDialogUtil.hpp"
#include "Features/Capture/cptrRenderUtil.hpp"
#include "Features/Capture/cptrWatermarkObject.hpp"

#include "Features/Channels/Markers/chnlMarkerMgr.hpp"
#include "Features/FilmGates/fgtFrameMgr.hpp"
#include "Features/RenderPrefs/rndrPrefsMgr.hpp"
#include "Features/RenderPrefs/rndrPrefsUtil.hpp"
#include "MainApp/mnmApp.hpp"
#include "MainApp/resource.h"
#include "MainApp/wxGUI/wxMainForm.hpp"
#include "Support/cams/camsCameraMgr.hpp"
#include "Support/cams/camsFollowUtil.hpp"
#include "Support/capt/captRenderOutputData.hpp"
#include "Support/capt/captRenderOutputDataUtil.hpp"
#include "Support/capt/captRenderProgressMgr.hpp"
#include "Support/capt/captStereoUtil.hpp"
#include "Support/cmps/cmpsCompassMgr.hpp"
#include "Support/cmps/cmpsSelectMgr.hpp"
#include "Support/fgmt/fgmtHighlight.hpp"
#include "Support/mnm/mnmAppUtil.hpp"
#include "Support/mnm/mnmConstants.hpp"
#include "Support/mnm/mnmDebugInfo.hpp"
#include "Support/mnm/mnmPaths.hpp"
#include "Support/mnm/mnmSecurityMgr.hpp"
#include "Support/mnm/mnmTimeCodeMgr.hpp"
#include "Support/mnm/mnmTimeCodeUtil.hpp"
#include "Support/mtrl/mtrlHighlight.hpp"
#include "Support/mtrl/mtrlScriptObject.hpp"
#include "Support/pfx/pfxPostEffectObject.hpp"
#include "Support/rlyr/rlyrRenderLayerMgr.hpp"
#include "Support/rman/rmanMgr.hpp"
#include "Support/mray/mrayMgr.hpp"
#include "Support/rprf/rprfPrefsObject.hpp"
#include "Support/rprf/rprfPrefsUtil.hpp"
#include "Support/tmln/tmlnTimeInOutMgr.hpp"
#include "Support/tmln/tmlnTimeLine.hpp"
#include "Support/tmln/tmlnTimeUtil.hpp"
#include "Support/xfrm/xfrmTransformMgr.hpp"
#include "Support/vis/visMgr.hpp"

#include <windows.h>
#undef CreateDirectory

//	library
#include "AudioDS/sn/snSoundManager.hpp"
#include "Core/app/appSimTime.hpp"
#include "Core/app/appTime.hpp"
#include "Core/Env/envThread.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/it/itStringUtil.hpp"
#include "Graphics/g2d/g2dExceptionX.hpp"
#include "Graphics/g2d/g2dFontUtil.hpp"
#include "Graphics/g2d/g2dSystem.hpp"
#include "Graphics/g2d/g2dWindow.hpp"
#include "Graphics/g3d/g3dConstants.hpp"
#include "Graphics/g3d/g3dPassBuffers.hpp"
#include "Graphics/g3d/g3dPrimitiveFragmentUtil.hpp"
#include "Graphics/g3d/g3dSceneRenderer.hpp"
#include "Graphics/G3d/g3dThreadControl.hpp"
#include "Graphics/g3d/g3dViewer.hpp"
#include "Graphics/g2d/g2dScreenCaptureUtil.hpp"
#include "Graphics/g2d/g2dImageCreate.hpp"
#include "Input/in/inDeviceMgr.hpp"
#include "Input/in/inVirtualJoystick.hpp"
#include "Tool/api3d/api3dScene.hpp"
#include "Tool/api3d/api3dSubdiv.hpp"
#include "Tool/api3d/api3dTargetRendererMgr.hpp"
#include "Tool/cam3d/cam3dMgr.hpp"
#include "Tool/doc/docSingleDocumentMgr.hpp"
#include "Tool/gpx/gpxProxyMgr.hpp"
#include "Tool/gpx/gpxRenderControl.hpp"
#include "Tool/gui/guiMessageBox.hpp"
#include "Tool/gui/guiSingleDocHandler.hpp"
#include "Tool/gui/guiStatusBarMgr.hpp"
#include "Tool/tma3d/tma3dScreenUtil.hpp"




#include <assert.h>
#include <string>
#include <sstream>

//============================================================================
//============================================================================
#if defined( ENV_USE_THREADS )
	#ifdef BATCH_MODE
	// No multithreaded capture needed for batch mode - because no gui
	const bool c_MULTITHREADED_CAPTURE  = false;
	#else
	const bool c_MULTITHREADED_CAPTURE  = true;
	#endif
#else
const bool c_MULTITHREADED_CAPTURE  = false;
#endif

//============================================================================
//============================================================================
namespace
{
	bool l_bRemovedMtrlHighlight = false;
	bool l_bRemovedFgmtHighlight = false;
	bool l_bRemovedSelectionBox = false;
	int l_ErrorCode = 1;
	float m_camera_percentage = 0.0;
	itString m_camera_label;
	itString m_cameras_label;
	itString m_scenes_label;
	float m_scenes_percentage;

	bool l_bDoneFirstPass = false;
	int l_StartCamIdx = -1;

	captRenderProgressData& m_Data = captRenderProgressMgr::Data();

#ifdef DEMO_VERSION
	cptrWatermarkObject* l_pWatermarkObject = NULL;
#endif

	//------------------------------------------------------------------------
	// remove the surface or material highlights when rendering
	//------------------------------------------------------------------------
	void removeHighlights()
	{
		if (mtrlHighlight::IsHighlightMaterial())
		{
			mtrlHighlight::HighlightMaterial(false);
			l_bRemovedMtrlHighlight = true;
		}
		if (fgmtHighlight::IsHighlightFragment())
		{
			fgmtHighlight::HighlightFragment(false);
			l_bRemovedFgmtHighlight = true;
		}
	}

	//------------------------------------------------------------------------
	// remove the selection box around objects when rendering
	//------------------------------------------------------------------------
	void removeSelectionBox()
	{
		cmpsSelectMgr::SetRenderable(false);
		l_bRemovedSelectionBox = true;
	}

	//------------------------------------------------------------------------
	// if there were previous highlights, restore them after rendering is done
	//------------------------------------------------------------------------
	void restoreHighlights()
	{
		if (l_bRemovedMtrlHighlight)
		{
			mtrlHighlight::HighlightMaterial(true);
			l_bRemovedMtrlHighlight = false;
		}

		if (l_bRemovedFgmtHighlight)
		{
			fgmtHighlight::HighlightFragment(true);
			l_bRemovedFgmtHighlight = false;
		}
	}

	//------------------------------------------------------------------------
	// if the selection box was visible before rendering, restore it
	//------------------------------------------------------------------------
	void restoreSelectionBox()
	{
		if (l_bRemovedSelectionBox)
		{
			cmpsSelectMgr::SetRenderable(true);
			l_bRemovedSelectionBox = false;
		}
	}

}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool cptrModeRender::sm_bUseAppWindow = false;
bool cptrModeRender::sm_bThreadingEnabled = true; // default needs to match PrefsData constructor

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
//------------------------------------------------------------------------
// Enable capture multithreading from preferences
//------------------------------------------------------------------------
void cptrModeRender::SetThreadingEnabled(bool i_bThreading)
{
	sm_bThreadingEnabled = i_bThreading;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
cptrModeRender::cptrModeRender()
:	m_bInitialized( false ),
	m_bCaptureStarted( false ),
	m_bCaptureFinished( false ),
	m_bSwitchToCameraRenderRange( true ),
	m_bAborted( false ),
	m_bAppExit( false ),
	m_bFirstRenderFrame( true ),
	m_bSkipCaptureOptions( false ),
	m_fSimTimeInc( 0.0f ),
	m_fLeadTimeDelta( 0.0f ),
	m_fEndTime(captRenderOutputData::c_InitEndTime),
	m_CurrentCameraIndex( 0 ),
	m_LastCameraIndex( 0 ),
	m_pSystem( NULL ),
	m_pWindow( NULL ),
	m_bAudioMuteState(false),
	m_bPaused(false),
	m_CurrentCameraNumber(0),
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
	captRenderOutputData& data = captRenderOutputDataUtil::Data();
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
	captRenderOutputData& data = captRenderOutputDataUtil::Data();

	if ( !m_bInitialized )
	{
		m_bInitialized = true;
		m_OldSubdivLevel = -1;

		//	set the mute state
		m_bAudioMuteState = snSoundManager::IsMuted();
		snSoundManager::Mute( true );
	}

	this->SetTerminateCondition( appMode::e_Continue );

	//	if there is no filename, then make them save it.
	//
	if (docSingleDocumentMgr::GetFilename().GetNumNames() == 0)
	{
		if (data.m_bBatchMode.GetValue()
			&& data.m_bBatchSkipDialog.GetValue())
		{
			// show the message on stats log
			itString s("Error: No output file specified. Skip Scene ");
			s += itString(data.m_CurrentSceneFilename.GetString().c_str());
			cptrRenderStatsDialogUtil::AddStatsMessage(s);
			cptrRenderStatsDialogUtil::Newline();
		}
		else
		{
			//	only perform if the security is present
			if (!mnmSecurityMgr::CheckSecurity3())
				return;
			guiSingleDocHandler::Save();
			mnmAppUtil::UpdateTitleBar( false );
		}
	}

	//	if they cancelled the save, don't let them continue.
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
		if (data.m_bBatchMode.GetValue()
			&& data.m_bBatchSkipDialog.GetValue())
		{
			//	batch mode -- abort current scene and continue
			DBG_ERROR("Cannot render the scene " << data.m_CurrentSceneFilename.GetString().c_str() << ".  There are no cameras created." );

			// show the message on stats log
			itString s("Cannot render the scene ");
			s += itString(data.m_CurrentSceneFilename.GetString().c_str());
			s += itString(".  There are no cameras created.");
			cptrRenderStatsDialogUtil::AddStatsMessage(s);
			cptrRenderStatsDialogUtil::Newline();
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
		captRenderOutputData& data = captRenderOutputDataUtil::Data();

		//	fill-in the data
		//
		//data.m_bKeepFrameOpenAfterRender = false;
		//data.m_fMarkerInTime = 0.0f;
		//data.m_fMarkerOutTime = -1.0f;

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

#ifdef DEMO_VERSION
	//	load a watermark texture
	l_pWatermarkObject = new cptrWatermarkObject();

	//char numberstr[64];
	//sprintf( numberstr,"%d", IDB_PNG1 );
	std::ostringstream oss;
	oss.setf(0, std::ios::floatfield);
	oss.setf(std::ios::fixed, std::ios::floatfield);
	oss << IDB_PNG1;
	std::string numberstr(oss.str());
	

	itString it_number(numberstr.c_str());
	fsLocator resource_name;
	resource_name.Push(it_number);
	//resource_name.Push(itString(L"C:"));
	//resource_name.Push(itString(L"Projects"));
	//resource_name.Push(itString(L"SourceCode"));
	//resource_name.Push(itString(L"MachStudio"));
	//resource_name.Push(itString(L"Source"));
	//resource_name.Push(itString(L"MainApp"));
	//resource_name.Push(itString(L"MSP_icon_256.png"));

	l_pWatermarkObject->Initialize( resource_name );

	//pWO->DeInitialize();
	//delete pWO;
#endif
}

//----------------------------------------------------------------------------
//	The object should clean up things that are not needed while the
//	mode is not running in the DeInitialize function.
//----------------------------------------------------------------------------
//virtual
void cptrModeRender::DeInitialize()
{
#ifdef DEMO_VERSION
	//	delete a watermark texture
	if (l_pWatermarkObject != NULL)
	{
		l_pWatermarkObject->DeInitialize();
		delete l_pWatermarkObject;
	}
	l_pWatermarkObject = NULL;
#endif

	// Clean up our sub window
	if (m_pSystem && m_bCaptureStarted)
	{
		mnmApp::EnableRender(true);
		if (!sm_bUseAppWindow)
			m_pSystem->DestroyWindow(m_pWindow);
		m_pWindow	= NULL;
	}

	if ( m_bInitialized )
	{
		snSoundManager::Mute( m_bAudioMuteState );
	}

	//	Log the render time
	if (!m_bAborted)
	{
		DBG_LOG("Render Time = " << (appTime::GetTime() - m_fRenderStartTime ));
		l_ErrorCode = 0;
	}

	//	hide the dialog and write out the capture preferences
	//
	cptrRenderProgressDialogUtil::Hide();

	//	reset the filename for this mode, and read in the normal capture options
	//
	cptrModeRender::ResetConfigFileName();
	captRenderOutputData& data = captRenderOutputDataUtil::Data();
	captRenderOutputDataUtil::ReadData(data);

	data.m_bKeepFrameOpenAfterRender = false;

	m_bInitialized		= false;
	m_bCaptureStarted	= false;
	mnmAppUtil::SetErrorCode(l_ErrorCode);
}

//----------------------------------------------------------------------------
//	Quit level
//----------------------------------------------------------------------------
void cptrModeRender::ExitMode()
{
	DBG_TRACE("ExitMode: top");
	cptrRenderStatsDataUtil::WriteRenderStats( cptrRenderStatsDataUtil::Data() );
	DBG_TRACE("ExitMode: wrote render stats");

	//	reset the lod level
	if ( m_OldSubdivLevel != -1 )
	{
		api3dSubdiv::SetSubdivLevel( m_OldSubdivLevel );
	}

	captRenderOutputDataUtil::RestoreShadows();
	//restore time
	restoreCurrentTime();
	// terminate this mode
	this->SetTerminateCondition(appMode::e_TerminateAndRemove);
	
	//check for Complete program termination
	if (m_bAppExit)
	{
		#ifdef USE_WXWIDGETS
			wxMainForm::Exit();
		#endif
	}
	DBG_TRACE("ExitMode: bottom");
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

	// If there is already a render thread active, have to 
	// wait until it finishes
	if (!g3dThreadControl::IsRenderThreadActive())
	{
		// Check to see if the previous render created an error state.
		// If so, report the error to the user.
		check_for_render_thread_error();

		//check to see if the dialog needs to be refreshed
		rprfPrefsUtil::UpdateDialog();
		
		//	update the state
		//
		UpdateState();
	}
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
	gpxRenderControl::ConfirmSingleThread();

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
	//DBG_TRACE("state: Begin RenderConfig");
	rlyrRenderLayerMgr::Update();

	//captRenderOutputData& data = captRenderOutputDataUtil::Data();
	//DBG_LOG("BeginStateRenderConfig");
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
	cptrRenderOptionsDialogUtil::Show( captRenderOutputDataUtil::Data() );
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
	//DBG_LOG("END STATE RENDER CONFIG");
	//captRenderOutputData& data = captRenderOutputDataUtil::Data();
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
	//DBG_TRACE("state: Begin StateInitCaptures");
	l_bDoneFirstPass = false;
	m_CaptureRenderThread.SetDoneFirstPass(false);
	m_bAborted = false;
	m_OldSubdivLevel = -1;

	captRenderOutputData& data = captRenderOutputDataUtil::Data();

	//	For some reason it is possible for this to become 0.
	//	TODO - track down when.  It had to do with going between capture + single frame capture.
	//if (data.m_fCaptureFPS.GetValue() < 1.0f)
		data.m_fCaptureFPS.SetValue( tmlnTimeLine::GetFPS() );
	//tmlnTimeLine::SetFPS(data.m_fCaptureFPS.GetValue());
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
		DBG_TRACE("OnStateInitCaptures: exitting");

		// this occurs here is the user hit the "X" on the capture dialog (for instance)
		//
		m_bAborted = true;
		if (cptrRenderUtil::GetPSflag())
			cptrRenderUtil::SetPSflag(false);
		GotoState( m_StateDeInitCaptures );
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void cptrModeRender::EndStateInitCaptures()
{
	DBG_TRACE("EndStateInitCaptures: top");
	if (m_bAborted)
		return;

	//check to see if there is at least one active render layer
	if (!rlyrRenderLayerMgr::CanRender())
	{
		guiMessageBox::Show("Cannot render this scene yet.  There are no active render layers", "Error", guiMessageBox::e_OKOnly);
		m_bAborted = true;
		GotoState( m_StateWaitToEndMode );
		return;
	}
	rlyrRenderLayerMgr::ClearAllRenderDataList();
	captRenderOutputData& data = captRenderOutputDataUtil::Data();

	// make sure sampling settings are within bounds that we can handle:
	static const int maxSize = 16384;
	int width	= data.m_nWidth.GetValue() * (data.m_bJitteredSampling.GetValue() ? 1 : data.m_nCaptureSampling.GetValue());
	int height	= data.m_nHeight.GetValue() * (data.m_bJitteredSampling.GetValue() ? 1 : data.m_nCaptureSampling.GetValue());
	if ((width > maxSize) || (height > maxSize))
	{
		if (data.m_bBatchMode.GetValue()
			&& data.m_bBatchSkipDialog.GetValue())
		{
			//	batch mode -- abort current scene and continue
			DBG_ERROR("Cannot render the scene " << data.m_CurrentSceneFilename.GetString().c_str() << ".  Sampling settings too high - please use jitter or reduce the number of samples." );

			// show the message on stats log
			itString s("Cannot render the scene ");
			s += itString(data.m_CurrentSceneFilename.GetString().c_str());
			s += itString(".  Sampling settings too high - please use jitter or reduce the number of samples.");
			cptrRenderStatsDialogUtil::AddStatsMessage(s);
			cptrRenderStatsDialogUtil::Newline();
		}
		else
		{
			//	batch mode -- abort current scene and continue
			DBG_ERROR("Cannot render this scene yet.  Sampling settings too high - please use jitter or reduce the number of samples." );
			guiMessageBox::Show("Cannot render this scene yet.  Sampling settings too high - please use jitter or reduce the number of samples.", "Error", guiMessageBox::e_OKOnly);
		}

		this->m_bAborted = true;
		GotoState( m_StateWaitToEndMode );
		return;
	}

	//	clear the statistics data
	//
	cptrRenderStatsDataUtil::ClearAllData();
	//get the current time
	getCurrentTime();
	//get the initial visible states of objects in the scene editor
	rlyrRenderLayerMgr::GetObjectsEditorVisibility();

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

	//check if the user has the name of the render layer in the file or directory
	if (!data.m_bUseLayerNameAsDirectory.GetValue() && !data.m_bUseLayerNameInFilename.GetValue())
	{
		int result = guiMessageBox::Show("Without the render layer name in the file or directory, you run the risk of overwriting the output files with each render.  Continue?", "Overwriting Output File Risk", guiMessageBox::e_YesNo);

		switch(result)
		{	
		case guiMessageBox::e_No:
			m_bAborted = true;
			cptrRenderUtil::SetPSflag(false);
			GotoState( m_StateDeInitCaptures );
			return;
		case guiMessageBox::e_Yes:
			break;
		default:
			break;
		}
	}
	//check if the user has the name of the render pass in the file or directory
	if (!data.m_bUseRenderPassAsDirectory.GetValue() && !data.m_bUseRenderPassInFilename.GetValue())
	{
		int result = guiMessageBox::Show("Without the render pass name in the file or directory, you run the risk of overwriting the output files with each render.  Continue?", "Overwriting Output File Risk", guiMessageBox::e_YesNo);

		switch(result)
		{	
		case guiMessageBox::e_No:
			m_bAborted = true;
			cptrRenderUtil::SetPSflag(false);
			GotoState( m_StateDeInitCaptures );
			return;
		case guiMessageBox::e_Yes:
			break;
		default:
			break;
		}
	}

	//DBG_LOG("Cameras");
	//int count = data.m_Cameras.GetNumberOfItems();
	//for (int k=0; k < count; ++k)
	//{
	//	DBG_LOG3("%02d - %s (%s)", k, data.m_Cameras.GetValueText(k).c_str(), (data.m_Cameras.GetValueFlag(k) ? "true":"false") );
	//}
	//DBG_LOG("CameraList");
	//count = data.m_CameraList.size();
	//for (int k=0; k < data.m_CameraList.size(); ++k)
	//{
	//	DBG_LOG3("%02d - %s (%s)", k, data.m_CameraList[k].m_CameraName.GetValue().c_str(), (data.m_CameraList[k].m_bCapture.GetValue() ? "true":"false") );
	//}

	if ( data.m_bCaptureAllCameras.GetValue() )
	{		
		int startcam_index = 0;
		//	if there has been a saved render state, then start the time
		//
		if (data.m_RenderPosTime != captRenderOutputData::c_InitRenderPosTime)
		{
			nameString camname( itStringUtil::GetStdString(data.m_RenderPosCamera.GetValue()) );
			startcam_index = camsCameraMgr::GetIndexForName(camname);
			if (startcam_index == -1)
				startcam_index = 0;

			init_layer_indices();
		}
		else
		{
			//get first active layer
			m_CurrentLayerIndex = -1;
			m_CurrentLayerIndex = this->get_next_layer_index();
		}
		m_CurrentCameraIndex = startcam_index;
		l_StartCamIdx = startcam_index;
		
		
		// Even with Director's Cuts, make this checkbox means only the real *cameras*
		m_LastCameraIndex = camsCameraMgr::GetNumCameras();
		m_LastLayerIndex = rlyrRenderLayerMgr::GetNumRenderLayers();
	}
	else
	{
		// Here, include the director's cuts in the camera count
		//m_LastCameraIndex		= camsCameraMgr::GetNumCameras();
		m_LastCameraIndex		= camsFollowUtil::GetTotalCount();

		//	if there has been a saved render state, then start the time
		//
		if (data.m_RenderPosTime != captRenderOutputData::c_InitRenderPosTime)
		{
			nameString camname( itStringUtil::GetStdString(data.m_RenderPosCamera.GetValue()) );
			int startcam_index = camsCameraMgr::GetIndexForName(camname);
			if (startcam_index == -1)
				startcam_index = 0;
			m_CurrentCameraIndex	= startcam_index;

			init_layer_indices();
		}
		else
		{
			//get the first active camera
			m_CurrentCameraIndex	= -1;
			m_CurrentCameraIndex	= this->get_next_camera_index();

			//get the first active layer for that camera
			m_LastLayerIndex = rlyrRenderLayerMgr::GetNumRenderLayers();
			m_CurrentLayerIndex = -1;
			m_CurrentLayerIndex = this->get_next_layer_index();
		}
	}

	if (m_CurrentLayerIndex > m_LastLayerIndex)
	{
		GotoState(m_StateEndCapture);
		return;
	}

	m_CurrentCameraNumber = 0;
	calculate_number_of_renderable_cameras();

	//visMgr::ShowIcons(false);

	if (!rlyrRenderLayerMgr::GetLayerActualData(m_CurrentLayerIndex).m_bRenderMatte &&
		!data.m_bUseLayerVisibility.GetValue())
	{
		visMgr::ConfirmGeometryVisible();
	}

	cmpsCompassMgr::SetRenderable( false );	// set all compasses invisible
	fgtFrameMgr::Show(false);

	//DBG_LOG2("first camera to render (index=%d) out of %d cameras", m_CurrentCameraIndex, m_NumberOfRenderableCameras);

	//	set the start + end time based on marker in/out
	//
	//if (data.m_bUseMarkerTimes.GetValue())
	//{
	//	//if ( !data.m_bBatchMode )
	//	{
	//		data.m_fMarkerInTime.SetValue( chnlMarkerMgr::GetMarkerInTime() );
	//		data.m_fMarkerOutTime.SetValue( chnlMarkerMgr::GetMarkerOutTime() );
	//	}

	//	data.m_fStartTime.SetValue( data.m_fMarkerInTime.GetValue() );
	//	data.m_fEndTime.SetValue( data.m_fMarkerOutTime.GetValue() );

	//	// fix a bug where the user creates a marker then shortens the end time of the scene to before marker
	//	if (data.m_fEndTime.GetValue() > tmlnTimeLine::GetMaximum()) 
	//		data.m_fEndTime.SetValue( tmlnTimeLine::GetMaximum() );
	//}

	//	if there has been a saved render state, then start the time
	//
	if (data.m_RenderPosTime.GetValue() != captRenderOutputData::c_InitRenderPosTime)
	{
		data.m_fStartTime.SetValue( data.m_RenderPosTime.GetValue() );
		data.m_RenderPosTime.SetValue( captRenderOutputData::c_InitRenderPosTime );	// reset it so it doesn't trigger anymore
	}

	//DBG_LOG2("Capture Start(%6.3f) End(%6.3f)", data.m_fStartTime, data.m_fEndTime );

	//	build the in/out lists for this scene
	tmlnTimeInOutMgr::ClearLists();
	tmlnTimeInOutMgr::BuildTimeInOutLists();

	if (data.m_RenderFrameRange.GetValue() == captRenderOutputData::eSpecifyRange)
	{
		// If the render frame option is "specify", then we only have to check the ranges
		// against the timeline minimum and maximum.
		
		// Start time cannot be before timeline minimum (is this necessary?)
		if (data.m_fStartTime.GetValue() < tmlnTimeLine::GetMinimum())
		{
			data.m_fStartTime = tmlnTimeLine::GetMinimum();
		}

		// End time less than start time means capture to end of timeline...
		if ( (data.m_fEndTime.GetValue() < data.m_fStartTime.GetValue())
			 || (data.m_fEndTime.GetValue() > tmlnTimeLine::GetMaximum()) )
		{
			cptrRenderUtil::SetCaptureTimeMax( tmlnTimeLine::GetMaximum() );
			m_fEndTime = tmlnTimeLine::GetMaximum();

			//DBG_LOG2( "ModeRender Capture Max- = %6.3f %d", capturemax, (int)capturemax );
		}
		else 
		{
			cptrRenderUtil::SetCaptureTimeMax( data.m_fEndTime.GetValue() );
			m_fEndTime = data.m_fEndTime.GetValue();

			//DBG_LOG2( "ModeRender Capture Max = %6.3f %d", capturemax, (int)capturemax );
		}
	}
	else
	{
		// Render frame range based on capture drivers or on timeline itself
		data.m_fStartTime.SetValue( tmlnTimeLine::GetMinimum() );
		data.m_fEndTime.SetValue( tmlnTimeLine::GetMaximum() );

		//TIME - These weren't in earlier code, but seem to be needed, right?
		cptrRenderUtil::SetCaptureTimeMax( data.m_fEndTime.GetValue() );
		m_fEndTime = data.m_fEndTime.GetValue();
	}


	//	show the Capture Status dialog
	//
	if (data.m_bShowRenderProgressDialog.GetValue())
	{
		cptrRenderProgressDialogUtil::Show();
		cptrRenderProgressDialogUtil::DisablePauseButton();
	}

	//DBG_LOG2( "cam indexes %d -> %d", m_CurrentCameraIndex, m_LastCameraIndex );

	//captRenderOutputData& data = captRenderOutputDataUtil::Data();

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

	cptrRenderUtil::Initialize();

	//Log_Settings();

	//	set the start time if not in batch
	if ( !(data.m_bBatchMode.GetValue()) )
		SetRenderStartTime(appTime::GetTime());
}

//----------------------------------------------------------------------------
//	BeginCapture
//----------------------------------------------------------------------------
void cptrModeRender::BeginStateBeginCapture()
{
	l_ErrorCode = 1;
	//DBG_TRACE("Begin BeginCapture");
	//remove highlights and compasses
	removeHighlights();
	removeSelectionBox();
	
	m_bCaptureStarted = false;
	m_bCaptureFinished = false;
	m_bSwitchToCameraRenderRange = true;

	//	Reset the output directory
	captRenderOutputData& data = captRenderOutputDataUtil::Data();
	if (data.m_OutputDirectoryRoot.GetValue().GetNumNames() == 0)
	{
		data.m_OutputDirectoryRoot.SetValue( gfPaths::GetPath( mnmPaths::e_SaveFootage ) );		// this resets the output dir
	}
	data.m_OutputDirectory.SetValue( data.m_OutputDirectoryRoot.GetValue() );
	DBG_LOG( "Capturing footage to " << data.m_OutputDirectory.GetValue() );

	//captRenderOutputDataUtil::Debug();

	camsFollowUtil::SetFollowIndex( m_CurrentCameraIndex );

	// TODO make an interest that gets called so we don't have to have
	//	system specific stuff in here.
	//

	//	set the lod level
	m_OldSubdivLevel = api3dSubdiv::GetSubdivLevel();
	// Subdiv smoothing is toggle now
	//api3dSubdiv::SetSubdivLevel( data.m_nSubdivLevel.GetValue() );
	api3dSubdiv::SetSubdivLevel( data.m_bSmoothing.GetValue() ? 1 : 0 );

	//
//	m_bParticlesActive = prtclObjectMgr::Paused();

	// TODO [rjk] currently this assumes there will only be 26 capture drivers for a single camera.
	//	currently they have 8 at most.
	//
	m_cCurrentRenderDriverLetter = 'A';

	//	update the render status dialog
	//
	update_progress_percentages(false);
	update_render_layer_progress();

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
			// stop any render threads from a previous mode,
			// this mode controls the rendering without interactive threading
			gpxRenderControl::ConfirmSingleThread();

			m_bCaptureStarted	= true;
			m_bFirstRenderFrame	= true;
			m_CaptureRenderThread.ResetRenderCaptureIteration();

			//tmlnTimeLine::SetValue( tmlnTimeLine::GetMinimum() );

			// This has to go after the timeline is updated
			// and before the frame is captured
			modeModeTime::Think();
			xfrmTransformMgr::UpdateTransforms();
			gpxProxyMgr::Update();
			api3dScene::Think( tmlnTimeLine::GetTimeInSeconds() );

			captRenderOutputData& data = captRenderOutputDataUtil::Data();

			// Create sub window for capturing
			//
			if (m_pSystem != NULL)
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
							m_pWindow = m_pSystem->CreateSubWindow(width, height, 300, 10);
							//m_pWindow = m_pSystem->CreateSubFullScreen(width, height, 32 );
							m_pWindow->SetTitle(itString(L"Render"));
						}
						catch ( const envExceptionX& i_Ex )
						{
							std::string msg = "Error creating capture window. Aborting Capture, " + i_Ex.GetErrorMessage();
							DBG_ERROR(msg);
							if (data.m_bBatchMode.GetValue()
								&& data.m_bBatchSkipDialog.GetValue())
							{
								cptrRenderStatsDialogUtil::AddStatsMessage(itString(msg.c_str()));
								cptrRenderStatsDialogUtil::Newline();
							}
							else
							{
								guiMessageBox::Show(msg.c_str(), "Capture Error", guiMessageBox::e_OKOnly);
							}
							m_bAborted = true;
							m_bCaptureStarted = false;
							m_pWindow = NULL;
							GotoState( m_StateEndCapture );
							return;
						}
					}

					mnmApp::EnableRender(false);
				}

				else if ( m_pWindow != NULL )
				{
					mnmApp::EnableRender(false);
				}

				m_CaptureRenderThread.SetWindow(m_pWindow);
				cptrRenderUtil::SetCaptureWindow(m_pWindow);

				if ( data.m_bDisplayTimeCode.GetValue() )
				{
					mnmTimeCodeUtil::SetWindow( this->m_pWindow );
					mnmTimeCodeMgr::Initialize();
					mnmTimeCodeMgr::SetShowTimeCode( true );
					mnmTimeCodeMgr::SetFrameRate( tmlnTimeLine::GetFPS() );
				}
			}

			itString cam_name;
			nameString theName;
			std::string cam_desc;
			camsFollowUtil::GetCurrentCameraName(theName);
			camsFollowUtil::GetCurrentCameraDescription(cam_desc);
			cam_name = theName.GetString().c_str();
			data.m_RenderPosCamera.SetValue( cam_name );			// for saving render position

			itString layer_name;
			nameString layer_desc = rlyrRenderLayerMgr::GetLayerName( m_CurrentLayerIndex );
			layer_name = layer_desc.GetString().c_str();
			data.m_RenderPosLayer.SetValue( layer_name );			// for saving render position

			// stats: start the camera
			if ( m_cCurrentRenderDriverLetter == 'A' )
			{
				cptrRenderStatsDataUtil::StartCamera( theName.GetString(), cam_desc );
				cptrRenderStatsDataUtil::StartRenderLayer( layer_desc.GetString() );
				cptrRenderStatsDialogUtil::UpdateStatsDialog();
			}

			//	Append letter on the end of the filename if the flag is set.
			//
			if (   (data.m_bUseRenderDriversAsUniqueCameras.GetValue())
				|| (data.m_bMoviePerDriver.GetValue()))
			{
				cam_name += m_cCurrentRenderDriverLetter;

				//	stats: start the render block
				//cptrRenderStatsDataUtil::StartCameraRenderBlock( itStringUtil::GetStdString(cam_name) );
			}

			if ( data.m_bUseCameraNameInFilename.GetValue() )
			{
				captRenderOutputDataUtil::SetCurrentCamera( cam_name );
			}

			if ( data.m_bUseLayerNameInFilename.GetValue() || data.m_bUseLayerNameAsDirectory.GetValue() )
			{
				captRenderOutputDataUtil::SetCurrentLayer( layer_name );
			}

			if ( data.m_bUseRenderPassInFilename.GetValue() || data.m_bUseRenderPassAsDirectory.GetValue() )
			{
				// TODO set this once the passes get pre-defined.  (also uncomment the read/write)
				//captRenderOutputDataUtil::SetCurrentRenderPass( render_pass );
			}

			//	Get the in/out times for this camera
			m_CurrentInOutList.Clear();
			//if ( data.m_bUseCaptureDrivers.GetValue() )
			if ( data.m_RenderFrameRange.GetValue() == captRenderOutputData::eCaptureDrivers )
			{
				tmlnTimeInOutMgr::GetDataList( theName.GetString(), m_CurrentInOutList );
			}
			m_CurrentInOutList.SetCompletedFlag();

			build_directory();
			if (m_bAborted)
			{
				//	if the user didn't have permission to build the directory, need to abort
				GotoState( m_StateEndCapture );
				return;
			}

			cptrRenderUtil::BeginCapture();

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
	rlyrRenderLayerMgr::ApplyLayerPrefs(m_CurrentLayerIndex);
}

//----------------------------------------------------------------------------
//virtual
void cptrModeRender::EndStateBeginCapture()
{
	captRenderOutputData& data = captRenderOutputDataUtil::Data();
	itString scene_label;
	scene_label += itString("Scene - ");
	scene_label += data.m_CurrentSceneFilename.GetValue();
	cptrRenderProgressDialogUtil::SetSceneLabel( scene_label );
	cptrRenderProgressDialogUtil::Raise();

	//DBG_LOG("Begin Capture (end) " << (data.m_bJitteredSampling.GetValue() ? "true":"false") );
}

//----------------------------------------------------------------------------
//	LeadIn
//----------------------------------------------------------------------------
void cptrModeRender::BeginStateLeadIn()
{
	//DBG_TRACE("Begin LeadIn");

	captRenderOutputData& data = captRenderOutputDataUtil::Data();
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
	xfrmTransformMgr::UpdateTransforms();
	gpxProxyMgr::Update();
	api3dScene::Think( tmlnTimeLine::GetTimeInSeconds() );

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
	//DBG_TRACE("Begin Capture");

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
		maTime timeline_frame = cptrRenderUtil::UpdateSimTime();

		set_timeline(timeline_frame);

		captRenderOutputData& data = captRenderOutputDataUtil::Data();

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
			maTime nexttime = m_CurrentInOutList.GetNextCaptureTime( tmlnTimeLine::GetValue() );
			if (   (nexttime == maTime::FromSeconds(-1.0f))	//TIME - magic number "-1" as seconds
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
				{
					m_cCurrentRenderDriverLetter++;
					m_CaptureRenderThread.ResetRenderCaptureIteration();
				}
				m_bSwitchToCameraRenderRange = true;
			}
		}

		if ( m_bSwitchToCameraRenderRange )
		{
			//	set a new directory based on the cam name + letter
			build_directory();

			int start_layer_index = rlyrRenderLayerMgr::GetNextLayerIndex( -1 );
			int loop = rlyrRenderLayerMgr::GetNumRenderLayers();
			// i is the render layer.
			// j is how many times we looped.
			for (int i = start_layer_index, j = 0; 
				i != -1 && j < loop; 
				i = rlyrRenderLayerMgr::GetNextLayerIndex(i), j++)
			{
				itString layer_name;
				nameString layer_desc = rlyrRenderLayerMgr::GetLayerName( i );
				layer_name = layer_desc.GetString().c_str();
				data.m_RenderPosLayer.SetValue( layer_name );			// for saving render position
				captRenderOutputDataUtil::SetCurrentLayer( layer_name );

				// loop over all enabled render passes, if render pass name is in directory.
				int loopPasses = 1;
				std::vector<rlyrPassesObject::ePassType> thePasses;
				rlyrPassesObject* passesObject = rlyrRenderLayerMgr::GetLayerRenderPasses(layer_desc);
				passesObject->CollectPasses(thePasses);
				loopPasses = thePasses.size();
				pfxPostEffectObject* pfxObject = rlyrRenderLayerMgr::GetLayerRenderPfx(layer_desc);
				if (pfxObject)
					pfxObject->ApplyPostEffect();

				for (int k = 0; k < loopPasses; k++)
				{
					rlyrPassesObject::ePassType passType = thePasses[k];
					itString pass_name = captRenderOutputDataUtil::GetRenderPassName(passType);
					captRenderOutputDataUtil::SetCurrentRenderPass( pass_name );

					// set up folder name in data util
					captRenderOutputDataUtil::GenerateDirectoryName();

					do_titlecard_render();
				}
			}

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
	xfrmTransformMgr::UpdateTransforms();
	gpxProxyMgr::Update();
	api3dScene::Think( tmlnTimeLine::GetTimeInSeconds() );

	mnmTimeCodeMgr::Update();

	if (bStartingNewCamera)
		reset_renderer_for_camera();

	//	update the progress dialog
	update_progress_percentages(false);

	//
	maTime start_time, end_time;
	bool bCapture = (m_bUseOnlyValidCaptureTimes ? 
						m_CurrentInOutList.GetTimesIfWithin( tmlnTimeLine::GetValue(), start_time, end_time, true ) 
						: true ); // true for return true if no drivers
	if (bCapture)
	{
		//	set the render window title
		//
		//char window_title[256];
		std::string timestring;
		tmlnTimeUtil::GetTimeString( tmlnTimeLine::GetValue(), timestring );
		//sprintf( window_title, "Render at %s", timestring.c_str() );
		std::ostringstream oss;
		oss<<"Render at "<<timestring;
		std::string window_title(oss.str());
		m_pWindow->SetTitle( itString(window_title.c_str()) );

		//	set a time code anytime we have a new camera
		//
		captRenderOutputData& data = captRenderOutputDataUtil::Data();
		if (bStartingNewCamera)
		{

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
		if (cptrRenderUtil::ReadyToCapture())
			try_render_capture();

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

	//	See if the user closed the window with the "X"
	if (!m_pWindow->IsOpen())
	{
		m_bAborted = true;
	}

	//	if the render was aborted, handle it.
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
	//DBG_TRACE("Begin LeadOut");

	captRenderOutputData& data = captRenderOutputDataUtil::Data();
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
	xfrmTransformMgr::UpdateTransforms();
	gpxProxyMgr::Update();
	api3dScene::Think( tmlnTimeLine::GetTimeInSeconds() );
	
	try_render_capture();

	m_fLeadTimeDelta -= m_fSimTimeInc;

	//	See if the user closed the window with the "X"
	if (!m_pWindow->IsOpen())
	{
		m_bAborted = true;
	}

	//	if the rendering was aborted, handle it
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
	DBG_TRACE("BeginStateEndCapture: top");

	//restore highlights and selection if previously on
	restoreHighlights();
	restoreSelectionBox();

	cptrRenderUtil::EndCapture();
	//rlyrRenderLayerMgr::RestoreEditorVisibility();
	rlyrRenderLayerMgr::RestoreActiveInRenderLayer();
	// ASSUME we will not try to render the m_pViewer after cptrRenderUtil::EndCapture
	// otherwise, we need to clean up the m_pViewer's target renderers, since EndCapture
	// may have invalidated them.
	//tmlnTimeLine::SetValue( tmlnTimeLine::GetMinimum() );

	// Frees motion sampler and accumulation buffer
	m_CaptureRenderThread.FreeInternalBuffers();

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
	DBG_TRACE("BeginStateDeInitCaptures: top");
	// Frees motion sampler and accumulation buffer
	m_CaptureRenderThread.FreeInternalBuffers();
	visMgr::ShowIcons(visMgr::IsShowIcons()?true:false);
	mnmTimeCodeMgr::DeInitialize();

	if ( m_bAborted )
	{
		ResetRendermanCapture();
		ResetMRayCapture();
		g3dPassBuffers::SetDoingFileRefl(false);
		ExitMode();
		return;
	}

	//tmlnTimeLine::SetValue( 0 );
	captRenderOutputData& data = captRenderOutputDataUtil::Data();
	// get the next layer in the list if there is one
	m_CurrentLayerIndex = get_next_layer_index();

	//	get the next camera in the list if there is one
	//
	m_CurrentCameraIndex = get_next_camera_index();
	update_progress_percentages(true);
	if ( m_CurrentCameraIndex >= m_LastCameraIndex )
	{
		camCamera *pCamera = camsFollowUtil::GetFollowCamera();
		if ( !l_bDoneFirstPass && pCamera->GetStereoType() == STEREO_DUALCAM )
		{
			l_bDoneFirstPass = true;
			m_CaptureRenderThread.SetDoneFirstPass(true);
			m_CurrentLayerIndex = -1;
			m_CurrentLayerIndex = get_next_layer_index();
			m_CurrentCameraIndex	= -1;
			m_CurrentCameraIndex	= this->get_next_camera_index();
			GotoState( m_StateBeginCapture );
			return;
		}

		//	if not in batch mode then call the post capture functionality
		//
		if ( !data.m_bBatchMode.GetValue() )
		{
			restoreCurrentTime();
			//	stats: end the scene
			cptrRenderStatsDataUtil::EndScene();
			cptrRenderStatsDialogUtil::UpdateStatsDialog();
			//	if finished the scene (not in batch) then delete the saved render position file.
			//
			if (data.m_RenderPosSaveFile.GetValue().GetNumNames() > 0)
			{
				//	if single frame, don't delete RP
				if (!data.m_bKeepFrameOpenAfterRender.GetValue())
					cptrRenderStateUtil::DeleteRenderPosition();
			}
		}

		GotoState(m_StatePostCapture);
	}
	else
	{
		//if new camera, restart layer rendering
		m_CurrentLayerIndex = -1;
		m_CurrentLayerIndex = get_next_layer_index();
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
	DBG_TRACE("BeginStatePostCapture: top");

	captRenderOutputData& data = captRenderOutputDataUtil::Data();
	if ( !data.m_bBatchMode.GetValue() )
	{
		//	do post capture things
		cptrPostRenderUtil::Execute();
	}
}

//----------------------------------------------------------------------------
void cptrModeRender::OnStatePostCapture()
{
	DBG_TRACE("OnStatePostCapture: top");
	//	decide where to go
	captRenderOutputData& data = captRenderOutputDataUtil::Data();

	MRayPostCapture();
	RenderManPostCapture();
	g3dPassBuffers::SetDoingFileRefl(false);

	if (data.m_bKeepFrameOpenAfterRender.GetValue())
	{
		std::ostringstream oss;
		oss << "Render complete";
		std::string window_title(oss.str());
		m_pWindow->SetTitle( itString(window_title.c_str()) );

		GotoState(m_StateWaitToEndMode);
	}
	else
		ExitMode();
}
//----------------------------------------------------------------------------
void cptrModeRender::EndStatePostCapture()
{
	tmlnTimeLine::SetFPS(g3dConstants::c_fDefaultFrameRate);	// set to a default value again.
}

//----------------------------------------------------------------------------
//	MRayPostCapture()
//----------------------------------------------------------------------------
void cptrModeRender::MRayPostCapture()
{
	if ( mrayMgr::GetMRayExportActive() )
	{
		captRenderOutputData& data = captRenderOutputDataUtil::Data();
		fsLocator rootPath = data.m_OutputDirectory.GetValue();
		std::string masterBatchPath = mrayMgr::WriteMasterBatch(rootPath);
		mrayMgr::LaunchMRayMasterBatch(masterBatchPath);
		ResetMRayCapture();
	}
}

//----------------------------------------------------------------------------
//	RenderManPostCapture()
//----------------------------------------------------------------------------
void cptrModeRender::RenderManPostCapture()
{
	if ( rmanMgr::GetRmanExportActive() )
	{
		fsLocator masterBatch = rmanMgr::WriteMasterBatch();
		rmanMgr::LaunchRendermanBatch(masterBatch,true);
		ResetRendermanCapture();
	}
}

//----------------------------------------------------------------------------
//	ResetMRayCapture()
//----------------------------------------------------------------------------
void cptrModeRender::ResetMRayCapture()
{
	mrayMgr::ResetCapture();
}

//----------------------------------------------------------------------------
//	ResetRendermanCapture()
//----------------------------------------------------------------------------
void cptrModeRender::ResetRendermanCapture()
{
	rmanMgr::ResetCapture();
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
	if ((m_pWindow == NULL) || (!m_pWindow->IsOpen()))
	{
		m_bAborted = true;
	}

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
	captRenderOutputData& data = captRenderOutputDataUtil::Data();

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

	//char buffer[512];
	//const char* buffer;
	std::string buffer;
	maTime starttime, endtime;
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

	maTime simtime = starttime;
	int hrs = 0, min = 0, sec = 0, msec = 0;
	int framenum = 0;

	//	build the titlecard
	//
	const g2dRGBColor color(255,255,255);
	const int c_XLOC = 70;
	const int c_YLOC = 50;
	const int c_YLOC_INC = 30;
	float fps = tmlnTimeLine::GetFPS();
	//float mintime = tmlnTimeLine::GetMinimum();
	//::sprintf(buffer,"Scene: %s", itStringUtil::GetStdString(data.m_CurrentSceneFilename.GetValue()).c_str() );
	std::ostringstream oss;
	oss <<"Scene: "<<itStringUtil::GetStdString(data.m_CurrentSceneFilename.GetValue());
	buffer = oss.str();
	text = buffer.c_str();
	m_pWindow->DrawText(c_XLOC,  c_YLOC+c_YLOC_INC*0, font, text, color);

	//::sprintf(buffer,"Camera: %s", itStringUtil::GetStdString(data.m_CurrentCamera.GetValue()).c_str() );
	oss <<"Camera: "<<itStringUtil::GetStdString(data.m_CurrentCamera.GetValue());
	buffer = oss.str();
	text = buffer.c_str();
	m_pWindow->DrawText(c_XLOC,  c_YLOC+c_YLOC_INC*1, font, text, color);

	std::string timestr;
	tmlnTimeUtil::GetTimeStringInHMSMAndFrames( starttime, fps, timestr );
	//::sprintf(buffer,"Start Time: %s", timestr.c_str());
	oss <<"Start Time: "<<timestr;
	buffer = oss.str();
	
	text = buffer.c_str();
	m_pWindow->DrawText(c_XLOC,  c_YLOC+c_YLOC_INC*2, font, text, color);

	tmlnTimeUtil::GetTimeStringInHMSMAndFrames( endtime, fps, timestr );
	//::sprintf(buffer,"End Time: %s", timestr.c_str());
	oss <<"End Time: "<<timestr;
	buffer = oss.str();
	
	text = buffer.c_str();
	m_pWindow->DrawText(c_XLOC, c_YLOC+c_YLOC_INC*3, font, text, color);

	tmlnTimeUtil::GetTimeStringInHMSMAndFrames( (endtime-starttime), fps, timestr );
	//::sprintf(buffer,"Duration: %s", timestr.c_str());
	oss <<"Duration: "<<timestr;
	buffer = oss.str();
	
	text = buffer.c_str();
	m_pWindow->DrawText(c_XLOC, c_YLOC+c_YLOC_INC*4, font, text, color);

	m_pWindow->EndScene();

	//	render out 1/2 second of the titlecard
	for (int i = 0; i < (int)(fps/2); ++i)
	{
		cptrRenderUtil::CaptureFrame();
	}
	m_pWindow->Present();
}


//--------------------------------------------------------------------
// Check to see if the previous render created an error state.
// If so, report the error to the user.
//--------------------------------------------------------------------
void cptrModeRender::check_for_render_thread_error()
{
	if (m_CaptureRenderThread.IsErrorState())
	{
		ResetRendermanCapture();
		ResetMRayCapture();
		g3dPassBuffers::SetDoingFileRefl(false);

		captRenderOutputData& data = captRenderOutputDataUtil::Data();
		if (m_CaptureRenderThread.ShouldAppExit())
		{
			// Error during capture
			std::string msg = m_CaptureRenderThread.GetErrorMessage();
			msg += " ";
			msg += mnmConstants::c_PRODUCT;
			msg += " has saved your render position.  ";
			msg += "Next time you render this scene, it will continue where it left off. ";
			msg += mnmConstants::c_PRODUCT;
			msg += " needs to close now.  If necessary, ";
			msg += mnmConstants::c_PRODUCT;
			msg += " will ask you to save the current scene before closing.";
			
			DBG_ERROR(msg);
			guiMessageBox::Show(msg.c_str(), "Capture Error", guiMessageBox::e_OKOnly);

			//save the render position at the previous frame
			maTime incr = tmlnTimeLine::GetFrameIncrement();
			maTime currtime = tmlnTimeLine::GetValue();
			currtime -= incr;
			tmlnTimeLine::SetValue(currtime);
			cptrRenderStateUtil::SaveRenderPosition();

			m_bAborted = true;
			m_bAppExit = true;
			GotoState( m_StateEndCapture );
		}
		else
		{
			std::string msg = m_CaptureRenderThread.GetErrorMessage();
			msg += " ";
			msg += mnmConstants::c_PRODUCT;
			msg += " has saved your render position.  ";
			msg += "Next time you render this scene, it will continue where it left off. ";

			DBG_ERROR(msg);
			if (data.m_bBatchMode.GetValue()
				&& data.m_bBatchSkipDialog.GetValue())
			{
				cptrRenderStatsDialogUtil::AddStatsMessage(itString(msg.c_str()));
				cptrRenderStatsDialogUtil::Newline();
			}
			else
			{
				guiMessageBox::Show(msg.c_str(), "Capture Error", guiMessageBox::e_OKOnly);
			}

			//save the render position at the previous frame
			maTime incr = tmlnTimeLine::GetFrameIncrement();
			maTime currtime = tmlnTimeLine::GetValue();
			currtime -= incr;
			tmlnTimeLine::SetValue(currtime);
			cptrRenderStateUtil::SaveRenderPosition();

			m_bAborted = true;
			GotoState( m_StateEndCapture );
		}

		// After reporting and handling error, clear the state
		m_CaptureRenderThread.ClearErrorState();
	}
}

//--------------------------------------------------------------------
// attempt a call to do_render_capture and catch exceptions that are
// thrown, aborting capture mode if exception is found.
//--------------------------------------------------------------------
void cptrModeRender::try_render_capture()
{	
	// Whether to capture in a thread is a combination of the compiler define
	// and the user preference set by PrefsData.
	if (c_MULTITHREADED_CAPTURE && cptrModeRender::sm_bThreadingEnabled)
	{
		// Start capture render in a thread
		envThread render_thread( std::bind(&cptrRenderThread::ThreadedRenderCapture, &m_CaptureRenderThread) );
		g3dThreadControl::WaitForRenderThreadStart();
	}
	else
	{
		// Do Capture render in single thread
		m_CaptureRenderThread.TryRenderCapture();

		// When not multi-threaded, check for errors in rendering immediately
		check_for_render_thread_error();
	}

	//	Output a line for each frame rendered
	//
	captRenderOutputData& data = captRenderOutputDataUtil::Data();
	nameString camname( itStringUtil::GetStdString(data.m_RenderPosCamera.GetValue()) );
	DBG_LOG("Scene: " << data.m_CurrentSceneFilename.GetValue() << " Camera: " << camname << " Frame: " << tmlnTimeLine::GetValue().AsFrame(tmlnTimeLine::GetFPS()) );
}


//----------------------------------------------------------------------------
//	lead in/out render output
//----------------------------------------------------------------------------
void cptrModeRender::do_lead_render()
{
	try_render_capture();
}

//----------------------------------------------------------------------------
//	set the timeline value to the passed in value.  if the time
//----------------------------------------------------------------------------
void cptrModeRender::set_timeline(const maTime& i_value, bool i_bSetAppSimTimeAlso)
{
	//captRenderOutputData& data = captRenderOutputDataUtil::Data();

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
	captRenderOutputData& data = captRenderOutputDataUtil::Data();
	m_CaptureRenderThread.ResetRenderCaptureIteration();

	//DBG_LOG("cameras = " << data.m_CameraList.size());

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
//	get the next render layer index
//--------------------------------------------------------------------
int cptrModeRender::get_next_layer_index()
{
	m_CurrentLayerIndex = rlyrRenderLayerMgr::GetNextLayerIndex( m_CurrentLayerIndex );
	if ( m_CurrentLayerIndex == -1 )
		m_CurrentLayerIndex = m_LastLayerIndex + 1;
	return m_CurrentLayerIndex;
}

//--------------------------------------------------------------------
//	Calculate the number of renderable cameras
//--------------------------------------------------------------------
void cptrModeRender::calculate_number_of_renderable_cameras()
{
	m_NumberOfRenderableCameras = 0;
	captRenderOutputData& data = captRenderOutputDataUtil::Data();
	

	if ( data.m_bBatchMode.GetValue() )
	{
		m_NumberOfRenderableCameras = m_LastCameraIndex;
	}
	else
	{
		DBG_ASSERT( (data.m_CameraList.size() >= m_LastCameraIndex), "Invalid number of cameras, " << m_LastCameraIndex << " < " << data.m_CameraList.size());

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
	captRenderOutputData& data = captRenderOutputDataUtil::Data();

	// tell the outputdatautil what the current camera name is.
	itString cam_dir_name;
	nameString theName;
	camsFollowUtil::GetCurrentCameraName( theName );
	cam_dir_name = theName.GetString().c_str();
	//	Append letter on the end of the folder
	//	if (output is a movie, and there is a movie
	//	per driver) or the driver flag is set
	//
	if (( data.m_bCaptureMovie.GetValue() && data.m_bMoviePerDriver.GetValue() )
		||
		(data.m_bUseCameraNameAsDirectory.GetValue() && data.m_bUseRenderDriversAsUniqueCameras.GetValue() ))
	{
		cam_dir_name += m_cCurrentRenderDriverLetter;
	}
	captRenderOutputDataUtil::SetCurrentCamera(cam_dir_name);

	DBG_TRACE("Output Root (1)=" << data.m_OutputDirectoryRoot.GetValue());
	DBG_TRACE("Output Dir  (1)=" << data.m_OutputDirectory.GetValue());

	// loop over all render layers and set up folders.

	int start_layer_index = rlyrRenderLayerMgr::GetNextLayerIndex( -1 );

	int loop = 1;
	if (data.m_bUseLayerNameAsDirectory.GetValue())
		loop = rlyrRenderLayerMgr::GetNumRenderLayers();

	m_Data.m_FrameTotal = rlyrRenderLayerMgr::GetNumRenderLayers();

	// i is the render layer.
	// j is how many times we looped.
	for (int i = start_layer_index, j = 0; 
		i != -1 && j < loop; 
		i = rlyrRenderLayerMgr::GetNextLayerIndex(i), j++)
	{
		itString layer_name;
		nameString layer_desc = rlyrRenderLayerMgr::GetLayerName( i );
		layer_name = layer_desc.GetString().c_str();
		data.m_RenderPosLayer.SetValue( layer_name );			// for saving render position
		captRenderOutputDataUtil::SetCurrentLayer( layer_name );

		// Apply post effect in this layer
		pfxPostEffectObject* pfxObject = rlyrRenderLayerMgr::GetLayerRenderPfx(layer_desc);
		if (pfxObject)
			pfxObject->ApplyPostEffect();

		// loop over all enabled render passes, if render pass name is in directory.
		int loopPasses = 1;
		std::vector<rlyrPassesObject::ePassType> thePasses;
		if (data.m_bUseRenderPassAsDirectory.GetValue())
		{
			rlyrPassesObject* passesObject = rlyrRenderLayerMgr::GetLayerRenderPasses(layer_desc);
			passesObject->CollectPasses(thePasses);
			loopPasses = thePasses.size();
		}

		for (int k = 0; k < loopPasses; k++)
		{
			if (data.m_bUseRenderPassAsDirectory.GetValue())
			{
				rlyrPassesObject::ePassType passType = thePasses[k];
				itString pass_name = captRenderOutputDataUtil::GetRenderPassName(passType);
				captRenderOutputDataUtil::SetCurrentRenderPass( pass_name );
			}

			// set up folder name in data util
			captRenderOutputDataUtil::GenerateDirectoryName();

		//	fsLocator dir = data.m_OutputDirectory.GetValue();
		//	captRenderOutputDataUtil::SetDirectory( dir );

			DBG_LOG( "Final Output Directory " << data.m_OutputDirectory.GetValue() );

			//	check if the directory exists, if not, create it.
			//
			if ( !fsFileUtil::DirectoryExists(data.m_OutputDirectory.GetValue()) )
			{
				try
				{
					fsFileUtil::CreateDirectory(data.m_OutputDirectory.GetValue());
				}
				catch ( const fsDirectoryDoesntExistX& /*i_Ex*/ )
				{
					std::string dir;
					fsFileUtil::LocatorToANSIFilename(data.m_OutputDirectory.GetValue(), dir);
					std::string msg = "Error trying to create Output directory: " + dir;
					DBG_ERROR(msg);
					if(data.m_bBatchMode.GetValue()
						&& data.m_bBatchSkipDialog.GetValue())
					{
						cptrRenderStatsDialogUtil::AddStatsMessage(itString(msg.c_str()));
						cptrRenderStatsDialogUtil::Newline();
					}
					else
					{
						guiMessageBox::Show(msg.c_str(), "Create Directory Error", guiMessageBox::e_OKOnly);
					}		
					m_bAborted = true;
				}
				catch ( const envExceptionX& i_Ex )
				{
					std::string msg = "Error trying to create Output directory, " + i_Ex.GetErrorMessage();
					DBG_ERROR(msg);
					if(data.m_bBatchMode.GetValue()
						&& data.m_bBatchSkipDialog.GetValue())
					{
						cptrRenderStatsDialogUtil::AddStatsMessage(itString(msg.c_str()));
						cptrRenderStatsDialogUtil::Newline();
					}
					else
					{
						guiMessageBox::Show(msg.c_str(), "Create Directory Error", guiMessageBox::e_OKOnly);
					}		
					m_bAborted = true;
				}	
			}
		}
	}

}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void cptrModeRender::update_render_layer_progress()
{

	captRenderOutputData& data = captRenderOutputDataUtil::Data();
	nameString layer_desc = rlyrRenderLayerMgr::GetLayerName( m_CurrentLayerIndex );
	std::stringstream rl_stream;
	rl_stream << "Render Layer - " << layer_desc.GetString();
	itString layer_label(rl_stream.str().c_str());

	m_Data.m_LayersCompleted = m_CurrentLayerIndex;
	m_Data.m_CurLayerName = layer_desc;
	cptrRenderProgressDialogUtil::SetRenderLayerProgress( layer_label );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
float cptrModeRender::get_camera_percentage()
{
	maTime timeline_time = tmlnTimeLine::GetValue();
	float camera_percentage = this->m_CurrentInOutList.GetPercentage( timeline_time );
	//m_Data.m_CurCamPercentage = camera_percentage;

	return camera_percentage;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void cptrModeRender::update_camera_percentage()
{
	//char buffer[256];
	nameString theName;
	camsFollowUtil::GetCurrentCameraName(theName);
	m_Data.m_CurCamName = theName;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
float cptrModeRender::get_cameras_percentage()
{
	//float timeline_time = tmlnTimeLine::GetValue();
	//float cameras_percentage = tmlnTimeInOutMgr::GetPercentageComplete(timeline_time);
	float cameras_percentage = 0.0f;
	if (m_NumberOfRenderableCameras > 0)
	{
		cameras_percentage = get_camera_percentage() * (1.0f/(float)m_NumberOfRenderableCameras);
		cameras_percentage += ((float)m_CurrentCameraNumber / (float)m_NumberOfRenderableCameras);
	}
	if (cameras_percentage > 1.0f) cameras_percentage = 1.0f;
	m_Data.m_CurCamPercentage = cameras_percentage;
	return cameras_percentage;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void cptrModeRender::update_cameras_percentage()
{
	//char buffer[256];
	//sprintf(buffer, "Camera %d of %d", m_CurrentCameraNumber+1, m_NumberOfRenderableCameras);
	std::ostringstream oss;
	oss.setf(0, std::ios::floatfield);
	oss.setf(std::ios::fixed, std::ios::floatfield);
	oss << "Camera  "<< m_CurrentCameraNumber+1 <<" of "<< m_NumberOfRenderableCameras;

	m_Data.m_CamerasTotal = m_NumberOfRenderableCameras;
	m_Data.m_CamerasCompleted = m_CurrentCameraNumber +1;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void cptrModeRender::update_scenes_percentage(bool i_bCompleted)
{
	captRenderOutputData& data = captRenderOutputDataUtil::Data();
	std::string filename;
	filename = itStringUtil::GetStdString( data.m_CurrentSceneFilename.GetValue() );
	m_Data.m_CurSceneName = nameString(filename.c_str());
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void cptrModeRender::update_progress_timeleft()
{	
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void cptrModeRender::update_progress_percentages(bool i_bCompleted)
{
	update_camera_percentage();
	update_cameras_percentage();
	update_scenes_percentage(i_bCompleted);
	update_progress_timeleft();
	update_progress_percentages_dialog();

	captRenderProgressMgr::FrameComplete();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void cptrModeRender::update_progress_percentages_dialog()
{

	captRenderOutputData& data = captRenderOutputDataUtil::Data();

	nameString theName;
	camsFollowUtil::GetCurrentCameraName(theName);

	std::ostringstream oss;
	oss << "Camera - "<< theName.GetString();
	std::string buffer(oss.str());
	itString camera_label(buffer.c_str());
	float percentage = get_camera_percentage();
	m_camera_percentage = percentage;
	m_camera_label = camera_label;


	oss.clear();
	oss.setf(0, std::ios::floatfield);
	oss.setf(std::ios::fixed, std::ios::floatfield);
	oss << "Camera  "<< m_CurrentCameraNumber+1 <<" of "<< m_NumberOfRenderableCameras;

	std::string buffer1(oss.str());
	itString cameras_label(buffer1.c_str());
	m_camera_label = cameras_label;

	oss.clear();
	oss.setf(0, std::ios::floatfield);
	oss << "Scene  "<<data.m_CurrentSceneNumber.GetValue() <<" of "<<data.m_NumberOfScenes.GetValue();
	std::string buffer2(oss.str());
	
	itString scenes_label(buffer2.c_str());
	
	percentage = 0.0f;
	percentage = ((float)(data.m_CurrentSceneNumber.GetValue()-1) / (float)data.m_NumberOfScenes.GetValue());
	if (percentage < 0.0f) percentage = 0.0f;
	percentage += get_cameras_percentage() * (1.0f / (float)data.m_NumberOfScenes.GetValue());
	m_scenes_percentage = percentage;
	m_scenes_label = scenes_label;

	cptrRenderProgressDialogUtil::SetCameraRenderPercentage( 100.0f*m_camera_percentage, m_camera_label );
	cptrRenderProgressDialogUtil::SetCamerasRenderPercentage( 100.0f*get_cameras_percentage(), m_cameras_label );
	cptrRenderProgressDialogUtil::SetScenesRenderPercentage( 100.0f*m_scenes_percentage, m_scenes_label );
	cptrRenderProgressDialogUtil::SetTimeElapsed( appTime::GetTime() - m_fRenderStartTime );

}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void cptrModeRender::reset_renderer_for_camera()
{
	// called after do_titlecard_render
	// tell renderer that we are starting a new camera
	// renderer can draw an initial frame for deltas.

// this code was only needed when we needed to prime the hdr exposure
// for adaptive exposure control.
#if 0
	if (!cptrRenderUtil::GetCaptureQuadrants())
	{
		camCamera *pCamera = camsFollowUtil::GetFollowCamera();
		if (m_pViewer && 
			(rlyrRenderLayerMgr::GetLayerActualData(m_CurrentLayerIndex).m_RendererType == g3dSceneRendererTypes::e_HDR))
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
#endif
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void cptrModeRender::ResetConfigFileName()
{
	fsLocator cfgdir = gfPaths::GetPath(mnmPaths::e_Configs);
	captRenderOutputData& data = captRenderOutputDataUtil::Data();

	cfgdir.Push( "RenderOutput.cfg" );
	data.m_RenderOutputDataFile.SetValue( cfgdir );
	cfgdir.Clear();
	cfgdir.Push( gfPaths::GetPath(mnmPaths::e_DefaultConfigs) );
	cfgdir.Push( "RenderOutput-Default.cfg" );
	data.m_RenderOutputDataDefaultFile.SetValue( cfgdir );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void cptrModeRender::ReadConfig()
{
	captRenderOutputData& data = captRenderOutputDataUtil::Data();
	//captRenderOutputDataUtil::ReadData(data);

	//	after reading in the data, set the output root dir if not already set.
	//	then set the full output dir to that as well
	if (data.m_OutputDirectoryRoot.GetValue().GetNumNames() == 0)
	{
		data.m_OutputDirectoryRoot.SetValue( gfPaths::GetPath( mnmPaths::e_SaveFootage ) );	// default value
	}
	data.m_OutputDirectory.SetValue( data.m_OutputDirectoryRoot.GetValue() );	// default value
}

//----------------------------------------------------------------------------
//	Used for debugging only
//----------------------------------------------------------------------------
void cptrModeRender::Log_Settings()
{
	captRenderOutputData& data = captRenderOutputDataUtil::Data();
	DBG_LOG("Footage Root=" << data.m_OutputDirectoryRoot.GetValue());
	DBG_LOG("Footage Dir =" << data.m_OutputDirectory.GetValue());
}

//--------------------------------------------------------------------
//	set up first and last layer indices
//--------------------------------------------------------------------
void cptrModeRender::init_layer_indices()
{
	captRenderOutputData& data = captRenderOutputDataUtil::Data();

	//now get the resume layer
	nameString layername( itStringUtil::GetStdString(data.m_RenderPosLayer.GetValue()) );
	m_LastLayerIndex = rlyrRenderLayerMgr::GetNumRenderLayers();
	m_CurrentLayerIndex = -1;
	int startlayer_index = rlyrRenderLayerMgr::GetLayerIndexByName(layername);

	//if the layer isn't found, grab the first active layer
	if (startlayer_index == -1)
		startlayer_index = this->get_next_layer_index();
	m_CurrentLayerIndex	= startlayer_index;
}


//------------------------------------------------------------------------
// get the current timeline frame
//------------------------------------------------------------------------
void cptrModeRender::getCurrentTime()
{
	m_CurrentTimeFrame = tmlnTimeLine::GetValue();
}

//------------------------------------------------------------------------
// set/restore the current timeline frame
//------------------------------------------------------------------------
void cptrModeRender::restoreCurrentTime()
{
	tmlnTimeLine::SetValue( m_CurrentTimeFrame );
}
