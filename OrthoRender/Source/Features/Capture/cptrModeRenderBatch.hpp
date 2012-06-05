/*****************************************************************************
**  cptrModeRenderBatch.hpp
**
**      The Batch Capture mode
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef CPTR_MODERENDERBATCH_HPP
#error cptrModeRenderBatch.hpp multiply included
#endif
#define CPTR_MODERENDERBATCH_HPP

#ifndef MODE_MODE_HPP
#include "Support/mode/modeMode.hpp"
#endif


//============================================================================
//============================================================================
class cptrModeRenderBatch : public modeMode
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		cptrModeRenderBatch( modeModeID i_ModeIDCapture );

		//--------------------------------------------------------------------
		// destructor
		//--------------------------------------------------------------------
		virtual ~cptrModeRenderBatch();

		//--------------------------------------------------------------------
		//	Initialize will be called before the first call of Think after
		//	the object is first created or DeInitialized.  During the
		//	lifetime of a mode, Initialize and DeInitialize may be called
		//	several times.  Children of appMode should remember to call
		//	appMode::Initialize() at the beginning of their Initialize
		//	function.
		//--------------------------------------------------------------------
		void Initialize();

		//--------------------------------------------------------------------
		//	The object should clean up things that are not needed while the
		//	mode is not running in the DeInitialize function.
		//--------------------------------------------------------------------
		void DeInitialize();

		//--------------------------------------------------------------------
		//	The mode should do it's per frame "work" in the Think function.
		// This simply calls the base class and handles screen captures.
		//--------------------------------------------------------------------
		void Think();

		//----------------------------------------------------------------------------
		//	IsModal() - signals whether the mode should be pushed onto the mode stack
		//	ON TOP of the current mode, or replace the current mode (via Pop)
		//
		//	true - the mode will be pushed on top of the current mode.
		//	false- the mode will replace the current mode.
		//----------------------------------------------------------------------------
		virtual bool IsModal();

		//--------------------------------------------------------------------
		//	Abort all the renders
		//--------------------------------------------------------------------
		void AbortRenders();

		//------------------------------------------------------------------------
		//	SetSkipCaptureOptions - if this is set, this mode won't call the
		//	capture options dialog.  it will assume that it has already been
		//	called by something else.
		//
		//	Note:  this value will reset with each call to this mode.
		//------------------------------------------------------------------------
		void SetSkipCaptureOptions( bool i_bSkip );

	private:
		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		void update_scenes_percentage();

	private:
		bool m_bInitialized;
		bool m_bAborted;
		bool m_bProcessing;
		bool m_bSkipCaptureOptions;

		enum stages
		{
			e_Begin = 0,
			e_DialogBatch,
			e_SetupForBatch,
			e_DialogCapture,
			e_Processing,
			e_EndProcessing,
			e_EndMode
		};
		stages	m_Stage;

		int		m_NumScenes;
		int		m_SceneIndex;
		modeModeID	m_ModeRenderID;
};


