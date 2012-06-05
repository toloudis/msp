/*****************************************************************************
**  appEventHandlers.hpp
**
**      appEventHandlers is a private component of the app package which
**	is used to list the event handlers, so that other components can pass
**	events to them.  The client never needs to do anything with this
**	component.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef APP_EVENTHANDLERS_HPP
#error appEventHandlers.hpp multiply included
#endif
#define APP_EVENTHANDLERS_HPP

#include <vector>
#include <list>

#ifndef APP_CHAREVENTHANDLER_HPP
#include "Core/app/appCharEventHandler.hpp"
#endif

#ifndef APP_MOUSEEVENTHANDLER_HPP
#include "Core/app/appMouseEventHandler.hpp"
#endif

#ifndef APP_FLOWEVENTHANDLER_HPP
#include "Core/app/appFlowEventHandler.hpp"
#endif


//========================================================================
//========================================================================
template<class T>
class appEventHandlerList;


//========================================================================
//	appEventHandlers contains functions to get the various handler lists.
//========================================================================
namespace appEventHandlers
{
	//--------------------------------------------------------------------
	//	appCharEventHandlerList is the type of the list
	//--------------------------------------------------------------------
	typedef std::vector<appCharEventHandler*> appCharEventHandlerList;

	//--------------------------------------------------------------------
	//	CharEventHandlers returns the list of CharEventHandlers.
	//--------------------------------------------------------------------
	appCharEventHandlerList& CharEventHandlers();

	//--------------------------------------------------------------------
	//	appCharEventHandlerList is the type of the list
	//--------------------------------------------------------------------
	typedef std::vector<appMouseEventHandler*> appMouseEventHandlerList;

	//--------------------------------------------------------------------
	//	MouseEventHandlers returns the list of MouseEventHandlers.
	//--------------------------------------------------------------------
	appMouseEventHandlerList& MouseEventHandlers();

	//--------------------------------------------------------------------
	//	appFlowEventHandlerList is the type of the list
	//--------------------------------------------------------------------
	typedef std::list<appFlowEventHandler*> appFlowEventHandlerList;

	//--------------------------------------------------------------------
	//	FlowEventHandlers returns the list of MouseEventHandlers.
	//--------------------------------------------------------------------
	appFlowEventHandlerList& FlowEventHandlers();
}
