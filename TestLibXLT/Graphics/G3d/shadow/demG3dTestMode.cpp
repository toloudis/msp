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

#include "appCharEvent.hpp"
#include "g2dFontUtil.hpp"
#include "maConstants.hpp"

//====================================================================
//====================================================================
demG3dTestMode::demG3dTestMode()
:	m_Font(NULL),
	m_QuitSignaled(false)
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
	m_Camera.SetClip(1.0f, 3000.0f);
	m_Camera.SetAspect(4.0f / 3.0f);
	m_Camera.SetFOV(90.0f);

	m_CameraManip.SetTarget(maPoint3d(0, 0, 0));
	m_CameraManip.SetYaw(0.2f);
	m_CameraManip.SetPitch(maConstants::c_fAngleToRad * 45.0f);
	m_CameraManip.SetRadius(30.0f);
	m_CameraManip.Attach(&m_Camera);

	this->SetEnableCharEvents(true);

	//	make our font
	m_Font = g2dFontUtil::LoadFont(itString("Arial"), 11);

	//	position camera
	maPoint3d origin(0, 0, 0);
	maPoint3d camera_pos(5, 10, 20);

	m_QuitSignaled = false;
}

//====================================================================
//	Think
//====================================================================
void demG3dTestMode::Think()
{
	m_CameraManip.Think();
}

//====================================================================
//====================================================================
void demG3dTestMode::DeInitialize()
{
	//	release our font
	g2dFontUtil::ReleaseFont(m_Font);

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
bool demG3dTestMode::GetQuitSignaled() const
{
	return m_QuitSignaled;
}

//====================================================================
//	GetFont returns the font that the mode should use
//====================================================================
g2dFontHandle demG3dTestMode::GetFont() const
{
	return m_Font;
}
