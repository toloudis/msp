/*****************************************************************************
**  appMouseEventHandler.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "Core/app/appMouseEventHandler.hpp"

#include <algorithm>

#ifndef APP_EVENTHANDLERS_HPP
#include "Core/app/private/appEventHandlers.hpp"
#endif


//--------------------------------------------------------------------
//--------------------------------------------------------------------
appMouseEventHandler::appMouseEventHandler()
:	m_ReceiveMoveEvents(true),
	m_Enable(true)
{
	appEventHandlers::MouseEventHandlers().push_back(this);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
appMouseEventHandler::~appMouseEventHandler()
{
	appEventHandlers::appMouseEventHandlerList& list = appEventHandlers::MouseEventHandlers();
	appEventHandlers::appMouseEventHandlerList::iterator to_remove = std::find(list.begin(), list.end(), this);
	list.erase(to_remove);
}

//--------------------------------------------------------------------
//	Call this function with a true parameter if you want to receive
//	appMouseMoveEvents, or false if you don't.  The default is true.
//--------------------------------------------------------------------
void appMouseEventHandler::SetReceiveMoveEvents(bool i_Receive)
{
	m_ReceiveMoveEvents = i_Receive;
}

//--------------------------------------------------------------------
//	This will return true if mouse move events will be given to the
//	event handler.
//--------------------------------------------------------------------
bool appMouseEventHandler::ReceiveMoveEvents() const		
{
	return m_ReceiveMoveEvents;
}

//--------------------------------------------------------------------
//	SetEnableMouseEvents can be used to enable or disable mouse
//	event reporting.
//--------------------------------------------------------------------
void appMouseEventHandler::SetEnableMouseEvents(bool i_Enable)
{
	m_Enable = i_Enable;
}


