/****************************************************************************\
**	dynSelectInterest.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Support/dyn/dynSelectInterest.hpp"

#include "Support/dyn/dynScriptObject.hpp"
#include "Support/dyn/GUI/dynDialogUtil.hpp"

#include "Tool/sel3d/sel3dMgr.hpp"
#include "Tool/sel3d/sel3dObject.hpp"


//============================================================================
//============================================================================
namespace
{
	//--------------------------------------------------------------------
	// Finds if the selected object or its parent is a script object
	//--------------------------------------------------------------------
	dynScriptObject*  get_script_object(sel3dObject* i_pSelObj)
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
			relObject* cur_obj = i_pSelObj;
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
	sel3dObject* pSelectedObject = sel3dMgr::GetSelected();
	if (pSelectedObject)
	{
		if ( dynScriptObject *pDynObj = get_script_object(pSelectedObject) )
		{
			//DBG_LOG( "selected an dynScriptObject" );

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
