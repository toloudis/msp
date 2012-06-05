/****************************************************************************\
**	quiCursor.cpp
**
**	class to invloke wxCursor
**	
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/qui/quiCursor.hpp"
#include "ToolUIQt/tqt/tqtCursor.hpp"



void quiCursor::SetWaitCursor()
{
#ifdef QT_FINISH_PORT
	tqtCursor::SetWaitCursor();
#endif
}
		
void quiCursor::EndWaitCursor()
{
#ifdef QT_FINISH_PORT
	tqtCursor::EndWaitCursor();
#endif
}

