/****************************************************************************\
**	prjltSelectInterest.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Systems/PrjLt/Object/prjltSelectInterest.hpp"

#include "Systems/PrjLt/Object/prjltObjectMgr.hpp"
#include "Systems/PrjLt/GUI/prjltDialogUtil.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Support/tmln/tmlnSelectionUtil.hpp"


//--------------------------------------------------------------------
//	SelectionChanged
//--------------------------------------------------------------------
//virtual 
void prjltSelectInterest::SelectionChanged()
{

}

//--------------------------------------------------------------------
//	AddedToSelection - called when an object is added to the
//		selection list.
//--------------------------------------------------------------------
//virtual 
void prjltSelectInterest::AddedToSelection( pick3dPickObject* i_pSelObj )
{
	if (tmlnScriptObject *pScriptObject = tmlnSelectionUtil::GetScriptObject(i_pSelObj))
	{
		if ( prjltScriptObject *pProjectedLight = dynamic_cast<prjltScriptObject*>(pScriptObject) )
		{
			pProjectedLight->SetSelected(true);
		}
	}
}

//--------------------------------------------------------------------
//	RemovedFromSelection - called when an object is removed from
//		the selection list.
//--------------------------------------------------------------------
//virtual 
void prjltSelectInterest::RemovedFromSelection( pick3dPickObject* i_pSelObj )
{
	if (tmlnScriptObject *pScriptObject = tmlnSelectionUtil::GetScriptObject(i_pSelObj))
	{
		if ( prjltScriptObject *pProjectedLight = dynamic_cast<prjltScriptObject*>(pScriptObject) )
		{
			// We know that the selection is related to a script object, but
			// there still may be something in the selection list related to the
			// script object, so we need to check the full list.
			std::vector<tmlnScriptObject*> objects;
			tmlnSelectionUtil::GetSelectedScriptObjects(objects);
			if (!envSTLHelpers::Contains(objects, pProjectedLight))
			{
				pProjectedLight->SetSelected(false);
			}
		}
	}
}

