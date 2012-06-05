/*****************************************************************************
**	rmpDialogUtil.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Support/rmp/rmpDialogUtil.hpp"

#include "Support/rmp/rmpDialogMgr.hpp"

//	library
#include "Core/prty/prtyInterestUtil.hpp"
#include "Core/prty/prtyObject.hpp"
#include "Tool/gui/guiDialogTabbedMgr.hpp"

#include <string>

//============================================================================
//============================================================================
namespace rmpDialogUtil
{
	//------------------------------------------------------------------------
	//  Initialize (Create the Dialog instance)
	//------------------------------------------------------------------------
	void  Initialize()
	{
		guiDialogTabbedMgr::Create("RampEditor", "Ramp Texture Editor");
		guiDialogTabbedMgr::AddTabPage("RampEditor", "Gradient Ramp");

		// add the first tab with its controls
		prtyObject* pRDO = rmpDialogMgr::GetDataObject();
		
		const bool cbSHOW_CATEGORY = true;
		const bool cbAUTO_COLLAPSE = false;
		guiDialogTabbedMgr::BuildForm("RampEditor","Gradient Ramp", (pRDO->GetListContainer()), cbSHOW_CATEGORY, cbAUTO_COLLAPSE);

		prtyInterestUtil::AddInterest(std::string("Ramp"), rmpDialogUtil::Show, false, true);
		//rprfPrefsUtil::SetUpdateFunctionVP(&Update);
	}

	//------------------------------------------------------------------------
	//  DeInitialize
	//------------------------------------------------------------------------
	void  DeInitialize()
	{
		if (guiDialogTabbedMgr::IsExisted("RampEditor"))
		{
			Hide();
			//rmpDialogMgr::CleanUp();
		}
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void  Show()
	{
		if (!guiDialogTabbedMgr::IsExisted("RampEditor"))
		{
			Initialize();
		}
		guiDialogTabbedMgr::Show("RampEditor");
	}

	//--------------------------------------------------------------------
	//  Hide - the dialog is going away, update the data
	//--------------------------------------------------------------------
	void  Hide()
	{
		if (guiDialogTabbedMgr::IsExisted("RampEditor"))
		{
			guiDialogTabbedMgr::Hide("RampEditor");
		}
		//rmpDialogMgr::WritePrefs(rmpDialogMgr::e_GradientEditor);
	}

	//------------------------------------------------------------------------
	//  Update - Property listings have changed so we need to update
	//------------------------------------------------------------------------
	//void Update()
	//{
	//	guiDialogTabbedMgr::Freeze("RampEditor");
	//	guiDialogTabbedMgr::RemoveTabPage("RampEditor", "Gradient Ramp");
	//	
	//	// add the first tab with its controls
	//	guiDialogTabbedMgr::AddTabPage("RampEditor", "Gradient Ramp");
	//	prtyObject* pRDO = rmpDialogMgr::GetDataObject();

	//	const bool cbSHOW_CATEGORY = false;
	//	const bool cbAUTO_COLLAPSE = true;
	//	guiDialogTabbedMgr::BuildForm("RampEditor","Gradient Ramp", (pRDO->GetListContainer()), cbSHOW_CATEGORY, cbAUTO_COLLAPSE);
	//	guiDialogTabbedMgr::Thaw("RampEditor");
	//}

}	// end of namespace

