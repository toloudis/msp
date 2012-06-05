/****************************************************************************\
**	mtrlSelectInterest.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Support/mtrl/mtrlSelectInterest.hpp"

#include "Support/mtrl/GUI/mtrlDialogUtil.hpp"
#include "Support/mtrl/GUI/mtrlOperations.hpp"
#include "Support/mtrl/GUI/mtrlPropertyObject.hpp"
#include "Support/mtrl/mtrlCommands.hpp"
#include "Support/mtrl/mtrlHighlight.hpp"
#include "Support/mtrl/mtrlScriptObject.hpp"

#include "Tool/sel3d/sel3dCastUtil.hpp"


//--------------------------------------------------------------------
//	SelectionChanged - called when the selection list is changed
//--------------------------------------------------------------------
//virtual 
void mtrlSelectInterest::SelectionChanged()
{
	if ( mtrlScriptObject *pMatObj = sel3dCastUtil::CastSelectedObject<mtrlScriptObject>() )
	{
		//DBG_LOG( "selected an mtrlScriptObject" );
		// This lines configure the tab that is placed into the Object dialog
		mtrlDialogUtil::UpdateDialog(pMatObj);
		mtrlDialogUtil::AddDataPage();

		// This case handles selection of a material pick object
		if ( mtrlPropertyObject *pPropObj = sel3dCastUtil::CastSelectedObject<mtrlPropertyObject>() )
		{
			// By setting the selected object and index, the mtrlOperations
			// callbacks for Copy, Paste, ChangeMaterial will operate on this selected
			// material. 
			//TODO bga - How do we handle this in multiple selection case?
			//DBG_LOG( "selected an mtrlPropertyObject" );
			mtrlOperations::SetSelectedMaterialIndex(pMatObj, pMatObj->GetIndexForName(pPropObj->GetName()));
			
			mtrlCommands::EnableMenus(true, true, true, pMatObj->IsLockMaterials());
		}
		else
		{
			mtrlOperations::SetSelectedMaterialIndex(pMatObj, -1);

			bool bHasMaterials = (pMatObj->GetNumMaterials() > 0);
			mtrlCommands::EnableMenus(true, bHasMaterials, false, pMatObj->IsLockMaterials());
		}
	}
	else
	{
		mtrlDialogUtil::UpdateDialog(NULL);
		// this is safe to call even if not added
		mtrlDialogUtil::RemoveDataPage();

		// Clear the selection from the operations namespace
		mtrlOperations::SetSelectedMaterialIndex( NULL, -1 );
		mtrlCommands::EnableMenus(false, false, false, false);
	}
}

//--------------------------------------------------------------------
//	AddedToSelection - called when an object is added to the
//		selection list.
//--------------------------------------------------------------------
//virtual 
void mtrlSelectInterest::AddedToSelection( sel3dObject* i_pSelObj )
{
	if ( mtrlScriptObject *pSurfaceObj = sel3dCastUtil::CastPickObject<mtrlScriptObject>(i_pSelObj) )
	{
		// This case handles selection of a Material pick object
		if ( mtrlPropertyObject *pPropObj = sel3dCastUtil::CastPickObject<mtrlPropertyObject>(i_pSelObj) )
		{
			// Begin the flash animation whenever a material is selected
			mtrlHighlight::BeginHighlightFlashAnimation();

			mtrlHighlight::HighlightMaterialIndex(pSurfaceObj, pSurfaceObj->GetIndexForName(pPropObj->GetName()), true);
		}
	}
}

//--------------------------------------------------------------------
//	RemovedFromSelection - called when an object is removed from
//		the selection list.
//--------------------------------------------------------------------
//virtual 
void mtrlSelectInterest::RemovedFromSelection( sel3dObject* i_pSelObj )
{
	if ( mtrlScriptObject *pSurfaceObj = sel3dCastUtil::CastPickObject<mtrlScriptObject>(i_pSelObj) )
	{
		// This case handles selection of a Material pick object
		if ( mtrlPropertyObject *pPropObj = sel3dCastUtil::CastPickObject<mtrlPropertyObject>(i_pSelObj) )
		{
			mtrlHighlight::HighlightMaterialIndex(pSurfaceObj, pSurfaceObj->GetIndexForName(pPropObj->GetName()), false);
		}
	}
}

//--------------------------------------------------------------------
// Returns the selectable object for the material part selected
// with the GPU pick code given. Returns NULL if no material selected.
//--------------------------------------------------------------------
sel3dObject* mtrlSelectInterest::GetPartFromPickCode(sel3dObject *i_pPicked,
													envType::UInt32 i_PickCode)
{
	if (mtrlScriptObject *pMatObject = sel3dCastUtil::CastPickObject<mtrlScriptObject>(i_pPicked))
	{
		// When picking materials, do a quiet override of materials
		//if (pMatObject->GetNumMaterials() == 0)
		//{
		//	mtrlOperations::OverrideMaterials(pMatObject);
		//}

		// Then pick the material from pick code.
		if (pMatObject->GetNumMaterials() > 0)
		{
			int index = pMatObject->GetMaterialIndexFromPickCode(i_PickCode);
			if (index >= 0)
			{
				return pMatObject->GetMaterialUI(index);
			}
		}
	}
	return NULL;
}


//--------------------------------------------------------------------
//	SelectFromPickCode - check the pick code of the selected
//	object and set the selected material index to match.
//--------------------------------------------------------------------
bool mtrlSelectInterest::SelectFromPickCode(sel3dObject *i_pPicked,
											envType::UInt32 i_PickCode,
											bool i_bAppendSelection)
{
	if (sel3dObject *pMaterialObj = mtrlSelectInterest::GetPartFromPickCode(i_pPicked, i_PickCode))
	{
		sel3dMgr::CreateUndoOperation();
		if (i_bAppendSelection)
			sel3dMgr::AddToSelection(pMaterialObj);
		else
			sel3dMgr::Select(pMaterialObj); 
		return true;
	}
	return false;
}
