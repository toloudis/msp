/****************************************************************************\
**	cmmNamedPropertyObject.hpp
**
**		Base class for objects that are both nameObject and
**	cmmSelectablePropertyObject. This class implements an object 
**	reference creator that identifies this object and its properties 
**	through nameMgr::GetObjectByName().
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef CMM_NAMEDPROPERTYOBJECT_HPP
#error cmmNamedPropertyObject.hpp multiply included
#endif
#define CMM_NAMEDPROPERTYOBJECT_HPP

#ifndef CMM_SELECTABLEPROPERTYOBJECT_HPP
#include "Systems/Common/Object/cmmSelectablePropertyObject.hpp"
#endif 
#ifndef NAME_OBJECT_HPP
#include "Core/name/nameObject.hpp"
#endif 


//============================================================================
//============================================================================
class cmmNamedPropertyObject : public cmmSelectablePropertyObject, 
							   public nameObject
{
	public:
		//--------------------------------------------------------------------
		//	CreateReferenceToSelf - create a reference that refers to the
		//	given object. This reference should be able to refer to 
		//  future instances of this object persistent through deletion
		//	and recreation through undo.
		//--------------------------------------------------------------------
		virtual shared_ptr<relObjectReference> CreateReferenceToSelf();
};
