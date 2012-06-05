/****************************************************************************\
**	envtSelectInterest.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Systems/Environments/Object/envtSelectInterest.hpp"

#include "Systems/Environments/GUI/envtDialogUtil.hpp"
#include "Systems/Environments/Object/envtObjectMgr.hpp"
#include "Systems/Environments/Undo/envtOperations.hpp"

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
void envtSelectInterest::SelectionChanged()
{
	pick3dPickObject* pSelectedObject = sel3dMgr::GetSelected();
	if (pSelectedObject)
	{
		if ( envtScriptObject *pDynObj = sel3dCastUtil::CastPickObject<envtScriptObject>(pSelectedObject) )
		{
			//DBG_LOG0( "selected an mtrlScriptObject" );

			envtDialogUtil::AddDataPage();
			envtDialogUtil::UpdateDialog(pDynObj);
			return;
		}
	}

	envtDialogUtil::UpdateDialog(NULL);
	// this is safe to call even if not added
	envtDialogUtil::RemoveDataPage();

	// Clear the selection from the operations namespace
	envtOperations::SetSelectedIndex( -1 );
}

//--------------------------------------------------------------------
//	AddedToSelection - called when an object is added to the
//		selection list.
//--------------------------------------------------------------------
//virtual 
void envtSelectInterest::AddedToSelection( pick3dPickObject* i_pSelObj )
{
}

//--------------------------------------------------------------------
//	RemovedFromSelection - called when an object is removed from
//		the selection list.
//--------------------------------------------------------------------
//virtual 
void envtSelectInterest::RemovedFromSelection( pick3dPickObject* i_pSelObj )
{
}
