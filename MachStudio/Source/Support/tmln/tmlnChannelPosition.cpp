/*****************************************************************************
**	tmlnChannelPosition.cpp
**
**	 Adapter for altering position of something
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "Support/tmln/tmlnChannelPosition.hpp"

#include "Core/prty/prtyPoint3d.hpp"

//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnChannelPosition::tmlnChannelPosition(const char* i_Name)
:	tmlnChannel(i_Name), m_OriginalPos(0,0,0)
{
}



//--------------------------------------------------------------------
//	The channel has the idea of an "original" position that it
//	goes to if there is no driver active.
//--------------------------------------------------------------------
void  tmlnChannelPosition::SetOriginalPosition(const maPoint3d &i_Pos)
{
	m_OriginalPos = i_Pos;
	this->MarkDirty();
}
const maPoint3d& tmlnChannelPosition::GetOriginalPosition() const
{
	return m_OriginalPos;
}


//--------------------------------------------------------------------
//	Reset to "original" value, the value when no driver is active.
//	It is up to the specific channel type implementation to
//	decide what that means.
//--------------------------------------------------------------------
void tmlnChannelPosition::Reset()
{
	this->SetPosition( m_OriginalPos );
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnChannelPositionProperty::tmlnChannelPositionProperty(prtyPoint3d& i_Property)
:	tmlnChannelPosition(i_Property.GetPropertyName().c_str()),
	m_Property(i_Property), 
	m_bScriptedValue(true)
{
	m_Property.SetAnimatable(true);
	m_Property.AddCallback(new prtyCallbackWrapper<tmlnChannelPositionProperty>(this, &tmlnChannelPositionProperty::ValueChanged));
}

//--------------------------------------------------------------------
//  Set new position for object, using property
//--------------------------------------------------------------------
void  tmlnChannelPositionProperty::SetPosition(const maPoint3d &i_Pos, Space i_Space)
{
	if (i_Space == e_WorldSpace)
		m_Property.SetWorldSpaceValue(i_Pos);
	else
		m_Property.SetValue(i_Pos);
	m_bScriptedValue = true;
}

//--------------------------------------------------------------------
//  Get position of object, using property
//--------------------------------------------------------------------
maPoint3d  tmlnChannelPositionProperty::GetPosition(Space i_Space) const
{
	if (i_Space == e_WorldSpace)
		return m_Property.GetWorldSpaceValue();
	else
		return m_Property.GetValue();
}

//--------------------------------------------------------------------
// HasValueVariation - returns true if the current value of the
//	property is different than the scripted value of the channel.
//	This means the user has edited the values and is an opportunity 
//	to automatically add a key frame.
//--------------------------------------------------------------------
//virtual 
bool tmlnChannelPositionProperty::HasValueVariation()
{
	//return (m_ScriptedValue != m_Property.GetValue());
	return (!m_bScriptedValue);
}

//--------------------------------------------------------------------
// Returns true if a channel can be keyed.
//	Default implementation returns false.
//--------------------------------------------------------------------
bool tmlnChannelPositionProperty::CanBeKeyed()
{
	return true;
}

//--------------------------------------------------------------------
// Callbacks for when property changes, updates original value
//--------------------------------------------------------------------
void tmlnChannelPositionProperty::ValueChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	// If we don't have any drivers, then update the "original position",
	// but don't mark it dirty so that we can detect this better
	// for auto-keying
	if (this->GetNumDrivers() == 0)
		m_OriginalPos = m_Property.GetValue();
	m_bScriptedValue = false;
}
