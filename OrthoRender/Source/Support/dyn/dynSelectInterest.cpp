/****************************************************************************\
**	dynSelectInterest.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "Support/dyn/dynSelectInterest.hpp"

#include "Support/dyn/GUI/dynDialogUtil.hpp"
#include "Support/dyn/dynScriptObject.hpp"

#include "Tool/pick3d/pick3dPickObject.hpp"

#include "Core/dbg/dbgLog.hpp"
#include "Tool/sel3d/sel3dMgr.hpp"

namespace
{
	//--------------------------------------------------------------------
	// Finds if the selected object or its parent is a script object
	//--------------------------------------------------------------------
	dynScriptObject*  get_script_object(pick3dPickObject* i_pSelObj)
	{
		if (!i_pSelObj) return NULL;

		// Sometimes the selected object is a spline control point
		// for a driver of a script object, so the parenting
		// relationship may be more than one level deep.
		//
		dynScriptObject* script_obj = dynamic_cast<dynScriptObject*>(i_pSelObj);
		if (!script_obj)
		{
			// If this is not a scripted object, it may be associated 
			// with a script object through the "parent object" relationship
			pick3dPickObject* cur_obj = i_pSelObj;
			while (!script_obj)
			{
				cur_obj = cur_obj->GetParentObject();
				if (!cur_obj) break;
				script_obj = dynamic_cast<dynScriptObject*>(cur_obj);
			}
		}
		return script_obj;
	}
}

//--------------------------------------------------------------------
//	SelectionChanged - called when the selection list is changed
//--------------------------------------------------------------------
//virtual 
void dynSelectInterest::SelectionChanged()
{
	pick3dPickObject* pSelectedObject = sel3dMgr::GetSelected();
	if (pSelectedObject)
	{
		if ( dynScriptObject *pDynObj = get_script_object(pSelectedObject) )
		{
			//DBG_LOG0( "selected an dynScriptObject" );

			dynDialogUtil::UpdateDialog(pDynObj);
			dynDialogUtil::AddDataPage();

		}
		else
		{
			// this is safe to call even if not added
			dynDialogUtil::RemoveDataPage();
		}
	}
	else
	{
		// this is safe to call even if not added
		dynDialogUtil::RemoveDataPage();
	}
}
