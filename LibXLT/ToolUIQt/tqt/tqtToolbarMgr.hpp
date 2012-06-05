/*****************************************************************************
**  tqtToolbarMgr.hpp
**
**      Interface to the toolbar manager.
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
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

#ifdef USE_QT
#include <QtGui/QToolBar>
#include <QtGui/QWidget>
#endif


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
	// Set directory to use to find toolbar icons
	//---------------------------------------------------------------------------
	void SetIconDirectory( std::vector<fsLocator>& i_IconPathList );

	//---------------------------------------------------------------------------
	// Add button with given image to toolbar with given name
	//---------------------------------------------------------------------------
	void AddToolBarButton( int i_Id,
						   const char * i_ToolBarName, 
						   const char * i_ToolStripButton_Image, 
						   const char * i_ToolStripButton_Name,
						   bool i_bCheckable = false);

	//---------------------------------------------------------------------------
	// Set enabled state of toolbar button
	//---------------------------------------------------------------------------
	void EnableButton( int i_Id, bool i_bEnabled );

	//---------------------------------------------------------------------------
	// Set toggled state of toolbar button
	//---------------------------------------------------------------------------
	void CheckButton( int i_Id, bool i_bChecked );

	//---------------------------------------------------------------------------
	// Set Help string to display when mouse is over given tool item
	//---------------------------------------------------------------------------
	void SetToolItemHelpString( int i_ObjectID, const char * i_HelpString );

	//---------------------------------------------------------------------------
	// Creates the actual toolbars. Should be called once all toolbar buttons
	//	have been added during application startup.
	//---------------------------------------------------------------------------
	void RealizeAllToolbars();

	//---------------------------------------------------------------------------
	// Perspectives save the size of toolbars, but when new buttons are added,
	// we need to make sure that the toolbars are big enough to show all of
	// their buttons.
	//---------------------------------------------------------------------------
	void ResizeToolbars();

#ifdef USE_QT
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
#endif
}


