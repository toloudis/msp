/*****************************************************************************
**  quiToolbarMgr.hpp
**
**      wxWidgets implementation of toolbar manager
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef QUI_TOOLBARMGR_HPP
#error quiToolbarMgr.hpp multiply included
#endif
#define QUI_TOOLBARMGR_HPP

#ifndef GUI_TOOLBARMGR_HPP
#include "Tool/gui/guiToolbarMgr.hpp"
#endif

//============================================================================
//============================================================================
class quiToolbarMgr : public guiToolbarMgrImpl
{
public:
	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	quiToolbarMgr();

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	~quiToolbarMgr();

	//---------------------------------------------------------------------------
	// Create tool strip grouping with given name
	//---------------------------------------------------------------------------
	virtual void Create(const char* i_ToolbarName);

	//---------------------------------------------------------------------------
	// Show the toolbar with the given name
	//---------------------------------------------------------------------------
	virtual void Show(const char* i_ToolbarName, bool i_bVisible);

	//---------------------------------------------------------------------------
	// Returns true if the toolbar is currently visible
	//---------------------------------------------------------------------------
	virtual bool IsVisible(const char* i_ToolbarName);

	//---------------------------------------------------------------------------
	// Refreshes a toolbar, use when toggle states change
	//---------------------------------------------------------------------------
	virtual void Refresh(const char* i_ToolbarName);

};
