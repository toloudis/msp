/*****************************************************************************
**  quiToolbarMgr.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#include "ToolUIQt/qui/quiToolbarMgr.hpp"

#include "ToolUIQt/tqt/tqtToolbarMgr.hpp"


//===========================================================================
//	quiToolbarMgr functions
//===========================================================================

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
quiToolbarMgr::quiToolbarMgr()
{
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
quiToolbarMgr::~quiToolbarMgr()
{
}


//---------------------------------------------------------------------------
// Create tool strip grouping with given name
//---------------------------------------------------------------------------
//virtual 
void quiToolbarMgr::Create(const char* i_ToolbarName)
{
#ifdef QT_FINISH_PORT
	tqtToolbarMgr::AddToolBar(i_ToolbarName);
#endif // USE_QT
}

//---------------------------------------------------------------------------
// Show the toolbar with the given name
//---------------------------------------------------------------------------
//virtual 
void quiToolbarMgr::Show(const char* i_ToolbarName, bool i_bVisible)
{
#ifdef QT_FINISH_PORT
	tqtToolbarMgr::Show(i_ToolbarName, i_bVisible);
#endif // USE_QT
}

//---------------------------------------------------------------------------
// Returns true if the toolbar is currently visible
//---------------------------------------------------------------------------
//virtual 
bool quiToolbarMgr::IsVisible(const char* i_ToolbarName)
{
#ifdef QT_FINISH_PORT
	return tqtToolbarMgr::IsVisible(i_ToolbarName);
#endif // USE_QT
	return false;
}

//---------------------------------------------------------------------------
// Refreshes a toolbar, use when toggle states change
//---------------------------------------------------------------------------
//virtual 
void quiToolbarMgr::Refresh(const char* i_ToolbarName)
{
#ifdef QT_FINISH_PORT
	tqtToolbarMgr::Refresh(i_ToolbarName);
#endif // USE_QT
}
