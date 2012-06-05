/*****************************************************************************
**	tmlnChannelFilePath.cpp
**
**	 Filename channel
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Support/tmln/tmlnChannelFilePath.hpp"

#include "Core/prty/prtyFilePath.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnChannelFilePath::tmlnChannelFilePath(const char* i_Name)
:	tmlnChannel(i_Name),
	m_OriginalValue(true)
{
}


//--------------------------------------------------------------------
//	The channel has the idea of an "original" state that it
//	goes to if there is no driver active.
//--------------------------------------------------------------------
void  tmlnChannelFilePath::SetOriginalValue(const fsLocator& i_Val)
{
	m_OriginalValue = i_Val;
	this->MarkDirty();
}
const fsLocator& tmlnChannelFilePath::GetOriginalValue() const
{
	return m_OriginalValue;
}

//--------------------------------------------------------------------
//	Reset to "original" value, the value when no driver is active.
//	It is up to the specific channel type implementation to
//	decide what that means.
//--------------------------------------------------------------------
void tmlnChannelFilePath::Reset()
{
	this->SetValue( m_OriginalValue );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnChannelFilePathProperty::tmlnChannelFilePathProperty(const char* i_Name, prtyFilePath& i_Property)
:	tmlnChannelFilePath(i_Name),
	m_Property(i_Property), 
	m_bScriptedValue(true)
{	
	m_Property.AddCallback(new prtyCallbackWrapper<tmlnChannelFilePathProperty>(this, &tmlnChannelFilePathProperty::ValueChanged));
}

//--------------------------------------------------------------------
//  Set new state for property
//--------------------------------------------------------------------
void  tmlnChannelFilePathProperty::SetValue(const fsLocator& i_Val)
{
	m_Property.SetValue(i_Val);
	m_bScriptedValue = true;
}

//--------------------------------------------------------------------
//  Get current state of property
//--------------------------------------------------------------------
const fsLocator& tmlnChannelFilePathProperty::GetValue() const
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
bool tmlnChannelFilePathProperty::HasValueVariation()
{
	//return (m_ScriptedValue != m_Property.GetValue());
	return (!m_bScriptedValue);
}

//--------------------------------------------------------------------
// Callbacks for when property changes, updates original value
//--------------------------------------------------------------------
void tmlnChannelFilePathProperty::ValueChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	// If we don't have any drivers, then update the "original position",
	// but don't mark it dirty so that we can detect this better
	// for auto-keying
	if (this->GetNumDrivers() == 0)
		m_OriginalValue = m_Property.GetValue();
	m_bScriptedValue = false;
}

