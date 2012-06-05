/*****************************************************************************
**	tmlnChannelRangedFloat.cpp
**
**	 float channel that has a meaningful "range" of allowed values that
**		allows it to be edited with a slider
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Support/tmln/tmlnChannelRangedFloat.hpp"

#include "Core/prty/prtyFloat.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnChannelRangedFloat::tmlnChannelRangedFloat(const char* i_Name,
												float i_MinValue, 
												float i_MaxValue)
:	tmlnChannelFloat(i_Name), 
	m_MinValue(i_MinValue), 
	m_MaxValue(i_MaxValue)
{
}

//--------------------------------------------------------------------
//	Min Value - minimum accepted value for this channel
//--------------------------------------------------------------------
void   tmlnChannelRangedFloat::SetMinValue(float i_Val)
{
	m_MinValue = i_Val;
}
float  tmlnChannelRangedFloat::GetMinValue() const
{
	return m_MinValue;
}

//--------------------------------------------------------------------
//	Max Value - maximum accepted value for this channel
//--------------------------------------------------------------------
void   tmlnChannelRangedFloat::SetMaxValue(float i_Val)
{
	m_MaxValue = i_Val;
}
float  tmlnChannelRangedFloat::GetMaxValue() const
{
	return m_MaxValue;
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnChannelRangedFloatProperty::tmlnChannelRangedFloatProperty(const char* i_Name, prtyFloat& i_Property)
:	tmlnChannelRangedFloat(i_Name, i_Property.GetMinimum(), i_Property.GetMaximum()),
	m_Property(i_Property), 
	m_bScriptedValue(true)
{	
	m_Property.AddCallback(new prtyCallbackWrapper<tmlnChannelRangedFloatProperty>(this, &tmlnChannelRangedFloatProperty::ValueChanged));
}

//--------------------------------------------------------------------
//  Set new value for property
//--------------------------------------------------------------------
void  tmlnChannelRangedFloatProperty::SetValue(float i_Val)
{
	m_Property.SetValue(i_Val);
	m_bScriptedValue = true;
}

//--------------------------------------------------------------------
//  Get current value of property
//--------------------------------------------------------------------
float  tmlnChannelRangedFloatProperty::GetValue() const
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
bool tmlnChannelRangedFloatProperty::HasValueVariation()
{
	//return (m_ScriptedValue != m_Property.GetValue());
	return (!m_bScriptedValue);
}

//--------------------------------------------------------------------
// Callbacks for when property changes, updates original value
//--------------------------------------------------------------------
void tmlnChannelRangedFloatProperty::ValueChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	// If we don't have any drivers, then update the "original position",
	// but don't mark it dirty so that we can detect this better
	// for auto-keying
	if (this->GetNumDrivers() == 0)
		m_OriginalValue = m_Property.GetValue();
	m_bScriptedValue = false;
}

