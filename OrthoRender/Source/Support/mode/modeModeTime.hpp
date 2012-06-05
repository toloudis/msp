/*****************************************************************************
**  modeModeTime.hpp
**
**      The mode that handles the timeline.
**
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef MODE_MODETIME_HPP
#error modeModeTime.hpp multiply included
#endif
#define MODE_MODETIME_HPP

#ifndef MODE_MODE_HPP
#include "Support/mode/modeMode.hpp"
#endif

//============================================================================
//	forward references
//============================================================================

//============================================================================
//============================================================================
class modeModeTime : public modeMode
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		modeModeTime();

		//--------------------------------------------------------------------
		// pure-virtual, children must be created
		//--------------------------------------------------------------------
		virtual ~modeModeTime();

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

	private:
		bool m_bInitialized;
};

