/*****************************************************************************
**  appFlowEventHandler.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "Core/app/appFlowEventHandler.hpp"

#include <algorithm>

#ifndef APP_EVENTHANDLERS_HPP
#include "Core/app/private/appEventHandlers.hpp"
#endif


//--------------------------------------------------------------------
//--------------------------------------------------------------------
appFlowEventHandler::appFlowEventHandler()
:	m_Enable(true)
{
	appEventHandlers::FlowEventHandlers().push_back(this);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
appFlowEventHandler::~appFlowEventHandler()
{
	appEventHandlers::appFlowEventHandlerList& list = appEventHandlers::FlowEventHandlers();
	appEventHandlers::appFlowEventHandlerList::iterator to_remove = std::find(list.begin(), list.end(), this);
	list.erase(to_remove);
}

//--------------------------------------------------------------------
//	SetEnableFlowEvents can be used to enable or disable char
//	event reporting.
//--------------------------------------------------------------------
void appFlowEventHandler::SetEnableFlowEvents(bool i_Enable)
{
	m_Enable = i_Enable;
}

//--------------------------------------------------------------------
//	Override this function to get appStartEvents.
//--------------------------------------------------------------------
void appFlowEventHandler::ReceiveStartEvent(appStartEvent& i_Event)
{
}

//--------------------------------------------------------------------
//	Override this function to get appStopEvents.
//--------------------------------------------------------------------
void appFlowEventHandler::ReceiveStopEvent(appStopEvent& i_Event)
{
}

//--------------------------------------------------------------------
//	Override this function to get appSuspendEvents.
//--------------------------------------------------------------------
void appFlowEventHandler::ReceiveSuspendEvent(appSuspendEvent& i_Event)
{
}

//--------------------------------------------------------------------
//	Override this function to get appResumeEvents.
//--------------------------------------------------------------------
void appFlowEventHandler::ReceiveResumeEvent(appResumeEvent& i_Event)
{
}

//--------------------------------------------------------------------
//	Override this function to get appQuitReqeustedEvents.
//--------------------------------------------------------------------
void appFlowEventHandler::ReceiveQuitRequestEvent(appQuitRequestEvent& i_Event)
{
}


