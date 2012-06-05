/****************************************************************************\
**	tqtCursor.cpp
**
**	class to invloke wxCursor
**	
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#include "ToolUIQt/tqt/tqtCursor.hpp"

#ifdef QT_FINISH_PORT

void tqtCursor::SetWaitCursor()
{
	wxSetCursor(*wxHOURGLASS_CURSOR);
}

void tqtCursor::EndWaitCursor()
{
	wxSetCursor(wxNullCursor);
}

#endif
