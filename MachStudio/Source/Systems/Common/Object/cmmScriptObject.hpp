/****************************************************************************\
**	cmmScriptObject.hpp
**
**		Base class for objects that are both prtyObject and sel3dObject,
**	this class implements a reference creator that uses the relationships
**	defined through the relObject base class to find the property object
**	and then the property.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef CMM_SCRIPTOBJECT_HPP
#error cmmScriptObject.hpp multiply included
#endif
#define CMM_SCRIPTOBJECT_HPP

#ifndef XTRA_SCRIPTOBJECT_HPP
#include "Support/xtra/xtraScriptObject.hpp"
#endif 

#include <map>

//============================================================================
//============================================================================
class cmmSelectablePropertyObject;

//============================================================================
//============================================================================
class cmmScriptObject : public xtraScriptObject
{
	public:
		//--------------------------------------------------------------------
		//	SetPropertyObject - create a parent-child relationship between
		//		this script object and the property object it is animating.
		//--------------------------------------------------------------------
		void SetPropertyObject(cmmSelectablePropertyObject& i_ChildObject);

		//--------------------------------------------------------------------
		//	Remap internal name attachments using the given map.
		//  This is part of the duplication process and makes sures 
		//	internal attachments are passed onto the duplicated objects.
		//--------------------------------------------------------------------
		virtual void RemapNames(const std::map<nameString, nameString> &i_DuplicateNameMap);

	private:
};
