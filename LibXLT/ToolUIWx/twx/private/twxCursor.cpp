/****************************************************************************\
**	twxCursor.cpp
**
**	class to invloke wxCursor
**	
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#include "ToolUIWx/twx/twxCursor.hpp"

#ifdef USE_WXWIDGETS

void twxCursor::SetWaitCursor()
{
	wxSetCursor(*wxHOURGLASS_CURSOR);
}

void twxCursor::EndWaitCursor()
{
	wxSetCursor(wxNullCursor);
}

#endif
