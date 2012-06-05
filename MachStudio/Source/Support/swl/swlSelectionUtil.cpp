/*****************************************************************************
**	swlSelectionUtil.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "Support/swl/swlSelectionUtil.hpp"

#include "Tool/sel3d/sel3dObject.hpp"
#include "Support/swl/swlScriptObject.hpp"
#include "Tool/sel3d/sel3dMgr.hpp"

#include "Core/env/envSTLHelpers.hpp"


namespace swlSelectionUtil
{
	namespace
	{
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		swlScriptObject*  get_script_object(sel3dObject* obj)
		{
			if (!obj) return NULL;

			// Sometimes the selected object is a spline control point
			// for a driver of a script object, so the parenting
			// relationship may be more than one level deep.
			//
			swlScriptObject* script_obj = dynamic_cast<swlScriptObject*>(obj);
			if (!script_obj)
			{
				// If this is not a scripted object, it may be associated 
				// with a script object through the "parent object" relationship
				relObject* cur_obj = obj;
				while (!script_obj)
				{
					cur_obj = cur_obj->GetParentObject();
					if (!cur_obj) break;
					script_obj = dynamic_cast<swlScriptObject*>(cur_obj);
				}
			}
			return script_obj;

		}
	}

	//--------------------------------------------------------------------
	//  Get the script object from the "top" selected object (the most
	//		recent selected object is more than one is selected).
	//--------------------------------------------------------------------
	swlScriptObject*  GetSelectedScriptObject()
	{
		return get_script_object( sel3dMgr::GetSelected() );
	}
	
	//--------------------------------------------------------------------
	//  Get script objects that are in the selected list
	//--------------------------------------------------------------------
	void  GetSelectedScriptObjects(std::vector<swlScriptObject*> &o_Objects)
	{
		const std::list<sel3dObject*> &selected_list = sel3dMgr::GetSelectedList();
		std::list<sel3dObject*>::const_iterator it, end = selected_list.end();
		for (it = selected_list.begin(); it != end; ++it)
		{
			swlScriptObject* pScriptObject = get_script_object(*it);
			if (pScriptObject
				 && (!envSTLHelpers::Contains(o_Objects, pScriptObject)))
			{
				o_Objects.push_back(pScriptObject);
			}
		}
	}

	int GetSelectedScriptObjectsCount()
	{
		const std::list<sel3dObject*> &selected_list = sel3dMgr::GetSelectedList();
		return selected_list.size();
	}


	//--------------------------------------------------------------------
	//  Get the script object from the given pick object
	//--------------------------------------------------------------------
	swlScriptObject*  GetScriptObject(sel3dObject* i_pObj)
	{
		return get_script_object( i_pObj );
	}

}	// end of namespace

