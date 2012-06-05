/*****************************************************************************
**	tmlnSelectionUtil.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Support/tmln/tmlnSelectionUtil.hpp"

#include "Tool/pick3d/pick3dPickObject.hpp"
#include "Support/tmln/tmlnScriptObject.hpp"
#include "Tool/sel3d/sel3dMgr.hpp"

#include "Core/env/envSTLHelpers.hpp"


namespace tmlnSelectionUtil
{
	namespace
	{
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		tmlnScriptObject*  get_script_object(pick3dPickObject* obj)
		{
			if (!obj) return NULL;

			// Sometimes the selected object is a spline control point
			// for a driver of a script object, so the parenting
			// relationship may be more than one level deep.
			//
			tmlnScriptObject* script_obj = dynamic_cast<tmlnScriptObject*>(obj);
			if (!script_obj)
			{
				// If this is not a scripted object, it may be associated 
				// with a script object through the "parent object" relationship
				pick3dPickObject* cur_obj = obj;
				while (!script_obj)
				{
					cur_obj = cur_obj->GetParentObject();
					if (!cur_obj) break;
					script_obj = dynamic_cast<tmlnScriptObject*>(cur_obj);
				}
			}
			return script_obj;

		}
	}

	//--------------------------------------------------------------------
	//  Get the script object from the "top" selected object (the most
	//		recent selected object is more than one is selected).
	//--------------------------------------------------------------------
	tmlnScriptObject*  GetSelectedScriptObject()
	{
		return get_script_object( sel3dMgr::GetSelected() );
	}
	
	//--------------------------------------------------------------------
	//  Get script objects that are in the selected list
	//--------------------------------------------------------------------
	void  GetSelectedScriptObjects(std::vector<tmlnScriptObject*> &o_Objects)
	{
		const std::list<pick3dPickObject*> &selected_list = sel3dMgr::GetSelectedList();
		std::list<pick3dPickObject*>::const_iterator it, end = selected_list.end();
		for (it = selected_list.begin(); it != end; ++it)
		{
			tmlnScriptObject* pScriptObject = get_script_object(*it);
			if (pScriptObject
				 && (!envSTLHelpers::Contains(o_Objects, pScriptObject)))
			{
				o_Objects.push_back(pScriptObject);
			}
		}
	}

	//--------------------------------------------------------------------
	//  Get the script object from the given pick object
	//--------------------------------------------------------------------
	tmlnScriptObject*  GetScriptObject(pick3dPickObject* i_pObj)
	{
		return get_script_object( i_pObj );
	}

}	// end of namespace

