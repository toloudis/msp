/*****************************************************************************
**	guiToolbarMgr.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Tool/gui/guiToolbarMgr.hpp"

#include "Core/dbg/dbgMsg.hpp"


//---------------------------------------------------------------------------
// Create tool strip grouping with given name
//---------------------------------------------------------------------------
void guiToolbarMgr::Create(const char* i_ToolbarName)
{
	DBG_ASSERT(sm_pImplementation, "guiToolbarMgr: No implementation");
	if (sm_pImplementation)
	{
		sm_pImplementation->Create(i_ToolbarName);
	}
}

//---------------------------------------------------------------------------
// Show the toolbar with the given name
//---------------------------------------------------------------------------
void guiToolbarMgr::Show(const char* i_ToolbarName, bool i_bVisible)
{
	DBG_ASSERT(sm_pImplementation, "guiToolbarMgr: No implementation");
	if (sm_pImplementation)
	{
		sm_pImplementation->Show(i_ToolbarName, i_bVisible);
	}
}

//---------------------------------------------------------------------------
// Returns true if the toolbar is currently visible
//---------------------------------------------------------------------------
bool guiToolbarMgr::IsVisible(const char* i_ToolbarName)
{
	DBG_ASSERT(sm_pImplementation, "guiToolbarMgr: No implementation");
	if (sm_pImplementation)
	{
		return sm_pImplementation->IsVisible(i_ToolbarName);
	}
	return false;
}


//---------------------------------------------------------------------------
// Refreshes a toolbar, use when toggle states change
//---------------------------------------------------------------------------
void guiToolbarMgr::Refresh(const char* i_ToolbarName)
{
	DBG_ASSERT(sm_pImplementation, "guiToolbarMgr: No implementation");
	if (sm_pImplementation)
	{
		sm_pImplementation->Refresh(i_ToolbarName);
	}
}
