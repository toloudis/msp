/*****************************************************************************
**	gpxTransformControl.cpp
**
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Tool/gpx/gpxTransformControl.hpp"

#include "Core/Ma/maConstants.hpp"
#include "Graphics/Sc/scTransformControl.hpp"
#include "Tool/gpx/gpxProxyUtil.hpp"


//--------------------------------------------------------------------
// Constructor takes reference to object it will control
//--------------------------------------------------------------------
gpxTransformControl::gpxTransformControl(scTransformControl &i_Control)
:	m_Control(i_Control)
{
#if USE_PROXIES
	m_bEulerAngles = true; 
	m_Rotation = i_Control.GetRotation();
	m_EulerAngles = i_Control.GetEulerAngles();
	m_Translation = i_Control.GetTranslation();
	m_Scale = i_Control.GetScale();
#endif

	PROXY_ADD();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
gpxTransformControl::~gpxTransformControl()
{
	PROXY_REMOVE();
}

//--------------------------------------------------------------------
// Return accessor to control anim for this proxy.
//--------------------------------------------------------------------
scTransformControl* gpxTransformControl::GetTransformControl()
{
	return &m_Control;
}

//--------------------------------------------------------------------
//	Proxies functions just set the data internally. The changes are
//	only pushed through to the light when "Update()" is called.
//--------------------------------------------------------------------
void gpxTransformControl::SetRotation(const maRotation& i_Rotation)
{
	PROXY_SET_OR_STORE(m_Control, SetRotation, m_Rotation, i_Rotation);
	
#if USE_PROXIES
	// Update euler angles, converting to degrees
	m_bEulerAngles = false; 
	i_Rotation.GetEuler(m_EulerAngles.m_X, m_EulerAngles.m_Y, m_EulerAngles.m_Z);
	m_EulerAngles *= maConstants::c_fRadToAngle;
#endif
}
void gpxTransformControl::SetEulerAngles(const maVector3d& i_Angles)
{
	PROXY_SET_OR_STORE(m_Control, SetEulerAngles, m_EulerAngles, i_Angles);

#if USE_PROXIES
	// Update quaternion
	m_bEulerAngles = true; 
	m_Rotation.SetEuler(i_Angles.m_X * maConstants::c_fAngleToRad, 
		i_Angles.m_Y * maConstants::c_fAngleToRad, 
		i_Angles.m_Z * maConstants::c_fAngleToRad);
#endif
}
void gpxTransformControl::SetTranslation(const maVector3d& i_Translation)
{
	PROXY_SET_OR_STORE(m_Control, SetTranslation, m_Translation, i_Translation);
}
void gpxTransformControl::SetScale(const maVector3d& i_Scale)
{
	PROXY_SET_OR_STORE(m_Control, SetScale, m_Scale, i_Scale);
}

//--------------------------------------------------------------------
//	Get functions just return the data internally based on
//	the "Set" calls earlier.
//--------------------------------------------------------------------
const maRotation& gpxTransformControl::GetRotation() const
{
	return PROXY_GET(m_Control, GetRotation, m_Rotation);
}

//--------------------------------------------------------------------
// Update() pushes changes from proxy object into the graphics
//	library object. Should return true if changes were made.
//	This function will only be called if "NeedsUpdate" is true
//	so it is not necessary to check this flag again.
//--------------------------------------------------------------------
//virtual 
bool gpxTransformControl::Update()
{
#if USE_PROXIES
	//envScopedLock proxy_lock(this->GetMutex());

	// Two ways to set the rotation value, keep track of which
	// way was used last in order to update that way also.
	if (m_bEulerAngles)
		m_Control.SetEulerAngles( m_EulerAngles );
	else
		m_Control.SetRotation( m_Rotation );

	m_Control.SetTranslation( m_Translation );
	m_Control.SetScale( m_Scale );

	this->SetNeedsUpdate(false);
#endif

	return true;
}
