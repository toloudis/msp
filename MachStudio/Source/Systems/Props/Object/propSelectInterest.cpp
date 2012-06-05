/****************************************************************************\
**	propSelectInterest.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Systems/Props/Object/propSelectInterest.hpp"

#include "Systems/Props/Object/propObjectMgr.hpp"
#include "Systems/Props/GUI/propDialogUtil.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Support/tmln/tmlnSelectionUtil.hpp"


//--------------------------------------------------------------------
//	SelectionChanged
//--------------------------------------------------------------------
//virtual 
void propSelectInterest::SelectionChanged()
{

}

//--------------------------------------------------------------------
//	AddedToSelection - called when an object is added to the
//		selection list.
//--------------------------------------------------------------------
//virtual 
void propSelectInterest::AddedToSelection( pick3dPickObject* i_pSelObj )
{
	if (tmlnScriptObject *pScriptObject = tmlnSelectionUtil::GetScriptObject(i_pSelObj))
	{
		if ( propScriptObject *pProp = dynamic_cast<propScriptObject*>(pScriptObject) )
		{
			pProp->SetSelected(true);
		}
	}
}

//--------------------------------------------------------------------
//	RemovedFromSelection - called when an object is removed from
//		the selection list.
//--------------------------------------------------------------------
//virtual 
void propSelectInterest::RemovedFromSelection( pick3dPickObject* i_pSelObj )
{
	if (tmlnScriptObject *pScriptObject = tmlnSelectionUtil::GetScriptObject(i_pSelObj))
	{
		if ( propScriptObject *pProp = dynamic_cast<propScriptObject*>(pScriptObject) )
		{
			// We know that the selection is related to a script object, but
			// there still may be something in the selection list related to the
			// script object, so we need to check the full list.
			std::vector<tmlnScriptObject*> objects;
			tmlnSelectionUtil::GetSelectedScriptObjects(objects);
			if (!envSTLHelpers::Contains(objects, pProp))
			{
				pProp->SetSelected(false);
			}
		}
	}
}
