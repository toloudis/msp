/*****************************************************************************
**  muiToolbarMgr.hpp
**
**      A non-managed interface to the managed menu manager.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef MUI_TOOLBARMGR_HPP
#error muiToolbarMgr.hpp multiply included
#endif
#define MUI_TOOLBARMGR_HPP

#ifndef GUI_TOOLBARMGR_HPP
#include "Tool/gui/guiToolbarMgr.hpp"
#endif

//============================================================================
//============================================================================
class muiToolbarMgr : public guiToolbarMgrImpl
{
public:
	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	muiToolbarMgr();

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	~muiToolbarMgr();

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

	//---------------------------------------------------------------------------
	// In the managed application, the main window needs to be resized when 
	// the toolbar visibility changes.
	//---------------------------------------------------------------------------
	typedef void (*ResizeMainWindowFunction)();
	static void SetResizeMainWindowFunction(ResizeMainWindowFunction i_ResizeFunction);

private:
	static ResizeMainWindowFunction sm_ResizeFunction;
};
