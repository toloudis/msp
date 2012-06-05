/****************************************************************************\
**	twxCursor.cpp
**
**	class to invloke wxCursor
**	
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef TWX_CURSOR_HPP
#error twxCursor.hpp multiply included
#endif
#define TWX_CURSOR_HPP

#ifndef TWX_WIDGETS_HPP
#include "ToolUIWx/twx/twxWidgets.hpp"
#endif

#ifdef USE_WXWIDGETS

namespace twxCursor
{
	void SetWaitCursor();
	void EndWaitCursor();
}
#endif
