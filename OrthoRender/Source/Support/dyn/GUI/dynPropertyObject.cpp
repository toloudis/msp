/****************************************************************************\
**  dynPropertyObject.cpp
**
**		see .hpp
**
**  Extra Large Technology
**  Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Support/dyn/GUI/dynPropertyObject.hpp"

#include "Core/prty/prtyRangedFloatUIInfo.hpp"
#include "Core/prty/prtyTextBoxUIInfo.hpp"
#include "Core/prty/prtyVector3dEditUpDownUIInfo.hpp"
#include "Graphics/Sc/scTransformControl.hpp"
#include "Tool/api3d/api3dScale.hpp"

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
dynPropertyObject::dynPropertyObject(const dynControlData& i_Data, 
									 scTransformControl* i_pControl,
									 pick3dPickObject* i_pParent)
:	m_pControl(i_pControl), m_pParent(i_pParent)
{
	// Only the node should be set early in the constructor. The
	// rest of the data should only be set after the callbacks
	// are set up.
	m_Data.m_Node = i_Data.m_Node;

	prtyTextBoxUIInfo* pTBUII = NULL;
	pTBUII = new prtyTextBoxUIInfo(&(m_Data.m_Name), "Control", "Name for joint control");
	AddProperty(pTBUII);
	pTBUII = new prtyTextBoxUIInfo(&(m_Data.m_Node), "Control", "Node or joint being controlled");
	pTBUII->SetReadOnly( true );
	AddProperty(pTBUII);

	prtyRangedFloatUIInfo* pRFUII = NULL;
	pRFUII = new prtyRangedFloatUIInfo(&(m_Data.m_RotateX), "Transform", "Rotation around X axis in degrees");
	pRFUII->SetMinimum(-180.0f);
	pRFUII->SetMaximum(180.0f);
	pRFUII->SetDecimalPlaces(2);
	pRFUII->SetNumTicks(200);
	AddProperty( pRFUII );
	pRFUII = new prtyRangedFloatUIInfo(&(m_Data.m_RotateY), "Transform", "Rotation around Y axis in degrees");
	pRFUII->SetMinimum(-180.0f);
	pRFUII->SetMaximum(180.0f);
	pRFUII->SetDecimalPlaces(2);
	pRFUII->SetNumTicks(200);
	AddProperty( pRFUII );
	pRFUII = new prtyRangedFloatUIInfo(&(m_Data.m_RotateZ), "Transform", "Rotation around Z axis in degrees");
	pRFUII->SetMinimum(-180.0f);
	pRFUII->SetMaximum(180.0f);
	pRFUII->SetDecimalPlaces(2);
	pRFUII->SetNumTicks(200);
	AddProperty( pRFUII );
	prtyVector3dEditUpDownUIInfo *pPVecUII = NULL;
	pPVecUII = new prtyVector3dEditUpDownUIInfo(&(m_Data.m_Translation), "Transform", "Translation applied at joint control");
	float increment = api3dScale::Scale( 0.01f );
	pPVecUII->SetIncrement(increment, increment, increment);
	AddProperty( pPVecUII );
	pPVecUII = new prtyVector3dEditUpDownUIInfo(&(m_Data.m_Scale), "Transform", "Scale applied at joint control");
	pPVecUII->SetIncrement(0.01f, 0.01f, 0.01f);
	AddProperty( pPVecUII );

	m_Data.m_Name.AddCallback(new prtyCallbackWrapper<dynPropertyObject>(this, &dynPropertyObject::NameChanged));
	m_Data.m_RotateX.AddCallback(new prtyCallbackWrapper<dynPropertyObject>(this, &dynPropertyObject::RotateChanged));
	m_Data.m_RotateY.AddCallback(new prtyCallbackWrapper<dynPropertyObject>(this, &dynPropertyObject::RotateChanged));
	m_Data.m_RotateZ.AddCallback(new prtyCallbackWrapper<dynPropertyObject>(this, &dynPropertyObject::RotateChanged));
	m_Data.m_Translation.AddCallback(new prtyCallbackWrapper<dynPropertyObject>(this, &dynPropertyObject::TransformChanged));
	m_Data.m_Scale.AddCallback(new prtyCallbackWrapper<dynPropertyObject>(this, &dynPropertyObject::TransformChanged));

	// Set up the initial data. Since the callbacks have now been connected,
	// these changes will be set into the control transform.
	m_Data.m_Name = i_Data.m_Name;
	m_Data.m_RotateX = i_Data.m_RotateX;
	m_Data.m_RotateY = i_Data.m_RotateY;
	m_Data.m_RotateZ = i_Data.m_RotateZ;
	m_Data.m_Scale = i_Data.m_Scale;
	m_Data.m_Translation = i_Data.m_Translation;
}

dynPropertyObject::~dynPropertyObject()
{
}


//============================================================================
// pick3dPickObject - virtual function overrides
//============================================================================

//--------------------------------------------------------------------
// Set Parent pointer to use when creating selectable property objects
//--------------------------------------------------------------------
void dynPropertyObject::SetParent(pick3dPickObject* i_pParent)
{
	m_pParent = i_pParent;
}

//------------------------------------------------------------------------
// Get parent object of this object in order to define relationships
//	between icons and their affected objects.
//------------------------------------------------------------------------
//virtual 
pick3dPickObject* dynPropertyObject::GetParentObject() const
{
	return m_pParent;
}

//------------------------------------------------------------------------
// Get the name of the object.
//------------------------------------------------------------------------
//virtual 
std::string dynPropertyObject::GetPick3dName() const
{
	return this->GetName();	
}

//----------------------------------------------------------------------------
// property callbacks
//----------------------------------------------------------------------------
void dynPropertyObject::NameChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
}
void dynPropertyObject::RotateChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	maVector3d euler_angles(m_Data.m_RotateX.GetValue(),
							m_Data.m_RotateY.GetValue(),
							m_Data.m_RotateZ.GetValue());
	m_pControl->SetEulerAngles(euler_angles);
}
void dynPropertyObject::TransformChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	m_pControl->SetTranslation(m_Data.m_Translation.GetValue());
	m_pControl->SetScale(m_Data.m_Scale.GetValue());
}
