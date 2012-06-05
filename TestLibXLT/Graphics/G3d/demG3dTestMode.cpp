/*****************************************************************************
**  demG3dTestMode.hpp
**
**		This mode displays a demonstration/test of the different model space
**	concepts used in the Terawatt 3d engine.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "demG3dTestMode.hpp"

#include "Core/app/appCharEvent.hpp"
#include "Core/app/appSimTime.hpp"
#include "Graphics/g2d/g2dFontUtil.hpp"
#include "Core/ma/maConstants.hpp"

//====================================================================
//====================================================================
demG3dTestMode::demG3dTestMode()
:	m_QuitSignaled(false),
	m_Yaw(0.2f),
	m_Pitch(maConstants::c_fAngleToRad * 45.0f),
	m_Radius(30.0f),
	m_PanX(0.0f),
	m_PanY(0.0f)
{
	this->SetEnableCharEvents(false);
}

//====================================================================
//====================================================================
demG3dTestMode::~demG3dTestMode()
{
}

//====================================================================
//====================================================================
void demG3dTestMode::Initialize()
{
	this->SetEnableCharEvents(true);

	//	position camera
	m_Camera.SetFOV(90.0f);
	m_Camera.SetAspect(4.0f / 3.0f);
	m_Camera.SetClip(1.0f, 1000.0f);											

	maPoint3d origin(0, 0, 0);
	maPoint3d camera_pos(5, 10, 20);

	m_Camera.LookAt(camera_pos, origin, maVector3d(0,1,0));

	m_QuitSignaled = false;
}

//====================================================================
//	Think
//====================================================================
void demG3dTestMode::Think()
{
	appSimTime::IncrementTime();

	//	set camera for this frame
	float z = m_Radius * cos(m_Yaw) * sin(m_Pitch);
	float x = (m_Radius) * sin(m_Yaw) * sin(m_Pitch);
	float y = (m_Radius) * cos(m_Pitch);

	maPoint3d target(x, y, z);
	maPoint3d origin(0, 0, 0);

	m_Camera.LookAt(target, origin, maVector3d(0,1,0));

	maVector3d shift = m_PanX * m_Camera.GetLeft() +
						m_PanY * m_Camera.GetUp();

	m_Camera.LookAt(target+shift, origin+shift, m_Camera.GetUp());


}

//====================================================================
//====================================================================
void demG3dTestMode::DeInitialize()
{

	this->SetEnableCharEvents(false);
}

//====================================================================
//	Override this function to get appCharEvents.
//====================================================================
void demG3dTestMode::ReceiveCharEvent(appCharEvent& i_Event)
{
	switch( i_Event.GetChar() )
	{
		case itString::CharType('q'):
		case itString::CharType(' '):
			m_QuitSignaled = true;
		break;

		case itString::CharType('j'):
			m_Yaw += maConstants::c_fAngleToRad * 5.0f;
		break;
		case itString::CharType('k'):
			m_Yaw -= maConstants::c_fAngleToRad * 5.0f;
		break;
		case itString::CharType('i'):
			m_Pitch += maConstants::c_fAngleToRad * 5.0f;
		break;
		case itString::CharType('m'):
			m_Pitch -= maConstants::c_fAngleToRad * 5.0f;
		break;
		case itString::CharType('='):
			m_Radius += 1.0f;
		break;
		case itString::CharType('-'):
			if (m_Radius > 1.0f)
				m_Radius -= 1.0f;
		break;
		case appCharEvent::e_LEFT:
			m_PanX -= 0.2f;
		break;
		case appCharEvent::e_RIGHT:
			m_PanX += 0.2f;
		break;
		case appCharEvent::e_UP:
			m_PanY -= 0.2f;
		break;
		case appCharEvent::e_DOWN:
			m_PanY += 0.2f;
		break;

	}
}

//====================================================================
//	GetQuitSignaled returns true if the user is done with this mode 
//	(pressed the space bar).
//====================================================================
bool demG3dTestMode::GetQuitSignaled() const
{
	return m_QuitSignaled;
}

//====================================================================
//	SetYaw changes the camera's yaw
//====================================================================
void demG3dTestMode::SetYaw(float i_Radians)
{
	m_Yaw = i_Radians;
}

//====================================================================
//	SetPitch changes the camera's pitch
//====================================================================
void demG3dTestMode::SetPitch(float i_Radians)
{
	m_Pitch = i_Radians;
}

//====================================================================
//	SetRadius changes the camera's distance from it's orbit point
//====================================================================
void demG3dTestMode::SetRadius(float i_Radius)
{
	m_Radius = i_Radius;
}


