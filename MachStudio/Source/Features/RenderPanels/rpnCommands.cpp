/*****************************************************************************
**  rpnCommands.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Features/RenderPanels/rpnCommands.hpp"

#include "Features/RenderPanels/rpnCommandChangeLayout.hpp"
#include "Features/RenderPanels/rpnCommandFocusCamera.hpp"
#include "Features/RenderPanels/rpnCommandFocusAllPanels.hpp"
#include "Features/RenderPanels/GUI/rpnDialogUtil.hpp"
#include "Features/RenderPanels/rpnOperations.hpp"
#include "Features/RenderPanels/wxGUI/rpnPanelGrid.hpp"

#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"


//	tools
#include "Tool/gui/guiCommandMgr.hpp"
#include "Tool/cma/cmaCommandSimple.hpp"
#include "Tool/cma/cmaCommandToggle.hpp"
#include "Tool/gui/guiMenuMgr.hpp"
#include "Tool/gui/guiToolbarMgr.hpp"


namespace
{
	const char* c_MenuName = "Panel Layouts";

	void add_menu_item(rpnCommandChangeLayout::LayoutStyle i_Style,
					   const char* i_Description)
	{
		int menuID = guiMenuMgr::AddMenuItem( c_MenuName, i_Description );
		cmaCommand* pCmd = new rpnCommandChangeLayout( i_Style, i_Description );
		guiCommandMgr::Add( pCmd, 
							rpnCommandChangeLayout::GetConstTagName(), 
							menuID );

	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Set_CamerasToolbar(bool i_bChecked)
	{
		guiToolbarMgr::Show(rpnDialogUtil::GetCamerasToolbarName(), i_bChecked);
	}
	bool Get_CamerasToolbar()
	{
		return guiToolbarMgr::IsVisible(rpnDialogUtil::GetCamerasToolbarName());
	}
}

//============================================================================
//============================================================================
namespace rpnCommands
{

	//--------------------------------------------------------------------
	// SetupMenu
	//--------------------------------------------------------------------
	void SetupMenu()
	{
		// Create commands to enable hot keys
		int menu_id;
		cmaCommand* pCmd = new cmaCommandSimple("Set Edit Cam", 
			"Cameras",
			"Move editor camera to current view",
			rpnOperations::SetEditCam);
#ifdef USE_WXWIDGETS
		guiMenuMgr::AddMenu("Actions", "Cameras");
		menu_id = guiMenuMgr::AddMenuItem( "Cameras", pCmd->GetTag().c_str(), 
			rpnDialogUtil::GetCamerasToolbarName(), "camera-seteditcam.png" ); 
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
#else
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), 1010, false );
#endif
		pCmd = new cmaCommandSimple("Swap Camera", 
			"Cameras",
			"Switch between editor camera and last selected scripted camera",
			rpnDialogUtil::DoSwapCam);
#ifdef USE_WXWIDGETS
		menu_id = guiMenuMgr::AddMenuItem( "Cameras", pCmd->GetTag().c_str(), 
			rpnDialogUtil::GetCamerasToolbarName(), "camera-swapcamera.png" ); 
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
#else
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), 1011, false );
#endif
		pCmd = new cmaCommandSimple("Previous Camera", 
			"Cameras",
			"Move to previous camera in list",
			&rpnOperations::PreviousCamera);
		menu_id = guiMenuMgr::AddMenuItem( "Cameras", pCmd->GetTag().c_str() ); 
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );

		pCmd = new cmaCommandSimple("Next Camera", 
			"Cameras",
			"Move to next camera in list",
			&rpnOperations::NextCamera);
		menu_id = guiMenuMgr::AddMenuItem( "Cameras", pCmd->GetTag().c_str() ); 
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );

		pCmd = new cmaCommandSimple("Select Active Camera", 
			"Cameras",
			"Select camera in the current view",
			&rpnOperations::SelectActiveCamera);
		menu_id = guiMenuMgr::AddMenuItem( "Cameras", pCmd->GetTag().c_str() ); 
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );

		// Create menu of layout choices
		guiMenuMgr::AddMenu( "View", c_MenuName );
		add_menu_item(rpnCommandChangeLayout::e_SinglePane, "Single Pane");
		add_menu_item(rpnCommandChangeLayout::e_TwoStacked, "Two Panes Stacked");
		add_menu_item(rpnCommandChangeLayout::e_TwoSideBySide, "Two Panes Side by Side");
		add_menu_item(rpnCommandChangeLayout::e_FourPanels, "Four Panels");

		//	COMMAND: Cameras toolbar
		guiMenuMgr::AddMenu( "View", "Toolbars" );
		pCmd = new cmaCommandToggle("Cameras Toolbar", 
									"Toolbars", 
									"View the camera choice toolbar",
									&Set_CamerasToolbar, 
									&Get_CamerasToolbar );
		menu_id = guiMenuMgr::AddCheckableMenuItem( "Toolbars", pCmd->GetTag().c_str() );
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
		cmmSystemDialogUtil::AddSystemCommand( "Main", "View Cameras Toolbar", pCmd );

		// COMMAND: Focus Camera
		guiMenuMgr::AddMenu( "View", "Focus Camera");
		pCmd = new rpnCommandFocusCamera("Focus Camera");
		menu_id = guiMenuMgr::AddMenuItem( "Focus Camera", "Focus the Active Camera" );
		
		guiCommandMgr::Add( pCmd, 
							rpnCommandFocusCamera::GetConstTagName(), 
							menu_id );

		// COMMAND: Focus Camera in all panels
		guiMenuMgr::AddMenu( "View", "Focus Camera");
		pCmd = new rpnCommandFocusAllPanels("Focus all Panel Cameras");
		menu_id = guiMenuMgr::AddMenuItem( "Focus Camera", "Focus all Panel Cameras" );
		
		guiCommandMgr::Add( pCmd, 
							rpnCommandFocusAllPanels::GetConstTagName(), 
							menu_id );
	}

}
