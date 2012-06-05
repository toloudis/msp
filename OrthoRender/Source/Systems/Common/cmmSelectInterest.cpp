/****************************************************************************\
**	cmmSelectInterest.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Systems/Common/cmmSelectInterest.hpp"

#include "Systems/Common/GUI/cmmObjectDialogUtil.hpp"
#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"

#include "Tool/pick3d/pick3dPickObject.hpp"
#include "Tool/sel3d/sel3dMgr.hpp"
#include "Tool/gui/guiDialogTabbedMgr.hpp"
#include "Support/tmln/tmlnScriptObject.hpp"
#include "Support/tmln/tmlnCreator.hpp"


//--------------------------------------------------------------------
//	SelectionChanged - called when the selection list is changed
//--------------------------------------------------------------------
//virtual 
void cmmSelectInterest::SelectionChanged()
{
	const std::list<pick3dPickObject*>& selected_list = sel3dMgr::GetSelectedList();
	if (selected_list.size() >= 1)
	{
		pick3dPickObject* pSelectedObject = sel3dMgr::GetSelected();

		cmmSystemDialogUtil::SelectObjectOnPlacedList( pSelectedObject );

		if ( prtyObject *pObject = dynamic_cast<prtyObject*>(pSelectedObject) )
		{
			cmmObjectDialogUtil::UpdateDialog();
			return;
		}
	}

	//	clear out the form
	cmmObjectDialogUtil::ClearDialog();

	// clear out driver form also
//WXGUI
/*
	guiDialogTabbedMgr::RemoveTabPages("Driver");
*/
}

//--------------------------------------------------------------------
//	AddedToSelection
//--------------------------------------------------------------------
//virtual 
void cmmSelectInterest::AddedToSelection( pick3dPickObject* i_pSelObj )
{
	cmmSystemDialogUtil::AddToSelectedObjectsOnPlacedList( i_pSelObj );
}

//--------------------------------------------------------------------
//	RemovedFromSelection
//--------------------------------------------------------------------
//virtual 
void cmmSelectInterest::RemovedFromSelection( pick3dPickObject* i_pSelObj )
{
	cmmSystemDialogUtil::RemoveFromSelectedObjectsOnPlacedList( i_pSelObj );
}
