/*****************************************************************************
**	brshColorDialogUtil.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Support/brsh/brshColorDialogUtil.hpp"

#include "Support/brsh/brshPaintBrushMgr.hpp"
#include "Support/brsh/wxGUI/brshColorDialog.hpp"
#include "Support/brsh/data/brshData.hpp"

#include "Core/prty/prtyObject.hpp"
#include "Tool/gui/guiDialogTabbedMgr.hpp"

#ifdef USE_WXWIDGETS
#include "ToolUIWx/twx/twxPaneMgr.hpp"
#include "ToolUIWx/twx/twxSystem.hpp"
#endif

#include "Core/env/envSTLHelpers.hpp"
#include "Core/prty/prtyInterestUtil.hpp"

#include <string>

//============================================================================
//============================================================================
namespace brshColorDialogUtil
{
	//------------------------------------------------------------------------
	//  Init
	//------------------------------------------------------------------------
	void  Init()
	{
#ifdef USE_WXWIDGETS
		/*if (brshColorDialog::Instance == NULL)
		{
			brshColorDialog::Instance = new brshColorDialog( twxSystem::g_pMainForm );
		}*/
		guiDialogTabbedMgr::Create("PaintBrush", "Paint");
		guiDialogTabbedMgr::AddTabPage("PaintBrush", "Paint Brush");
		//	create the entries for the paint brush tab
		prtyObject* pDO = brshPaintBrushMgr::GetBrushObject();
		const bool cbSHOW_CATEGORY = true;
		const bool cbAUTO_COLLAPSE = false;

		//pwxFormControlBuilder::BuildForm(pTabPage_Brush, (pDO->GetListContainer()), cbSHOW_CATEGORY, cbAUTO_COLLAPSE );
		guiDialogTabbedMgr::BuildForm("PaintBrush", "Paint Brush", (pDO->GetListContainer()), cbSHOW_CATEGORY, cbAUTO_COLLAPSE );
		prtyInterestUtil::AddInterest(std::string("Paint"), brshColorDialogUtil::Show, false);
#endif
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void  Show()
	{
//#ifdef USE_WXWIDGETS
//		brshData& brush_data = brshPaintBrushMgr::Data();
//		brush_data.m_bEnabled.SetValue(true);
//		twxPaneMgr::Show(brshColorDialog::Instance);
//#endif
		if (!guiDialogTabbedMgr::IsExisted("PaintBrush"))
		{
			Init();
		}
		brshData& brush_data = brshPaintBrushMgr::Data();
		brush_data.m_bEnabled.SetValue(true);
		guiDialogTabbedMgr::Show("PaintBrush");
	}

	//--------------------------------------------------------------------
	//  Hide - the dialog is going away, update the data
	//--------------------------------------------------------------------
	void  Hide()
	{
		if (guiDialogTabbedMgr::IsExisted("PaintBrush"))
		{
			guiDialogTabbedMgr::Hide("PaintBrush");
		}
	}
	//--------------------------------------------------------------------
	//  CleanUp - the dialog is going away, update the data
	//--------------------------------------------------------------------
	void  CleanUp()
	{
		bool bEnable = false;
		bool bClearData = true;
		brshPaintBrushMgr::EnablePaint(bEnable, bClearData);
	}

	//--------------------------------------------------------------------
	//  IsVisible - return whether or not the dialog is visible
	//--------------------------------------------------------------------
	bool IsVisible()
	{
		return guiDialogTabbedMgr::IsVisible("PaintBrush");
	}
}
