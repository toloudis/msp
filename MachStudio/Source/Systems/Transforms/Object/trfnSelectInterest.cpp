/****************************************************************************\
**	trfnSelectInterest.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Systems/Transforms/Object/trfnSelectInterest.hpp"

#include "Support/tmln/tmlnSelectionUtil.hpp"
#include "Systems/Transforms/Object/trfnObjectMgr.hpp"
#include "Systems/Transforms/Undo/trfnOperations.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Tool/sel3d/sel3dCastUtil.hpp"


//--------------------------------------------------------------------
//	SelectionChanged
//--------------------------------------------------------------------
//virtual 
void trfnSelectInterest::SelectionChanged()
{
}

//--------------------------------------------------------------------
//	AddedToSelection - called when an object is added to the
//		selection list.
//--------------------------------------------------------------------
//virtual 
void trfnSelectInterest::AddedToSelection( sel3dObject* i_pSelObj )
{
	if (tmlnScriptObject *pScriptObject = tmlnSelectionUtil::GetScriptObject(i_pSelObj))
	{
		if ( trfnScriptObject *pTransform = dynamic_cast<trfnScriptObject*>(pScriptObject) )
		{
			pTransform->SetSelected(true);
		}
	}
}

//--------------------------------------------------------------------
//	RemovedFromSelection - called when an object is removed from
//		the selection list.
//--------------------------------------------------------------------
//virtual 
void trfnSelectInterest::RemovedFromSelection( sel3dObject* i_pSelObj )
{
	if (tmlnScriptObject *pScriptObject = tmlnSelectionUtil::GetScriptObject(i_pSelObj))
	{
		if ( trfnScriptObject *pTransform = dynamic_cast<trfnScriptObject*>(pScriptObject) )
		{
			// We know that the selection is related to the script object, but
			// there still may be something in the selection list related to the
			// same script object, so we need to check the full list.
			std::vector<tmlnScriptObject*> objects;
			tmlnSelectionUtil::GetSelectedScriptObjects(objects);
			if (!envSTLHelpers::Contains(objects, pTransform))
			{
				pTransform->SetSelected(false);
			}
		}
	}
}
