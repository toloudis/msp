/****************************************************************************\
**	tqtCursor.cpp
**
**	class to invloke wxCursor
**	
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef TQT_CURSOR_HPP
#error tqtCursor.hpp multiply included
#endif
#define TQT_CURSOR_HPP

#ifndef TQT_WIDGETS_HPP
#include "ToolUIQt/tqt/tqtWidgets.hpp"
#endif

#ifdef QT_FINISH_PORT

namespace tqtCursor
{
	void SetWaitCursor();
	void EndWaitCursor();
}
#endif
