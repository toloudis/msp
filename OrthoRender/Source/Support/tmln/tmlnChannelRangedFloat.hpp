/*****************************************************************************
**	tmlnChannelRangedFloat.hpp
**
**	 float channel that has a meaningful "range" of allowed values that
**		allows it to be edited with a slider
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef TMLN_CHANNELRANGEDFLOAT_HPP
#error tmlnChannelRangedFloat.hpp multiply included
#endif
#define TMLN_CHANNELRANGEDFLOAT_HPP

#ifndef TMLN_CHANNELFLOAT_HPP
#include "Support/tmln/tmlnChannelFloat.hpp"
#endif


//============================================================================
//============================================================================
class tmlnChannelRangedFloat : public tmlnChannelFloat
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	tmlnChannelRangedFloat(const char* i_Name, float i_MinValue, float i_MaxValue);

	//--------------------------------------------------------------------
	//	Min Value - minimum accepted value for this channel
	//--------------------------------------------------------------------
	virtual void  SetMinValue(float i_Val);
	virtual float GetMinValue() const;

	//--------------------------------------------------------------------
	//	Max Value - maximum accepted value for this channel
	//--------------------------------------------------------------------
	virtual void  SetMaxValue(float i_Val);
	virtual float GetMaxValue() const;

private:
	float m_MinValue;
	float m_MaxValue;
};

//============================================================================
//============================================================================
class tmlnChannelRangedFloatProperty : public tmlnChannelRangedFloat
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	tmlnChannelRangedFloatProperty(const char* i_Name, prtyFloat& i_Property);

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

private:
	//--------------------------------------------------------------------
	// Callbacks for when property changes, updates original value
	//--------------------------------------------------------------------
	void ValueChanged(prtyProperty *i_pProperty, bool i_bDirty);

	bool m_bScriptedValue;
	prtyFloat& m_Property;
};
