/*****************************************************************************
**  plbkModePlayback.hpp
**
**      The playback mode
**
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef PLBK_MODEPLAYBACK_HPP
#error plbkModePlayback.hpp multiply included
#endif
#define PLBK_MODEPLAYBACK_HPP

#ifndef MODE_MODETIME_HPP
#include "Support/mode/modeModeTime.hpp"
#endif


//============================================================================
//============================================================================
class plbkModePlayback : public modeModeTime
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		plbkModePlayback();

		//--------------------------------------------------------------------
		// pure-virtual, children must be created
		//--------------------------------------------------------------------
		virtual ~plbkModePlayback();

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

		//--------------------------------------------------------------------
		//	The mode should do it's per frame "work" in the Think function.
		// This simply calls the base class and handles screen captures.
		//--------------------------------------------------------------------
		virtual void Think();

		//----------------------------------------------------------------------------
		//	IsModal() - signals whether the mode should be pushed onto the mode stack
		//	ON TOP of the current mode, or replace the current mode (via Pop)
		//
		//	true - the mode will be pushed on top of the current mode.
		//	false- the mode will replace the current mode.
		//----------------------------------------------------------------------------
		virtual bool IsModal();

		//----------------------------------------------------------------------------
		//	Quit mode
		//----------------------------------------------------------------------------
		void ExitMode();

	//
	//	playback controls
	//

		//--------------------------------------------------------------------
		//	playback controls
		//--------------------------------------------------------------------
		void Pause();
		void FastFwd();
		void FastRev();
		void Play();
		void GoEnd();
		void GoBegin();
		void PlayRev();
		void SetLooping( bool i_bLoopAtEnd );
		void SetLowRes( bool i_bLowRes );
		void SetMute( bool i_bMute );
		void SetSloMo( float i_slowMotionRate );
		void TogglePlayAndPause();


	private:
		bool	m_bInitialized;
};


