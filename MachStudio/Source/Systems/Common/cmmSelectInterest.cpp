/****************************************************************************\
**	cmmSelectInterest.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Systems/Common/cmmSelectInterest.hpp"
#include "Systems/Common/GUI/cmmObjectDialogUtil.hpp"
#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"

#include "Support/tmln/tmlnDriverDialogUtil.hpp"
#include "Support/tmln/tmlnScriptObject.hpp"
#include "Support/tmln/tmlnCreator.hpp"

#include "Tool/sel3d/sel3dMgr.hpp"

//--------------------------------------------------------------------
//	SelectionChanged - called when the selection list is changed
//--------------------------------------------------------------------
//virtual 
void cmmSelectInterest::SelectionChanged()
{
	const std::list<sel3dObject*>& selected_list = sel3dMgr::GetSelectedList();
	if (selected_list.size() >= 1)
	{
		sel3dObject* pSelectedObject = sel3dMgr::GetSelected();

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
	tmlnDriverDialogUtil::ClearDriverProperties();
}

//--------------------------------------------------------------------
//	AddedToSelection
//--------------------------------------------------------------------
//virtual 
void cmmSelectInterest::AddedToSelection( sel3dObject* i_pSelObj )
{
	cmmSystemDialogUtil::AddToSelectedObjectsOnPlacedList( i_pSelObj );
}

//--------------------------------------------------------------------
//	RemovedFromSelection
//--------------------------------------------------------------------
//virtual 
void cmmSelectInterest::RemovedFromSelection( sel3dObject* i_pSelObj )
{
	cmmSystemDialogUtil::RemoveFromSelectedObjectsOnPlacedList( i_pSelObj );
}
