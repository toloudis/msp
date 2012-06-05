/*****************************************************************************
**	tmlnChannelVideo.cpp
**
**	 Filename channel
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Support/tmln/tmlnChannelVideo.hpp"

#include "Core/prty/prtyVideo.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnChannelVideo::tmlnChannelVideo(const char* i_Name)
:	tmlnChannel(i_Name),
	m_OriginalValue(true)
{
}


//--------------------------------------------------------------------
//	The channel has the idea of an "original" state that it
//	goes to if there is no driver active.
//--------------------------------------------------------------------
void  tmlnChannelVideo::SetOriginalValue(const fsLocator& i_Val)
{
	m_OriginalValue = i_Val;
	this->MarkDirty();
}
const fsLocator& tmlnChannelVideo::GetOriginalValue() const
{
	return m_OriginalValue;
}

//--------------------------------------------------------------------
//	Reset to "original" value, the value when no driver is active.
//	It is up to the specific channel type implementation to
//	decide what that means.
//--------------------------------------------------------------------
void tmlnChannelVideo::Reset()
{
//	this->SetValue( m_OriginalValue );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnChannelVideoProperty::tmlnChannelVideoProperty(prtyVideo& i_Property)
:	tmlnChannelVideo(i_Property.GetPropertyName().c_str()),
	m_Property(i_Property), 
	m_bScriptedValue(true)
{	
	m_Property.SetAnimatable(true);
	m_Property.AddCallback(new prtyCallbackWrapper<tmlnChannelVideoProperty>(this, &tmlnChannelVideoProperty::ValueChanged));
}

//--------------------------------------------------------------------
//  Set new state for property
//--------------------------------------------------------------------
void  tmlnChannelVideoProperty::SetValue(const CVideoData& i_Val)
{
	m_Property.SetValue(i_Val);
	m_bScriptedValue = true;
}

//--------------------------------------------------------------------
//  Get current state of property
//--------------------------------------------------------------------
const CVideoData& tmlnChannelVideoProperty::GetValue() const
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
bool tmlnChannelVideoProperty::HasValueVariation()
{
	//return (m_ScriptedValue != m_Property.GetValue());
	return (!m_bScriptedValue);
}

//--------------------------------------------------------------------
// Returns true if a channel can be keyed.
//	Default implementation returns false.
//--------------------------------------------------------------------
bool tmlnChannelVideoProperty::CanBeKeyed()
{
	return true;
}

//--------------------------------------------------------------------
// Callbacks for when property changes, updates original value
//--------------------------------------------------------------------
void tmlnChannelVideoProperty::ValueChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	// If we don't have any drivers, then update the "original position",
	// but don't mark it dirty so that we can detect this better
	// for auto-keying
//	if (this->GetNumDrivers() == 0)
//		m_OriginalValue = m_Property.GetValue();
	m_bScriptedValue = false;
}

void tmlnChannelVideoProperty::Animate( float i_Time )
{
//	m_Property.Set
}
