/*****************************************************************************
**  appMode.cpp
**
**      See .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Core/app/appMode.hpp"

#include "Core/app/appSimTime.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
appMode::appMode()
:	m_bQuitRequested(false)
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
appMode::~appMode()
{
}

//--------------------------------------------------------------------
//	The mode should do it's per frame "work" in the Think function.
//--------------------------------------------------------------------
void appMode::Think()
{
	appSimTime::IncrementTime();
}

//--------------------------------------------------------------------
//	Initialize will be called before the first call of Think after
//	the object is first created or DeInitialized.  During the
//	lifetime of a mode, Initialize and DeInitialize may be called
//	several times.  Children of appMode should remember to call
//	appMode::Initialize() at the beginning of their Initialize
//	function.
//--------------------------------------------------------------------
void appMode::Initialize()
{
	m_Condition = e_Continue;
	m_bQuitRequested = false;
}

//--------------------------------------------------------------------
//	The object should clean up things that are not needed while the
//	mode is not running in the DeInitialize function.
//--------------------------------------------------------------------
void appMode::DeInitialize()
{
}


//--------------------------------------------------------------------
//	GetTerminateCondition returns whichever terminate rondition is
//	requested by the mode.
//--------------------------------------------------------------------
appMode::TerminateCondition appMode::GetTerminateCondition() const
{
	return m_Condition;
}

//--------------------------------------------------------------------
//	SetTerminateCondition should be called when a mode is ready to
//	end (it's not neccesary to call it to set "e_Continue", as this
//	is the default).  The mode will
//--------------------------------------------------------------------
void appMode::SetTerminateCondition(TerminateCondition i_Condition)
{
	m_Condition = i_Condition;
}

//--------------------------------------------------------------------
//	Override this function to get appQuitRequestedEvents.
//--------------------------------------------------------------------
void appMode::ReceiveQuitRequestEvent(appQuitRequestEvent& i_Event)
{
	m_bQuitRequested = true;
}
