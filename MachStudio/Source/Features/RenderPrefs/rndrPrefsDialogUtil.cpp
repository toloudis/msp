/*****************************************************************************
**	rndrPrefsDialogUtil.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Features/RenderPrefs/rndrPrefsDialogUtil.hpp"

#include "Features/RenderPrefs/rndrPrefsMgr.hpp"

#include "Support/pfx/pfxPostEffectMgr.hpp"
#include "Support/rprf/rprfPrefsUtil.hpp"

//	library
#include "Core/prty/prtyObject.hpp"
#include "Tool/gui/guiDialogTabbedMgr.hpp"


//============================================================================
//============================================================================
namespace rndrPrefsDialogUtil
{
	namespace
	{
		const std::string l_PFXTabName("Post Effect");
	}
	//------------------------------------------------------------------------
	//  CreateDialog - common step of creating form whether to be
	//	displayed modal or modeless
	//------------------------------------------------------------------------
	void  CreateDialog()
	{
		guiDialogTabbedMgr::Create("RenderPrefs", "Render Preferences");
		guiDialogTabbedMgr::AddTabPage("RenderPrefs", "Viewport");
		guiDialogTabbedMgr::AddTabPage("RenderPrefs", l_PFXTabName.c_str());

		// add the first tab with its controls
		prtyObject* pRDO = rndrPrefsMgr::GetDataObject(rndrPrefsMgr::e_ViewportPrefs);
		prtyObject* pPFO = pfxPostEffectMgr::GetDataObject(pfxPostEffectMgr::e_ViewportPfx);

		const bool cbSHOW_CATEGORY = true;
		const bool cbAUTO_COLLAPSE = false;
		guiDialogTabbedMgr::BuildForm("RenderPrefs","Viewport", (pRDO->GetListContainer()), cbSHOW_CATEGORY, cbAUTO_COLLAPSE);
		guiDialogTabbedMgr::BuildForm("RenderPrefs",l_PFXTabName.c_str(), (pPFO->GetListContainer()), cbSHOW_CATEGORY, cbAUTO_COLLAPSE);
		//rprfPrefsUtil:::SetUpdateFunctionVP(&Update);
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void  Show()
	{
		guiDialogTabbedMgr::Show("RenderPrefs");
	}

	//--------------------------------------------------------------------
	//  Hide - the dialog is going away, update the data
	//--------------------------------------------------------------------
	void  Hide()
	{
		rndrPrefsMgr::WritePrefs(rndrPrefsMgr::e_ViewportPrefs);
	}

	//------------------------------------------------------------------------
	//  Update - Property listings have changed so we need to update
	//------------------------------------------------------------------------
	void Update()
	{
		guiDialogTabbedMgr::Freeze("RenderPrefs");
		guiDialogTabbedMgr::RemoveTabPage("RenderPrefs", "Viewport");
		guiDialogTabbedMgr::RemoveTabPage("RenderPrefs", l_PFXTabName.c_str());

		// add the first tab with its controls
		guiDialogTabbedMgr::AddTabPage("RenderPrefs", "Viewport");
		guiDialogTabbedMgr::AddTabPage("RenderPrefs", l_PFXTabName.c_str());

		prtyObject* pRDO = rndrPrefsMgr::GetDataObject(rndrPrefsMgr::e_ViewportPrefs);
		prtyObject* pPFO = pfxPostEffectMgr::GetDataObject(pfxPostEffectMgr::e_ViewportPfx);

		const bool cbSHOW_CATEGORY = true;
		const bool cbAUTO_COLLAPSE = false;
		guiDialogTabbedMgr::BuildForm("RenderPrefs","Viewport", (pRDO->GetListContainer()), cbSHOW_CATEGORY, cbAUTO_COLLAPSE);		
		guiDialogTabbedMgr::BuildForm("RenderPrefs",l_PFXTabName.c_str(), (pPFO->GetListContainer()), cbSHOW_CATEGORY, cbAUTO_COLLAPSE);
		
		guiDialogTabbedMgr::Thaw("RenderPrefs");
	}

	//------------------------------------------------------------------------
	//  UpdatePfxPage - Property listings have changed so we need to update
	//------------------------------------------------------------------------
	void UpdatePfxPage()
	{
		guiDialogTabbedMgr::Freeze("RenderPrefs");
		guiDialogTabbedMgr::DeleteTabPage("RenderPrefs", l_PFXTabName.c_str());

		// add the first tab with its controls
		guiDialogTabbedMgr::AddTabPage("RenderPrefs", l_PFXTabName.c_str());

		prtyObject* pPFO = pfxPostEffectMgr::GetDataObject(pfxPostEffectMgr::e_ViewportPfx);

		const bool cbSHOW_CATEGORY = true;
		const bool cbAUTO_COLLAPSE = false;
		guiDialogTabbedMgr::BuildForm("RenderPrefs",l_PFXTabName.c_str(), (pPFO->GetListContainer()), cbSHOW_CATEGORY, cbAUTO_COLLAPSE);
		
		guiDialogTabbedMgr::Thaw("RenderPrefs");
	}

}	// end of namespace

