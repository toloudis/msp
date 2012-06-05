/*****************************************************************************
**  inGamepad.hpp
**
**      inGamepad is derived from inDevice. It provides a Think function to poll
**		for its current state, as well as appropriate accessors for retrieving
**		info about the state.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "Input/in/inGamepad.hpp"

#include "Input/in/private/inGamepadPAC.hpp"
#include "Core/ma/maConstants.hpp"

//========================================================================
//	Constructor i_Ranges set the
//	range of values for the given axis, so i_XRange = 1000 sets a range of
//	-1000 to 1000. i_DeadZone is a percentage representing how far you
//	can press the gamepad before values start being counted. i_Saturation
//	is a percentage representing when to start counting the value returned
//	as at the full range. The ranges returned are unaffected by dead zone
//	and saturation. If deadzone plus saturation is greater than 99%, the
//	axes will be unable to return data. For this reason, deadzone will be 
//	truncated to the range 0-49% and saturation to 51-100%
//========================================================================
inGamepad::inGamepad(int i_Range, float i_DeadZone, float i_Saturation, inGamepadPAC* i_PAC)
 : m_pPAC(i_PAC)
{
//	m_DigitalDir.resize(e_NumSticks);
//	m_Dir.resize(e_NumSticks);
//	m_AnalogPressure.resize(e_NumSticks);
//	m_Twist.resize(e_NumSticks);
//	m_X.resize(e_NumSticks);
//	m_Y.resize(e_NumSticks);
//	m_Z.resize(e_NumSticks);
	memset(&m_PastState, 0, sizeof(m_PastState));
	memset(m_CurState, 0, sizeof(m_CurState));
	memset(m_ButtonPressure, 0, sizeof(m_ButtonPressure));
	m_Range = i_Range;
}

//========================================================================
//	Destructor
//========================================================================
inGamepad::~inGamepad()
{
	if (m_pPAC != NULL)
		delete m_pPAC;
}

//========================================================================
//	Think gives the device a chance to update its state once per frame
//========================================================================
void inGamepad::Think()
{
	if (!this->IsEnabled())
	{
		return;
	}
	int i;
	memcpy(m_PastState, m_CurState, sizeof(m_PastState));
	memset(m_CurState, 0, sizeof(m_CurState));
	memset(m_ButtonPressure, 0, sizeof(m_ButtonPressure));

	for (i = 0; i < e_NumSticks; ++i)
	{
		m_DigitalDir[i] = inDigitalDir::e_NONE;
		m_Dir[i] = 0;
		m_AnalogPressure[i] = 0;
		m_Twist[i] = 0;
		m_X[i] = 0;
		m_Y[i] = 0;
		m_Z[i] = 0;
	}

	m_pPAC->Think(	e_NumSticks,
					e_PadBufferSize,
					m_Dir, 
					m_AnalogPressure, 
					m_Twist, 
					m_X, 
					m_Y, 
					m_Z, 
					m_CurState, 
					m_ButtonPressure);

	for (i = 0; i < e_NumSticks; ++i)
	{
		if (-1 == m_Dir[i])
		{
			m_DigitalDir[i] = inDigitalDir::e_NONE;
		}
		else if ((maConstants::c_fPI * 1.875) < m_Dir[i] || m_Dir[i] <= (maConstants::c_fPI * 0.125))
		{
			m_DigitalDir[i] = inDigitalDir::e_N;
		}
		else if ((maConstants::c_fPI * 0.125) < m_Dir[i] && m_Dir[i] <= (maConstants::c_fPI * 0.375))
		{
			m_DigitalDir[i] = inDigitalDir::e_NE;
		}
		else if ((maConstants::c_fPI * 0.375) < m_Dir[i] && m_Dir[i] <= (maConstants::c_fPI * 0.625))
		{
			m_DigitalDir[i] = inDigitalDir::e_E;
		}
		else if ((maConstants::c_fPI * 0.625) < m_Dir[i] && m_Dir[i] <= (maConstants::c_fPI * 0.875))
		{
			m_DigitalDir[i] = inDigitalDir::e_SE;
		}
		else if ((maConstants::c_fPI * 0.875) < m_Dir[i] && m_Dir[i] <= (maConstants::c_fPI * 1.125))
		{
			m_DigitalDir[i] = inDigitalDir::e_S;
		}
		else if ((maConstants::c_fPI * 1.125) < m_Dir[i] && m_Dir[i] <= (maConstants::c_fPI * 1.375))
		{
			m_DigitalDir[i] = inDigitalDir::e_SW;
		}
		else if ((maConstants::c_fPI * 1.375) < m_Dir[i] && m_Dir[i] <= (maConstants::c_fPI * 1.625))
		{
			m_DigitalDir[i] = inDigitalDir::e_W;
		}
		else if ((maConstants::c_fPI * 1.625) < m_Dir[i] && m_Dir[i] <= (maConstants::c_fPI * 1.875))
		{
			m_DigitalDir[i] = inDigitalDir::e_NW;
		}
	}
}

//========================================================================
//	IsPressed Pass in e_ANYBUTTON to check if any button is pressed, Not a 
//	valid parameter when requesting pressure info.
//========================================================================
bool inGamepad::IsPressed(int i_Button) const
{
	if (0 > i_Button)
	{
		int i;
		for (i = 0; i < e_PadBufferSize; ++i)
		{
			if (IsPressed(i))
			{
				return true;
			}
		}

		return false;
	}
	else if (!this->IsEnabled() || i_Button >= e_PadBufferSize)
	{
		return false;
	}

	return (!(m_PastState[i_Button]) && m_CurState[i_Button]);
}

bool inGamepad::IsPressed(int i_Button, float& o_AnalogPressure) const
{
	if (!this->IsEnabled() || 0 > i_Button || i_Button >= e_PadBufferSize)
	{
		o_AnalogPressure = 0;
		return false;
	}

	if (IsPressed(i_Button))
	{
		o_AnalogPressure = m_ButtonPressure[i_Button];
		return true;
	}

	o_AnalogPressure = 0;
	return false;
}

//========================================================================
//	IsHeld Pass in e_ANYBUTTON to check if any button is pressed, Not a 
//	valid parameter when requesting pressure info.
//========================================================================
bool inGamepad::IsHeld(int i_Button) const
{
	if (0 > i_Button)
	{
		int i;
		for (i = 0; i < e_PadBufferSize; ++i)
		{
			if (IsHeld(i))
			{
				return true;
			}
		}

		return false;
	}
	else if (!this->IsEnabled() || i_Button >= e_PadBufferSize)
	{
		return false;
	}

	return (m_PastState[i_Button] && m_CurState[i_Button]);
}

bool inGamepad::IsHeld(int i_Button, float& o_AnalogPressure) const
{
	if (!this->IsEnabled() || 0 > i_Button || i_Button >= e_PadBufferSize)
	{
		o_AnalogPressure = 0;
		return false;
	}

	if (IsHeld(i_Button))
	{
		o_AnalogPressure = m_ButtonPressure[i_Button];
		return true;
	}

	o_AnalogPressure = 0;
	return false;
}

//========================================================================
//	IsReleased Pass in e_ANYBUTTON to check if any button is pressed.
//========================================================================
bool inGamepad::IsReleased(int i_Button) const
{
	if (0 > i_Button)
	{
		int i;
		for (i = 0; i < e_PadBufferSize; ++i)
		{
			if (IsReleased(i))
			{
				return true;
			}
		}

		return false;
	}
	else if (!this->IsEnabled() || i_Button >= e_PadBufferSize)
	{
		return false;
	}

	return (m_PastState[i_Button] && !(m_CurState[i_Button]));
}

//========================================================================
//	IsDirPressed returns 0 if there is no direction being pressed on 
//	i_Stick, else returns 1
//========================================================================
bool inGamepad::IsDirPressed(int i_Stick) const
{
	float Pressure;
	float Twist;
	if (-1 == this->GetAnalogDir(i_Stick, Pressure, Twist) && 0 == Twist)
	{
		return false;
	}

	return true;
}

//========================================================================
//	GetDigitalDir returns the current digital direction for i_Stick
//========================================================================
inDigitalDir::DigitalDir inGamepad::GetDigitalDir(int i_Stick) const
{
	if (!this->IsEnabled() || 0 > i_Stick || i_Stick >= e_NumSticks )
	{
		return inDigitalDir::e_NONE;
	}

	return m_DigitalDir[i_Stick];
}

//========================================================================
//	GetAnalogDir returns i_Sticks' direction in various configurations
//	the float returned and o_Twist are direction in radians, and 
//	o_AnalogPressure is a value from 0 - 1 representing how far the stick
//	is being pushed
//========================================================================
float inGamepad::GetAnalogDir(int i_Stick) const
{
	if (!this->IsEnabled() || 0 > i_Stick || i_Stick >= e_NumSticks )
	{
		return inDigitalDir::e_NONE;
	}

	return m_Dir[i_Stick];
}

float inGamepad::GetAnalogDir(int i_Stick, float& o_AnalogPressure) const
{
	if (!this->IsEnabled() || 0 > i_Stick || i_Stick >= e_NumSticks )
	{
		o_AnalogPressure = 0;
		return inDigitalDir::e_NONE;
	}

	o_AnalogPressure = m_AnalogPressure[i_Stick];
	return m_Dir[i_Stick];
}

float inGamepad::GetAnalogDir(int i_Stick, float& o_AnalogPressure, float& o_Twist) const
{
	if (!this->IsEnabled() || 0 > i_Stick || i_Stick >= e_NumSticks )
	{
		o_AnalogPressure = 0;
		o_Twist = 0;
		return inDigitalDir::e_NONE;
	}

	o_AnalogPressure = m_AnalogPressure[i_Stick];
	o_Twist = m_Twist[i_Stick];
	return m_Dir[i_Stick];
}

void inGamepad::GetAnalogDir(int i_Stick, int& o_X, int& o_Y) const
{
	if (!this->IsEnabled() || 0 > i_Stick || i_Stick >= e_NumSticks )
	{
		o_X = 0;
		o_Y = 0;
		return;
	}

	o_X = m_X[i_Stick];
	o_Y = m_Y[i_Stick];
}

void inGamepad::GetAnalogDir(int i_Stick, int& o_X, int& o_Y, int& o_Z) const
{
	if (!this->IsEnabled() || 0 > i_Stick || i_Stick >= e_NumSticks )
	{
		o_X = 0;
		o_Y = 0;
		o_Z = 0;
		return;
	}

	o_X = m_X[i_Stick];
	o_Y = m_Y[i_Stick];
	o_Z = m_Z[i_Stick];
}
