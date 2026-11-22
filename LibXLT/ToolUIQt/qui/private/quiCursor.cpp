/****************************************************************************\
**	quiCursor.cpp
**
**	class to invoke the Qt wait cursor
**	
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/qui/quiCursor.hpp"
#include "ToolUIQt/tqt/tqtCursor.hpp"



void quiCursor::SetWaitCursor()
{
#ifdef USE_QT
	tqtCursor::SetWaitCursor();
#endif
}
		
void quiCursor::EndWaitCursor()
{
#ifdef USE_QT
	tqtCursor::EndWaitCursor();
#endif
}

