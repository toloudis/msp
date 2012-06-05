/****************************************************************************\
**	cmraSelectInterest.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Systems/Cameras/Object/cmraSelectInterest.hpp"

#include "Systems/Cameras/Object/cmraObjectMgr.hpp"
#include "Systems/Cameras/GUI/cmraDialogUtil.hpp"
#include "Systems/Cameras/Object/cmraCameraObject.hpp"
#include "Systems/Cameras/Object/cmraScriptObject.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Support/mnm/mnmConstants.hpp"
#include "Tool/sel3d/sel3dMgr.hpp"
#include "Support/tmln/tmlnSelectionUtil.hpp"



//--------------------------------------------------------------------
//	SelectionChanged
//--------------------------------------------------------------------
//virtual 
void cmraSelectInterest::SelectionChanged()
{
}

//--------------------------------------------------------------------
//	AddedToSelection - called when an object is added to the
//		selection list.
//--------------------------------------------------------------------
//virtual 
void cmraSelectInterest::AddedToSelection( pick3dPickObject* i_pSelObj )
{
	if (tmlnScriptObject *pScriptObject = tmlnSelectionUtil::GetScriptObject(i_pSelObj))
	{
		if ( cmraScriptObject *pCamera = dynamic_cast<cmraScriptObject*>(pScriptObject) )
		{
			pCamera->SetSelected(true);
		}
	}
}

//--------------------------------------------------------------------
//	RemovedFromSelection - called when an object is removed from
//		the selection list.
//--------------------------------------------------------------------
//virtual 
void cmraSelectInterest::RemovedFromSelection( pick3dPickObject* i_pSelObj )
{
	if (tmlnScriptObject *pScriptObject = tmlnSelectionUtil::GetScriptObject(i_pSelObj))
	{
		if ( cmraScriptObject *pCamera = dynamic_cast<cmraScriptObject*>(pScriptObject) )
		{
			// We know that the selection is related to a script object, but
			// there still may be something in the selection list related to the
			// script object, so we need to check the full list.
			std::vector<tmlnScriptObject*> objects;
			tmlnSelectionUtil::GetSelectedScriptObjects(objects);
			if (!envSTLHelpers::Contains(objects, pCamera))
			{
				pCamera->SetSelected(false);
			}
		}
	}
}

