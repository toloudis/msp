/****************************************************************************\
**	pick3dPickObject.hpp
**
**		A pickable object.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef PICK3D_PICKOBJECT_HPP
#error pick3dPickObject.hpp multiply included
#endif
#define PICK3D_PICKOBJECT_HPP

#include <string>


//============================================================================
//============================================================================
class pick3dPickObject
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		pick3dPickObject();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~pick3dPickObject();

		//--------------------------------------------------------------------
		// Get parent object of this object in order to define relationships
		//	between icons and their affected objects.
		//--------------------------------------------------------------------
		//virtual pick3dPickObject* GetParentObject() const;

		//--------------------------------------------------------------------
		// Get the name of the object.
		//--------------------------------------------------------------------
		//virtual std::string GetDisplayName() const;
};
