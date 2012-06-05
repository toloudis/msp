/*****************************************************************************
**  inMouse.cpp
**
**      inMouse is derived from inDevice. It provides a Think function to poll
**		for its current state, as well as appropriate accessors for retrieving
**		info about the state.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "Input/in/inMouse.hpp"

#include <math.h>

#include "Input/in/inDeviceMgr.hpp"
#include "Input/in/private/inMousePAC.hpp"
#include "Core/ma/maConstants.hpp"
#include "Core/ma/maVector2d.hpp"

//========================================================================
//	Constructor
//========================================================================
inMouse::inMouse() : m_pPAC(new inMousePAC)
{
	memset(&m_PastState, 0, sizeof(m_PastState));
	memset(&m_CurState, 0, sizeof(m_CurState));

	m_dX = 0;
	m_dY = 0;
	m_dZ = 0;
}

//========================================================================
//	Destructor
//========================================================================
inMouse::~inMouse()
{
	delete m_pPAC;
}

//========================================================================
//	Think gives the device a chance to update its state once per frame
//========================================================================
void inMouse::Think()
{
	if (!this->IsEnabled())
	{
		return;
	}

	m_dX = 0;
	m_dY = 0;
	m_dZ = 0;

	memcpy(m_PastState, m_CurState, sizeof(m_PastState));
	memset(m_CurState, 0, sizeof(m_CurState));
	m_pPAC->Think(m_dX, m_dY, m_dZ, m_CurState);

	//we have to fix the radians so that 0 is North and it goes clockwise areound to 2pi
	m_Dir = atan2f(m_dX, -m_dY);//-y to flip the y axis for this calc
	if (0 > m_Dir)
	{
		m_Dir += maConstants::c_fPI_Times_2;
	}

	maVector2d vect(m_dX, m_dY);
	m_Pressure = vect.Length() / static_cast<float>(e_MOUSERANGE);
	if (1 < m_Pressure)
	{
		m_Pressure = 1;
	}

	if (!m_dX && !m_dY)
	{
		m_DigitalDir = inDigitalDir::e_NONE;
	}
	else if ((maConstants::c_fPI * 1.875) < m_Dir || m_Dir <= (maConstants::c_fPI * 0.125))
	{
		m_DigitalDir = inDigitalDir::e_N;
	}
	else if ((maConstants::c_fPI * 0.125) < m_Dir && m_Dir <= (maConstants::c_fPI * 0.375))
	{
		m_DigitalDir = inDigitalDir::e_NE;
	}
	else if ((maConstants::c_fPI * 0.375) < m_Dir && m_Dir <= (maConstants::c_fPI * 0.625))
	{
		m_DigitalDir = inDigitalDir::e_E;
	}
	else if ((maConstants::c_fPI * 0.625) < m_Dir && m_Dir <= (maConstants::c_fPI * 0.875))
	{
		m_DigitalDir = inDigitalDir::e_SE;
	}
	else if ((maConstants::c_fPI * 0.875) < m_Dir && m_Dir <= (maConstants::c_fPI * 1.125))
	{
		m_DigitalDir = inDigitalDir::e_S;
	}
	else if ((maConstants::c_fPI * 1.125) < m_Dir && m_Dir <= (maConstants::c_fPI * 1.375))
	{
		m_DigitalDir = inDigitalDir::e_SW;
	}
	else if ((maConstants::c_fPI * 1.375) < m_Dir && m_Dir <= (maConstants::c_fPI * 1.625))
	{
		m_DigitalDir = inDigitalDir::e_W;
	}
	else if ((maConstants::c_fPI * 1.625) < m_Dir && m_Dir <= (maConstants::c_fPI * 1.875))
	{
		m_DigitalDir = inDigitalDir::e_NW;
	}
}

//========================================================================
//	IsPressed Pass in e_ANYBUTTON to check if any button is pressed, Not a 
//	valid parameter when requesting pressure info.
//========================================================================
bool inMouse::IsPressed(int i_Button)
{
	if (0 > i_Button)
	{
		int i;
		for (i = 0; i < e_MOUSEBUFSIZE; ++i)
		{
			if (IsPressed(i))
			{
				return true;
			}
		}

		return false;
	}
	else if (!this->IsEnabled() || i_Button >= e_MOUSEBUFSIZE)
	{
		return false;
	}

	return (!(m_PastState[i_Button]) && m_CurState[i_Button]);
}

bool inMouse::IsPressed(int i_Button, float& o_AnalogPressure)
{
	if (!this->IsEnabled() || 0 > i_Button || i_Button >= e_MOUSEBUFSIZE)
	{
		o_AnalogPressure = 0;
		return false;
	}

	if (IsPressed(i_Button))
	{
		o_AnalogPressure = 1;
		return true;
	}

	o_AnalogPressure = 0;
	return false;
}

//========================================================================
//	IsHeld Pass in e_ANYBUTTON to check if any button is pressed, Not a 
//	valid parameter when requesting pressure info.
//========================================================================
bool inMouse::IsHeld(int i_Button)
{
	if (0 > i_Button)
	{
		int i;
		for (i = 0; i < e_MOUSEBUFSIZE; ++i)
		{
			if (IsHeld(i))
			{
				return true;
			}
		}

		return false;
	}
	else if (!this->IsEnabled() || i_Button >= e_MOUSEBUFSIZE)
	{
		return false;
	}

	return (m_PastState[i_Button] && m_CurState[i_Button]);
}

bool inMouse::IsHeld(int i_Button, float& o_AnalogPressure)
{
	if (!this->IsEnabled() || 0 > i_Button || i_Button >= e_MOUSEBUFSIZE)
	{
		o_AnalogPressure = 0;
		return false;
	}

	if (IsHeld(i_Button))
	{
		o_AnalogPressure = 1;
		return true;
	}

	o_AnalogPressure = 0;
	return false;
}

//========================================================================
//	IsReleased Pass in e_ANYBUTTON to check if any button is pressed.
//========================================================================
bool inMouse::IsReleased(int i_Button)
{
	if (0 > i_Button)
	{
		int i;
		for (i = 0; i < e_MOUSEBUFSIZE; ++i)
		{
			if (IsReleased(i))
			{
				return true;
			}
		}

		return false;
	}
	else if (!this->IsEnabled() || i_Button >= e_MOUSEBUFSIZE)
	{
		return false;
	}

	return (m_PastState[i_Button] && !(m_CurState[i_Button]));
}

//========================================================================
//	GetPosition returns the relative change in mouse position since
//	the last frame, and optionally mouse wheel rotation.
//========================================================================
void inMouse::GetPosition(int& o_dX, int& o_dY)
{
	if (!this->IsEnabled())
	{
		o_dX = 0;
		o_dY = 0;
	}

	o_dX = m_dX;
	o_dY = m_dY;
}

void inMouse::GetPosition(int& o_dX, int& o_dY, float& o_dZ)
{
	if (!this->IsEnabled())
	{
		o_dX = 0;
		o_dY = 0;
		o_dZ = 0;
	}

	o_dX = m_dX;
	o_dY = m_dY;
	o_dZ = m_dZ;
}

//========================================================================
//	IsDirPressed returns 0 if there is no direction being pressed on 
//	i_Stick, else returns 1.  The stick is left for compliance with these calls in
//	other devices as there is no real concept of a stick for the mouse.
//	The mouse only has stick 0 all other sticks will return false
//========================================================================
bool inMouse::IsDirPressed(int i_Stick) const
{
	if ( i_Stick != 0 )
	{
		return false;
	}

	if (m_dX || m_dY || m_dZ)
	{
		return true;
	}

	return false;
}

//========================================================================
//	GetDigitalDir returns the current digital direction for i_Stick.  The 
//	stick is left for compliance with these calls in
//	other devices as there is no real concept of a stick for the mouse
//========================================================================
inDigitalDir::DigitalDir inMouse::GetDigitalDir(int i_Stick) const
{
	if ( i_Stick != 0 )
	{
		return inDigitalDir::e_NONE;
	}
	return m_DigitalDir;
}

//========================================================================
//	GetAnalogDir returns i_Sticks' direction in various configurations
//	the float returned and o_Twist are direction in radians, and 
//	o_AnalogPressure is a value from 0 - 1 representing how far the stick
//	is being pushed. The stick is left for compliance with these calls in
//	other devices as there is no real concept of a stick for the mouse
//========================================================================
float inMouse::GetAnalogDir(int i_Stick) const
{
	if ( i_Stick != 0 )
	{
		return 0;
	}
	return m_Dir;
}

float inMouse::GetAnalogDir(int i_Stick, float& o_AnalogPressure) const
{
	if ( i_Stick != 0 )
	{
		o_AnalogPressure = 0;
		return 0;
	}
	o_AnalogPressure = m_Pressure;
	
	return m_Dir;
}

float inMouse::GetAnalogDir(int i_Stick, float& o_AnalogPressure, float& o_Twist) const
{
	if ( i_Stick != 0 )
	{
		o_AnalogPressure = 0;
		o_Twist = 0;
		return 0;
	}
	o_AnalogPressure = m_Pressure;
	o_Twist = m_dZ;
	
	return m_Dir;
}

void inMouse::GetAnalogDir(int i_Stick, int& o_X, int& o_Y) const
{
	if ( i_Stick != 0 )
	{
		o_X = 0;
		o_Y = 0;
		return;
	}
	o_X = m_dX;
	o_Y = m_dY;
}

void inMouse::GetAnalogDir(int i_Stick, int& o_X, int& o_Y, int& o_Z) const
{
	if ( i_Stick != 0 )
	{
		o_X = 0;
		o_Y = 0;
		o_Z = 0;
		return;
	}
	o_X = m_dX;
	o_Y = m_dY;
	o_Z = m_dZ;
}