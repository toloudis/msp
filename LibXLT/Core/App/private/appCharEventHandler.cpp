/*****************************************************************************
**  appCharEventHandler.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "Core/app/appCharEventHandler.hpp"

#include <algorithm>

#include "Core/dbg/dbgMsg.hpp"
#include "Core/app/private/appEventHandlers.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
appCharEventHandler::appCharEventHandler()
:	m_Enable(true)
{
	appEventHandlers::CharEventHandlers().push_back(this);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
appCharEventHandler::~appCharEventHandler()
{
	appEventHandlers::appCharEventHandlerList& list = appEventHandlers::CharEventHandlers();
	appEventHandlers::appCharEventHandlerList::iterator to_remove = std::find(list.begin(), list.end(), this);
	list.erase(to_remove);
}

//--------------------------------------------------------------------
//	SetEnableCharEvents can be used to enable or disable char
//	event reporting.
//--------------------------------------------------------------------
void appCharEventHandler::SetEnableCharEvents(bool i_Enable)
{
	m_Enable = i_Enable;
}


