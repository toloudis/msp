/*****************************************************************************
**  lyrsLayerObject.hpp
**
**      A lyrsLayerObject is the property object for a layer
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef LYRS_LAYEROBJECT_HPP
#error lyrsLayerObject.hpp multiply included
#endif
#define LYRS_LAYEROBJECT_HPP

#ifndef LYRS_DATA_HPP
#include "Systems/Layers/Data/lyrsData.hpp"
#endif
#ifndef LYER_LAYERMGR_HPP
#include "Support/lyer/lyerLayerMgr.hpp"
#endif 
#ifndef CMM_NAMEDPROPERTYOBJECT_HPP
#include "Systems/Common/Object/cmmNamedPropertyObject.hpp"
#endif 

#include <vector>

//============================================================================
//	forward references
//============================================================================
class fsResourceTrackerData;
class nameString;

//============================================================================
//============================================================================
class lyrsLayerObject : public cmmNamedPropertyObject
{
public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		lyrsLayerObject(const lyrsData& i_Data,
						const nameString& i_Name);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~lyrsLayerObject();

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

		//--------------------------------------------------------------------
		//  get a list of resources.  the resources will be appended to the
		//	passed in list.
		//--------------------------------------------------------------------
		virtual void GetResourceList( fsResourceTrackerData& io_List );

	//============================================================================
	//	Data
	//============================================================================

		//--------------------------------------------------------------------
		// Get values as a layer data structure
		//--------------------------------------------------------------------
		const lyrsData& GetData() const;

		//--------------------------------------------------------------------
		// Set from layer data structure
		//--------------------------------------------------------------------
		void SetData(const lyrsData &i_Data);


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

	private:
		//--------------------------------------------------------------------
		// Callbacks for when properties change, updates member data
		//--------------------------------------------------------------------
		void NameChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void ValueChanged(prtyProperty *i_pProperty, bool i_bDirty);

		mutable lyrsData	m_Data;
		lyerLayerHandle		m_pLayer;

		sel3dObject*	m_pParent;
};


