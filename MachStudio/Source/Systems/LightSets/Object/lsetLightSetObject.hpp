/*****************************************************************************
**  lsetLightSetObject.hpp
**
**      A lsetLightSetObject is the property object for a light set
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef LSET_LIGHTSETOBJECT_HPP
#error lsetLightSetObject.hpp multiply included
#endif
#define LSET_LIGHTSETOBJECT_HPP

#ifndef LSET_DATA_HPP
#include "Systems/LightSets/Data/lsetData.hpp"
#endif

#ifndef CMM_NAMEDPROPERTYOBJECT_HPP
#include "Systems/Common/Object/cmmNamedPropertyObject.hpp"
#endif 

#include <vector>

//============================================================================
//	forward references
//============================================================================
class ltstLightSet;
class nameString;

//============================================================================
//============================================================================
class lsetLightSetObject : public cmmNamedPropertyObject
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
		virtual std::string GetDisplayName() const;

		//--------------------------------------------------------------------
		// Get parent object of this object in order to define relationships
		//	between icons and their affected objects.
		//--------------------------------------------------------------------
		//virtual sel3dObject* GetParentObject() const;
		//void SetParentObject(sel3dObject* i_pPO);


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

		ltstLightSet* GetLightSet();

	private:
		//--------------------------------------------------------------------
		// Callbacks for when properties change, updates member data
		//--------------------------------------------------------------------
		void NameChanged(prtyProperty *i_pProperty, bool i_bDirty);

		mutable lsetData	m_Data;
		ltstLightSet*	m_pLightSet;

		sel3dObject*	m_pParent;
};


