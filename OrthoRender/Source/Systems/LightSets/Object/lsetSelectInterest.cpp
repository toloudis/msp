/****************************************************************************\
**	lsetSelectInterest.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Systems/LightSets/Object/lsetSelectInterest.hpp"

#include "Systems/LightSets/GUI/lsetDialogUtil.hpp"
#include "Systems/LightSets/Object/lsetObjectMgr.hpp"
#include "Systems/LightSets/Undo/lsetOperations.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Support/tmln/tmlnSelectionUtil.hpp"
#include "Tool/sel3d/sel3dCastUtil.hpp"

namespace
{
}

//--------------------------------------------------------------------
//	SelectionChanged
//--------------------------------------------------------------------
//virtual 
void lsetSelectInterest::SelectionChanged()
{
	pick3dPickObject* pSelectedObject = sel3dMgr::GetSelected();
	if (pSelectedObject)
	{
		if ( lsetScriptObject *pDynObj = sel3dCastUtil::CastPickObject<lsetScriptObject>(pSelectedObject) )
		{
			//DBG_LOG0( "selected an mtrlScriptObject" );

			lsetDialogUtil::AddDataPage();
			lsetDialogUtil::UpdateDialog(pDynObj);
			return;
		}
	}

	lsetDialogUtil::UpdateDialog(NULL);
	// this is safe to call even if not added
	lsetDialogUtil::RemoveDataPage();

	// Clear the selection from the operations namespace
//	lsetOperations::SetSelectedIndex( -1 );
}

//--------------------------------------------------------------------
//	AddedToSelection - called when an object is added to the
//		selection list.
//--------------------------------------------------------------------
//virtual 
void lsetSelectInterest::AddedToSelection( pick3dPickObject* i_pSelObj )
{
}

//--------------------------------------------------------------------
//	RemovedFromSelection - called when an object is removed from
//		the selection list.
//--------------------------------------------------------------------
//virtual 
void lsetSelectInterest::RemovedFromSelection( pick3dPickObject* i_pSelObj )
{
}
