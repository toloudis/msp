/*****************************************************************************
**  demMatTestMode.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "demMatTestMode.hpp"

#include "appCharEvent.hpp"
#include "appSimTime.hpp"
#include "g2dFontUtil.hpp"
#include "maConstants.hpp"


//====================================================================
//====================================================================
demMatTestMode::demMatTestMode()
:	m_QuitSignaled(false),
	m_Yaw(0.2f),
	m_Pitch(maConstants::c_fAngleToRad * 45.0f),
	m_Radius(30.0f)
{
	this->SetEnableCharEvents(false);
}

//====================================================================
//====================================================================
demMatTestMode::~demMatTestMode()
{
}

//====================================================================
//====================================================================
void demMatTestMode::Initialize()
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
void demMatTestMode::Think()
{
	appSimTime::IncrementTime();

	//	set camera for this frame
	float z = m_Radius * cos(m_Yaw) * sin(m_Pitch);
	float x = m_Radius * sin(m_Yaw) * sin(m_Pitch);
	float y = m_Radius * cos(m_Pitch);

	maPoint3d origin(0, 0, 0);

	m_Camera.LookAt(maPoint3d(x,y,z), origin, maVector3d(0,1,0));
}

//====================================================================
//====================================================================
void demMatTestMode::DeInitialize()
{

	this->SetEnableCharEvents(false);
}

//====================================================================
//	Override this function to get appCharEvents.
//====================================================================
void demMatTestMode::ReceiveCharEvent(appCharEvent& i_Event)
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

	}
}

//====================================================================
//	GetQuitSignaled returns true if the user is done with this mode 
//	(pressed the space bar).
//====================================================================
bool demMatTestMode::GetQuitSignaled() const
{
	return m_QuitSignaled;
}

//====================================================================
//	SetYaw changes the camera's yaw
//====================================================================
void demMatTestMode::SetYaw(float i_Radians)
{
	m_Yaw = i_Radians;
}

//====================================================================
//	SetPitch changes the camera's pitch
//====================================================================
void demMatTestMode::SetPitch(float i_Radians)
{
	m_Pitch = i_Radians;
}

//====================================================================
//	SetRadius changes the camera's distance from it's orbit point
//====================================================================
void demMatTestMode::SetRadius(float i_Radius)
{
	m_Radius = i_Radius;
}


