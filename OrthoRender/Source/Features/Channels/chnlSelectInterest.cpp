/****************************************************************************\
**	chnlSelectInterest.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Features/Channels/chnlSelectInterest.hpp"

#include "Features/Channels/chnlDialogUtil.hpp"
#include "Tool/pick3d/pick3dPickObject.hpp"
#include "Support/tmln/tmlnScriptObject.hpp"

//	library
//#include "Core/dbg/dbgAssert.hpp"
#include "Tool/sel3d/sel3dMgr.hpp"


//--------------------------------------------------------------------
//	SelectionChanged
//--------------------------------------------------------------------
//virtual 
void chnlSelectInterest::SelectionChanged()
{
	pick3dPickObject* pSelectedObject = sel3dMgr::GetSelected();
	if (pSelectedObject)
	{
		tmlnScriptObject* script_obj = dynamic_cast<tmlnScriptObject*>(pSelectedObject);
		if (pSelectedObject && !script_obj)
		{
			// If this is not a scripted object, it may be associated 
			// with a script object through the "parent object" relationship
			pick3dPickObject* cur_obj = pSelectedObject;
			while (!script_obj)
			{
				cur_obj = cur_obj->GetParentObject();
				if (!cur_obj) break;
				script_obj = dynamic_cast<tmlnScriptObject*>(cur_obj);
			}
		}
		chnlDialogUtil::ObjectSelected(script_obj);
		chnlDialogUtil::SetCanAddChannels(script_obj != NULL);
	}
	else
	{
		chnlDialogUtil::ObjectSelected(NULL);
		chnlDialogUtil::SetCanAddChannels(false);
	}
}
