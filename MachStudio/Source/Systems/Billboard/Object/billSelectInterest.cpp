/****************************************************************************\
**	billSelectInterest.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Systems/Billboard/Object/billSelectInterest.hpp"

#include "Systems/Billboard/Object/billObjectMgr.hpp"
#include "Systems/Billboard/GUI/billDialogUtil.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Support/tmln/tmlnSelectionUtil.hpp"


//--------------------------------------------------------------------
//	SelectionChanged
//--------------------------------------------------------------------
//virtual 
void billSelectInterest::SelectionChanged()
{

}

//--------------------------------------------------------------------
//	AddedToSelection - called when an object is added to the
//		selection list.
//--------------------------------------------------------------------
//virtual 
void billSelectInterest::AddedToSelection( sel3dObject* i_pSelObj )
{
	if (tmlnScriptObject *pScriptObject = tmlnSelectionUtil::GetScriptObject(i_pSelObj))
	{
		if ( billScriptObject *pBillboard = dynamic_cast<billScriptObject*>(pScriptObject) )
		{
			pBillboard->SetSelected(true);
		}
	}
}

//--------------------------------------------------------------------
//	RemovedFromSelection - called when an object is removed from
//		the selection list.
//--------------------------------------------------------------------
//virtual 
void billSelectInterest::RemovedFromSelection( sel3dObject* i_pSelObj )
{
	if (tmlnScriptObject *pScriptObject = tmlnSelectionUtil::GetScriptObject(i_pSelObj))
	{
		if ( billScriptObject *pBillboard = dynamic_cast<billScriptObject*>(pScriptObject) )
		{
			// We know that the selection is related to a script object, but
			// there still may be something in the selection list related to the
			// script object, so we need to check the full list.
			std::vector<tmlnScriptObject*> objects;
			tmlnSelectionUtil::GetSelectedScriptObjects(objects);
			if (!envSTLHelpers::Contains(objects, pBillboard))
			{
				pBillboard->SetSelected(false);
			}
		}
	}
}

