/*****************************************************************************
**	dynChannelControl.hpp
**
**	 Channel for animating rotation and translation around control node
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef DYN_CHANNELCONTROL_HPP
#error dynChannelControl.hpp multiply included
#endif
#define DYN_CHANNELCONTROL_HPP


#ifndef TMLN_CHANNEL_HPP
#include "Support/tmln/tmlnChannel.hpp"
#endif
#ifndef MA_ROTATION_HPP
#include "Core/ma/maRotation.hpp"
#endif

class prtyFloat;
class prtyPoint3d;
class prtyVector3d;
class prtyProperty;
class gpxTransformControl;
class scTransformControl;

class dynChannelControl : public tmlnChannel
{
public:
	//--------------------------------------------------------------------
	// This channel does not own the control animation
	//--------------------------------------------------------------------
	dynChannelControl(const char* i_Name, 
					  prtyFloat& i_RotateX,
					  prtyFloat& i_RotateY,
					  prtyFloat& i_RotateZ,
					  prtyPoint3d& i_Translation,
					  prtyVector3d& i_Scale,
					  gpxTransformControl* i_pControl);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~dynChannelControl();

	//--------------------------------------------------------------------
	//  Current Rotation value, stored as maRotation (quaternions)
	//--------------------------------------------------------------------
	void  SetRotation(const maRotation &i_Rot,
						 bool i_bFromDriver);
	const maRotation &  GetRotation() const;

	//--------------------------------------------------------------------
	//  Current Rotation value, stored as Euler angles (X,Y,Z) in degrees
	//--------------------------------------------------------------------
	void  SetEulerAngles(const maVector3d &i_Angles,
						 bool i_bFromDriver);
	maVector3d  GetEulerAngles() const;

	//--------------------------------------------------------------------
	//  Current Translation value
	//--------------------------------------------------------------------
	void  SetTranslation(const maVector3d &i_Trans,
						 bool i_bFromDriver);
	const maVector3d &  GetTranslation() const;

	//--------------------------------------------------------------------
	//  Current Scale value
	//--------------------------------------------------------------------
	void  SetScale(const maVector3d &i_Scale,
						 bool i_bFromDriver);
	const maVector3d &  GetScale() const;

	//--------------------------------------------------------------------
	//  Original Rotation value, stored as Euler angles (X,Y,Z) in degrees
	//--------------------------------------------------------------------
	void  SetOriginalRotation(const maVector3d &i_Rot);
	const maVector3d &  GetOriginalRotation() const;

	//--------------------------------------------------------------------
	//  Original Translation value
	//--------------------------------------------------------------------
	void  SetOriginalTranslation(const maVector3d &i_Trans);
	const maVector3d &  GetOriginalTranslation() const;

	//--------------------------------------------------------------------
	//  Original Scale value
	//--------------------------------------------------------------------
	void  SetOriginalScale(const maVector3d &i_Scale);
	const maVector3d &  GetOriginalScale() const;

	//--------------------------------------------------------------------
	//	Reset to "original" value, the value when no driver is active.
	//	It is up to the specific channel type implementation to
	//	decide what that means.
	//--------------------------------------------------------------------
	virtual void Reset();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	scTransformControl* GetControlAnimation();

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
	void notify_callback();

	//--------------------------------------------------------------------
	// Callbacks for when property changes, updates original values
	//--------------------------------------------------------------------
	void ValueChanged(prtyProperty *i_pProperty, bool i_bDirty);

	// This channel controls multiple properties
	prtyFloat& m_RotateX;
	prtyFloat& m_RotateY;
	prtyFloat& m_RotateZ;
	prtyPoint3d& m_Translation;
	prtyVector3d& m_Scale;

	gpxTransformControl* m_pControl;
	maVector3d m_OriginalRotation;
	maVector3d m_OriginalTranslation;
	maVector3d m_OriginalScale;
	bool m_bScriptedValue;
};
