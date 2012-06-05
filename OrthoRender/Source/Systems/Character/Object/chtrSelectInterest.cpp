/****************************************************************************\
**	chtrSelectInterest.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Systems/Character/Object/chtrSelectInterest.hpp"

#include "Systems/Character/Object/chtrObjectMgr.hpp"
#include "Systems/Character/GUI/chtrDialogUtil.hpp"
#include "Systems/Character/Expressions/chtrExpressionsDialogUtil.hpp"
#include "Systems/Character/Expressions/chtrExpressionObject.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Support/tmln/tmlnSelectionUtil.hpp"


//--------------------------------------------------------------------
//	SelectionChanged
//--------------------------------------------------------------------
//virtual 
void chtrSelectInterest::SelectionChanged()
{
	pick3dPickObject* pSelectedObject = sel3dMgr::GetSelected();
	if (pSelectedObject)
	{
		if ( chtrObject *Character = dynamic_cast<chtrObject*>(pSelectedObject) )
		{
			int index = chtrObjectMgr::GetIndexForObject(Character);
			if (index >= 0)
			{
				chtrScriptObject *pScriptObject = chtrObjectMgr::GetObject(index);

				chtrExpressionsDialogUtil::AddExpressionsPage();
				chtrExpressionsDialogUtil::UpdateExpressions(pScriptObject->ExpressionObject());
			}
		}
		else
		{
			//	clear out the form
			chtrExpressionsDialogUtil::RemoveExpressionsPage();
		}
	}
	else
	{
		//	clear out the form
		chtrExpressionsDialogUtil::RemoveExpressionsPage();
	}
}

//--------------------------------------------------------------------
//	AddedToSelection - called when an object is added to the
//		selection list.
//--------------------------------------------------------------------
//virtual 
void chtrSelectInterest::AddedToSelection( pick3dPickObject* i_pSelObj )
{
	if (tmlnScriptObject *pScriptObject = tmlnSelectionUtil::GetScriptObject(i_pSelObj))
	{
		if ( chtrScriptObject *pCharacter = dynamic_cast<chtrScriptObject*>(pScriptObject) )
		{
			pCharacter->SetSelected(true);
		}
	}
}

//--------------------------------------------------------------------
//	RemovedFromSelection - called when an object is removed from
//		the selection list.
//--------------------------------------------------------------------
//virtual 
void chtrSelectInterest::RemovedFromSelection( pick3dPickObject* i_pSelObj )
{
	if (tmlnScriptObject *pScriptObject = tmlnSelectionUtil::GetScriptObject(i_pSelObj))
	{
		if ( chtrScriptObject *pCharacter = dynamic_cast<chtrScriptObject*>(pScriptObject) )
		{
			// We know that the selection is related to a character object, but
			// there still may be something in the selection list related to the
			// script object, so we need to check the full list.
			std::vector<tmlnScriptObject*> objects;
			tmlnSelectionUtil::GetSelectedScriptObjects(objects);
			if (!envSTLHelpers::Contains(objects, pCharacter))
			{
				pCharacter->SetSelected(false);
			}
		}
	}
}
