/*****************************************************************************
**	tmlnChannelTextureFileName.cpp
**
**	 TextureFileName channel
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Support/tmln/tmlnChannelTextureFileName.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnChannelTextureFileName::tmlnChannelTextureFileName(const char* i_Name)
:	tmlnChannel(i_Name),
	m_OriginalValue(true)
{
}


//--------------------------------------------------------------------
//	The channel has the idea of an "original" state that it
//	goes to if there is no driver active.
//--------------------------------------------------------------------
void  tmlnChannelTextureFileName::SetOriginalValue(const prtyTextureFileData& i_Val)
{
	m_OriginalValue = i_Val;
	this->MarkDirty();
}
const prtyTextureFileData& tmlnChannelTextureFileName::GetOriginalValue() const
{
	return m_OriginalValue;
}

//--------------------------------------------------------------------
//	Reset to "original" value, the value when no driver is active.
//	It is up to the specific channel type implementation to
//	decide what that means.
//--------------------------------------------------------------------
void tmlnChannelTextureFileName::Reset()
{
	this->SetValue( m_OriginalValue );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnChannelTextureFileNameProperty::tmlnChannelTextureFileNameProperty(prtyTextureFileName& i_Property)
:	tmlnChannelTextureFileName(i_Property.GetPropertyName().c_str()),
	m_Property(i_Property), 
	m_bScriptedValue(true)
{	
	m_Property.SetAnimatable(true);
	m_Property.AddCallback(new prtyCallbackWrapper<tmlnChannelTextureFileNameProperty>(this, &tmlnChannelTextureFileNameProperty::ValueChanged));
}

//--------------------------------------------------------------------
//  Set new state for property
//--------------------------------------------------------------------
void  tmlnChannelTextureFileNameProperty::SetValue(const prtyTextureFileData& i_Val)
{
	prtyTextureFileData val;
	val.m_TextureLocator = i_Val.m_TextureLocator;
	val.m_RampObject = i_Val.m_RampObject;
	val.m_CurrentCallback = m_Property.GetFullValue().m_CurrentCallback;  //preserve callback string
	m_Property.SetValue(val);
	m_bScriptedValue = true;
}

//--------------------------------------------------------------------
//  Get current state of property
//--------------------------------------------------------------------
const prtyTextureFileData& tmlnChannelTextureFileNameProperty::GetValue() const
{
	return m_Property.GetFullValue();
}

//--------------------------------------------------------------------
// HasValueVariation - returns true if the current value of the
//	property is different than the scripted value of the channel.
//	This means the user has edited the values and is an opportunity 
//	to automatically add a key frame.
//--------------------------------------------------------------------
//virtual 
bool tmlnChannelTextureFileNameProperty::HasValueVariation()
{
	//return (m_ScriptedValue != m_Property.GetValue());
	return (!m_bScriptedValue);
}

//--------------------------------------------------------------------
// Returns true if a channel can be keyed.
//	Default implementation returns false.
//--------------------------------------------------------------------
bool tmlnChannelTextureFileNameProperty::CanBeKeyed()
{
	return true;
}

//--------------------------------------------------------------------
// Callbacks for when property changes, updates original value
//--------------------------------------------------------------------
void tmlnChannelTextureFileNameProperty::ValueChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	// If we don't have any drivers, then update the "original position",
	// but don't mark it dirty so that we can detect this better
	// for auto-keying
	if (this->GetNumDrivers() == 0)
		m_OriginalValue = m_Property.GetFullValue();
	m_bScriptedValue = false;
}

