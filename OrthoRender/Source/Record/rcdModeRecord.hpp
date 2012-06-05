/*****************************************************************************
**  rcdModeRecord.hpp
**
**      The playback mode
**
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#ifdef RCD_MODERECORD_HPP
#error rcdModeRecord.hpp multiply included
#endif
#define RCD_MODERECORD_HPP

#ifndef MODE_MODETIME_HPP
#include "Support/mode/modeModeTime.hpp"
#endif


//============================================================================
//============================================================================
class rcdModeRecord : public modeModeTime
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		rcdModeRecord();

		//--------------------------------------------------------------------
		// pure-virtual, children must be created
		//--------------------------------------------------------------------
		virtual ~rcdModeRecord();

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

	//
	//	playback controls
	//

		//--------------------------------------------------------------------
		//	playback controls
		//--------------------------------------------------------------------
		void Pause();
		void Play(float i_BeginTime, float i_EndTime);

	private:
		bool m_bInitialized;
};


