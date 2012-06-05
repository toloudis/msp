/*****************************************************************************
**	tmlnChannelScale.cpp
**
**	 Adapter for altering scale of something
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "Support/tmln/tmlnChannelScale.hpp"

#include "Core/prty/prtyPoint3d.hpp"

//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnChannelScale::tmlnChannelScale(const char* i_Name)
:	tmlnChannel(i_Name), m_OriginalScale(1,1,1)
{
}



//--------------------------------------------------------------------
//	The channel has the idea of an "original" Scale that it
//	goes to if there is no driver active.
//--------------------------------------------------------------------
void  tmlnChannelScale::SetOriginalScale(const maPoint3d &i_Scale)
{
	m_OriginalScale = i_Scale;
	this->MarkDirty();
}
const maPoint3d& tmlnChannelScale::GetOriginalScale() const
{
	return m_OriginalScale;
}


//--------------------------------------------------------------------
//	Reset to "original" value, the value when no driver is active.
//	It is up to the specific channel type implementation to
//	decide what that means.
//--------------------------------------------------------------------
void tmlnChannelScale::Reset()
{
	this->SetScale( m_OriginalScale );
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnChannelScaleProperty::tmlnChannelScaleProperty(prtyPoint3d& i_Property)
:	tmlnChannelScale(i_Property.GetPropertyName().c_str()),
	m_Property(i_Property), 
	m_bScriptedValue(true)
{
	m_Property.SetAnimatable(true);
	m_Property.AddCallback(new prtyCallbackWrapper<tmlnChannelScaleProperty>(this, &tmlnChannelScaleProperty::ValueChanged));
}

//--------------------------------------------------------------------
//  Set new Scale for object, using property
//--------------------------------------------------------------------
void  tmlnChannelScaleProperty::SetScale(const maPoint3d &i_Scale)
{
	m_Property.SetValue(i_Scale);
	m_bScriptedValue = true;
}

//--------------------------------------------------------------------
//  Get Scale of object, using property
//--------------------------------------------------------------------
maPoint3d  tmlnChannelScaleProperty::GetScale() const
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
bool tmlnChannelScaleProperty::HasValueVariation()
{
	//return (m_ScriptedValue != m_Property.GetValue());
	return (!m_bScriptedValue);
}

//--------------------------------------------------------------------
// Returns true if a channel can be keyed.
//	Default implementation returns false.
//--------------------------------------------------------------------
bool tmlnChannelScaleProperty::CanBeKeyed()
{
	return true;
}

//--------------------------------------------------------------------
// Callbacks for when property changes, updates original value
//--------------------------------------------------------------------
void tmlnChannelScaleProperty::ValueChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	// If we don't have any drivers, then update the "original Scale",
	// but don't mark it dirty so that we can detect this better
	// for auto-keying
	if (this->GetNumDrivers() == 0)
		m_OriginalScale = m_Property.GetValue();
	m_bScriptedValue = false;
}
