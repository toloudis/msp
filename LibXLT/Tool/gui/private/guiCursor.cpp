/*****************************************************************************
**	guiCursor.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Tool/gui/guiCursor.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void guiCursor::SetWaitCursor()
{
	if (sm_pImplementation)
	{
		sm_pImplementation->SetWaitCursor();
	}
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void guiCursor::EndWaitCursor()
{
	if (sm_pImplementation)
	{
		sm_pImplementation->EndWaitCursor();
	}
}
