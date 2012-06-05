/*****************************************************************************
**	tmlnChannelColor.cpp
**
**	 maFloatRGBA color channel
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/


#include "Support/tmln/tmlnChannelColor.hpp"

#include "Core/prty/prtyColor.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnChannelColor::tmlnChannelColor(const char* i_Name)
:	tmlnChannel(i_Name), m_OriginalColor(1,1,1,0)
{
}

//--------------------------------------------------------------------
//	The channel has the idea of an "original" color that it
//	goes to if there is no driver active.
//--------------------------------------------------------------------
void  tmlnChannelColor::SetOriginalColor(const maFloatRGBA &i_Color)
{
	m_OriginalColor = i_Color;
	this->MarkDirty();
}
const maFloatRGBA& tmlnChannelColor::GetOriginalColor() const
{
	return m_OriginalColor;
}

//--------------------------------------------------------------------
//	Reset to "original" value, the value when no driver is active.
//	It is up to the specific channel type implementation to
//	decide what that means.
//--------------------------------------------------------------------
void tmlnChannelColor::Reset()
{
	this->SetColor( m_OriginalColor );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnChannelColorProperty::tmlnChannelColorProperty(const char* i_Name, prtyColor& i_Property)
:	tmlnChannelColor(i_Name),
	m_Property(i_Property), 
	m_bScriptedValue(true)
{
	m_Property.AddCallback(new prtyCallbackWrapper<tmlnChannelColorProperty>(this, &tmlnChannelColorProperty::ValueChanged));
}

//--------------------------------------------------------------------
//  Set new Color for object, using property
//--------------------------------------------------------------------
void  tmlnChannelColorProperty::SetColor(const maFloatRGBA &i_Color)
{
	m_Property.SetValue(i_Color);
	m_bScriptedValue = true;
}

//--------------------------------------------------------------------
//  Get Color of object, using property
//--------------------------------------------------------------------
maFloatRGBA  tmlnChannelColorProperty::GetColor() const
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
bool tmlnChannelColorProperty::HasValueVariation()
{
	//maFloatRGBA diff_val = m_ScriptedValue - m_Property.GetValue();
	//float sq_diff = diff_val.GetRed() * diff_val.GetRed() +
	//	diff_val.GetBlue() * diff_val.GetBlue() +
	//	diff_val.GetGreen() * diff_val.GetGreen() +
	//	diff_val.GetAlpha() * diff_val.GetAlpha();

	//const float c_Epsilon = 0.001f;
	//return (sq_diff > c_Epsilon);
	return (!m_bScriptedValue);
}

//--------------------------------------------------------------------
// Callbacks for when property changes, updates original value
//--------------------------------------------------------------------
void tmlnChannelColorProperty::ValueChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	// If we don't have any drivers, then update the "original Color",
	// but don't mark it dirty so that we can detect this better
	// for auto-keying
	if (this->GetNumDrivers() == 0)
		m_OriginalColor = m_Property.GetValue();
	m_bScriptedValue = false;
}

