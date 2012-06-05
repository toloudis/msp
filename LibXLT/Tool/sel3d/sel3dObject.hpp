/****************************************************************************\
**	sel3dObject.hpp
**
**	Base class for objects that can be selected
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef SEL3D_OBJECT_HPP
#error sel3dObject.hpp multiply included
#endif
#define SEL3D_OBJECT_HPP

#ifndef REL_OBJECT_HPP
#include "Core/rel/relObject.hpp"
#endif 

#include <string>


//============================================================================
//============================================================================
class sel3dObject : public relObject
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		sel3dObject();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~sel3dObject();

		//--------------------------------------------------------------------
		// Get parent object of this object in order to define relationships
		//	between icons and their affected objects.
		//--------------------------------------------------------------------
		//virtual sel3dObject* GetParentObject() const;

		//--------------------------------------------------------------------
		// Get the name of the object.
		//--------------------------------------------------------------------
		//virtual std::string GetDisplayName() const;
};
