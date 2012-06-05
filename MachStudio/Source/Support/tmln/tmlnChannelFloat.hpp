/*****************************************************************************
**	tmlnChannelFloat.hpp
**
**	 single float value channel
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef TMLN_CHANNELFLOAT_HPP
#error tmlnChannelFloat.hpp multiply included
#endif
#define TMLN_CHANNELFLOAT_HPP

#ifndef TMLN_CHANNEL_HPP
#include "Support/tmln/tmlnChannel.hpp"
#endif


//============================================================================
//============================================================================
class prtyFloat;
class prtyInt32;
class prtyProperty;


//============================================================================
//============================================================================
class tmlnChannelFloat : public tmlnChannel
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	tmlnChannelFloat(const char* i_Name);

	//--------------------------------------------------------------------
	//  Set new value for channel, needs to be implemented in
	// derived class
	//--------------------------------------------------------------------
	virtual void  SetValue(float i_Val) = 0;

	//--------------------------------------------------------------------
	//  Get current value of channel
	//--------------------------------------------------------------------
	virtual float  GetValue() const = 0;

	//--------------------------------------------------------------------
	//	The channel has the idea of an "original" value that it
	//	goes to if there is no driver active.
	//--------------------------------------------------------------------
	void  SetOriginalValue(float i_Val);
	float GetOriginalValue() const;

	//--------------------------------------------------------------------
	//	Reset to "original" value, the value when no driver is active.
	//	It is up to the specific channel type implementation to
	//	decide what that means.
	//--------------------------------------------------------------------
	virtual void Reset();

	//--------------------------------------------------------------------
	// Set whether this property should consider the current units
	// when displaying its value in the user interface.
	// prtyFloat has a default value of false.
	//--------------------------------------------------------------------
	bool GetUseUnits() const;
	void SetUseUnits(bool i_bUseUnits);

protected:
	float m_OriginalValue;
	bool m_bUseUnits;
};

//============================================================================
//============================================================================
class tmlnChannelFloatProperty : public tmlnChannelFloat
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	tmlnChannelFloatProperty(prtyFloat& i_Property);

	//--------------------------------------------------------------------
	//  Set new value for property
	//--------------------------------------------------------------------
	virtual void  SetValue(float i_Val);

	//--------------------------------------------------------------------
	//  Get current value of property
	//--------------------------------------------------------------------
	virtual float  GetValue() const;

	//--------------------------------------------------------------------
	// HasValueVariation - returns true if the current value of the
	//	property is different than the scripted value of the channel.
	//	This means the user has edited the values and is an opportunity 
	//	to automatically add a key frame.
	//--------------------------------------------------------------------
	virtual bool HasValueVariation();

	//--------------------------------------------------------------------
	// Returns true if a channel can be keyed.
	//--------------------------------------------------------------------
	virtual bool CanBeKeyed();

	//--------------------------------------------------------------------
	//	Reset to "original" value, the value when no driver is active.
	//	It is up to the specific channel type implementation to
	//	decide what that means.
	//--------------------------------------------------------------------
	virtual void Reset();

private:
	//--------------------------------------------------------------------
	// Callbacks for when property changes, updates original value
	//--------------------------------------------------------------------
	void ValueChanged(prtyProperty *i_pProperty, bool i_bDirty);

	bool m_bScriptedValue;
	prtyFloat& m_Property;
};

//============================================================================
//============================================================================
class tmlnChannelInt32Property : public tmlnChannelFloat
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	tmlnChannelInt32Property(prtyInt32& i_Property);

	//--------------------------------------------------------------------
	//  Set new value for property
	//--------------------------------------------------------------------
	virtual void  SetValue(float i_Val);

	//--------------------------------------------------------------------
	//  Get current value of property
	//--------------------------------------------------------------------
	virtual float	GetValue() const;

	//--------------------------------------------------------------------
	// HasValueVariation - returns true if the current value of the
	//	property is different than the scripted value of the channel.
	//	This means the user has edited the values and is an opportunity 
	//	to automatically add a key frame.
	//--------------------------------------------------------------------
	virtual bool HasValueVariation();

	//--------------------------------------------------------------------
	// Returns true if a channel can be keyed.
	//--------------------------------------------------------------------
	virtual bool CanBeKeyed();

private:
	//--------------------------------------------------------------------
	// Callbacks for when property changes, updates original value
	//--------------------------------------------------------------------
	void ValueChanged(prtyProperty *i_pProperty, bool i_bDirty);

	bool m_bScriptedValue;
	prtyInt32& m_Property;
};
