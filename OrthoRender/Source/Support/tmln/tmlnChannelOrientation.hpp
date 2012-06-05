/*****************************************************************************
**	tmlnChannelOrientation.hpp
**
**	 channel for altering the Orientation
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef TMLN_CHANNELORIENTATION_HPP
#error tmlnChannelOrientation.hpp multiply included
#endif
#define TMLN_CHANNELORIENTATION_HPP

#ifndef TMLN_CHANNEL_HPP
#include "Support/tmln/tmlnChannel.hpp"
#endif
#ifndef MA_ROTATION_HPP
#include "Core/ma/maRotation.hpp"
#endif


//============================================================================
//============================================================================
class prtyRotation;
class prtyProperty;


//============================================================================
//============================================================================
class tmlnChannelOrientation : public tmlnChannel
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	tmlnChannelOrientation(const char* i_Name);

	//--------------------------------------------------------------------
	//  Set new orientation for object, needs to be implemented in
	// derived class
	//--------------------------------------------------------------------
	virtual void  SetQuaternion(const maRotation &i_Orientation) = 0;
	virtual maRotation  GetQuaternion() const = 0;

	//--------------------------------------------------------------------
	//	Set rotation through 3 euler angles (in radians), 
	//	needs to be implemented in derived class
	//--------------------------------------------------------------------
	virtual void SetEuler(float i_X, float i_Y, float i_Z) = 0;
	virtual void GetEuler(float &o_X, float &o_Y, float &o_Z) const = 0;

	//--------------------------------------------------------------------
	//	The channel has the idea of an "original" orientation that it
	//	goes to if there is no driver active.
	//--------------------------------------------------------------------
	void  SetOriginalValue(float i_X, float i_Y, float i_Z);
	void  GetOriginalValue(float &o_X, float &o_Y, float &o_Z) const;

	//--------------------------------------------------------------------
	//	Reset to "original" value, the value when no driver is active.
	//	It is up to the specific channel type implementation to
	//	decide what that means.
	//--------------------------------------------------------------------
	virtual void Reset();

protected:
	maVector3d m_OriginalOrientation;
};

//============================================================================
//============================================================================
class tmlnChannelOrientationProperty : public tmlnChannelOrientation
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	tmlnChannelOrientationProperty(const char* i_Name, prtyRotation& i_Property);

	//--------------------------------------------------------------------
	//  Set new orientation for object, using property
	//--------------------------------------------------------------------
	virtual void  SetQuaternion(const maRotation &i_Rot);
	virtual maRotation  GetQuaternion() const;

	//--------------------------------------------------------------------
	//	Set rotation through 3 euler angles (in radians)
	//--------------------------------------------------------------------
	virtual void SetEuler(float i_X, float i_Y, float i_Z);
	virtual void GetEuler(float &o_X, float &o_Y, float &o_Z) const;

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
	prtyRotation& m_Property;
};
