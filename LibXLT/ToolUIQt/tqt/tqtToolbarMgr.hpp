/*****************************************************************************
**  tqtToolbarMgr.hpp
**
**     Manages the toolbars of the main window. Toolbar buttons are the
**	QActions of their menu items (see tqtMenuMgr), so the enabled and
**	checked states are shared with the menu.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef TQT_TOOLBARMGR_HPP
#error tqtToolbarMgr.hpp multiply included
#endif
#define TQT_TOOLBARMGR_HPP

#ifndef TQT_WIDGETS_HPP
#include "ToolUIQt/tqt/tqtWidgets.hpp"
#endif

#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif

#include <vector>

#ifdef USE_QT

//============================================================================
//============================================================================
namespace tqtToolbarMgr
{
	//---------------------------------------------------------------------------
	// Create tool strip grouping with given name
	//---------------------------------------------------------------------------
	void AddToolBar(const char* i_ToolbarName);

	//---------------------------------------------------------------------------
	// Show the toolbar with the given name
	//---------------------------------------------------------------------------
	void Show(const char* i_ToolbarName, bool i_bVisible);

	//---------------------------------------------------------------------------
	// Returns true if the toolbar is currently visible
	//---------------------------------------------------------------------------
	bool IsVisible(const char* i_ToolbarName);

	//---------------------------------------------------------------------------
	// Refreshes a toolbar, use when toggle states change
	//---------------------------------------------------------------------------
	void Refresh(const char* i_ToolbarName);

	//---------------------------------------------------------------------------
	// Set directories to use to find toolbar icons
	//---------------------------------------------------------------------------
	void SetIconDirectory( std::vector<fsLocator>& i_IconPathList );

	//---------------------------------------------------------------------------
	// Add the menu item's action to the toolbar as a button, using the
	//	given image from the icon directories. Returns false if the image
	//	was not found, in which case no button is added.
	//---------------------------------------------------------------------------
	bool AddToolBarButton( QAction* i_pAction,
						   const char * i_ToolBarName,
						   const char * i_ToolStripButton_Image,
						   const char * i_ToolStripButton_Name,
						   bool i_bCheckable = false);

	//---------------------------------------------------------------------------
	// Creates the actual toolbars. Should be called once all toolbar buttons
	//	have been added during application startup.
	//---------------------------------------------------------------------------
	void RealizeAllToolbars();

	//---------------------------------------------------------------------------
	// Make sure the toolbars are big enough to show their buttons after
	// a layout was restored.
	//---------------------------------------------------------------------------
	void ResizeToolbars();

	//---------------------------------------------------------------------------
	// Access to toolbar control, can be used as parent for controls that
	//	will be added to the toolbar.
	//---------------------------------------------------------------------------
	QToolBar* GetToolbarByName(const char* i_ToolbarName);

	//---------------------------------------------------------------------------
	// Add a custom control to the toolbar with the given name
	//---------------------------------------------------------------------------
	void AddControlToToolBar(const char* i_ToolbarName,
							 QWidget* i_pControl);

	//---------------------------------------------------------------------------
	// Forget all toolbars (the main window owns and deletes them)
	//---------------------------------------------------------------------------
	void CleanUp();
}

#endif // USE_QT
