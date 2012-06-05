/****************************************************************************\
**	lyrsSelectInterest.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Systems/Layers/Object/lyrsSelectInterest.hpp"

#include "Support/tmln/tmlnSelectionUtil.hpp"
#include "Systems/Layers/GUI/lyrsDialogUtil.hpp"
#include "Systems/Layers/Object/lyrsObjectMgr.hpp"
#include "Systems/Layers/Undo/lyrsOperations.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Tool/sel3d/sel3dCastUtil.hpp"


//--------------------------------------------------------------------
//	SelectionChanged
//--------------------------------------------------------------------
//virtual 
void lyrsSelectInterest::SelectionChanged()
{
	sel3dObject* pSelectedObject = sel3dMgr::GetSelected();
	if (pSelectedObject)
	{
		if ( lyrsLayerObject *pDynObj = sel3dCastUtil::CastPickObject<lyrsLayerObject>(pSelectedObject) )
		{
			//DBG_LOG( "selected an mtrlScriptObject" );

			lyrsDialogUtil::AddDataPage();
			lyrsDialogUtil::UpdateDialog(pDynObj);
			return;
		}
	}

	lyrsDialogUtil::UpdateDialog(NULL);
	// this is safe to call even if not added
	lyrsDialogUtil::RemoveDataPage();

	// Clear the selection from the operations namespace
//	lyrsOperations::SetSelectedIndex( -1 );
}

//--------------------------------------------------------------------
//	AddedToSelection - called when an object is added to the
//		selection list.
//--------------------------------------------------------------------
//virtual 
void lyrsSelectInterest::AddedToSelection( sel3dObject* i_pSelObj )
{
}

//--------------------------------------------------------------------
//	RemovedFromSelection - called when an object is removed from
//		the selection list.
//--------------------------------------------------------------------
//virtual 
void lyrsSelectInterest::RemovedFromSelection( sel3dObject* i_pSelObj )
{
}
