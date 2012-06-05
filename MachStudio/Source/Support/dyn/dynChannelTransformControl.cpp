/*****************************************************************************
**	dynChannelTransformControl.cpp
**
**	 Channel for animating rotation and translation around control node
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/


#include "Support/dyn/dynChannelTransformControl.hpp"

#include "Core/dbg/dbgLog.hpp"
#include "Core/ma/maConstants.hpp"
#include "Graphics/sc/scTransformControl.hpp"

//--------------------------------------------------------------------
//--------------------------------------------------------------------
dynChannelTransformControl::dynChannelTransformControl(const char* i_Name,
							   scTransformControl* i_pControl,
							   ChannelChangedCallback* i_pCallback)
: tmlnChannel(i_Name),
	m_pControl(i_pControl),
	m_pCallback(i_pCallback)
{

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
dynChannelTransformControl::~dynChannelTransformControl()
{
	// don't delete rotate control, it is owned by the scObject it
	// is attached to
}


//--------------------------------------------------------------------
//  Current Rotation value, stored as maRotation (quaternions)
//--------------------------------------------------------------------
void  dynChannelTransformControl::SetRotation(const maRotation &i_Rot,
												 bool i_bFromDriver)
{
	m_pControl->SetRotation(i_Rot);
	m_bScriptedValue = i_bFromDriver;
	notify_callback();
}
const maRotation &  dynChannelTransformControl::GetRotation() const
{
	return m_pControl->GetRotation();
}

//--------------------------------------------------------------------
//  Current Rotation value, stored as Euler angles (X,Y,Z) in degrees
//--------------------------------------------------------------------
void  dynChannelTransformControl::SetEulerAngles(const maVector3d &i_Angles,
												 bool i_bFromDriver)
{
	m_pControl->SetEulerAngles(i_Angles);
	m_bScriptedValue = i_bFromDriver;
	notify_callback();
}
const maVector3d &  dynChannelTransformControl::GetEulerAngles() const
{
	return m_pControl->GetEulerAngles();
}

//--------------------------------------------------------------------
//  Current Translation value
//--------------------------------------------------------------------
void  dynChannelTransformControl::SetTranslation(const maVector3d &i_Trans,
												 bool i_bFromDriver)
{
	m_pControl->SetTranslation(i_Trans);
	m_bScriptedValue = i_bFromDriver;
	notify_callback();
}
const maVector3d &  dynChannelTransformControl::GetTranslation() const
{
	return m_pControl->GetTranslation();
}

//--------------------------------------------------------------------
//  Oringinal Rotation value
//--------------------------------------------------------------------
void  dynChannelTransformControl::SetOriginalRotation(const maVector3d &i_Rot)
{
	m_OriginalRotation = i_Rot;
	this->MarkDirty();
}
const maVector3d &  dynChannelTransformControl::GetOriginalRotation() const
{
	return m_OriginalRotation;
}

//--------------------------------------------------------------------
//  Oringinal Translation value
//--------------------------------------------------------------------
void  dynChannelTransformControl::SetOriginalTranslation(const maVector3d &i_Trans)
{
	m_OriginalTranslation = i_Trans;
	this->MarkDirty();
}
const maVector3d &  dynChannelTransformControl::GetOriginalTranslation() const
{
	return m_OriginalTranslation;
}

//--------------------------------------------------------------------
//	Reset to "original" value, the value when no driver is active.
//	It is up to the specific channel type implementation to
//	decide what that means.
//--------------------------------------------------------------------
//virtual 
void dynChannelTransformControl::Reset()
{
	// convert Euler degree angles to quaternion
	//maRotation rot(m_OriginalRotation.m_X * maConstants::c_fAngleToRad, 
	//			   m_OriginalRotation.m_Y * maConstants::c_fAngleToRad, 
	//			   m_OriginalRotation.m_Z * maConstants::c_fAngleToRad);
	//m_pControl->SetRotation(rot);
	m_pControl->SetEulerAngles(m_OriginalRotation);

	m_pControl->SetTranslation(m_OriginalTranslation);

	m_bScriptedValue = true;

	notify_callback();
}

//--------------------------------------------------------------------
// HasValueVariation - returns true if the current value of the
//	property is different than the scripted value of the channel.
//	This means the user has edited the values and is an opportunity 
//	to automatically add a key frame.
//--------------------------------------------------------------------
//virtual 
bool dynChannelTransformControl::HasValueVariation()
{
	return (!m_bScriptedValue);
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
void dynChannelTransformControl::notify_callback()
{
	if (m_pCallback)
	{
		m_pCallback->ChannelChanged(this);
	}
}
