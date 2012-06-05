/*****************************************************************************
**	tmlnChannelBoolean.cpp
**
**	 boolean channel (on/off, enabled, true/false, etc.)
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/


#include "Support/tmln/tmlnChannelBoolean.hpp"

#include "Core/prty/prtyBoolean.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnChannelBoolean::tmlnChannelBoolean(const char* i_Name)
:	tmlnChannel(i_Name), m_OriginalState(true)
{
}

//--------------------------------------------------------------------
//	The channel has the idea of an "original" state that it
//	goes to if there is no driver active.
//--------------------------------------------------------------------
void  tmlnChannelBoolean::SetOriginalState(bool i_Val)
{
	m_OriginalState = i_Val;
	this->MarkDirty();
}
bool tmlnChannelBoolean::GetOriginalState() const
{
	return m_OriginalState;
}

//--------------------------------------------------------------------
//	Reset to "original" value, the value when no driver is active.
//	It is up to the specific channel type implementation to
//	decide what that means.
//--------------------------------------------------------------------
void tmlnChannelBoolean::Reset()
{
	this->SetState( m_OriginalState );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnChannelBooleanProperty::tmlnChannelBooleanProperty(const char* i_Name, prtyBoolean& i_Property)
:	tmlnChannelBoolean(i_Name),
	m_Property(i_Property), 
	m_bScriptedValue(true)
{	
	m_Property.AddCallback(new prtyCallbackWrapper<tmlnChannelBooleanProperty>(this, &tmlnChannelBooleanProperty::ValueChanged));
}

//--------------------------------------------------------------------
//  Set new state for property
//--------------------------------------------------------------------
void  tmlnChannelBooleanProperty::SetState(bool i_Val)
{
	m_Property.SetValue(i_Val);
	m_bScriptedValue = true;
}

//--------------------------------------------------------------------
//  Get current state of property
//--------------------------------------------------------------------
bool  tmlnChannelBooleanProperty::GetState() const
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
bool tmlnChannelBooleanProperty::HasValueVariation()
{
	//return (m_ScriptedValue != m_Property.GetValue());
	return (!m_bScriptedValue);
}

//--------------------------------------------------------------------
// Callbacks for when property changes, updates original value
//--------------------------------------------------------------------
void tmlnChannelBooleanProperty::ValueChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	// If we don't have any drivers, then update the "original position",
	// but don't mark it dirty so that we can detect this better
	// for auto-keying
	if (this->GetNumDrivers() == 0)
		m_OriginalState = m_Property.GetValue();
	m_bScriptedValue = false;
}

