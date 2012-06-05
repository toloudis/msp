/*****************************************************************************
**  muiToolbarMgr.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "ToolUIManaged/mui/muiToolbarMgr.hpp"

#include "ToolUIManaged/tma/tmaToolBarMgr.hpp"


//===========================================================================
//===========================================================================
muiToolbarMgr::ResizeMainWindowFunction muiToolbarMgr::sm_ResizeFunction = NULL;

//===========================================================================
//	muiToolbarMgr functions
//===========================================================================

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
muiToolbarMgr::muiToolbarMgr()
{
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
muiToolbarMgr::~muiToolbarMgr()
{
}


//---------------------------------------------------------------------------
// Create tool strip grouping with given name
//---------------------------------------------------------------------------
//virtual 
void muiToolbarMgr::Create(const char* i_ToolbarName)
{
}

//---------------------------------------------------------------------------
// Show the toolbar with the given name
//---------------------------------------------------------------------------
//virtual 
void muiToolbarMgr::Show(const char* i_ToolbarName, bool i_bVisible)
{
}

//---------------------------------------------------------------------------
// Returns true if the toolbar is currently visible
//---------------------------------------------------------------------------
//virtual 
bool muiToolbarMgr::IsVisible(const char* i_ToolbarName)
{
	return false;
}

//---------------------------------------------------------------------------
// Refreshes a toolbar, use when toggle states change
//---------------------------------------------------------------------------
//virtual 
void muiToolbarMgr::Refresh(const char* i_ToolbarName)
{

}

//---------------------------------------------------------------------------
// In the managed application, the main window needs to be resized when 
// the toolbar visibility changes.
//---------------------------------------------------------------------------
//static 
void muiToolbarMgr::SetResizeMainWindowFunction(ResizeMainWindowFunction i_ResizeFunction)
{
	sm_ResizeFunction = i_ResizeFunction;
}
