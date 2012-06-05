/****************************************************************************\
**	grupSelectInterest.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Systems/Groups/Object/grupSelectInterest.hpp"

#include "Systems/Groups/GUI/grupDialogUtil.hpp"
#include "Systems/Groups/Object/grupObjectMgr.hpp"
#include "Systems/Groups/Undo/grupOperations.hpp"

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
void grupSelectInterest::SelectionChanged()
{
	pick3dPickObject* pSelectedObject = sel3dMgr::GetSelected();
	if (pSelectedObject)
	{
		if ( grupGroupObject *pDynObj = sel3dCastUtil::CastPickObject<grupGroupObject>(pSelectedObject) )
		{
			//DBG_LOG0( "selected an mtrlScriptObject" );

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
void grupSelectInterest::AddedToSelection( pick3dPickObject* i_pSelObj )
{
}

//--------------------------------------------------------------------
//	RemovedFromSelection - called when an object is removed from
//		the selection list.
//--------------------------------------------------------------------
//virtual 
void grupSelectInterest::RemovedFromSelection( pick3dPickObject* i_pSelObj )
{
}
