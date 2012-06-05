/*****************************************************************************
**	tmlnChannelFloat.cpp
**
**	 single float value channel
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Support/tmln/tmlnChannelFloat.hpp"

#include "Core/env/envType.hpp"
#include "Core/prty/prtyFloat.hpp"
#include "Core/prty/prtyInt32.hpp"

#include <math.h>

//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnChannelFloat::tmlnChannelFloat(const char* i_Name)
:	tmlnChannel(i_Name), m_OriginalValue(0.0f)
{
}

//--------------------------------------------------------------------
//	The channel has the idea of an "original" value that it
//	goes to if there is no driver active.
//--------------------------------------------------------------------
void  tmlnChannelFloat::SetOriginalValue(float i_Val)
{
	m_OriginalValue = i_Val;
	this->MarkDirty();
}
float tmlnChannelFloat::GetOriginalValue() const
{
	return m_OriginalValue;
}

//--------------------------------------------------------------------
//	Reset to "original" value, the value when no driver is active.
//	It is up to the specific channel type implementation to
//	decide what that means.
//--------------------------------------------------------------------
void tmlnChannelFloat::Reset()
{
	this->SetValue( m_OriginalValue );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnChannelFloatProperty::tmlnChannelFloatProperty(const char* i_Name, prtyFloat& i_Property)
:	tmlnChannelFloat(i_Name),
	m_Property(i_Property), 
	m_bScriptedValue(true)
{	
	m_Property.AddCallback(new prtyCallbackWrapper<tmlnChannelFloatProperty>(this, &tmlnChannelFloatProperty::ValueChanged));
}

//--------------------------------------------------------------------
//  Set new value for property
//--------------------------------------------------------------------
void  tmlnChannelFloatProperty::SetValue(float i_Val)
{
	m_Property.SetValue(i_Val);
	m_bScriptedValue = true;
}

//--------------------------------------------------------------------
//  Get current value of property
//--------------------------------------------------------------------
float  tmlnChannelFloatProperty::GetValue() const
{
	return m_Property.GetValue();
}

//--------------------------------------------------------------------
// HasValueVariation - returns true if the current value of the
//	property is different than the scripted value of the channel.
//	This means the user has edited the values and is an opportunity 
//	to automatically add a key frame.
//--------------------------------------------------------------------
//virtual 
bool tmlnChannelFloatProperty::HasValueVariation()
{
	//return (m_ScriptedValue != m_Property.GetValue());
	return (!m_bScriptedValue);
}

//--------------------------------------------------------------------
// Callbacks for when property changes, updates original value
//--------------------------------------------------------------------
void tmlnChannelFloatProperty::ValueChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	// If we don't have any drivers, then update the "original position",
	// but don't mark it dirty so that we can detect this better
	// for auto-keying
	if (this->GetNumDrivers() == 0)
		m_OriginalValue = m_Property.GetValue();
	m_bScriptedValue = false;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnChannelInt32Property::tmlnChannelInt32Property(const char* i_Name, prtyInt32& i_Property)
:	tmlnChannelFloat(i_Name),
	m_Property(i_Property), 
	m_bScriptedValue(true)
{	
	m_Property.AddCallback(new prtyCallbackWrapper<tmlnChannelInt32Property>(this, &tmlnChannelInt32Property::ValueChanged));
}

//--------------------------------------------------------------------
//  Set new value for property
//--------------------------------------------------------------------
void  tmlnChannelInt32Property::SetValue(float i_Val)
{
	envType::Int32 val = 0;
	if (i_Val > 0)
		val = (envType::Int32)floor(i_Val + 0.5f);
	else if (i_Val < 0)
		val = (envType::Int32)ceil(i_Val - 0.5f);

	m_Property.SetValue(val);
	m_bScriptedValue = true;
}

//--------------------------------------------------------------------
//  Get current value of property
//--------------------------------------------------------------------
float  tmlnChannelInt32Property::GetValue() const
{
	return (float)m_Property.GetValue();
}

//--------------------------------------------------------------------
// HasValueVariation - returns true if the current value of the
//	property is different than the scripted value of the channel.
//	This means the user has edited the values and is an opportunity 
//	to automatically add a key frame.
//--------------------------------------------------------------------
//virtual 
bool tmlnChannelInt32Property::HasValueVariation()
{
	//return (m_ScriptedValue != m_Property.GetValue());
	return (!m_bScriptedValue);
}

//--------------------------------------------------------------------
// Callbacks for when property changes, updates original value
//--------------------------------------------------------------------
void tmlnChannelInt32Property::ValueChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	// If we don't have any drivers, then update the "original position",
	// but don't mark it dirty so that we can detect this better
	// for auto-keying
	if (this->GetNumDrivers() == 0)
		m_OriginalValue = (float)m_Property.GetValue();
	m_bScriptedValue = false;
}

