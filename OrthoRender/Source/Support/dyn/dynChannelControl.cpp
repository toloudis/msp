/*****************************************************************************
**	dynChannelControl.cpp
**
**	 Channel for animating rotation and translation around control node
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Support/dyn/dynChannelControl.hpp"

#include "Core/dbg/dbgLog.hpp"
#include "Core/ma/maConstants.hpp"
#include "Core/prty/prtyFloat.hpp"
#include "Core/prty/prtyVector3d.hpp"
#include "Graphics/sc/scTransformControl.hpp"

//--------------------------------------------------------------------
//--------------------------------------------------------------------
dynChannelControl::dynChannelControl(const char* i_Name,
								  prtyFloat& i_RotateX,
								  prtyFloat& i_RotateY,
								  prtyFloat& i_RotateZ,
								  prtyVector3d& i_Translation,
								  prtyVector3d& i_Scale,
								  scTransformControl* i_pControl)
: tmlnChannel(i_Name),
	m_RotateX(i_RotateX),
	m_RotateY(i_RotateY),
	m_RotateZ(i_RotateZ),
	m_Translation(i_Translation),
	m_Scale(i_Scale),
	m_pControl(i_pControl)
{
	this->SetOriginalRotation(this->GetEulerAngles());
	this->SetOriginalTranslation(this->GetTranslation());
	this->SetOriginalScale(this->GetScale());

	m_RotateX.AddCallback(new prtyCallbackWrapper<dynChannelControl>(this, &dynChannelControl::ValueChanged));
	m_RotateY.AddCallback(new prtyCallbackWrapper<dynChannelControl>(this, &dynChannelControl::ValueChanged));
	m_RotateZ.AddCallback(new prtyCallbackWrapper<dynChannelControl>(this, &dynChannelControl::ValueChanged));
	m_Translation.AddCallback(new prtyCallbackWrapper<dynChannelControl>(this, &dynChannelControl::ValueChanged));
	m_Scale.AddCallback(new prtyCallbackWrapper<dynChannelControl>(this, &dynChannelControl::ValueChanged));
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
dynChannelControl::~dynChannelControl()
{
	// don't delete rotate control, it is owned by the scObject it
	// is attached to
}


//--------------------------------------------------------------------
//  Current Rotation value, stored as maRotation (quaternions)
//--------------------------------------------------------------------
void  dynChannelControl::SetRotation(const maRotation &i_Rot,
												 bool i_bFromDriver)
{
	// Have to set the euler values of the properties here also.
	// Do this first so that the quaternion value set at the
	// end is the final value.
	maVector3d euler;
	i_Rot.GetEuler(euler.m_X, euler.m_Y, euler.m_Z);
	euler *= maConstants::c_fRadToAngle;

	m_RotateX.SetValue(euler.m_X);
	m_RotateY.SetValue(euler.m_Y);
	m_RotateZ.SetValue(euler.m_Z);

	m_pControl->SetRotation(i_Rot);
	m_bScriptedValue = i_bFromDriver;
}
const maRotation &  dynChannelControl::GetRotation() const
{
	return m_pControl->GetRotation();
}

//--------------------------------------------------------------------
//  Current Rotation value, stored as Euler angles (X,Y,Z) in degrees
//--------------------------------------------------------------------
void  dynChannelControl::SetEulerAngles(const maVector3d &i_Angles,
												 bool i_bFromDriver)
{
	maVector3d euler = i_Angles;
	m_RotateX.SetValue(euler.m_X);
	m_RotateY.SetValue(euler.m_Y);
	m_RotateZ.SetValue(euler.m_Z);
	m_bScriptedValue = i_bFromDriver;
}
maVector3d  dynChannelControl::GetEulerAngles() const
{
	//return m_pControl->GetEulerAngles();
	return maVector3d( m_RotateX.GetValue(),
					   m_RotateY.GetValue(),
					   m_RotateZ.GetValue());
}

//--------------------------------------------------------------------
//  Current Translation value
//--------------------------------------------------------------------
void  dynChannelControl::SetTranslation(const maVector3d &i_Trans,
												 bool i_bFromDriver)
{
	m_Translation.SetValue(i_Trans);
	m_bScriptedValue = i_bFromDriver;
}
const maVector3d &  dynChannelControl::GetTranslation() const
{
	//return m_pControl->GetTranslation();
	return m_Translation.GetValue();
}

//--------------------------------------------------------------------
//  Current Scale value
//--------------------------------------------------------------------
void  dynChannelControl::SetScale(const maVector3d &i_Scale,
					 bool i_bFromDriver)
{
	m_Scale.SetValue(i_Scale);
	m_bScriptedValue = i_bFromDriver;
}
const maVector3d &  dynChannelControl::GetScale() const
{
	//return m_pControl->GetScale();
	return m_Scale.GetValue();
}

//--------------------------------------------------------------------
//  Oringinal Rotation value
//--------------------------------------------------------------------
void  dynChannelControl::SetOriginalRotation(const maVector3d &i_Rot)
{
	m_OriginalRotation = i_Rot;
	this->MarkDirty();
}
const maVector3d &  dynChannelControl::GetOriginalRotation() const
{
	return m_OriginalRotation;
}

//--------------------------------------------------------------------
//  Oringinal Translation value
//--------------------------------------------------------------------
void  dynChannelControl::SetOriginalTranslation(const maVector3d &i_Trans)
{
	m_OriginalTranslation = i_Trans;
	this->MarkDirty();
}
const maVector3d &  dynChannelControl::GetOriginalTranslation() const
{
	return m_OriginalTranslation;
}

//--------------------------------------------------------------------
//  Original Scale value
//--------------------------------------------------------------------
void  dynChannelControl::SetOriginalScale(const maVector3d &i_Scale)
{
	m_OriginalScale = i_Scale;
	this->MarkDirty();
}
const maVector3d &  dynChannelControl::GetOriginalScale() const
{
	return m_OriginalScale;
}

//--------------------------------------------------------------------
//	Reset to "original" value, the value when no driver is active.
//	It is up to the specific channel type implementation to
//	decide what that means.
//--------------------------------------------------------------------
//virtual 
void dynChannelControl::Reset()
{
	// convert Euler degree angles to quaternion
	//maRotation rot(m_OriginalRotation.m_X * maConstants::c_fAngleToRad, 
	//			   m_OriginalRotation.m_Y * maConstants::c_fAngleToRad, 
	//			   m_OriginalRotation.m_Z * maConstants::c_fAngleToRad);
	//m_pControl->SetRotation(rot);

	this->SetEulerAngles(m_OriginalRotation, true);
	this->SetTranslation(m_OriginalTranslation, true);
	this->SetScale(m_OriginalScale, true);

	m_bScriptedValue = true;
}

//--------------------------------------------------------------------
// HasValueVariation - returns true if the current value of the
//	property is different than the scripted value of the channel.
//	This means the user has edited the values and is an opportunity 
//	to automatically add a key frame.
//--------------------------------------------------------------------
//virtual 
bool dynChannelControl::HasValueVariation()
{
	return (!m_bScriptedValue);
}

//--------------------------------------------------------------------
// Callbacks for when property changes, updates original values
//--------------------------------------------------------------------
void dynChannelControl::ValueChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	// If we don't have any drivers, then update the "original value",
	// but don't mark it dirty so that we can detect this better
	// for auto-keying
	if (this->GetNumDrivers() == 0)
	{
		this->SetOriginalRotation(this->GetEulerAngles());
		this->SetOriginalTranslation(this->GetTranslation());
		this->SetOriginalScale(this->GetScale());
	}
	m_bScriptedValue = false;
}
