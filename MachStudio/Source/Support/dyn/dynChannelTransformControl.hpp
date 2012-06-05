/*****************************************************************************
**	dynChannelTransformControl.hpp
**
**	 Channel for animating rotation and translation around control node
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/


#ifdef DYN_CHANNELTRANSFORMCONTROL_HPP
#error dynChannelTransformControl.hpp multiply included
#endif
#define DYN_CHANNELTRANSFORMCONTROL_HPP


#ifndef TMLN_CHANNEL_HPP
#include "Support/tmln/tmlnChannel.hpp"
#endif
#ifndef MA_ROTATION_HPP
#include "Core/ma/maRotation.hpp"
#endif

class scTransformControl;

class dynChannelTransformControl : public tmlnChannel
{
public:
	//--------------------------------------------------------------------
	// Callback for when control changes, in order to update GUI
	//--------------------------------------------------------------------
	class ChannelChangedCallback
	{
	public:
		virtual void ChannelChanged(const dynChannelTransformControl*) = 0;
	};

	//--------------------------------------------------------------------
	// This channel does not own the control animation
	//--------------------------------------------------------------------
	dynChannelTransformControl(const char* i_Name, 
							   scTransformControl* i_pControl,
							   ChannelChangedCallback* i_pCallback = NULL);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~dynChannelTransformControl();

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
	const maVector3d &  GetEulerAngles() const;

	//--------------------------------------------------------------------
	//  Current Translation value
	//--------------------------------------------------------------------
	void  SetTranslation(const maVector3d &i_Trans,
						 bool i_bFromDriver);
	const maVector3d &  GetTranslation() const;

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
	//	Reset to "original" value, the value when no driver is active.
	//	It is up to the specific channel type implementation to
	//	decide what that means.
	//--------------------------------------------------------------------
	virtual void Reset();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	inline scTransformControl* GetControlAnimation();

	//--------------------------------------------------------------------
	// HasValueVariation - returns true if the current value of the
	//	property is different than the scripted value of the channel.
	//	This means the user has edited the values and is an opportunity 
	//	to automatically add a key frame.
	//--------------------------------------------------------------------
	virtual bool HasValueVariation();

private:
	void notify_callback();

	scTransformControl* m_pControl;
	maVector3d m_OriginalRotation;
	maVector3d m_OriginalTranslation;
	ChannelChangedCallback* m_pCallback;
	bool m_bScriptedValue;
};

//--------------------------------------------------------------------
//--------------------------------------------------------------------
inline scTransformControl* dynChannelTransformControl::GetControlAnimation()
{
	return m_pControl;
}
