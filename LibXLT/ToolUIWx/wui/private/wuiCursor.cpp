/****************************************************************************\
**	wuiCursor.cpp
**
**	class to invloke wxCursor
**	
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIWx/wui/wuiCursor.hpp"
#include "ToolUIWx/twx/twxCursor.hpp"



void wuiCursor::SetWaitCursor()
{
#ifdef USE_WXWIDGETS
	twxCursor::SetWaitCursor();
#endif
}
		
void wuiCursor::EndWaitCursor()
{
#ifdef USE_WXWIDGETS
	twxCursor::EndWaitCursor();
#endif
}

