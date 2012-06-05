/*****************************************************************************
**	prefsCommands.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Features/Prefs/prefsCommands.hpp"

#include "Features/Prefs/PrefsDialogUtil.hpp"
#include "Features/Prefs/wxGUI/prefsLayoutMgr.hpp"

#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"

#include "Core/prty/prtyText.hpp"
#include "Core/prty/prtyTextBoxUIInfo.hpp"
#include "Tool/cma/cmaCommandMgr.hpp"
#include "Tool/cma/cmaCommandSimple.hpp"
#include "Tool/cma/cmaCommandToggle.hpp"
#include "Tool/gui/guiCommandMgr.hpp"
#include "Tool/gui/guiMenuMgr.hpp"
#include "Tool/gui/guiToolbarMgr.hpp"
#include "Tool/gui/guiPropertyDialog.hpp"


//============================================================================
//============================================================================
namespace
{
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Set_LayoutToolbar(bool i_bChecked)
	{
		guiToolbarMgr::Show(PrefsDialogUtil::GetLayoutToolbarName(), i_bChecked);
	}
	bool Get_LayoutToolbar()
	{
		return guiToolbarMgr::IsVisible(PrefsDialogUtil::GetLayoutToolbarName());
	}
	
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Execute_SaveLayout()
	{
#ifdef USE_WXWIDGETS
		prtyText layoutName("Layout Name");
		layoutName.SetValue(prefsLayoutMgr::GetCurrentLayoutName());
		prtyPropertyUIInfoContainer layoutInfo;
		layoutInfo.Add(new prtyTextBoxUIInfo(&layoutName));
		if (guiPropertyDialog::ShowModal("Save Layout", 
										 layoutInfo, 
										 "Choose name for layout") == guiPropertyDialog::e_OK)
		{
			if (prefsLayoutMgr::SaveNamedLayout(layoutName.GetValue()))
				PrefsDialogUtil::UpdateLayouts(layoutName.GetValue());
		}
#endif // USE_WXWIDGETS
	}
	
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Execute_DeleteLayout()
	{
#ifdef USE_WXWIDGETS
		if (!prefsLayoutMgr::GetCurrentLayoutName().empty())
		{
			prefsLayoutMgr::DeleteNamedLayout(prefsLayoutMgr::GetCurrentLayoutName());
			PrefsDialogUtil::UpdateLayouts();
		}
#endif // USE_WXWIDGETS
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Execute_NextLayout()
	{
#ifdef USE_WXWIDGETS
		prefsLayoutMgr::SwitchToNextLayout();
		PrefsDialogUtil::UpdateLayouts(prefsLayoutMgr::GetCurrentLayoutName());
#endif // USE_WXWIDGETS
	}
	void Execute_PrevLayout()
	{
#ifdef USE_WXWIDGETS
		prefsLayoutMgr::SwitchToPrevLayout();
		PrefsDialogUtil::UpdateLayouts(prefsLayoutMgr::GetCurrentLayoutName());
#endif // USE_WXWIDGETS
	}
}


//------------------------------------------------------------------------
//  AddToMenu() - add Prefs actions to menus
//------------------------------------------------------------------------
void  prefsCommands::AddToMenu()
{
	//
	//	commands
	//
	int menu_id;
	cmaCommand* pCmd;

	//	COMMAND: Show Preferences Dialog
	pCmd = new cmaCommandSimple("Preferences", 
								"Edit", 
								"Application Preferences",
								&PrefsDialogUtil::Show );
	menu_id = guiMenuMgr::AddMenuItem( "Edit", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Tools", "Preferences", pCmd );
	
	// Only wxWidgets has layout as toolbar
	PrefsDialogUtil::CreateLayoutToolbar();
	guiMenuMgr::AddMenu("Windows", "Layouts");

	//	COMMAND: Save Layout
	pCmd = new cmaCommandSimple("Save Layout", 
		"Layouts",
		"Save current pane configuration as a named layout",
		&Execute_SaveLayout);
	menu_id = guiMenuMgr::AddMenuItem( "Layouts", pCmd->GetTag().c_str(), 
		PrefsDialogUtil::GetLayoutToolbarName(), "layout-save.png" ); 
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );

	//	COMMAND: Delete Layout
	pCmd = new cmaCommandSimple("Delete Layout", 
		"Layouts",
		"Delete previously saved named layout",
		&Execute_DeleteLayout);
	menu_id = guiMenuMgr::AddMenuItem( "Layouts", pCmd->GetTag().c_str(),
		PrefsDialogUtil::GetLayoutToolbarName(), "layout-delete.png"); 
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );

	//	COMMAND: Next Layout
	pCmd = new cmaCommandSimple("Next Layout", 
		"Layouts",
		"Switch to next named layout",
		&Execute_NextLayout);
	menu_id = guiMenuMgr::AddMenuItem( "Layouts", pCmd->GetTag().c_str()); 
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	
	//	COMMAND: Previous Layout
	pCmd = new cmaCommandSimple("Previous Layout", 
		"Layouts",
		"Switch to previous named layout",
		&Execute_PrevLayout);
	menu_id = guiMenuMgr::AddMenuItem( "Layouts", pCmd->GetTag().c_str()); 
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );

	//	COMMAND: Layout toolbar
	guiMenuMgr::AddMenu( "View", "Toolbars" );
	pCmd = new cmaCommandToggle("Layout Toolbar", 
								"Toolbars", 
								"View the layout choice toolbar",
								&Set_LayoutToolbar, 
								&Get_LayoutToolbar );
	menu_id = guiMenuMgr::AddCheckableMenuItem( "Toolbars", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Main", "View Layout Toolbar", pCmd );
}

