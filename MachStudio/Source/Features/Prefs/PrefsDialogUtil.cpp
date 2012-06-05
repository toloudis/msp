/*****************************************************************************
**	PrefsDialogUtil.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Features/Prefs/PrefsData.hpp"
#include "Features/Prefs/PrefsDialogUtil.hpp"
#include "Features/Prefs/PrefsMgr.hpp"
#include "Features/Prefs/wxGUI/prefsLayoutChoice.hpp"
//#include "Features/Prefs/prefsQuickMgr.hpp"

#include "Tool/cma/cmaCommandMgr.hpp"
#include "Tool/gui/guiDialogTabbedMgr.hpp"
#include "ToolUIWx/twx/twxToolbarMgr.hpp"


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
		//
		guiDialogTabbedMgr::Create("Prefs", "Preferences");
		guiDialogTabbedMgr::AddTabPage("Prefs","Preferences");

		//	Main
		//
		prtyObject* pDO = PrefsMgr::GetDataObject();
		pDO->SortListByCategory();
		const bool cbSHOW_CATEGORY = true;
		const bool cbAUTO_COLLAPSE = false;
		guiDialogTabbedMgr::BuildForm( "Prefs", "Preferences", (pDO->GetListContainer()), cbSHOW_CATEGORY, cbAUTO_COLLAPSE );

		// Add other pref tabs
		//
		guiDialogTabbedMgr::AddTabPage("Prefs","Hot Keys");

		//	Hot Keys
		//
		prtyPropertyUIInfoContainer plist;
		cmaCommandMgr::GetCommandPropertyUIInfoList( plist );
		plist.SortByCategory();
		
		guiDialogTabbedMgr::BuildForm("Prefs","Hot Keys", plist, true );

	}

	//--------------------------------------------------------------------
	// Create combo box with layout configurations 
	//--------------------------------------------------------------------
	void  CreateLayoutToolbar()
	{
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

	}

	//------------------------------------------------------------------------
	//  UpdateLayouts - update list of named layouts
	//	If i_CurrentLayoutName is non-empty, select it in the combo box
	//------------------------------------------------------------------------
	void  UpdateLayouts(const std::string& i_CurrentLayoutName)
	{
#ifdef USE_WXWIDGETS
		if (prefsLayoutChoice::Instance != NULL)
		{
			prefsLayoutChoice::Instance->Update();
			if (!i_CurrentLayoutName.empty())
			{
				prefsLayoutChoice::Instance->SetStringSelection(wxString(i_CurrentLayoutName.c_str(), wxConvUTF8));
			}
		}
#endif
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void  Show()
	{

		// In widgets, the dialog is created once when the main form is created
		// and then it is just shown and hidden from then on
		guiDialogTabbedMgr::Show("Prefs");

	}

}	// end of namespace

