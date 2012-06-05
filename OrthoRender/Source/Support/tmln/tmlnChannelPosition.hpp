/*****************************************************************************
**	tmlnChannelPosition.hpp
**
**		maPoint3d position channel
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef TMLN_CHANNELPOSITION_HPP
#error tmlnChannelPosition.hpp multiply included
#endif
#define TMLN_CHANNELPOSITION_HPP


#ifndef TMLN_CHANNEL_HPP
#include "Support/tmln/tmlnChannel.hpp"
#endif
#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif

//============================================================================
//============================================================================
class prtyPoint3d;
class prtyProperty;

//============================================================================
//============================================================================
class tmlnChannelPosition : public tmlnChannel
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	tmlnChannelPosition(const char* i_Name);

	//--------------------------------------------------------------------
	//  Set new position for object, needs to be implemented in
	// derived class
	//--------------------------------------------------------------------
	virtual void  SetPosition(const maPoint3d &i_Pos) = 0;

	//--------------------------------------------------------------------
	//  Get position of object, for blending with current value
	//--------------------------------------------------------------------
	virtual maPoint3d  GetPosition() const = 0;

	//--------------------------------------------------------------------
	//	The channel has the idea of an "original" position that it
	//	goes to if there is no driver active.
	//--------------------------------------------------------------------
	void  SetOriginalPosition(const maPoint3d &i_Pos);
	const maPoint3d& GetOriginalPosition() const;

	//--------------------------------------------------------------------
	//	Reset to "original" value, the value when no driver is active.
	//	It is up to the specific channel type implementation to
	//	decide what that means.
	//--------------------------------------------------------------------
	virtual void Reset();

protected:
	maPoint3d m_OriginalPos;
};

//============================================================================
//============================================================================
class tmlnChannelPositionProperty : public tmlnChannelPosition
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	tmlnChannelPositionProperty(const char* i_Name, prtyPoint3d& i_Property);

	//--------------------------------------------------------------------
	//  Set new position for object, using property
	//--------------------------------------------------------------------
	virtual void  SetPosition(const maPoint3d &i_Pos);

	//--------------------------------------------------------------------
	//  Get position of object, using property
	//--------------------------------------------------------------------
	virtual maPoint3d  GetPosition() const;

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
	prtyPoint3d& m_Property;
};
