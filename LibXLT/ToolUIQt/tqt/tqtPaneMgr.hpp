/*****************************************************************************
**	tqtPaneMgr.hpp
**
**		Interface to the AUI manager for dockable content panes
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef TQT_PANEMGR_HPP
#error tqtPaneMgr.hpp multiply included
#endif
#define TQT_PANEMGR_HPP

#ifndef TQT_WIDGETS_HPP
#include "ToolUIQt/tqt/tqtWidgets.hpp"
#endif

#ifdef USE_QT

#include <QtGui/QWidget>


//============================================================================
//============================================================================
namespace tqtPaneMgr
{
	//---------------------------------------------------------------------------
	// Create the pane manager
	//---------------------------------------------------------------------------
	void Init();

	//---------------------------------------------------------------------------
	// Shut down the pane management
	//---------------------------------------------------------------------------
	void CleanUp();

#ifdef QT_FINISH_PORT
	//---------------------------------------------------------------------------
	// Add new pane with given info
	//---------------------------------------------------------------------------
	void AddPane(QWidget *i_pWindow, const wxAuiPaneInfo &i_PaneInfo);
#endif

	//---------------------------------------------------------------------------
	// Refresh panes in situations where the frame size changed without
	// the pane manager knowing (i.e. when status bar changes visibility)
	//---------------------------------------------------------------------------
	void Update();

	//---------------------------------------------------------------------------
	// Show the pane with the given name or window.
	// To Hide the pane, call with i_bShow==false
	//---------------------------------------------------------------------------
	void Show(const char* i_PaneName, bool i_bShow = true);
	void Show(QWidget *i_pWindow, bool i_bShow = true);

	//---------------------------------------------------------------------------
	// Return true if theis pane is visible currently
	//---------------------------------------------------------------------------
	bool IsVisible(const char* i_PaneName);
	bool IsVisible(QWidget *i_pWindow);

	//---------------------------------------------------------------------------
	// SetCaption - alter caption for pane
	//---------------------------------------------------------------------------
	void SetCaption(QWidget *i_pWindow, const std::string& i_Caption);

	//---------------------------------------------------------------------------
	// Resize the pane for the given window
	//---------------------------------------------------------------------------
	void ResizePane(QWidget *i_pWindow, int i_Width, int i_Height);

	//---------------------------------------------------------------------------
	// A Perspective is a string that represents the configuration of panes.
	//	Using these functions the layout can be saved and restored.
	//---------------------------------------------------------------------------
	bool GetCurrentPerspective(std::string& o_Perspective);
	void SetCurrentPerspective(const std::string& i_Perspective);
}

#endif // USE_QT