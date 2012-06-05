/****************************************************************************\
**	mtrlSelectInterest.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Support/mtrl/mtrlSelectInterest.hpp"

#include "Support/mtrl/mtrlCommands.hpp"
#include "Support/mtrl/GUI/mtrlDialogUtil.hpp"
#include "Support/mtrl/GUI/mtrlOperations.hpp"
#include "Support/mtrl/GUI/mtrlPropertyObject.hpp"
#include "Support/mtrl/mtrlScriptObject.hpp"

#include "Core/dbg/dbgLog.hpp"
#include "Tool/sel3d/sel3dCastUtil.hpp"

namespace
{
}

//--------------------------------------------------------------------
//	SelectionChanged - called when the selection list is changed
//--------------------------------------------------------------------
//virtual 
void mtrlSelectInterest::SelectionChanged()
{
	if ( mtrlScriptObject *pMatObj = sel3dCastUtil::CastSelectedObject<mtrlScriptObject>() )
	{
		//DBG_LOG0( "selected an mtrlScriptObject" );
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
			DBG_LOG0( "selected an mtrlPropertyObject" );
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
//	SelectFromPickCode - check the pick code of the selected
//	object and set the selected material index to match.
//--------------------------------------------------------------------
bool mtrlSelectInterest::SelectFromPickCode(pick3dPickObject *i_pPicked,
											envType::UInt32 i_PickCode)
{
	if (mtrlScriptObject *pMatObject = sel3dCastUtil::CastPickObject<mtrlScriptObject>(i_pPicked))
	{
		if (pMatObject->GetNumMaterials() > 0)
		{
			int index = pMatObject->GetMaterialIndexFromPickCode(i_PickCode);
			if (index >= 0)
			{
#ifdef _MANAGED
				mtrlOperations::SetSelectedMaterialIndex( pMatObject, index );
#else
				sel3dMgr::CreateUndoOperation();
				sel3dMgr::Select(pMatObject->GetMaterialUI(index));
#endif
				return true;
			}
		}
	}
	return false;
}
