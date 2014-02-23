/****************************************************************************\
**	twcEvent.hpp
**
**		Defines common event types for our custom TWC controls
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef TWC_EVENT_HPP
#error twcEvent.hpp multiply included
#endif
#define TWC_EVENT_HPP

#ifndef TWX_WIDGETS_HPP
#include "ToolUIWx/twx/twxWidgets.hpp"
#endif

#ifdef USE_WXWIDGETS

//============================================================================
// ValueChanged event : TWC controls throw this event type when the
//	value in the control has changed after ENTER is pressed, or the
//	focus leaves the control. Users should register for this event, 
//	not for the individual enter pressed and leave events.
//============================================================================
DECLARE_LOCAL_EVENT_TYPE(wxEVT_VALUE_CHANGED, -1)

#endif // USE_WXWIDGETS
