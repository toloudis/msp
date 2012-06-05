/*****************************************************************************
**	rndrPrefsDialogUtil.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Features/RenderPrefs/rndrPrefsDialogUtil.hpp"

#include "Features/RenderPrefs/rndrPrefsMgr.hpp"

//	library
#include "Core/prty/prtyObject.hpp"
#include "ToolUIManaged/prtym/prtyFormControlBuilder.hpp"
#include "Tool/gui/guiDialogTabbedMgr.hpp"


//============================================================================
//============================================================================
namespace rndrPrefsDialogUtil
{
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void  Show()
	{
		guiDialogTabbedMgr::Create("RenderPrefs", "Render Preferences");

		// clear it out if already there. don't want two tabs with "Viewport"
		// if the dialog is already open.
		guiDialogTabbedMgr::RemoveTabPage("RenderPrefs", "Viewport"); 

		guiDialogTabbedMgr::AddTabPage("RenderPrefs", "Viewport");

		// add the first tab with its controls
		prtyObject* pRDO = rndrPrefsMgr::GetDataObject(rndrPrefsMgr::e_ViewportPrefs);

		guiDialogTabbedMgr::BuildForm("RenderPrefs","Viewport", (pRDO->GetList()), true);

		// modeless
		guiDialogTabbedMgr::Show("RenderPrefs");
	}

	//--------------------------------------------------------------------
	//  Hide - the dialog is going away, update the data
	//--------------------------------------------------------------------
	void  Hide()
	{
		rndrPrefsMgr::WritePrefs(rndrPrefsMgr::e_ViewportPrefs);
	}
}	// end of namespace

