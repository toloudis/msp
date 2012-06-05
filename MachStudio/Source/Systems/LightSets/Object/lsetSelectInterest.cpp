/****************************************************************************\
**	lsetSelectInterest.cpp
**
**		see .hpp
**
**	StudioGPU
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
	sel3dObject* pSelectedObject = sel3dMgr::GetSelected();
	if (pSelectedObject)
	{
		if ( lsetScriptObject *pDynObj = sel3dCastUtil::CastPickObject<lsetScriptObject>(pSelectedObject) )
		{
			//DBG_LOG( "selected an mtrlScriptObject" );

			lsetDialogUtil::AddDataPage();
			lsetDialogUtil::UpdateDialog(pDynObj->GetName());
			return;
		}
	}

	lsetDialogUtil::ClearDialog();
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
void lsetSelectInterest::AddedToSelection( sel3dObject* i_pSelObj )
{
}

//--------------------------------------------------------------------
//	RemovedFromSelection - called when an object is removed from
//		the selection list.
//--------------------------------------------------------------------
//virtual 
void lsetSelectInterest::RemovedFromSelection( sel3dObject* i_pSelObj )
{
}
