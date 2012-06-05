/*****************************************************************************
**  twxPaneMgr.hpp
**
**      Interface to the AUI manager for dockable content panes
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef TWX_PANEMGR_HPP
#error twxPaneMgr.hpp multiply included
#endif
#define TWX_PANEMGR_HPP

#ifndef TWX_WIDGETS_HPP
#include "ToolUIWx/twx/twxWidgets.hpp"
#endif


#ifdef USE_WXWIDGETS

#include <wx/aui/aui.h>

//============================================================================
//============================================================================
namespace twxPaneMgr 
{
	//---------------------------------------------------------------------------
	// Create the pane manager
	//---------------------------------------------------------------------------
	void Init();

	//---------------------------------------------------------------------------
	// Shut down the pane management
	//---------------------------------------------------------------------------
	void CleanUp();

	//---------------------------------------------------------------------------
	// Add new pane with given info
	//---------------------------------------------------------------------------
	void AddPane(wxWindow *i_pWindow, const wxAuiPaneInfo &i_PaneInfo);

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
	void Show(wxWindow *i_pWindow, bool i_bShow = true);

	//---------------------------------------------------------------------------
	// Return true if theis pane is visible currently
	//---------------------------------------------------------------------------
	bool IsVisible(const char* i_PaneName);
	bool IsVisible(wxWindow *i_pWindow);

	//---------------------------------------------------------------------------
	// SetCaption - alter caption for pane
	//---------------------------------------------------------------------------
	void SetCaption(wxWindow *i_pWindow, const std::string& i_Caption);

	//---------------------------------------------------------------------------
	// Resize the pane for the given window
	//---------------------------------------------------------------------------
	void ResizePane(wxWindow *i_pWindow, int i_Width, int i_Height);

	//---------------------------------------------------------------------------
	// A Perspective is a string that represents the configuration of panes.
	//	Using these functions the layout can be saved and restored.
	//---------------------------------------------------------------------------
	bool GetCurrentPerspective(std::string& o_Perspective);
	void SetCurrentPerspective(const std::string& i_Perspective);
}

#endif // USE_WXWIDGETS