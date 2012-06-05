/****************************************************************************\
**	ptltSelectInterest.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Systems/PtLt/Object/ptltSelectInterest.hpp"

#include "Systems/PtLt/Object/ptltObjectMgr.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Support/tmln/tmlnSelectionUtil.hpp"


//--------------------------------------------------------------------
//	SelectionChanged
//--------------------------------------------------------------------
//virtual 
void ptltSelectInterest::SelectionChanged()
{
}

//--------------------------------------------------------------------
//	AddedToSelection - called when an object is added to the
//		selection list.
//--------------------------------------------------------------------
//virtual 
void ptltSelectInterest::AddedToSelection( sel3dObject* i_pSelObj )
{
	if (tmlnScriptObject *pScriptObject = tmlnSelectionUtil::GetScriptObject(i_pSelObj))
	{
		if ( ptltScriptObject *pPointLight = dynamic_cast<ptltScriptObject*>(pScriptObject) )
		{
			pPointLight->SetSelected(true);
		}
	}
}

//--------------------------------------------------------------------
//	RemovedFromSelection - called when an object is removed from
//		the selection list.
//--------------------------------------------------------------------
//virtual 
void ptltSelectInterest::RemovedFromSelection( sel3dObject* i_pSelObj )
{
	if (tmlnScriptObject *pScriptObject = tmlnSelectionUtil::GetScriptObject(i_pSelObj))
	{
		if ( ptltScriptObject *pPointLight = dynamic_cast<ptltScriptObject*>(pScriptObject) )
		{
			// We know that the selection is related to the script object, but
			// there still may be something in the selection list related to the
			// same script object, so we need to check the full list.
			std::vector<tmlnScriptObject*> objects;
			tmlnSelectionUtil::GetSelectedScriptObjects(objects);
			if (!envSTLHelpers::Contains(objects, pPointLight))
			{
				pPointLight->SetSelected(false);
			}
		}
	}
}
