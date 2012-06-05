/********************************************************************************************\
**  dynPropertyObject.hpp
**
**	Selectable property object representing a joint control.
**
**  Extra Large Technology
**  Copyright(C) 2008 - All Rights Reserved
\********************************************************************************************/
#ifdef DYN_PROPERTYOBJECT_HPP
#error dynPropertyObject.hpp multiply included
#endif
#define DYN_PROPERTYOBJECT_HPP

#ifndef DYN_CONTROLDATA_HPP
#include "Support/dyn/dynControlData.hpp"
#endif 
#ifndef PRTY_OBJECT_HPP
#include "Core/prty/prtyObject.hpp"
#endif
#ifndef PICK3D_PICKOBJECT_HPP
#include "Tool/pick3d/pick3dPickObject.hpp"
#endif 


//============================================================================
//============================================================================
class dynControlData;
class scTransformControl;


//============================================================================
//============================================================================
class dynPropertyObject : public prtyObject, public pick3dPickObject
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	dynPropertyObject(const dynControlData& i_Data, 
						scTransformControl* i_pControl,
						pick3dPickObject* i_pParent);

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
	prtyVector3d& GetPropertyTranslation()	{ return m_Data.m_Translation; }
	prtyVector3d& GetPropertyScale()		{ return m_Data.m_Scale; }


//============================================================================
// pick3dPickObject - virtual function overrides
//============================================================================

	//--------------------------------------------------------------------
	// Set Parent pointer to use when creating selectable property objects
	//--------------------------------------------------------------------
	void SetParent(pick3dPickObject* i_pParent);

	//------------------------------------------------------------------------
	// Get parent object of this object in order to define relationships
	//	between icons and their affected objects.
	//------------------------------------------------------------------------
	virtual pick3dPickObject* GetParentObject() const;

	//------------------------------------------------------------------------
	// Get the name of the object for display when selected
	//------------------------------------------------------------------------
	virtual std::string GetPick3dName() const;

private:
	dynControlData m_Data; 
	scTransformControl* m_pControl;
	pick3dPickObject* m_pParent;

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