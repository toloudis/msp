/*****************************************************************************
**  lsetLightSetObject.hpp
**
**      A lsetLightSetObject is the property object for a light set
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef LSET_LIGHTSETOBJECT_HPP
#error lsetLightSetObject.hpp multiply included
#endif
#define LSET_LIGHTSETOBJECT_HPP

#ifndef LSET_DATA_HPP
#include "Systems/LightSets/Data/lsetData.hpp"
#endif

#ifndef NAME_OBJECT_HPP
#include "Core/name/nameObject.hpp"
#endif
#ifndef PRTY_OBJECT_HPP
#include "Core/prty/prtyObject.hpp"
#endif
#ifndef PICK3D_PICKOBJECT_HPP
#include "Tool/pick3d/pick3dPickObject.hpp"
#endif

#include <vector>

//============================================================================
//	forward references
//============================================================================
class ltstLightSet;
class nameString;

//============================================================================
//============================================================================
class lsetLightSetObject : 
	public nameObject, 
	public prtyObject,
	public pick3dPickObject
{
public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		lsetLightSetObject(const nameString& i_Name);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~lsetLightSetObject();

	//============================================================================
	//	Overloads
	//============================================================================

		//--------------------------------------------------------------------
		// Get the name of the object.
		//--------------------------------------------------------------------
		virtual std::string GetPick3dName() const;

		//--------------------------------------------------------------------
		// Get parent object of this object in order to define relationships
		//	between icons and their affected objects.
		//--------------------------------------------------------------------
		virtual pick3dPickObject* GetParentObject() const;
		void SetParentObject(pick3dPickObject* i_pPO);


	//============================================================================
	//	Data
	//============================================================================

		//--------------------------------------------------------------------
		// Get values as a light set data structure
		//--------------------------------------------------------------------
		const lsetData& GetData() const;

		//--------------------------------------------------------------------
		// Set from light set data structure
		//--------------------------------------------------------------------
		void SetData(const lsetData &i_Data);


	//============================================================================
	//	properties
	//============================================================================

		//--------------------------------------------------------------------
		// Name property access
		//--------------------------------------------------------------------
		prtyName&	PropertyName();
		const prtyName&	GetPropertyName() const;

		//--------------------------------------------------------------------
		// AmbientLight property access
		//--------------------------------------------------------------------
		prtyColor&	PropertyAmbientLight();
		const prtyColor&	GetPropertyAmbientLight() const;


	//============================================================================
	//	3D icon/geometry
	//============================================================================

		//--------------------------------------------------------------------
		// SetName
		//--------------------------------------------------------------------
		void SetName(const nameString& i_Name);

		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		//void Add(const nameString& i_ObjectName);
		//void Remove(const nameString& i_ObjectName);
		//void RefreshNames();

	private:
		//--------------------------------------------------------------------
		// Callbacks for when properties change, updates member data
		//--------------------------------------------------------------------
		void NameChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void ValueChanged(prtyProperty *i_pProperty, bool i_bDirty);

		mutable lsetData	m_Data;
		ltstLightSet*	m_pLightSet;

		pick3dPickObject*	m_pParent;
};


