/****************************************************************************\
**	tqcEvent.hpp
**
**		Defines common event types for our custom TQC controls
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef TQC_EVENT_HPP
#error tqcEvent.hpp multiply included
#endif
#define TQC_EVENT_HPP

#ifndef TQT_WIDGETS_HPP
#include "ToolUIQt/tqt/tqtWidgets.hpp"
#endif

#ifdef QT_FINISH_PORT

//============================================================================
// ValueChanged event : TQC controls throw this event type when the
//	value in the control has changed after ENTER is pressed, or the
//	focus leaves the control. Users should register for this event, 
//	not for the individual enter pressed and leave events.
//============================================================================
DECLARE_EVENT_TYPE(wxEVT_VALUE_CHANGED, -1)

#endif // USE_QT
