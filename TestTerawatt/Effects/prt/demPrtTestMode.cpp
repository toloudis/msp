/*****************************************************************************
**  demPrtTestMode.hpp
**
**		This mode is a convenient base class for some of the demo/test modes
**	for the Sc package.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "demPrtTestMode.hpp"

#include "appCharEvent.hpp"
#include "g2dFontUtil.hpp"
#include "maConstants.hpp"

//====================================================================
//====================================================================
demPrtTestMode::demPrtTestMode()
:	m_QuitSignaled(false)
{
	this->SetEnableCharEvents(false);
}

//====================================================================
//====================================================================
demPrtTestMode::~demPrtTestMode()
{
}

//====================================================================
//====================================================================
void demPrtTestMode::Initialize()
{
	this->SetEnableCharEvents(true);

	//	position camera
	//m_Camera.SetPosition(maPoint3d(3, 3, 3));
	m_Camera.SetClip(1.0f, 3000.0f);
	m_Camera.SetAspect(4.0f / 3.0f);
	m_Camera.SetFOV(90.0f);

	m_CameraManip.SetTarget(maPoint3d(0, 0, 0));
	m_CameraManip.SetYaw(0.2f);
	m_CameraManip.SetPitch(maConstants::c_fAngleToRad * 45.0f);
	m_CameraManip.SetRadius(30.0f);
	m_CameraManip.Attach(&m_Camera);

	m_QuitSignaled = false;
}

//====================================================================
//	Think
//====================================================================
void demPrtTestMode::Think()
{
	//	set camera for this frame
	m_CameraManip.Think();
	//m_Camera.Set();
}

//====================================================================
//====================================================================
void demPrtTestMode::DeInitialize()
{
	this->SetEnableCharEvents(false);
}

//====================================================================
//	Override this function to get appCharEvents.
//====================================================================
void demPrtTestMode::ReceiveCharEvent(appCharEvent& i_Event)
{
	switch( i_Event.GetChar() )
	{
		case itString::CharType('q'):
		case itString::CharType(' '):
			m_QuitSignaled = true;
		break;

		case itString::CharType('j'):
			m_CameraManip.SetYaw( m_CameraManip.GetYaw() + maConstants::c_fAngleToRad * 5.0f );
		break;
		case itString::CharType('k'):
			m_CameraManip.SetYaw( m_CameraManip.GetYaw() - maConstants::c_fAngleToRad * 5.0f );
		break;
		case itString::CharType('i'):
			m_CameraManip.SetPitch( m_CameraManip.GetPitch() + maConstants::c_fAngleToRad * 5.0f );
		break;
		case itString::CharType('m'):
			m_CameraManip.SetPitch( m_CameraManip.GetPitch() - maConstants::c_fAngleToRad * 5.0f );
		break;
		case itString::CharType('='):
			m_CameraManip.SetRadius( m_CameraManip.GetRadius() + 1.0f );
		break;
		case itString::CharType('-'):
			if( m_CameraManip.GetRadius() > 1.0f )
				m_CameraManip.SetRadius( m_CameraManip.GetRadius() - 1.0f );
		break;

		case itString::CharType('a'):
			m_CameraManip.SetTarget( m_CameraManip.GetTarget() + m_Camera.GetLeft() );
		break;
		case itString::CharType('d'):
			m_CameraManip.SetTarget( m_CameraManip.GetTarget() - m_Camera.GetLeft() );
		break;
		case itString::CharType('w'):
			m_CameraManip.SetTarget( m_CameraManip.GetTarget() + m_Camera.GetUp() );
		break;
		case itString::CharType('x'):
			m_CameraManip.SetTarget( m_CameraManip.GetTarget() - m_Camera.GetUp() );
		break;
		case itString::CharType('e'):
			m_CameraManip.SetTarget( m_CameraManip.GetTarget() + m_Camera.GetDirection() );
		break;
		case itString::CharType('c'):
			m_CameraManip.SetTarget( m_CameraManip.GetTarget() - m_Camera.GetDirection() );
		break;

		case itString::CharType('r'):
			m_Camera.SetFOV( m_Camera.GetFOV() + 1);
		break;
		case itString::CharType('f'):
			m_Camera.SetFOV( m_Camera.GetFOV() - 1);
		break;
	}
}

//====================================================================
//	GetQuitSignaled returns true if the user is done with this mode
//	(pressed the space bar).
//====================================================================
bool demPrtTestMode::GetQuitSignaled() const
{
	return m_QuitSignaled;
}


//====================================================================
//	SetYaw changes the camera's yaw
//====================================================================
void demPrtTestMode::SetYaw(float i_Radians)
{
	m_CameraManip.SetYaw(i_Radians);
}

//====================================================================
//	SetPitch changes the camera's pitch
//====================================================================
void demPrtTestMode::SetPitch(float i_Radians)
{
	m_CameraManip.SetPitch(i_Radians);
}

//====================================================================
//	SetRadius changes the camera's distance from it's orbit point
//====================================================================
void demPrtTestMode::SetRadius(float i_Radius)
{
	m_CameraManip.SetRadius(i_Radius);
}

//====================================================================
//====================================================================
camCamera& demPrtTestMode::Camera()
{
	return m_Camera;
}
const camCamera& demPrtTestMode::GetCamera() const
{
	return m_Camera;
}

//====================================================================
//====================================================================
const camCameraManipOrbit& demPrtTestMode::GetCameraManip() const
{
	return m_CameraManip;
}

