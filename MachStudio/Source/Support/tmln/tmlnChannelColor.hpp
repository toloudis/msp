/*****************************************************************************
**	tmlnChannelColor.hpp
**
**	 maFloatRGBA color channel
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef TMLN_CHANNELCOLOR_HPP
#error tmlnChannelColor.hpp multiply included
#endif
#define TMLN_CHANNELCOLOR_HPP

#ifndef TMLN_CHANNEL_HPP
#include "Support/tmln/tmlnChannel.hpp"
#endif
#ifndef MA_FLOATRGBA_HPP
#include "Core/ma/maFloatRGBA.hpp"
#endif


//============================================================================
//============================================================================
class prtyColor;
class prtyProperty;


//============================================================================
//============================================================================
class tmlnChannelColor : public tmlnChannel
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	tmlnChannelColor(const char* i_Name);

	//--------------------------------------------------------------------
	//  Set new color for object, needs to be implemented in
	// derived class
	//--------------------------------------------------------------------
	virtual void  SetColor(const maFloatRGBA &i_Color) = 0;

	//--------------------------------------------------------------------
	//  Get color of object, for blending with current value
	//--------------------------------------------------------------------
	virtual maFloatRGBA  GetColor() const = 0;

	//--------------------------------------------------------------------
	//	The channel has the idea of an "original" color that it
	//	goes to if there is no driver active.
	//--------------------------------------------------------------------
	void  SetOriginalColor(const maFloatRGBA &i_Color);
	const maFloatRGBA& GetOriginalColor() const;

	//--------------------------------------------------------------------
	//	Reset to "original" value, the value when no driver is active.
	//	It is up to the specific channel type implementation to
	//	decide what that means.
	//--------------------------------------------------------------------
	virtual void Reset();

protected:
	maFloatRGBA m_OriginalColor;
};

//============================================================================
//============================================================================
class tmlnChannelColorProperty : public tmlnChannelColor
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	tmlnChannelColorProperty(prtyColor& i_Property);

	//--------------------------------------------------------------------
	//  Set new color for object, using property
	//--------------------------------------------------------------------
	virtual void  SetColor(const maFloatRGBA &i_Pos);

	//--------------------------------------------------------------------
	//  Get color of object, using property
	//--------------------------------------------------------------------
	virtual maFloatRGBA  GetColor() const;

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
	prtyColor& m_Property;
};