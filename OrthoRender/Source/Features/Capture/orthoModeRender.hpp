/*****************************************************************************
**  orthoModeRender.hpp
**
**      The Main Render mode
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef ORTHO_MODERENDER_HPP
#error orthoModeRender.hpp multiply included
#endif
#define ORTHO_MODERENDER_HPP

#ifndef ORTHO_REMOTECOMMANDMGR_HPP
#include "Features/Requests/orthoRemoteCommandMgr.hpp"
#endif

#ifndef MODE_MODETIME_HPP
#include "Support/mode/modeModeTime.hpp"
#endif

#ifndef TMLN_TIMEINOUTDATA_HPP
#include "Support/tmln/tmlnTimeInOutData.hpp"
#endif

#ifndef DM_FSM_HPP
#include "Core/dm/dmFSM.hpp"
#endif


//============================================================================
//============================================================================
class camCamera;
class cptrAccumulationBuffer;
class cptrMotionSampler;
class g2dSystem;
class g2dWindow;
class g3dSceneRenderer;
class g3dViewer;


//============================================================================
//============================================================================
class orthoModeRender : public modeModeTime,
						public dmFSM
{
	public:
		enum ModeRenderStates
		{
			e_StateInitCaptures = 0,
			e_StateBeginCapture,
			e_StateLeadIn,
			e_StateCapture,
			e_StateLeadOut,
			e_StateEndCapture,
			e_StateDeInitCaptures,
			e_StatePostCapture,
			e_StateWaitToEndMode,
		};

		//------------------------------------------------------------------------
		//	SetUseApplicationWindow() - if true, capture mode should use
		//	the application window to render into. Otherwise, it should
		//	create a subwindow of the correct size to render into.
		//------------------------------------------------------------------------
		static void SetUseApplicationWindow(bool i_bUseAppWindow);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		orthoModeRender();

		//--------------------------------------------------------------------
		// pure-virtual, children must be created
		//--------------------------------------------------------------------
		virtual ~orthoModeRender();

		//--------------------------------------------------------------------
		//	Initialize will be called before the first call of Think after
		//	the object is first created or DeInitialized.  During the
		//	lifetime of a mode, Initialize and DeInitialize may be called
		//	several times.  Children of appMode should remember to call
		//	appMode::Initialize() at the beginning of their Initialize
		//	function.
		//--------------------------------------------------------------------
		virtual void Initialize();

		//--------------------------------------------------------------------
		//	The object should clean up things that are not needed while the
		//	mode is not running in the DeInitialize function.
		//--------------------------------------------------------------------
		virtual void DeInitialize();

		//----------------------------------------------------------------------------
		//	Exit mode
		//----------------------------------------------------------------------------
		void ExitMode();

		//--------------------------------------------------------------------
		//	The mode should do it's per frame "work" in the Think function.
		// This simply calls the base class and handles screen captures.
		//--------------------------------------------------------------------
		void Think();

		//------------------------------------------------------------------------
		//	SetSystem() - set the g2d system for creating new windows
		//------------------------------------------------------------------------
		void SetSystem( g2dSystem* i_pSystem );

		//------------------------------------------------------------------------
		//	SetSkipCaptureOptions - if this is set, this mode won't call the
		//	capture options dialog.  it will assume that it has already been
		//	called by something else.
		//
		//	Note:  this value will reset with each call to this mode.
		//------------------------------------------------------------------------
		void SetSkipCaptureOptions( bool i_bSkip );

		//------------------------------------------------------------------------
		//	SetUseOnlyValidCaptureTimes - set this to false to ignore all user
		//	capture times so any time slice is a valid one.
		//------------------------------------------------------------------------
		void SetUseOnlyValidCaptureTimes( bool i_bUseValid );

		//------------------------------------------------------------------------
		//	Allow another mode (like batch render) to set the start time
		//------------------------------------------------------------------------
		void SetRenderStartTime( float i_fRenderStartTime );

		//----------------------------------------------------------------------------
		//	IsModal() - signals whether the mode should be pushed onto the mode stack
		//	ON TOP of the current mode, or replace the current mode (via Pop)
		//
		//	true - the mode will be pushed on top of the current mode.
		//	false- the mode will replace the current mode.
		//----------------------------------------------------------------------------
		virtual bool IsModal();

		//--------------------------------------------------------------------
		//	Abort all the render
		//--------------------------------------------------------------------
		void AbortRender();

		//--------------------------------------------------------------------
		//	Pause the render
		//--------------------------------------------------------------------
		void PauseRender(bool i_bPause = true);

		//--------------------------------------------------------------------
		//	Is the render paused
		//--------------------------------------------------------------------
		bool IsRenderPaused();

public:
		//
		//	states
		//
		//	BeginState - is executed ONCE at the start of the state
		//	OnState - is executed every frame while in the state
		//	EndState - is executed on ending the state
		//

		//--------------------------------------------------------------------
		//	RenderConfig
		//--------------------------------------------------------------------
		virtual void BeginStateRenderConfig();
		virtual void OnStateRenderConfig();
		virtual void EndStateRenderConfig();

		//--------------------------------------------------------------------
		//	InitCaptures
		//--------------------------------------------------------------------
		virtual void BeginStateInitCaptures();
		virtual void OnStateInitCaptures();
		virtual void EndStateInitCaptures();

		//--------------------------------------------------------------------
		//	BeginCapture
		//--------------------------------------------------------------------
		virtual void BeginStateBeginCapture();
		virtual void OnStateBeginCapture();
		virtual void EndStateBeginCapture();

		//--------------------------------------------------------------------
		//	LeadIn
		//--------------------------------------------------------------------
		virtual void BeginStateLeadIn();
		virtual void OnStateLeadIn();
		virtual void EndStateLeadIn();

		//--------------------------------------------------------------------
		//	Capture
		//--------------------------------------------------------------------
		virtual void BeginStateCapture();
		virtual void OnStateCapture();
		virtual void EndStateCapture();

		//--------------------------------------------------------------------
		//	LeadOut
		//--------------------------------------------------------------------
		virtual void BeginStateLeadOut();
		virtual void OnStateLeadOut();
		virtual void EndStateLeadOut();

		//--------------------------------------------------------------------
		//	EndCapture
		//--------------------------------------------------------------------
		virtual void BeginStateEndCapture();
		virtual void OnStateEndCapture();
		virtual void EndStateEndCapture();

		//--------------------------------------------------------------------
		//	DeInitCaptures
		//--------------------------------------------------------------------
		virtual void BeginStateDeInitCaptures();
		virtual void OnStateDeInitCaptures();
		virtual void EndStateDeInitCaptures();

		//--------------------------------------------------------------------
		//	PostCapture
		//--------------------------------------------------------------------
		virtual void BeginStatePostCapture();
		virtual void OnStatePostCapture();
		virtual void EndStatePostCapture();

		//--------------------------------------------------------------------
		//	WaitToEndMode
		//--------------------------------------------------------------------
		virtual void BeginStateWaitToEndMode();
		virtual void OnStateWaitToEndMode();
		virtual void EndStateWaitToEndMode();

	private:
		//--------------------------------------------------------------------
		//	set the current state based on the last state of the object
		//--------------------------------------------------------------------
		void set_current_state();

		//--------------------------------------------------------------------
		//	SetCaptureData() - set the capture data
		//--------------------------------------------------------------------
		void SetCaptureData();

		//--------------------------------------------------------------------
		// do title card render
		//--------------------------------------------------------------------
		void do_titlecard_render();

		//--------------------------------------------------------------------
		// do render in secondary window and capture the frame
		//--------------------------------------------------------------------
		void do_render_capture();

		//--------------------------------------------------------------------
		//	lead in/out render output
		//--------------------------------------------------------------------
		void do_lead_render();

		//--------------------------------------------------------------------
		//	set the timeline value to the passed in value.  if the time
		//--------------------------------------------------------------------
		void set_timeline(float i_value, bool i_bSetAppSimTimeAlso = false);

		//--------------------------------------------------------------------
		//	get the next camera index based on the camera list (or all cams)
		//--------------------------------------------------------------------
		int get_next_camera_index();

		//--------------------------------------------------------------------
		//	Calculate the number of renderable cameras
		//--------------------------------------------------------------------
		void calculate_number_of_renderable_cameras();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void build_directory();

		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		float get_camera_percentage();

		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		void update_camera_percentage();
		
		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		float get_cameras_percentage();
		
		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		void update_cameras_percentage();
		
		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		void update_scenes_percentage(bool i_bCompleted);
		
		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		void update_progress_percentages(bool i_bCompleted);

		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		void update_progress_timeleft();

		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		void reset_renderer_for_camera();

		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		void RenderJitteredFrame();

	protected:
		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		virtual void ResetConfigFileName();

	private:
		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		void ReadConfig();

		//----------------------------------------------------------------------------
		//	Used for debugging only
		//----------------------------------------------------------------------------
		void Log_Settings();

		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		void ApplyRenderPrefs();

		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		void batch_process_apply_masks();

	protected:
		bool m_bInitialized;
		bool m_bAborted;
		int m_RenderPrefsObjectID;

	private:
		static bool sm_bUseAppWindow;
		bool m_bCaptureStarted;
		bool m_bSkipCaptureOptions;
		bool m_bSwitchToCameraRenderRange;
		bool m_bCaptureFinished;
		bool m_bFirstRenderFrame;
		bool m_bAudioMuteState;
		bool m_bPaused;
		bool m_bUseOnlyValidCaptureTimes;

		float m_fSimTimeInc;
		float m_fLeadTimeDelta;
		float m_fEndTime;
		float m_fRenderStartTime;

		int m_CurrentCameraIndex;
		int m_LastCameraIndex;
		int m_NumberOfRenderableCameras;
		int m_CurrentCameraNumber;	// not index, but count.

		int m_OldSubdivLevel;
		bool m_bParticlesActive;
		char m_cCurrentRenderDriverLetter;

		g2dSystem* m_pSystem;
		g2dWindow* m_pWindow;
		g3dSceneRenderer *m_pRenderer;
		g3dViewer *m_pViewer;

		cptrMotionSampler* m_MotionSampler;
		cptrAccumulationBuffer* m_AccBuf;

		tmlnTimeInOutDataList		m_CurrentInOutList;

		int	m_OutputFilenamesIndex;
		std::vector<orthoRemoteCommandMgr::RemoteFilenameData>	m_OutputFilenames;

		//state machine declarations
		dmState<orthoModeRender>		m_StateRenderConfig;
		dmState<orthoModeRender>		m_StateInitCaptures;
		dmState<orthoModeRender>		m_StateBeginCapture;
		dmState<orthoModeRender>		m_StateLeadIn;
		dmState<orthoModeRender>		m_StateCapture;
		dmState<orthoModeRender>		m_StateLeadOut;
		dmState<orthoModeRender>		m_StateEndCapture;
		dmState<orthoModeRender>		m_StateDeInitCaptures;
		dmState<orthoModeRender>		m_StatePostCapture;
		dmState<orthoModeRender>		m_StateWaitToEndMode;
};


