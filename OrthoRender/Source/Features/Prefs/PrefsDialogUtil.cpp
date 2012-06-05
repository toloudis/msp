/*****************************************************************************
**	PrefsDialogUtil.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Features/Prefs/PrefsDialogUtil.hpp"

#include "Features/Prefs/mGUI/prefsLayoutConfigForm.h"
#include "Features/Prefs/wxGUI/prefsLayoutChoice.hpp"
#include "Features/Prefs/prefsQuickMgr.hpp"

#include "Tool/cma/cmaCommandMgr.hpp"
#include "Tool/gui/guiDialogTabbedMgr.hpp"
#include "ToolUIManaged/prtym/prtyFormControlBuilder.hpp"
#include "ToolUIManaged/tma/tmaDialogTabbedMgr.hpp"
#include "ToolUIWx/twx/twxToolbarMgr.hpp"


//============================================================================
//============================================================================
#ifdef _MANAGED
using namespace Features;
#endif

//============================================================================
//============================================================================
namespace
{
	const char *c_LayoutToolbarName = "Layout";
}

//============================================================================
//============================================================================
namespace PrefsDialogUtil
{
	//--------------------------------------------------------------------
	// Return name of toolbar for layouts in order to show/hide it 
	//	in a command.
	//--------------------------------------------------------------------
	const char* GetLayoutToolbarName()
	{
		return c_LayoutToolbarName;
	}

	//------------------------------------------------------------------------
	//  CreateDialog - common step of creating form whether to be
	//	displayed modal or modeless
	//------------------------------------------------------------------------
	void  CreateDialog()
	{
		//WXGUI
		/*
		//
		guiDialogTabbedMgr::Create("Prefs", "Preferences");
		guiDialogTabbedMgr::AddTabPage("Prefs","Preferences");

		//	Main
		//
		prtyObject* pDO = PrefsMgr::GetDataObject();
		pDO->SortListByCategory();
		const bool cbSHOW_CATEGORY = true;
		const bool cbAUTO_COLLAPSE = false;
		guiDialogTabbedMgr::BuildForm( "Prefs", "Preferences", (pDO->GetList()), cbSHOW_CATEGORY, cbAUTO_COLLAPSE );

		// Add other pref tabs
		//
		guiDialogTabbedMgr::AddTabPage("Prefs","Hot Keys");

		//	Hot Keys
		//
		prtyPropertyUIInfoContainer plist;
		cmaCommandMgr::GetCommandPropertyUIInfoList( plist );
		plist.SortByCategory();
		
#ifdef _MANAGED
		prtyFormControlBuilder::SetLabelWidth(200);
#endif
		guiDialogTabbedMgr::BuildForm("Prefs","Hot Keys", plist.GetList(), true );

#ifdef _MANAGED

		tmaDialogTabbed^ pPrefs = tmaDialogTabbedMgr::GetDialogFromName("Prefs");

		//	Layout
		prefsLayoutConfigForm::FormInstance = gcnew prefsLayoutConfigForm();
		tmaSystem::g_pMainForm->AddOwnedForm(prefsLayoutConfigForm::FormInstance);

		prefsLayoutConfigForm::FormInstance->Update();

		pPrefs->AddTabPage( prefsLayoutConfigForm::FormInstance->GetTabPage() );

		//	Quick Commands
		//
		pPrefs->AddTabPage( prefsQuickMgr::GetTabPage() );
#endif
	*/
	}

	//--------------------------------------------------------------------
	// Create combo box with layout configurations 
	//--------------------------------------------------------------------
	void  CreateLayoutToolbar()
	{
		//WXGUI
		/*
#ifdef USE_WXWIDGETS
		// In wxWidgets version, the layout choices appear in 
		// a combo box in a toolbar pane
		if (prefsLayoutChoice::Instance == NULL)
		{
			prefsLayoutChoice::Instance = new prefsLayoutChoice(twxToolbarMgr::GetToolbarByName(c_LayoutToolbarName)); 
			prefsLayoutChoice::Instance->Update();
			twxToolbarMgr::AddControlToToolBar(c_LayoutToolbarName, prefsLayoutChoice::Instance);
		}
#endif

	*/
	}

	//------------------------------------------------------------------------
	//  UpdateLayouts - update list of named layouts
	//	If i_CurrentLayoutName is non-empty, select it in the combo box
	//------------------------------------------------------------------------
	void  UpdateLayouts(const std::string& i_CurrentLayoutName)
	{
		//WXGUI
		/*
#ifdef USE_WXWIDGETS
		if (prefsLayoutChoice::Instance != NULL)
		{
			prefsLayoutChoice::Instance->Update();
			if (!i_CurrentLayoutName.empty())
				prefsLayoutChoice::Instance->SetStringSelection(i_CurrentLayoutName);
		}
#endif
	*/
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void  Show()
	{
		//WXGUI
		/*
#ifdef _MANAGED
		// In managed code, the dialog is created, shown modally and then destroyed
		PrefsMgr::ReadPrefs();

		CreateDialog();

		// modal
		tmaDialogTabbedMgr::ShowDialog("Prefs");

		//	Apply and write out the preferences
		PrefsMgr::ApplyPrefs();
		PrefsMgr::WritePrefs();
		prefsQuickMgr::WritePrefs();
		PrefsMgr::WriteHotKeys();

		tmaDialogTabbedMgr::Close("Prefs");
#else
		// In widgets, the dialog is created once when the main form is created
		// and then it is just shown and hidden from then on
		guiDialogTabbedMgr::Show("Prefs");
#endif
	*/
	}

}	// end of namespace

