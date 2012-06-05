/****************************************************************************\
**	envtSelectInterest.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Systems/Environments/Object/envtSelectInterest.hpp"

#include "Support/tmln/tmlnSelectionUtil.hpp"
#include "Systems/Environments/GUI/envtDialogUtil.hpp"
#include "Systems/Environments/Object/envtDefaultEnvironment.hpp"
#include "Systems/Environments/Object/envtSwlEnvironment.hpp"
#include "Systems/Environments/Object/envtObjectMgr.hpp"
#include "Systems/Environments/Undo/envtOperations.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Tool/sel3d/sel3dCastUtil.hpp"


//--------------------------------------------------------------------
//	SelectionChanged
//--------------------------------------------------------------------
//virtual 
void envtSelectInterest::SelectionChanged()
{
	sel3dObject* pSelectedObject = sel3dMgr::GetSelected();
	if (pSelectedObject)
	{
		if ( envtScriptObject *pDynObj = sel3dCastUtil::CastPickObject<envtScriptObject>(pSelectedObject) )
		{
			//DBG_LOG( "selected an mtrlScriptObject" );

			envtDialogUtil::AddDataPage();
			envtDialogUtil::UpdateDialog(pDynObj->GetName());
			return;
		}
		else if ( envtDefaultEnvironment *pDynObj = sel3dCastUtil::CastPickObject<envtDefaultEnvironment>(pSelectedObject) )
		{
			DBG_LOG( "selected an mtrlScriptObject" );

			envtDialogUtil::AddDataPage();
			envtDialogUtil::UpdateDialog(pDynObj->GetDisplayName());
			return;
		}
		else if (envtSwlEnvironment *pDynObj = sel3dCastUtil::CastPickObject<envtSwlEnvironment>(pSelectedObject))
		{
			DBG_LOG( "selected an mtrlScriptObject" );

			envtDialogUtil::AddDataPage();
			envtDialogUtil::UpdateDialog(pDynObj->GetDisplayName());
			return;
		}
	}

	envtDialogUtil::ClearDialog();
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
void envtSelectInterest::AddedToSelection( sel3dObject* i_pSelObj )
{
}

//--------------------------------------------------------------------
//	RemovedFromSelection - called when an object is removed from
//		the selection list.
//--------------------------------------------------------------------
//virtual 
void envtSelectInterest::RemovedFromSelection( sel3dObject* i_pSelObj )
{
}
