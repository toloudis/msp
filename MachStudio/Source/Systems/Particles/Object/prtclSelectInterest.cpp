/****************************************************************************\
**	prtclSelectInterest.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Systems/Particles/Object/prtclSelectInterest.hpp"

#include "Systems/Particles/Object/prtclObjectMgr.hpp"
#include "Systems/Particles/GUI/prtclDialogUtil.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Support/tmln/tmlnSelectionUtil.hpp"


//--------------------------------------------------------------------
//	SelectionChanged
//--------------------------------------------------------------------
//virtual 
void prtclSelectInterest::SelectionChanged()
{

}

//--------------------------------------------------------------------
//	AddedToSelection - called when an object is added to the
//		selection list.
//--------------------------------------------------------------------
//virtual 
void prtclSelectInterest::AddedToSelection( pick3dPickObject* i_pSelObj )
{
	if (tmlnScriptObject *pScriptObject = tmlnSelectionUtil::GetScriptObject(i_pSelObj))
	{
		if ( prtclScriptObject *pParticle = dynamic_cast<prtclScriptObject*>(pScriptObject) )
		{
			pParticle->SetSelected(true);
		}
	}
}

//--------------------------------------------------------------------
//	RemovedFromSelection - called when an object is removed from
//		the selection list.
//--------------------------------------------------------------------
//virtual 
void prtclSelectInterest::RemovedFromSelection( pick3dPickObject* i_pSelObj )
{
	if (tmlnScriptObject *pScriptObject = tmlnSelectionUtil::GetScriptObject(i_pSelObj))
	{
		if ( prtclScriptObject *pParticle = dynamic_cast<prtclScriptObject*>(pScriptObject) )
		{
			// We know that the selection is related to a script object, but
			// there still may be something in the selection list related to the
			// script object, so we need to check the full list.
			std::vector<tmlnScriptObject*> objects;
			tmlnSelectionUtil::GetSelectedScriptObjects(objects);
			if (!envSTLHelpers::Contains(objects, pParticle))
			{
				pParticle->SetSelected(false);
			}
		}
	}
}

