/*****************************************************************************
**	tmlnChannelBoolean.hpp
**
**	 boolean channel (on/off, enabled, true/false, etc.)
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/


#ifdef TMLN_CHANNELBOOLEAN_HPP
#error tmlnChannelBoolean.hpp multiply included
#endif
#define TMLN_CHANNELBOOLEAN_HPP


#ifndef TMLN_CHANNEL_HPP
#include "Support/tmln/tmlnChannel.hpp"
#endif

//============================================================================
//============================================================================
class prtyBoolean;
class prtyProperty;

//============================================================================
//============================================================================
class tmlnChannelBoolean : public tmlnChannel
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	tmlnChannelBoolean(const char* i_Name);

	//--------------------------------------------------------------------
	//  Set new state for object, needs to be implemented in
	// derived class
	//--------------------------------------------------------------------
	virtual void  SetState(bool i_Val) = 0;

	//--------------------------------------------------------------------
	//  Get current state of object
	//--------------------------------------------------------------------
	virtual bool  GetState() const = 0;

	//--------------------------------------------------------------------
	//	The channel has the idea of an "original" state that it
	//	goes to if there is no driver active.
	//--------------------------------------------------------------------
	void  SetOriginalState(bool i_Val);
	bool GetOriginalState() const;

	//--------------------------------------------------------------------
	//	Reset to "original" value, the value when no driver is active.
	//	It is up to the specific channel type implementation to
	//	decide what that means.
	//--------------------------------------------------------------------
	virtual void Reset();

protected:
	bool m_OriginalState;
};

//============================================================================
//============================================================================
class tmlnChannelBooleanProperty : public tmlnChannelBoolean
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	tmlnChannelBooleanProperty(const char* i_Name, prtyBoolean& i_Property);

	//--------------------------------------------------------------------
	//  Set new state for property
	//--------------------------------------------------------------------
	virtual void  SetState(bool i_Val);

	//--------------------------------------------------------------------
	//  Get current state of property
	//--------------------------------------------------------------------
	virtual bool  GetState() const;

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
	prtyBoolean& m_Property;
};
