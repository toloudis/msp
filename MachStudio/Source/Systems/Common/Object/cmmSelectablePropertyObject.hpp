/****************************************************************************\
**	cmmSelectablePropertyObject.hpp
**
**		Base class for objects that are both prtyObject and sel3dObject,
**	this class implements a reference creator that uses the relationships
**	defined through the relObject base class to find the property object
**	and then the property.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef CMM_SELECTABLEPROPERTYOBJECT_HPP
#error cmmSelectablePropertyObject.hpp multiply included
#endif
#define CMM_SELECTABLEPROPERTYOBJECT_HPP

#ifndef PRTY_OBJECT_HPP
#include "Core/prty/prtyObject.hpp"
#endif 
#ifndef SEL3D_OBJECT_HPP
#include "Tool/sel3d/sel3dObject.hpp"
#endif 


//============================================================================
//============================================================================
class cmmSelectablePropertyObject : public prtyObject, public sel3dObject
{
	public:
		//--------------------------------------------------------------------
		//	CreateReferenceForProperty - given a property, create a
		//	shared_ptr to a prtyPropertyReference to this property.
		//--------------------------------------------------------------------
		virtual shared_ptr<prtyPropertyReference> CreateReferenceForProperty(prtyProperty& i_Property);

};
