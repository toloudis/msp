/*****************************************************************************
**	tmlnChannelOrientation.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Support/tmln/tmlnChannelOrientation.hpp"

#include "Core/prty/prtyRotation.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnChannelOrientation::tmlnChannelOrientation(const char* i_Name)
:	tmlnChannel(i_Name), 
	m_OriginalOrientation(0,0,0)
{
}

//--------------------------------------------------------------------
//	The channel has the idea of an "original" orientation that it
//	goes to if there is no driver active.
//--------------------------------------------------------------------
void  tmlnChannelOrientation::SetOriginalValue(float i_X, float i_Y, float i_Z)
{
	m_OriginalOrientation.Set(i_X, i_Y, i_Z);
	this->MarkDirty();
}
void  tmlnChannelOrientation::GetOriginalValue(float &o_X, float &o_Y, float &o_Z) const
{
	o_X = m_OriginalOrientation.m_X;
	o_Y = m_OriginalOrientation.m_Y;
	o_Z = m_OriginalOrientation.m_Z;
}

//--------------------------------------------------------------------
//	Reset to "original" value, the value when no driver is active.
//	It is up to the specific channel type implementation to
//	decide what that means.
//--------------------------------------------------------------------
void tmlnChannelOrientation::Reset()
{
	this->SetEuler( m_OriginalOrientation.m_X,
					m_OriginalOrientation.m_Y,
					m_OriginalOrientation.m_Z );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnChannelOrientationProperty::tmlnChannelOrientationProperty(const char* i_Name, prtyRotation& i_Property)
:	tmlnChannelOrientation(i_Name),
	m_Property(i_Property), 
	m_bScriptedValue(true)
{	
	m_Property.AddCallback(new prtyCallbackWrapper<tmlnChannelOrientationProperty>(this, &tmlnChannelOrientationProperty::ValueChanged));
}

//--------------------------------------------------------------------
//  Set new orientation using property
//--------------------------------------------------------------------
void  tmlnChannelOrientationProperty::SetQuaternion(const maRotation &i_Rot)
{
	m_Property.SetQuaternion(i_Rot);
	m_bScriptedValue = true;
}

//--------------------------------------------------------------------
//  Get orientation of object using property
//--------------------------------------------------------------------
maRotation  tmlnChannelOrientationProperty::GetQuaternion() const
{
	return m_Property.GetQuaternion();
}

//--------------------------------------------------------------------
//	Set rotation through 3 euler angles (in radians)
//--------------------------------------------------------------------
void tmlnChannelOrientationProperty::SetEuler(float i_X, float i_Y, float i_Z)
{
	m_Property.SetEuler(i_X, i_Y, i_Z);
	m_bScriptedValue = true;
}
void tmlnChannelOrientationProperty::GetEuler(float &o_X, float &o_Y, float &o_Z) const
{
	m_Property.GetEuler(o_X, o_Y, o_Z);
}

//--------------------------------------------------------------------
// HasValueVariation - returns true if the current value of the
//	property is different than the scripted value of the channel.
//	This means the user has edited the values and is an opportunity 
//	to automatically add a key frame.
//--------------------------------------------------------------------
//virtual 
bool tmlnChannelOrientationProperty::HasValueVariation()
{	
	//// calc cosine
	//maRotation prop_val = m_Property.GetValue();
	//float cosom = m_ScriptedValue.GetX() * prop_val.GetX() + 
	//		m_ScriptedValue.GetY() * prop_val.GetY() + 
	//		m_ScriptedValue.GetZ() * prop_val.GetZ() +
	//		m_ScriptedValue.GetW() * prop_val.GetW();

	//// calculate coefficients
	//const float c_Epsilon = 0.0001f;
	//return ( fabsf(1.0f - cosom) > c_Epsilon );
	return (!m_bScriptedValue);
}

//--------------------------------------------------------------------
// Callbacks for when property changes, updates original value
//--------------------------------------------------------------------
void tmlnChannelOrientationProperty::ValueChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	// If we don't have any drivers, then update the "original position",
	// but don't mark it dirty so that we can detect this better
	// for auto-keying
	if (this->GetNumDrivers() == 0)
	{
		m_Property.GetEuler(m_OriginalOrientation.m_X,
							m_OriginalOrientation.m_Y,
							m_OriginalOrientation.m_Z);
	}
	m_bScriptedValue = false;
}
