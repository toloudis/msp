/*****************************************************************************
**  lsetGlobalObject.hpp
**
**      A lsetGlobalObject is the property object for controlling 
**	global ambient color
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef LSET_GLOBALOBJECT_HPP
#error lsetGlobalObject.hpp multiply included
#endif
#define LSET_GLOBALOBJECT_HPP

#ifndef NAME_OBJECT_HPP
#include "Core/name/nameObject.hpp"
#endif
#ifndef PRTY_COLOR_HPP
#include "Core/prty/prtyColor.hpp"
#endif 
#ifndef PRTY_OBJECT_HPP
#include "Core/prty/prtyObject.hpp"
#endif
#ifndef PICK3D_PICKOBJECT_HPP
#include "Tool/pick3d/pick3dPickObject.hpp"
#endif

//============================================================================
//============================================================================
class lsetGlobalObject : 
	public prtyObject,
	public pick3dPickObject
{
public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		lsetGlobalObject();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~lsetGlobalObject();

	//============================================================================
	//	Overloads
	//============================================================================

		//--------------------------------------------------------------------
		// Get the name of the object.
		//--------------------------------------------------------------------
		virtual std::string GetPick3dName() const;

	//============================================================================
	//	properties
	//============================================================================

		//--------------------------------------------------------------------
		// AmbientLight property access
		//--------------------------------------------------------------------
		prtyColor&	PropertyAmbientLight();
		const prtyColor&	GetPropertyAmbientLight() const;


	private:
		//--------------------------------------------------------------------
		// Callbacks for when properties change, updates member data
		//--------------------------------------------------------------------
		void ValueChanged(prtyProperty *i_pProperty, bool i_bDirty);

		prtyColor	m_AmbientLight;
};


