/****************************************************************************\
**	sbrdSelectInterest.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/Storyboards/Object/sbrdSelectInterest.hpp"

#include "Systems/Storyboards/Object/sbrdObjectMgr.hpp"
#include "Systems/Storyboards/GUI/sbrdDialogUtil.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Support/tmln/tmlnSelectionUtil.hpp"


//--------------------------------------------------------------------
//	SelectionChanged
//--------------------------------------------------------------------
//virtual 
void sbrdSelectInterest::SelectionChanged()
{
}

//--------------------------------------------------------------------
//	AddedToSelection - called when an object is added to the
//		selection list.
//--------------------------------------------------------------------
//virtual 
void sbrdSelectInterest::AddedToSelection( pick3dPickObject* i_pSelObj )
{
	if (tmlnScriptObject *pScriptObject = tmlnSelectionUtil::GetScriptObject(i_pSelObj))
	{
		if ( sbrdScriptObject *pBillboard = dynamic_cast<sbrdScriptObject*>(pScriptObject) )
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
void sbrdSelectInterest::RemovedFromSelection( pick3dPickObject* i_pSelObj )
{
	if (tmlnScriptObject *pScriptObject = tmlnSelectionUtil::GetScriptObject(i_pSelObj))
	{
		if ( sbrdScriptObject *pBillboard = dynamic_cast<sbrdScriptObject*>(pScriptObject) )
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

