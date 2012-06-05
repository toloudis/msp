/*****************************************************************************
**  cptrModeRenderFrame.hpp
**
**      The Batch Capture mode
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef CPTR_MODERENDERFRAME_HPP
#error cptrModeRenderFrame.hpp multiply included
#endif
#define CPTR_MODERENDERFRAME_HPP

#ifndef MODE_MODE_HPP
#include "Support/mode/modeMode.hpp"
#endif


//============================================================================
//============================================================================
class cptrModeRenderFrame : public modeMode
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		cptrModeRenderFrame( modeModeID i_ModeIDCapture );

		//--------------------------------------------------------------------
		// destructor
		//--------------------------------------------------------------------
		virtual ~cptrModeRenderFrame();

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

	private:
		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		void ResetConfigFileName();

	private:
		bool	m_bInitialized;
		bool	m_bAborted;
		bool	m_bProcessing;

		enum stages
		{
			e_Begin = 0,
			e_DialogBatch,
			e_DialogCapture,
			e_Processing,
			e_EndProcessing,
			e_EndMode
		};
		stages	m_Stage;

		int		m_NumScenes;
		int		m_SceneIndex;
		modeModeID	m_ModeRenderID;
		float	m_fRestoreTime;
};


