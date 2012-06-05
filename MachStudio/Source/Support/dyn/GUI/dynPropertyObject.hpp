/********************************************************************************************\
**  dynPropertyObject.hpp
**
**	Selectable property object representing a joint control.
**
**  StudioGPU
**  Copyright(C) 2008 - All Rights Reserved
\********************************************************************************************/
#ifdef DYN_PROPERTYOBJECT_HPP
#error dynPropertyObject.hpp multiply included
#endif
#define DYN_PROPERTYOBJECT_HPP

#ifndef DYN_CONTROLDATA_HPP
#include "Support/dyn/dynControlData.hpp"
#endif 
#ifndef CMM_SELECTABLEPROPERTYOBJECT_HPP
#include "Systems/Common/Object/cmmSelectablePropertyObject.hpp"
#endif 


//============================================================================
//============================================================================
class dynControlData;
class scTransformControl;
class gpxTransformControl;


//============================================================================
//============================================================================
class dynPropertyObject : public cmmSelectablePropertyObject
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	dynPropertyObject(const dynControlData& i_Data, 
						scTransformControl* i_pControl);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual ~dynPropertyObject();

	//------------------------------------------------------------------------
	// Quick access to name 
	//------------------------------------------------------------------------
	inline const std::string&  GetName() const;

	//------------------------------------------------------------------------
	// Get access to data
	//------------------------------------------------------------------------
	inline const dynControlData& GetControlData() const;
	
	//------------------------------------------------------------------------
	// Access to properties
	//------------------------------------------------------------------------
	prtyFloat& GetPropertyRotateX()			{ return m_Data.m_RotateX; }
	prtyFloat& GetPropertyRotateY()			{ return m_Data.m_RotateY; }
	prtyFloat& GetPropertyRotateZ()			{ return m_Data.m_RotateZ; }
	prtyPoint3d& GetPropertyTranslation()	{ return m_Data.m_Translation; }
	prtyVector3d& GetPropertyScale()		{ return m_Data.m_Scale; }


//============================================================================
// sel3dObject - virtual function overrides
//============================================================================

	//--------------------------------------------------------------------
	// Set Parent pointer to use when creating selectable property objects
	//--------------------------------------------------------------------
	//void SetParent(sel3dObject* i_pParent);

	//------------------------------------------------------------------------
	// Get parent object of this object in order to define relationships
	//	between icons and their affected objects.
	//------------------------------------------------------------------------
	//virtual sel3dObject* GetParentObject() const;

	//------------------------------------------------------------------------
	// Get the name of the object for display when selected
	//------------------------------------------------------------------------
	virtual std::string GetDisplayName() const;

	//------------------------------------------------------------------------
	// Access to proxy for channel
	//------------------------------------------------------------------------
	inline gpxTransformControl* GetControlProxy() { return m_pControlProxy; }

private:
	dynControlData m_Data; 
	gpxTransformControl* m_pControlProxy; // thread-safe proxy
	//scTransformControl* m_pControl;
	//sel3dObject* m_pParent;

	//------------------------------------------------------------------------
	// Property callbacks
	//------------------------------------------------------------------------
	void NameChanged(prtyProperty *i_pProperty, bool i_bDirty);
	void RotateChanged(prtyProperty *i_pProperty, bool i_bDirty);
	void TransformChanged(prtyProperty *i_pProperty, bool i_bDirty);
};

//------------------------------------------------------------------------
// Quick access to name given in constructor
//------------------------------------------------------------------------
inline const std::string& dynPropertyObject::GetName() const
{
	return m_Data.m_Name.GetValue();
}

//--------------------------------------------------------------------
// Get access to data
//--------------------------------------------------------------------
inline const dynControlData& dynPropertyObject::GetControlData() const
{
	return m_Data;
}