/****************************************************************************\
**  appApplicationPACPS2.cpp
**
**      itLocaleUtilPACPS2.cpp defines the appApplication class 
**	PAC for the PS2.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "appApplicationPACPS2.hpp"

#include "appApplication.hpp"
#include "appCharEvent.hpp"
#include "appCharEventHandler.hpp"
#include "appEventHandlers.hpp"
#include "appFlowEvent.hpp"
#include "appMouseEvent.hpp"

#include <algorithm>

//============================================================================
//	anonymous namespace for local data/functions
//============================================================================

namespace
{

appApplicationPAC* l_App = NULL;
bool l_PostedStartEvent = false;
bool l_Suspended = false;

//============================================================================
//============================================================================
void PostStartEvent()
{
	appEventHandlers::appFlowEventHandlerList& sorted_list = appEventHandlers::FlowEventHandlers();
	appEventHandlers::appFlowEventHandlerList::iterator it = sorted_list.begin();
	appEventHandlers::appFlowEventHandlerList::iterator end = sorted_list.end();

	appStartEvent event;

	while ( it != end )
	{
		if( (*it)->GetEnableFlowEvents() )
		{
			(*it)->ReceiveStartEvent(event);

			if ( event.WasConsumed() )
					break;
		}

		it++;
	}

	l_PostedStartEvent = true;
}

//============================================================================
//============================================================================
void PostStopEvent()
{
	appEventHandlers::appFlowEventHandlerList& sorted_list = appEventHandlers::FlowEventHandlers();
	appEventHandlers::appFlowEventHandlerList::iterator it = sorted_list.begin();
	appEventHandlers::appFlowEventHandlerList::iterator end = sorted_list.end();

	appStopEvent event;

	while ( it != end )
	{
		if( (*it)->GetEnableFlowEvents() )
		{
			(*it)->ReceiveStopEvent(event);

			if ( event.WasConsumed() )
				break;
		}

		++it;
	}
}

//============================================================================
//============================================================================
void PostSuspendEvent()
{
	// We don't want to suspend before we start.
	if( !l_PostedStartEvent ) return;

	appEventHandlers::appFlowEventHandlerList& sorted_list = appEventHandlers::FlowEventHandlers();
	appEventHandlers::appFlowEventHandlerList::iterator it = sorted_list.begin();
	appEventHandlers::appFlowEventHandlerList::iterator end = sorted_list.end();

	appSuspendEvent event;

	while ( it != end )
	{
		if( (*it)->GetEnableFlowEvents() )
		{
			(*it)->ReceiveSuspendEvent(event);

			if ( event.WasConsumed() )
				break;
		}

		it++;
	}
}

//============================================================================
//============================================================================
void PostResumeEvent()
{
	// We don't want to resume before we start!
	if( !l_PostedStartEvent ) return;

	appEventHandlers::appFlowEventHandlerList& sorted_list = appEventHandlers::FlowEventHandlers();
	appEventHandlers::appFlowEventHandlerList::iterator it = sorted_list.begin();
	appEventHandlers::appFlowEventHandlerList::iterator end = sorted_list.end();

	appResumeEvent event;

	while ( it != end )
	{
		if( (*it)->GetEnableFlowEvents() )
		{
			(*it)->ReceiveResumeEvent(event);

			if ( event.WasConsumed() )
				break;
		}

		it++;
	}
}

//============================================================================
//============================================================================
void PostQuitRequestEvent()
{
	// We don't want to quit before we start!
	if( !l_PostedStartEvent ) return;

	appEventHandlers::appFlowEventHandlerList& sorted_list = appEventHandlers::FlowEventHandlers();
	appEventHandlers::appFlowEventHandlerList::iterator it = sorted_list.begin();
	appEventHandlers::appFlowEventHandlerList::iterator end = sorted_list.end();

	appQuitRequestEvent event;

	while ( it != end )
	{
		if( (*it)->GetEnableFlowEvents() )
		{
			(*it)->ReceiveQuitRequestEvent(event);

			if ( event.WasConsumed() )
				break;
		}

		it++;
	}
}

}


//====================================================================
//====================================================================
appApplicationPAC::appApplicationPAC()
{
	DBG_ASSERT0(l_App == NULL, "Tried to create two simultaneous applications");
	l_App = this;
	m_Quit = false;
}

//====================================================================
//====================================================================
appApplicationPAC::~appApplicationPAC()
{
	l_App = NULL;
}

//====================================================================
//	Run() is called to start the application.  When control
//	returns from Run(), the application is finished.  The
//	appApplication* is used to call the Think() function of the
//	child appApplication.
//====================================================================
void appApplicationPAC::Run(appApplication* i_App)
{
	// Post the Start event, just before we start the regular message loop
	//
	m_Quit = false;
	PostStartEvent();

	//	Application game loop
	//
	while( !m_Quit )
		i_App->Think();

	//	At this point, we are finishing the application, so post the stop event
	//
	PostStopEvent();
}

//====================================================================
//	Exit() is called by the client when it decides it is
//	ready to end the program.
//====================================================================
void appApplicationPAC::Exit()
{
	m_Quit = true;
}

//====================================================================
//	SetWindowSize is used to resize the application window.  This
//	function can be called from a appStartEvent handler, before the
//	window has been created.  The width and height refer to the client
//	area of the window (the part inside the title bar and border).
//====================================================================
void appApplicationPAC::SetWindowSize(int i_X, int i_Y, int i_Width, int i_Height)
{
	//	nop - no windowed mode
}

//====================================================================
//	SetWindowTitle is used to set the text that appears in the
//	window's title bar.  This function can be called from a 
//	appStartEvent handler, before the window has been created. 
//====================================================================
void appApplicationPAC::SetWindowTitle(const itString& i_Title)
{
	//	nop - no window title
}

//====================================================================
//	The windows appApplicationPAC defines this Instance function
//	for use by other Windows PAC components. 
//====================================================================
appApplicationPAC* appApplicationPAC::Instance()
{
	return l_App;
}

//====================================================================
//	IsSuspended is true if the application doesn't have focus
//====================================================================
bool appApplicationPAC::IsSuspended()
{
	return false;	//	PS2 can't lose focus
}

//========================================================================
//	Don't call Init() and CleanUp() yourself; they are called 
//	by the package Init and Cleanup.
//========================================================================
void appApplicationPAC::Init()
{
}

void appApplicationPAC::CleanUp() throw()
{
}

//========================================================================
//	DialogLoopTasks should be called by things which use a tight loop
//	(render multiple times without returning control from
//	appApplication::Think) so that OS tasks can continue to be updated -
//	for instance, windows message handling.
//========================================================================
void appApplicationPAC::DialogLoopTasks()
{
	//	nothing to do here at the moment
}
