/*****************************************************************************
**	tmlnChannelScale.hpp
**
**		maPoint3d Scale channel
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef TMLN_CHANNELSCALE_HPP
#error tmlnChannelScale.hpp multiply included
#endif
#define TMLN_CHANNELSCALE_HPP


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
class tmlnChannelScale : public tmlnChannel
{
public:
	enum Space
	{
		e_ObjectSpace,
		e_WorldSpace
	};

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	tmlnChannelScale(const char* i_Name);

	//--------------------------------------------------------------------
	//  Set new Scale for object, needs to be implemented in
	// derived class
	//--------------------------------------------------------------------
	virtual void  SetScale(const maPoint3d &i_Scale) = 0;

	//--------------------------------------------------------------------
	//  Get Scale of object, for blending with current value
	//--------------------------------------------------------------------
	virtual maPoint3d  GetScale() const = 0;

	//--------------------------------------------------------------------
	//	The channel has the idea of an "original" Scale that it
	//	goes to if there is no driver active.
	//--------------------------------------------------------------------
	void  SetOriginalScale(const maPoint3d &i_Scale);
	const maPoint3d& GetOriginalScale() const;

	//--------------------------------------------------------------------
	//	Reset to "original" value, the value when no driver is active.
	//	It is up to the specific channel type implementation to
	//	decide what that means.
	//--------------------------------------------------------------------
	virtual void Reset();

protected:
	maPoint3d m_OriginalScale;
};

//============================================================================
//============================================================================
class tmlnChannelScaleProperty : public tmlnChannelScale
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	tmlnChannelScaleProperty(prtyPoint3d& i_Property);

	//--------------------------------------------------------------------
	//  Set new Scale for object, using property
	//--------------------------------------------------------------------
	virtual void  SetScale(const maPoint3d &i_Scale);

	//--------------------------------------------------------------------
	//  Get Scale of object, using property
	//--------------------------------------------------------------------
	virtual maPoint3d  GetScale() const;

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
	prtyPoint3d& m_Property;
};
