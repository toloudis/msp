/*****************************************************************************
**  wuiToolbarMgr.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#include "ToolUIWx/wui/wuiToolbarMgr.hpp"

#include "ToolUIWx/twx/twxToolbarMgr.hpp"


//===========================================================================
//	wuiToolbarMgr functions
//===========================================================================

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
wuiToolbarMgr::wuiToolbarMgr()
{
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
wuiToolbarMgr::~wuiToolbarMgr()
{
}


//---------------------------------------------------------------------------
// Create tool strip grouping with given name
//---------------------------------------------------------------------------
//virtual 
void wuiToolbarMgr::Create(const char* i_ToolbarName)
{
#ifdef USE_WXWIDGETS
	twxToolbarMgr::AddToolBar(i_ToolbarName);
#endif // USE_WXWIDGETS
}

//---------------------------------------------------------------------------
// Show the toolbar with the given name
//---------------------------------------------------------------------------
//virtual 
void wuiToolbarMgr::Show(const char* i_ToolbarName, bool i_bVisible)
{
#ifdef USE_WXWIDGETS
	twxToolbarMgr::Show(i_ToolbarName, i_bVisible);
#endif // USE_WXWIDGETS
}

//---------------------------------------------------------------------------
// Returns true if the toolbar is currently visible
//---------------------------------------------------------------------------
//virtual 
bool wuiToolbarMgr::IsVisible(const char* i_ToolbarName)
{
#ifdef USE_WXWIDGETS
	return twxToolbarMgr::IsVisible(i_ToolbarName);
#endif // USE_WXWIDGETS
	return false;
}

//---------------------------------------------------------------------------
// Refreshes a toolbar, use when toggle states change
//---------------------------------------------------------------------------
//virtual 
void wuiToolbarMgr::Refresh(const char* i_ToolbarName)
{
#ifdef USE_WXWIDGETS
	twxToolbarMgr::Refresh(i_ToolbarName);
#endif // USE_WXWIDGETS
}
