/*****************************************************************************
**  appMode.hpp
**
**      appMode is an abstract interface representing a program mode.
**	A program can only use one mode at a time.  Before a mode begins
**	(receives its first Think), the virtual Initialize function is called.
**	After the mode ends (requests termination), the virtual DeInitialize
**	function is called.
**		When a mode decides that it is finished or that program flow needs
**	to go to a new mode, it should call the SetTerminateCondition function.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef APP_MODE_HPP
#error appMode.hpp multiply included
#endif
#define APP_MODE_HPP

#ifndef APP_FLOWEVENTHANDLER_HPP
#include "Core/app/appFlowEventHandler.hpp"
#endif


//============================================================================
//============================================================================
class appMode : public appFlowEventHandler
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		appMode();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~appMode() = 0;

		//--------------------------------------------------------------------
		//	The mode should do it's per frame "work" in the Think function.
		//--------------------------------------------------------------------
		virtual void Think() = 0;

		//--------------------------------------------------------------------
		//	Initialize will be called before the first call of Think after
		//	the object is first created or DeInitialized.  During the
		//	lifetime of a mode, Initialize and DeInitialize may be called
		//	several times.  Children of appMode should remember to call
		//	appMode::Initialize() at the beginning of their Initialize
		//	function.
		//--------------------------------------------------------------------
		virtual void Initialize() = 0;

		//--------------------------------------------------------------------
		//	The object should clean up things that are not needed while the
		//	mode is not running in the DeInitialize function.
		//--------------------------------------------------------------------
		virtual void DeInitialize() = 0;

		//--------------------------------------------------------------------
		//	The TerminateCondition describes the behavior requested by the
		//	mode when it is finished.
		//
		//	e_Continue - don't terminate the mode.  (The default)
		//	e_TerminateAndPersist - DeInitialize the mode, and move on to the
		//		next mode higher up the stack (which was typically pushed on
		//		by the terminating mode).
		//	e_TerminateAndDestroy - DeInitialize the mode, delete it, and move
		//		on to the next mode higher up the stack, if available, or
		//		to the previous mode lower on the stack if not.
		//	e_TerminateAndRemove - Like e_TerminateAndDestroy, except does
		//		not delete the mode.  This allows the same mode object to be
		//		used (pushed) later.
		//--------------------------------------------------------------------
		enum TerminateCondition
		{
			e_Continue,
			e_TerminateAndPersist,
			e_TerminateAndDestroy,
			e_TerminateAndRemove
		};

		//--------------------------------------------------------------------
		//	GetTerminateCondition returns whichever terminate rondition is
		//	requested by the mode.
		//--------------------------------------------------------------------
		TerminateCondition GetTerminateCondition() const;

		//--------------------------------------------------------------------
		//	Override this function to get appQuitRequestedEvents.
		//--------------------------------------------------------------------
		virtual void ReceiveQuitRequestEvent(appQuitRequestEvent& i_Event);

	protected:

		//--------------------------------------------------------------------
		//	SetTerminateCondition should be called when a mode is ready to
		//	end (it's not neccesary to call it to set "e_Continue", as this
		//	is the default).
		//--------------------------------------------------------------------
		void SetTerminateCondition(TerminateCondition i_Condition);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		inline bool IsQuitRequested();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		inline void SetQuitRequested( bool i_bQuit );

	private:

		TerminateCondition m_Condition;
		bool m_bQuitRequested;
};


//--------------------------------------------------------------------
//--------------------------------------------------------------------
inline bool 
appMode::IsQuitRequested()
{
	return m_bQuitRequested;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
inline void 
appMode::SetQuitRequested( bool i_bQuit )
{
	m_bQuitRequested = i_bQuit;
}

