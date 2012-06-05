/****************************************************************************\
**	grupSelectInterest.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Systems/Groups/Object/grupSelectInterest.hpp"

#include "Support/tmln/tmlnSelectionUtil.hpp"
#include "Systems/Groups/GUI/grupDialogUtil.hpp"
#include "Systems/Groups/Object/grupObjectMgr.hpp"
#include "Systems/Groups/Undo/grupOperations.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Tool/sel3d/sel3dCastUtil.hpp"


//--------------------------------------------------------------------
//	SelectionChanged
//--------------------------------------------------------------------
//virtual 
void grupSelectInterest::SelectionChanged()
{
	sel3dObject* pSelectedObject = sel3dMgr::GetSelected();
	if (pSelectedObject)
	{
		if ( grupGroupObject *pDynObj = sel3dCastUtil::CastPickObject<grupGroupObject>(pSelectedObject) )
		{
			//DBG_LOG( "selected an mtrlScriptObject" );

			grupDialogUtil::AddDataPage();
			grupDialogUtil::UpdateDialog(pDynObj);
			return;
		}
	}

	grupDialogUtil::UpdateDialog(NULL);
	// this is safe to call even if not added
	grupDialogUtil::RemoveDataPage();

	// Clear the selection from the operations namespace
//	grupOperations::SetSelectedIndex( -1 );
}

//--------------------------------------------------------------------
//	AddedToSelection - called when an object is added to the
//		selection list.
//--------------------------------------------------------------------
//virtual 
void grupSelectInterest::AddedToSelection( sel3dObject* i_pSelObj )
{
}

//--------------------------------------------------------------------
//	RemovedFromSelection - called when an object is removed from
//		the selection list.
//--------------------------------------------------------------------
//virtual 
void grupSelectInterest::RemovedFromSelection( sel3dObject* i_pSelObj )
{
}
