/*****************************************************************************\
**	sel3dCastUtil.hpp
**
**	Utilities for getting the selection objects cast into the type
**	you are looking for.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef SEL3D_CASTUTIL_HPP
#error sel3dCastUtil.hpp multiply included
#endif
#define SEL3D_CASTUTIL_HPP

#ifndef SEL3D_MGR_HPP
#include "Tool/sel3d/sel3dMgr.hpp"
#endif 
#ifndef SEL3D_OBJECT_HPP
#include "Tool/sel3d/sel3dObject.hpp"
#endif 
#ifndef PICK3D_PICKOBJECT_HPP
#include "Tool/pick3d/pick3dPickObject.hpp"
#endif 


//============================================================================
//============================================================================
namespace sel3dCastUtil
{
	//--------------------------------------------------------------------
	// Convert the results of a GPU pick to a selectable object. This
	// may return NULL if the picked object is not of a selectable type.
	//--------------------------------------------------------------------
	sel3dObject* ConvertPickToSelection(pick3dPickObject *i_pPickObject);

	//--------------------------------------------------------------------
	//	Cast pick object to type, looking at parent objects
	//	until one of the type needed is found.
	//	Returns NULL if it cannot be found.
	//--------------------------------------------------------------------
	template<class T>
	T*  CastPickObject(sel3dObject* i_pObject)
	{
		if (!i_pObject) return NULL;

		// First try to cast the object itself
		T* casted_obj = dynamic_cast<T*>(i_pObject);
		if (!casted_obj)
		{
			// If this is not the right type, use the "parent object" 
			// relationship to look for a containing object that has
			// the right type.
			relObject* cur_obj = i_pObject;
			while (!casted_obj)
			{
				cur_obj = cur_obj->GetParentObject();
				if (!cur_obj) break;
				casted_obj = dynamic_cast<T*>(cur_obj);
			}
		}
		return casted_obj;
	}

	//--------------------------------------------------------------------
	//	Cast the selected object to the type desired
	//--------------------------------------------------------------------
	template<class T>
	T*  CastSelectedObject()
	{
		sel3dObject* pSelectedObject = sel3dMgr::GetSelected();
		if (pSelectedObject)
		{
			return CastPickObject<T>(pSelectedObject);
		}
		return NULL;
	}
};
