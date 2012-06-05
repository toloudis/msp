/*****************************************************************************
**	SelectListDialogUtil.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Features/SelectList/SelectListDialogUtil.hpp"

#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"
#include "Tool/sel3d/sel3dMgr.hpp"


//============================================================================
//============================================================================
namespace SelectListDialogUtil
{
	namespace
	{
		SelectListInterest* l_pInterest = 0;
	}

	//--------------------------------------------------------------------
	//	SelectionChanged - called when the selection list is changed
	//--------------------------------------------------------------------
	//virtual 
	void SelectListInterest::SelectionChanged()
	{
		pick3dPickObject* pSelectedObject = sel3dMgr::GetSelected();
		if (pSelectedObject)
		{
			cmmSystemDialogUtil::UpdatePickList(sel3dMgr::GetPickList(), pSelectedObject);
		}
		else
		{
			cmmSystemDialogUtil::ClearPickList();
		}

	}


	//--------------------------------------------------------------------
	// Init
	//--------------------------------------------------------------------
	void  Init()
	{
		l_pInterest = new SelectListInterest();
		sel3dMgr::RegisterSelectInterest(l_pInterest);
	}

	//--------------------------------------------------------------------
	//  Clean up dialogs
	//--------------------------------------------------------------------
	void  CleanUp()
	{
		sel3dMgr::UnRegisterSelectInterest(l_pInterest);
		delete l_pInterest;
	}

}	// end of namespace

