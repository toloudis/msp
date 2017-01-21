/*****************************************************************************
**  inPCVJoy.cpp
**
**      inPCVJoy holds multiple inDevices and queries each to determine
**		the state of a Terawatt button given the state of all inDevices its holding
**		it is derived from inPCVJoy to provide a windows specific
**		implementation
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Core/env/envPlatform.hpp"

#include "Core/ma/maConstants.hpp"
#include "Input/in/inDeviceMgr.hpp"
#include "Input/in/inPCVJoy.hpp"

#include <algorithm>


//============================================================================
//============================================================================
namespace
{
	const float lc_fDirN = 0;
	const float lc_fDirNE = float(maConstants::c_dPI / 4.0);
	const float lc_fDirE = float(maConstants::c_dPI / 2.0);
	const float lc_fDirSE = float(maConstants::c_dPI * 3.0 / 4.0);
	const float lc_fDirS = float(maConstants::c_dPI);
	const float lc_fDirSW = float(maConstants::c_dPI * 5.0 / 4.0);
	const float lc_fDirW = float(maConstants::c_dPI * 3.0 / 2.0);
	const float lc_fDirNW = float(maConstants::c_dPI * 7.0 / 4.0);
	const int lc_nInvalidDir = -1;

	int l_NumSticks = 5;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
inPCVJoy::DeviceMap::DeviceMap()
{
	m_Type = inPCVJoy::e_INVALIDDEVICE;
	m_Keyboard = NULL;
	m_Mouse = NULL;
	m_Gamepad = NULL;
	m_Joystick = NULL;

	m_Sticks.resize(l_NumSticks);
	m_StickSensitivity.resize(l_NumSticks);
	m_DigitalMaps.resize(l_NumSticks);
	m_bInvertX.resize(l_NumSticks);
	m_bInvertY.resize(l_NumSticks);
	m_bInvertZ.resize(l_NumSticks);

	int i;
	for (i = 0; i < l_NumSticks; ++i)
	{
		m_Sticks[i] = -1;
		m_StickSensitivity[i] = 1;
		DigitalMap DM;
		DM.m_North = -1;
		DM.m_East = -1;
		DM.m_South = -1;
		DM.m_West = -1;
		m_DigitalMaps[i] = DM;
		m_bInvertX[i] = false;
		m_bInvertY[i] = false;
		m_bInvertZ[i] = false;
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool inPCVJoy::DeviceMap::operator==(const DeviceMap& DMap)
{
	if (this->m_Type == DMap.m_Type &&
		this->m_Gamepad == DMap.m_Gamepad &&
		this->m_Joystick == DMap.m_Joystick &&
		this->m_Keyboard == DMap.m_Keyboard &&
		this->m_Mouse == DMap.m_Mouse
		)
	{
		return true;
	}

	return false;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool inPCVJoy::DeviceMap::IsDeviceEnabled() const
{
	switch (this->m_Type)
	{
	case inPCVJoy::e_KEYBOARD:
		return this->m_Keyboard->IsEnabled();
		break;
	case inPCVJoy::e_MOUSE:
		return this->m_Mouse->IsEnabled();
		break;
	case inPCVJoy::e_GAMEPAD:
		return this->m_Gamepad->IsEnabled();
		break;
	case inPCVJoy::e_JOYSTICK:
		return this->m_Joystick->IsEnabled();
		break;
	default:
		return false;
		break;
	}
}

//------------------------------------------------------------------------
//	Constructor
//------------------------------------------------------------------------
inPCVJoy::inPCVJoy(int i_NumSticks/* = 5*/)
:	m_NumSticks(i_NumSticks)
{
	l_NumSticks = i_NumSticks;

	XMod[0] = 0;
	XMod[1] = 1;
	XMod[2] = 1;
	XMod[3] = 1;
	XMod[4] = 0;
	XMod[5] = -1;
	XMod[6] = -1;
	XMod[7] = -1;
	YMod[0] = -1;
	YMod[1] = -1;
	YMod[2] = 0;
	YMod[3] = 1;
	YMod[4] = 1;
	YMod[5] = 1;
	YMod[6] = 0;
	YMod[7] = -1;
	
	int i = inDigitalDir::e_NUMDIRS;
	for (i = 0; i < inDigitalDir::e_NUMDIRS; ++i)
	{
		XYRangedMod[i] = maVector2d(XMod[i] * inDeviceMgr::GetRange(), YMod[i] * inDeviceMgr::GetRange());
	}

	ResetConsumed();

}

//------------------------------------------------------------------------
//	Destructor
//------------------------------------------------------------------------
inPCVJoy::~inPCVJoy()
{
	m_HeldConsumed.clear();
	m_PressedConsumed.clear();
	m_ReleasedConsumed.clear();
	m_DirConsumed.clear();
	m_Maps.clear();
}

//------------------------------------------------------------------------
//	IsPressed returns true if any of the devices registered currently have
//	a button pressed that maps to i_TerawattButton. Can also return a value
//	from 0 to 1 in o_AnalogPressure representing how far the button is being
//	pushed.Pass in e_ANYBUTTON to check if any button is pressed, Not a 
//	valid parameter when requesting pressure info.
//------------------------------------------------------------------------
bool inPCVJoy::IsPressed(int i_TerawattButton) const
{
	// check to see if the terawatt button has been consumed
	if ( IsPressedConsumed(i_TerawattButton) ) return false;

	std::vector<int> Actual;
	for (int i = 0; i < m_Maps.size(); ++i)
	{
		if (!m_Maps[i].IsDeviceEnabled())
		{
			continue;
		}

		FindActualButtons(m_Maps[i], i_TerawattButton, Actual);
		for (int j = 0; j < Actual.size(); ++j)
		{
			switch (m_Maps[i].m_Type)
			{
			case e_KEYBOARD:
				if (m_Maps[i].m_Keyboard->IsPressed((inKeys::Keys)Actual[j]))
				{
					return true;
				}
				break;
			case e_MOUSE:
				if (m_Maps[i].m_Mouse->IsPressed(Actual[j]))
				{
					return true;
				}
				break;
			case e_GAMEPAD:
				if (m_Maps[i].m_Gamepad->IsPressed(Actual[j]))
				{
					return true;
				}
				break;
			case e_JOYSTICK:
				if (m_Maps[i].m_Joystick->IsPressed(Actual[j]))
				{
					return true;
				}
				break;
			default:
				DBG_WARNING("Unsupported Device Type #" << m_Maps[i].m_Type);
				break;
			}
		}

		Actual.clear();
	}

	return false;
}

bool inPCVJoy::IsPressed(int i_TerawattButton, float& o_AnalogPressure) const
{
	// check to see if the terawatt button has been consumed
	if ( IsPressedConsumed(i_TerawattButton) ) return false;

	std::vector<int> Actual;
	int AnalogCount = 0;
	float AnalogTotal = 0;
	o_AnalogPressure = 0;

	for (int i = 0; i < m_Maps.size(); ++i)
	{
		if (!m_Maps[i].IsDeviceEnabled())
		{
			continue;
		}

		FindActualButtons(m_Maps[i], i_TerawattButton, Actual);
		for (int j = 0; j < Actual.size(); ++j)
		{
			switch (m_Maps[i].m_Type)
			{
			case e_KEYBOARD:
				if (m_Maps[i].m_Keyboard->IsPressed((inKeys::Keys)Actual[j], o_AnalogPressure))
				{
					++AnalogCount;
					AnalogTotal += o_AnalogPressure;
				}
				break;
			case e_MOUSE:
				if (m_Maps[i].m_Mouse->IsPressed(Actual[j], o_AnalogPressure))
				{
					++AnalogCount;
					AnalogTotal += o_AnalogPressure;
				}
				break;
			case e_GAMEPAD:
				if (m_Maps[i].m_Gamepad->IsPressed(Actual[j], o_AnalogPressure))
				{
					++AnalogCount;
					AnalogTotal += o_AnalogPressure;
				}
				break;
			case e_JOYSTICK:
				if (m_Maps[i].m_Joystick->IsPressed(Actual[j], o_AnalogPressure))
				{
					++AnalogCount;
					AnalogTotal += o_AnalogPressure;
				}
				break;
			default:
				DBG_WARNING("Unsupported Device Type #" << m_Maps[i].m_Type);
				break;
			}
		}

		Actual.clear();
	}

	if (!AnalogCount)
	{
		return false;
	}
	else if (1 < AnalogCount)
	{
		AnalogTotal /= AnalogCount;
	}

	o_AnalogPressure = AnalogTotal;
	return true;
}

//------------------------------------------------------------------------
//	IsHeld returns true if any of the devices registered currently have and
//	also had a button pressed that maps to i_TerawattButton. Can also return
//	a value from 0 to 1 in o_AnalogPressure representing how far the button
//	is being pushed, this value is also averaged among all current presses.
//	Pass in e_ANYBUTTON to check if any button is pressed, Not a 
//	valid parameter when requesting pressure info.
//------------------------------------------------------------------------
bool inPCVJoy::IsHeld(int i_TerawattButton) const
{
	// check to see if the terawatt button has been consumed
	if ( IsHeldConsumed(i_TerawattButton) ) return false;

	std::vector<int> Actual;
	for (int i = 0; i < m_Maps.size(); ++i)
	{
		if (!m_Maps[i].IsDeviceEnabled())
		{
			continue;
		}

		FindActualButtons(m_Maps[i], i_TerawattButton, Actual);
		for (int j = 0; j < Actual.size(); ++j)
		{
			switch (m_Maps[i].m_Type)
			{
			case e_KEYBOARD:
				if (m_Maps[i].m_Keyboard->IsHeld((inKeys::Keys)Actual[j]))
				{
					return true;
				}
				break;
			case e_MOUSE:
				if (m_Maps[i].m_Mouse->IsHeld(Actual[j]))
				{
					return true;
				}
				break;
			case e_GAMEPAD:
				if (m_Maps[i].m_Gamepad->IsHeld(Actual[j]))
				{
					return true;
				}
				break;
			case e_JOYSTICK:
				if (m_Maps[i].m_Joystick->IsHeld(Actual[j]))
				{
					return true;
				}
				break;
			default:
				DBG_WARNING("Unsupported Device Type #" << m_Maps[i].m_Type);
				break;
			}
		}

		Actual.clear();
	}

	return false;
}

bool inPCVJoy::IsHeld(int i_TerawattButton, float& o_AnalogPressure) const
{
	// check to see if the terawatt button has been consumed
	if ( IsHeldConsumed(i_TerawattButton) ) return false;

	std::vector<int> Actual;
	int AnalogCount = 0;
	float AnalogTotal = 0;

	for (int i = 0; i < m_Maps.size(); ++i)
	{
		if (!m_Maps[i].IsDeviceEnabled())
		{
			continue;
		}

		FindActualButtons(m_Maps[i], i_TerawattButton, Actual);
		for (int j = 0; j < Actual.size(); ++j)
		{
			switch (m_Maps[i].m_Type)
			{
			case e_KEYBOARD:
				if (m_Maps[i].m_Keyboard->IsHeld((inKeys::Keys)Actual[j], o_AnalogPressure))
				{
					++AnalogCount;
					AnalogTotal += o_AnalogPressure;
				}
				break;
			case e_MOUSE:
				if (m_Maps[i].m_Mouse->IsHeld(Actual[j], o_AnalogPressure))
				{
					++AnalogCount;
					AnalogTotal += o_AnalogPressure;
				}
				break;
			case e_GAMEPAD:
				if (m_Maps[i].m_Gamepad->IsHeld(Actual[j], o_AnalogPressure))
				{
					++AnalogCount;
					AnalogTotal += o_AnalogPressure;
				}
				break;
			case e_JOYSTICK:
				if (m_Maps[i].m_Joystick->IsHeld(Actual[j], o_AnalogPressure))
				{
					++AnalogCount;
					AnalogTotal += o_AnalogPressure;
				}
				break;
			default:
				DBG_WARNING("Unsupported Device Type #" << m_Maps[i].m_Type);
				break;
			}
		}

		Actual.clear();
	}

	if (!AnalogCount)
	{
		return false;
	}
	else if (1 < AnalogCount)
	{
		AnalogTotal /= AnalogCount;
	}

	o_AnalogPressure = AnalogTotal;
	return true;
}

//------------------------------------------------------------------------
//	IsReleased returns true if any of the devices registered currently have
//	a button pressed that maps to i_TerawattButton. Pass in e_ANYBUTTON to
//	check if any button is pressed.
//------------------------------------------------------------------------
bool inPCVJoy::IsReleased(int i_TerawattButton) const
{
	// check to see if the terawatt button has been consumed
	if ( IsReleasedConsumed(i_TerawattButton) ) return false;

	std::vector<int> Actual;
	for (int i = 0; i < m_Maps.size(); ++i)
	{
		if (!m_Maps[i].IsDeviceEnabled())
		{
			continue;
		}

		FindActualButtons(m_Maps[i], i_TerawattButton, Actual);
		for (int j = 0; j < Actual.size(); ++j)
		{
			switch (m_Maps[i].m_Type)
			{
			case e_KEYBOARD:
				if (m_Maps[i].m_Keyboard->IsReleased((inKeys::Keys)Actual[j]))
				{
					return true;
				}
				break;
			case e_MOUSE:
				if (m_Maps[i].m_Mouse->IsReleased(Actual[j]))
				{
					return true;
				}
				break;
			case e_GAMEPAD:
				if (m_Maps[i].m_Gamepad->IsReleased(Actual[j]))
				{
					return true;
				}
				break;
			case e_JOYSTICK:
				if (m_Maps[i].m_Joystick->IsReleased(Actual[j]))
				{
					return true;
				}
				break;
			default:
				DBG_WARNING("Unsupported Device Type #" << m_Maps[i].m_Type);
				break;
			}
		}

		Actual.clear();
	}

	return false;
}

//------------------------------------------------------------------------
//	IsDirPressed returns 0 if there is no direction being pressed on 
//	i_TerawattStick, else returns 1
//------------------------------------------------------------------------
bool inPCVJoy::IsDirPressed(int i_TerawattStick) const
{
	// check to see if the terawatt button has been consumed
	if ( IsDirConsumed(i_TerawattStick) ) return false;

	int i;
	for (i = 0; i < m_Maps.size(); ++i)
	{
		if (!m_Maps[i].IsDeviceEnabled())
		{
			continue;
		}

		switch (m_Maps[i].m_Type)
		{ 
			case e_KEYBOARD:
			{
				DigitalMap DMap = m_Maps[i].m_DigitalMaps[i_TerawattStick];
				if (-1 == DMap.m_North)
				{
					break;
				}

				if (m_Maps[i].m_Keyboard->IsPressed(static_cast<inKeys::Keys>(DMap.m_North)))
				{
					return true;
				}
				if (m_Maps[i].m_Keyboard->IsPressed(static_cast<inKeys::Keys>(DMap.m_East)))
				{
					return true;
				}
				if (m_Maps[i].m_Keyboard->IsPressed(static_cast<inKeys::Keys>(DMap.m_South)))
				{
					return true;
				}
				if (m_Maps[i].m_Keyboard->IsPressed(static_cast<inKeys::Keys>(DMap.m_West)))
				{
					return true;
				}
				break;
			}
			case e_MOUSE:
				if ( m_Maps[i].m_Mouse->IsDirPressed(m_Maps[i].m_Sticks[i_TerawattStick]) )
				{
					return true;
				}
				break;
			case e_GAMEPAD:
				if ( m_Maps[i].m_Gamepad->IsDirPressed(m_Maps[i].m_Sticks[i_TerawattStick]) )
				{
					return true;
				}
				break;
			case e_JOYSTICK:
				if (m_Maps[i].m_Joystick->IsDirPressed(i_TerawattStick))
				{
					return true;
				}
				break;
			case e_INVALIDDEVICE:
				DBG_WARNING("Unsupported Device Type #" << m_Maps[i].m_Type);
				break;
		}
	}

	return false;
}

//------------------------------------------------------------------------
//	GetDigitalDir returns an enumeration value representing the direction
//	the actual joystick is being pushed in.
//------------------------------------------------------------------------
inDigitalDir::DigitalDir inPCVJoy::GetDigitalDir(int i_TerawattStick) const
{
	int i;
	int X = 0;
	int Y = 0;
	std::vector<inDigitalDir::DigitalDir> Dir;
	inDigitalDir::DigitalDir temp_dir;
	for (i = 0; i < m_Maps.size(); ++i)
	{
		if (!m_Maps[i].IsDeviceEnabled())
		{
			continue;
		}

		switch (m_Maps[i].m_Type)
		{
		case e_KEYBOARD:
			temp_dir = GetKeyboardDigitalDir(m_Maps[i].m_Keyboard, m_Maps[i].m_DigitalMaps[i_TerawattStick]);
			InvertDigitalDir(temp_dir, m_Maps[i].m_bInvertX[i_TerawattStick], m_Maps[i].m_bInvertY[i_TerawattStick]);
			Dir.push_back(temp_dir);
			break;
		case e_MOUSE:
			temp_dir = m_Maps[i].m_Mouse->GetDigitalDir(m_Maps[i].m_Sticks[i_TerawattStick]);
			InvertDigitalDir(temp_dir, m_Maps[i].m_bInvertX[i_TerawattStick], m_Maps[i].m_bInvertY[i_TerawattStick]);
			Dir.push_back(temp_dir);
			break;
		case e_GAMEPAD:
			temp_dir = m_Maps[i].m_Gamepad->GetDigitalDir(m_Maps[i].m_Sticks[i_TerawattStick]);
			InvertDigitalDir(temp_dir, m_Maps[i].m_bInvertX[i_TerawattStick], m_Maps[i].m_bInvertY[i_TerawattStick]);
			Dir.push_back(temp_dir);
			temp_dir = GetGamepadDigitalDir(m_Maps[i].m_Gamepad, m_Maps[i].m_DigitalMaps[i_TerawattStick]);
			InvertDigitalDir(temp_dir, m_Maps[i].m_bInvertX[i_TerawattStick], m_Maps[i].m_bInvertY[i_TerawattStick]);
			Dir.push_back(temp_dir);
			break;
		case e_JOYSTICK:
			temp_dir = m_Maps[i].m_Joystick->GetDigitalDir(i_TerawattStick);
			InvertDigitalDir(temp_dir, m_Maps[i].m_bInvertX[i_TerawattStick], m_Maps[i].m_bInvertY[i_TerawattStick]);
			Dir.push_back(temp_dir);
			break;
		default:
			DBG_WARNING("Unsupported Device Type #" << m_Maps[i].m_Type);
			break;
		}
	}

	X = 0;
	Y = 0;
	for (i = 0; i < Dir.size(); ++i)
	{
		if (inDigitalDir::e_NONE != Dir[i])
		{
			X += XMod[Dir[i]];
			Y += YMod[Dir[i]];
		}
	}

	if (0 > Y)
	{
		if (0 < X)
		{
			return inDigitalDir::e_NE;
		}
		else if (0 > X)
		{
			return inDigitalDir::e_NW;
		}
		else
		{
			return inDigitalDir::e_N;
		}
	}
	else if (0 < Y)
	{
		if (0 < X)
		{
			return inDigitalDir::e_SE;
		}
		else if (0 > X)
		{
			return inDigitalDir::e_SW;
		}
		else
		{
			return inDigitalDir::e_S;
		}
	}
	else
	{
		if (0 < X)
		{
			return inDigitalDir::e_E;
		}
		else if (0 > X)
		{
			return inDigitalDir::e_W;
		}
		else
		{
			return inDigitalDir::e_NONE;
		}
	}
}

//------------------------------------------------------------------------
//	GetAnalogDir returns a float in radians representing the direction the
//	actual joystick is being pushed in. o_AnalogPressure and o_Twist are
//	values from 0 to 1 representing how far the stick is being pushed and
//	far it is being rotated respectively.
//	The non return value versions give stick location in cartesian style
//	coordinates.
//------------------------------------------------------------------------
float inPCVJoy::GetAnalogDir(int i_TerawattStick) const
{
	int i;
	float tempDir;
	int tempX;
	int tempY;
	inDigitalDir::DigitalDir tempDigitalDir;
	std::vector<float> Dir;
	std::vector<maVector2d> XY;
	for (i = 0; i < m_Maps.size(); ++i)
	{
		if (!m_Maps[i].IsDeviceEnabled())
		{
			continue;
		}

		switch (m_Maps[i].m_Type)
		{
		case e_KEYBOARD:
			tempDigitalDir = GetKeyboardDigitalDir(m_Maps[i].m_Keyboard, m_Maps[i].m_DigitalMaps[i_TerawattStick]);
			if (inDigitalDir::e_NONE != tempDigitalDir)
			{
				Dir.push_back(tempDigitalDir * (maConstants::c_fPI_Div_2 / 2.0));
				tempX = XYRangedMod[tempDigitalDir].GetX();
				tempY = XYRangedMod[tempDigitalDir].GetY();
				InvertAnalogDir(tempX, tempY, m_Maps[i].m_bInvertX[i_TerawattStick], m_Maps[i].m_bInvertY[i_TerawattStick]);
				XY.push_back(maVector2d(tempX, tempY));
			}
			break;
		case e_MOUSE:
			{
				int stick = m_Maps[i].m_Sticks[i_TerawattStick];

				if (m_Maps[i].m_Mouse->IsDirPressed(stick))
				{
					tempDir = m_Maps[i].m_Mouse->GetAnalogDir(stick);
					if (-1 != tempDir)
					{
						m_Maps[i].m_Mouse->GetAnalogDir(stick, tempX, tempY);
						InvertAnalogDir(tempDir, m_Maps[i].m_bInvertX[i_TerawattStick], 
										m_Maps[i].m_bInvertY[i_TerawattStick]);
						InvertAnalogDir(tempX, tempY, m_Maps[i].m_bInvertX[i_TerawattStick], 
										m_Maps[i].m_bInvertX[i_TerawattStick]);
						Dir.push_back(tempDir);
						XY.push_back(maVector2d(tempX, tempY));
					}
				}
			}
			break;
		case e_GAMEPAD:
			{
				int stick = m_Maps[i].m_Sticks[i_TerawattStick];

				if (m_Maps[i].m_Gamepad->IsDirPressed(stick))
				{
					tempDir = m_Maps[i].m_Gamepad->GetAnalogDir(stick);
					if (-1 != tempDir)
					{
						m_Maps[i].m_Gamepad->GetAnalogDir(stick, tempX, tempY);
						InvertAnalogDir(tempDir, m_Maps[i].m_bInvertX[i_TerawattStick], 
										m_Maps[i].m_bInvertY[i_TerawattStick]);
						InvertAnalogDir(tempX, tempY, m_Maps[i].m_bInvertX[i_TerawattStick], 
										m_Maps[i].m_bInvertX[i_TerawattStick]);
						Dir.push_back(tempDir);
						XY.push_back(maVector2d(tempX, tempY));
					}
				}

				tempDigitalDir = GetGamepadDigitalDir(m_Maps[i].m_Gamepad, m_Maps[i].m_DigitalMaps[i_TerawattStick]);
				if (inDigitalDir::e_NONE != tempDigitalDir)
				{
					Dir.push_back(tempDigitalDir * (maConstants::c_fPI_Div_2 / 2.0));
					tempX = XYRangedMod[tempDigitalDir].GetX();
					tempY = XYRangedMod[tempDigitalDir].GetY();
					InvertAnalogDir(tempX, tempY, m_Maps[i].m_bInvertX[i_TerawattStick], m_Maps[i].m_bInvertY[i_TerawattStick]);
					XY.push_back(maVector2d(tempX, tempY));
				}
			}
			break;
		case e_JOYSTICK:
			{
				int stick = m_Maps[i].m_Sticks[i_TerawattStick];

				if (m_Maps[i].m_Joystick->IsDirPressed(stick))
				{
					tempDir = m_Maps[i].m_Joystick->GetAnalogDir(stick);
					if (-1 != tempDir)
					{
						m_Maps[i].m_Joystick->GetAnalogDir(stick, tempX, tempY);
						InvertAnalogDir(tempDir, m_Maps[i].m_bInvertX[i_TerawattStick], 
										m_Maps[i].m_bInvertY[i_TerawattStick]);
						InvertAnalogDir(tempX, tempY, m_Maps[i].m_bInvertX[i_TerawattStick], 
										m_Maps[i].m_bInvertX[i_TerawattStick]);
						Dir.push_back(tempDir);
						XY.push_back(maVector2d(tempX, tempY));
					}
				}
			}
			break;
		default:
			DBG_WARNING("Unsupported Device Type #" << m_Maps[i].m_Type);
			break;
		}
	}

	//determine direction
	tempDir = -1;
	if (1 < Dir.size())
	{
		//util function for averaging any number of directions by averaging the vectors
		AverageDir(XY, tempX, tempY, tempDir);
	}
	else if (1 == Dir.size())
	{
		//only one directionvalue, so just send it back
		tempDir = Dir[0];
	}

	return tempDir;
}

float inPCVJoy::GetAnalogDir(int i_TerawattStick, float& o_AnalogPressure) const
{
	int i;
	float tempDir;
	float tempPressure;
	int tempX;
	int tempY;
	inDigitalDir::DigitalDir tempDigitalDir;
	std::vector<float> Dir;
	std::vector<float> Pressure;
	std::vector<maVector2d> XY;
	for (i = 0; i < m_Maps.size(); ++i)
	{
		if (!m_Maps[i].IsDeviceEnabled())
		{
			continue;
		}

		switch (m_Maps[i].m_Type)
		{
		case e_KEYBOARD:
			tempDigitalDir = GetKeyboardDigitalDir(m_Maps[i].m_Keyboard, m_Maps[i].m_DigitalMaps[i_TerawattStick]);
			if (inDigitalDir::e_NONE != tempDigitalDir)
			{
				Pressure.push_back(1 * m_Maps[i].m_StickSensitivity[i_TerawattStick]);
				Dir.push_back(tempDigitalDir * (maConstants::c_fPI_Div_2 / 2.0));
				tempX = XYRangedMod[tempDigitalDir].GetX();
				tempY = XYRangedMod[tempDigitalDir].GetY();
				InvertAnalogDir(tempX, tempY, m_Maps[i].m_bInvertX[i_TerawattStick], m_Maps[i].m_bInvertY[i_TerawattStick]);
				XY.push_back(maVector2d(tempX, tempY));
			}
			break;
		case e_MOUSE:
			{
				int stick = m_Maps[i].m_Sticks[i_TerawattStick];

				if (m_Maps[i].m_Mouse->IsDirPressed(stick))
				{
					tempDir = m_Maps[i].m_Mouse->GetAnalogDir(stick, tempPressure);
					if (-1 != tempDir)
					{
						m_Maps[i].m_Mouse->GetAnalogDir(stick, tempX, tempY);
						InvertAnalogDir(tempDir, m_Maps[i].m_bInvertX[i_TerawattStick], 
										m_Maps[i].m_bInvertY[i_TerawattStick]);
						InvertAnalogDir(tempX, tempY, m_Maps[i].m_bInvertX[i_TerawattStick], 
										m_Maps[i].m_bInvertX[i_TerawattStick]);
						Dir.push_back(tempDir);
						XY.push_back(maVector2d(tempX, tempY));
						Pressure.push_back(tempPressure * m_Maps[i].m_StickSensitivity[i_TerawattStick]);
					}
				}
			}
			break;
		case e_GAMEPAD:
			{
				int stick = m_Maps[i].m_Sticks[i_TerawattStick];

				if (m_Maps[i].m_Gamepad->IsDirPressed(stick))
				{
					tempDir = m_Maps[i].m_Gamepad->GetAnalogDir(stick, tempPressure);
					if (-1 != tempDir)
					{
						m_Maps[i].m_Gamepad->GetAnalogDir(stick, tempX, tempY);
						InvertAnalogDir(tempDir, m_Maps[i].m_bInvertX[i_TerawattStick], 
										m_Maps[i].m_bInvertY[i_TerawattStick]);
						InvertAnalogDir(tempX, tempY, m_Maps[i].m_bInvertX[i_TerawattStick], 
										m_Maps[i].m_bInvertX[i_TerawattStick]);
						Dir.push_back(tempDir);
						XY.push_back(maVector2d(tempX, tempY));
						Pressure.push_back(tempPressure * m_Maps[i].m_StickSensitivity[i_TerawattStick]);
					}
				}

				tempDigitalDir = GetGamepadDigitalDir(m_Maps[i].m_Gamepad, m_Maps[i].m_DigitalMaps[i_TerawattStick]);
				if (inDigitalDir::e_NONE != tempDigitalDir)
				{
					Pressure.push_back(1 * m_Maps[i].m_StickSensitivity[i_TerawattStick]);
					Dir.push_back(tempDigitalDir * (maConstants::c_fPI_Div_2 / 2.0));
					tempX = XYRangedMod[tempDigitalDir].GetX();
					tempY = XYRangedMod[tempDigitalDir].GetY();
					InvertAnalogDir(tempX, tempY, m_Maps[i].m_bInvertX[i_TerawattStick], m_Maps[i].m_bInvertY[i_TerawattStick]);
					XY.push_back(maVector2d(tempX, tempY));
				}
			}
			break;
		case e_JOYSTICK:
			{
				int stick = m_Maps[i].m_Sticks[i_TerawattStick];

				if (m_Maps[i].m_Joystick->IsDirPressed(stick))
				{
					tempDir = m_Maps[i].m_Joystick->GetAnalogDir(stick, tempPressure);
					if (-1 != tempDir)
					{
						m_Maps[i].m_Joystick->GetAnalogDir(stick, tempX, tempY);
						InvertAnalogDir(tempDir, m_Maps[i].m_bInvertX[i_TerawattStick], 
										m_Maps[i].m_bInvertY[i_TerawattStick]);
						InvertAnalogDir(tempX, tempY, m_Maps[i].m_bInvertX[i_TerawattStick], 
										m_Maps[i].m_bInvertX[i_TerawattStick]);
						Dir.push_back(tempDir);
						XY.push_back(maVector2d(tempX, tempY));
						Pressure.push_back(tempPressure * m_Maps[i].m_StickSensitivity[i_TerawattStick]);
					}
				}
			}
			break;
		default:
			DBG_WARNING("Unsupported Device Type #" << m_Maps[i].m_Type);
			break;
		}
	}

	//determine direction and pressure
	tempPressure = 0;
	tempDir = -1;
	if (1 < Dir.size())
	{
		//util function for averaging any number of directions by averaging the vectors
		AverageDir(XY, tempX, tempY, tempDir);
		if (-1 != tempDir)
		{
			//only calculate pressure when there really is a direction, if none, then there is no pressure
			for (i = 0; i < Pressure.size(); ++i)
			{
				tempPressure += Pressure[i];
			}
			tempPressure /= static_cast<float>(Pressure.size());
		}
	}
	else if (1 == Dir.size())
	{
		//only one direction and pressure value, so just send them back
		tempPressure = Pressure[0];
		tempDir = Dir[0];
	}
	o_AnalogPressure = tempPressure;

	return tempDir;
}

float inPCVJoy::GetAnalogDir(int i_TerawattStick, float& o_AnalogPressure, float& o_Twist) const
{
	int i;
	float tempDir;
	float tempPressure;
	float tempTwist;
	int tempX;
	int tempY;
	inDigitalDir::DigitalDir tempDigitalDir;
	std::vector<float> Dir;
	std::vector<float> Pressure;
	std::vector<float> Twist;
	std::vector<maVector2d> XY;
	for (i = 0; i < m_Maps.size(); ++i)
	{
		if (!m_Maps[i].IsDeviceEnabled())
		{
			continue;
		}

		switch (m_Maps[i].m_Type)
		{
		case e_KEYBOARD:
			tempDigitalDir = GetKeyboardDigitalDir(m_Maps[i].m_Keyboard, m_Maps[i].m_DigitalMaps[i_TerawattStick]);
			if (inDigitalDir::e_NONE != tempDigitalDir)
			{
				Pressure.push_back(1 * m_Maps[i].m_StickSensitivity[i_TerawattStick]);
				Dir.push_back(tempDigitalDir * (maConstants::c_fPI_Div_2 / 2.0));
				tempX = XYRangedMod[tempDigitalDir].GetX();
				tempY = XYRangedMod[tempDigitalDir].GetY();
				InvertAnalogDir(tempX, tempY, m_Maps[i].m_bInvertX[i_TerawattStick], m_Maps[i].m_bInvertY[i_TerawattStick]);
				XY.push_back(maVector2d(tempX, tempY));
			}
			break;
		case e_MOUSE:
			{
				int stick = m_Maps[i].m_Sticks[i_TerawattStick];

				if (m_Maps[i].m_Mouse->IsDirPressed(stick))
				{
					tempDir = m_Maps[i].m_Mouse->GetAnalogDir(stick, tempPressure, tempTwist);
					if (tempTwist)
					{
						InvertAnalogDir(tempTwist, m_Maps[i].m_bInvertZ[i_TerawattStick]);
						Twist.push_back(tempTwist);
					}
					if (-1 != tempDir)
					{
						m_Maps[i].m_Mouse->GetAnalogDir(i_TerawattStick, tempX, tempY);
						InvertAnalogDir(tempDir, m_Maps[i].m_bInvertX[i_TerawattStick], 
										m_Maps[i].m_bInvertY[i_TerawattStick]);
						InvertAnalogDir(tempX, tempY, m_Maps[i].m_bInvertX[i_TerawattStick], 
										m_Maps[i].m_bInvertX[i_TerawattStick]);
						Dir.push_back(tempDir);
						XY.push_back(maVector2d(tempX, tempY));
						Pressure.push_back(tempPressure * m_Maps[i].m_StickSensitivity[i_TerawattStick]);
					}
				}
			}
			break;
		case e_GAMEPAD:
			{
				int stick = m_Maps[i].m_Sticks[i_TerawattStick];

				if (m_Maps[i].m_Gamepad->IsDirPressed(stick))
				{
					tempDir = m_Maps[i].m_Gamepad->GetAnalogDir(stick, tempPressure, tempTwist);
					if (tempTwist)
					{
						InvertAnalogDir(tempTwist, m_Maps[i].m_bInvertZ[i_TerawattStick]);
						Twist.push_back(tempTwist);
					}
					if (-1 != tempDir)
					{
						m_Maps[i].m_Gamepad->GetAnalogDir(stick, tempX, tempY);
						InvertAnalogDir(tempDir, m_Maps[i].m_bInvertX[i_TerawattStick], 
										m_Maps[i].m_bInvertY[i_TerawattStick]);
						InvertAnalogDir(tempX, tempY, m_Maps[i].m_bInvertX[i_TerawattStick], 
										m_Maps[i].m_bInvertX[i_TerawattStick]);
						Dir.push_back(tempDir);
						XY.push_back(maVector2d(tempX, tempY));
						Pressure.push_back(tempPressure * m_Maps[i].m_StickSensitivity[i_TerawattStick]);
					}
				}

				tempDigitalDir = GetGamepadDigitalDir(m_Maps[i].m_Gamepad, m_Maps[i].m_DigitalMaps[i_TerawattStick]);
				if (inDigitalDir::e_NONE != tempDigitalDir)
				{
					Pressure.push_back(1 * m_Maps[i].m_StickSensitivity[i_TerawattStick]);
					Dir.push_back(tempDigitalDir * (maConstants::c_fPI_Div_2 / 2.0));
					tempX = XYRangedMod[tempDigitalDir].GetX();
					tempY = XYRangedMod[tempDigitalDir].GetY();
					InvertAnalogDir(tempX, tempY, m_Maps[i].m_bInvertX[i_TerawattStick], m_Maps[i].m_bInvertY[i_TerawattStick]);
					XY.push_back(maVector2d(tempX, tempY));
				}
			}
			break;
		case e_JOYSTICK:
			{
				int stick = m_Maps[i].m_Sticks[i_TerawattStick];

				if (m_Maps[i].m_Joystick->IsDirPressed(stick))
				{
					tempDir = m_Maps[i].m_Joystick->GetAnalogDir(stick, tempPressure, tempTwist);
					if (tempTwist)
					{
						InvertAnalogDir(tempTwist, m_Maps[i].m_bInvertZ[i_TerawattStick]);
						Twist.push_back(tempTwist);
					}
					if (-1 != tempDir)
					{
						m_Maps[i].m_Joystick->GetAnalogDir(stick, tempX, tempY);
						InvertAnalogDir(tempDir, m_Maps[i].m_bInvertX[i_TerawattStick], 
										m_Maps[i].m_bInvertY[i_TerawattStick]);
						InvertAnalogDir(tempX, tempY, m_Maps[i].m_bInvertX[i_TerawattStick], 
										m_Maps[i].m_bInvertX[i_TerawattStick]);
						Dir.push_back(tempDir);
						XY.push_back(maVector2d(tempX, tempY));
						Pressure.push_back(tempPressure * m_Maps[i].m_StickSensitivity[i_TerawattStick]);
					}
				}
			}
			break;
		default:
			DBG_WARNING("Unsupported Device Type #" << m_Maps[i].m_Type);
			break;
		}
	}

	//sum up the twist values, then clamp the values to the axial range
	tempTwist = 0;
	for (i = 0; i < Twist.size(); ++i)
	{
		tempTwist += Twist[i];
	}

	if (tempTwist < -inDeviceMgr::GetRange())
	{
		tempTwist = -inDeviceMgr::GetRange();
	}
	else if (tempTwist > inDeviceMgr::GetRange())
	{
		tempTwist = inDeviceMgr::GetRange();
	}
	o_Twist = tempTwist;

	//determine direction and pressure
	tempPressure = 0;
	tempDir = -1;
	if (1 < Dir.size())
	{
		//util function for averaging any number of directions by averaging the vectors
		AverageDir(XY, tempX, tempY, tempDir);
		if (-1 != tempDir)
		{
			//only calculate pressure when there really is a direction, if none, then there is no pressure
			for (i = 0; i < Pressure.size(); ++i)
			{
				tempPressure += Pressure[i];
			}
			tempPressure /= static_cast<float>(Pressure.size());
		}
	}
	else if (1 == Dir.size())
	{
		//only one direction and pressure value, so just send them back
		tempPressure = Pressure[0];
		tempDir = Dir[0];
	}
	o_AnalogPressure = tempPressure;

	return tempDir;
}

void inPCVJoy::GetAnalogDir(int i_TerawattStick, int& o_x, int& o_y) const
{
	int i;
	float tempDir;
	int tempX;
	int tempY;
	inDigitalDir::DigitalDir tempDigitalDir;
	std::vector<maVector2d> XY;
	for (i = 0; i < m_Maps.size(); ++i)
	{
		if (!m_Maps[i].IsDeviceEnabled())
		{
			continue;
		}

		switch (m_Maps[i].m_Type)
		{
		case e_KEYBOARD:
			tempDigitalDir = GetKeyboardDigitalDir(m_Maps[i].m_Keyboard, m_Maps[i].m_DigitalMaps[i_TerawattStick]);
			if (inDigitalDir::e_NONE != tempDigitalDir)
			{	
				tempX = XYRangedMod[tempDigitalDir].GetX();
				tempY = XYRangedMod[tempDigitalDir].GetY();
				InvertAnalogDir(tempX, tempY, m_Maps[i].m_bInvertX[i_TerawattStick], m_Maps[i].m_bInvertY[i_TerawattStick]);
				XY.push_back(maVector2d(tempX * m_Maps[i].m_StickSensitivity[i_TerawattStick], tempY * m_Maps[i].m_StickSensitivity[i_TerawattStick]));
			}
			break;
		case e_MOUSE:
			{
				int stick = m_Maps[i].m_Sticks[i_TerawattStick];

				if (m_Maps[i].m_Mouse->IsDirPressed(stick))
				{
					m_Maps[i].m_Mouse->GetAnalogDir(stick, tempX, tempY);
					InvertAnalogDir(tempX, tempY, m_Maps[i].m_bInvertX[i_TerawattStick], m_Maps[i].m_bInvertY[i_TerawattStick]);
					XY.push_back(maVector2d(tempX * m_Maps[i].m_StickSensitivity[i_TerawattStick], tempY * m_Maps[i].m_StickSensitivity[i_TerawattStick]));
				}
			}
			break;
		case e_GAMEPAD:
			{
				int stick = m_Maps[i].m_Sticks[i_TerawattStick];

				if (m_Maps[i].m_Gamepad->IsDirPressed(stick))
				{
					m_Maps[i].m_Gamepad->GetAnalogDir(stick, tempX, tempY);
					InvertAnalogDir(tempX, tempY, m_Maps[i].m_bInvertX[i_TerawattStick], m_Maps[i].m_bInvertY[i_TerawattStick]);
					XY.push_back(maVector2d(tempX * m_Maps[i].m_StickSensitivity[i_TerawattStick], tempY * m_Maps[i].m_StickSensitivity[i_TerawattStick]));
				}

				tempDigitalDir = GetGamepadDigitalDir(m_Maps[i].m_Gamepad, m_Maps[i].m_DigitalMaps[i_TerawattStick]);
				if (inDigitalDir::e_NONE != tempDigitalDir)
				{	
					tempX = XYRangedMod[tempDigitalDir].GetX();
					tempY = XYRangedMod[tempDigitalDir].GetY();
					InvertAnalogDir(tempX, tempY, m_Maps[i].m_bInvertX[i_TerawattStick], m_Maps[i].m_bInvertY[i_TerawattStick]);
					XY.push_back(maVector2d(tempX * m_Maps[i].m_StickSensitivity[i_TerawattStick], tempY * m_Maps[i].m_StickSensitivity[i_TerawattStick]));
				}
			}
			break;
		case e_JOYSTICK:
			{
				int stick = m_Maps[i].m_Sticks[i_TerawattStick];

				if (m_Maps[i].m_Joystick->IsDirPressed(stick))
				{
					m_Maps[i].m_Joystick->GetAnalogDir(stick, tempX, tempY);
					InvertAnalogDir(tempX, tempY, m_Maps[i].m_bInvertX[i_TerawattStick], m_Maps[i].m_bInvertY[i_TerawattStick]);
					XY.push_back(maVector2d(tempX * m_Maps[i].m_StickSensitivity[i_TerawattStick], tempY * m_Maps[i].m_StickSensitivity[i_TerawattStick]));
				}
			}
			break;
		default:
			DBG_WARNING("Unsupported Device Type #" << m_Maps[i].m_Type);
			break;
		}
	}

	tempX = 0;
	tempY = 0;
	o_x = 0;
	o_y = 0;
	//determine x and y
	if (1 < XY.size())
	{
		//util function for averaging any number of directions by averaging the vectors
		AverageDir(XY, tempX, tempY, tempDir);
		if (-1 != tempDir)
		{
			o_x = tempX;
			o_y = tempY;
		}
	}
	else if (1 == XY.size())
	{
		//only one direction and pressure value, so just send them back
		o_x = XY[0].GetX();
		o_y = XY[0].GetY();
	}
}

void inPCVJoy::GetAnalogDir(int i_TerawattStick, int& o_x, int& o_y, int& o_z) const
{
	int i;
	float tempDir;
	int tempX;
	int tempY;
	int tempZ;
	inDigitalDir::DigitalDir tempDigitalDir;
	std::vector<maVector2d> XY;
	std::vector<int> Z;
	for (i = 0; i < m_Maps.size(); ++i)
	{
		if (!m_Maps[i].IsDeviceEnabled())
		{
			continue;
		}

		switch (m_Maps[i].m_Type)
		{
		case e_KEYBOARD:
			tempDigitalDir = GetKeyboardDigitalDir(m_Maps[i].m_Keyboard, m_Maps[i].m_DigitalMaps[i_TerawattStick]);
			if (inDigitalDir::e_NONE != tempDigitalDir)
			{	
				tempX = XYRangedMod[tempDigitalDir].GetX();
				tempY = XYRangedMod[tempDigitalDir].GetY();
				InvertAnalogDir(tempX, tempY, m_Maps[i].m_bInvertX[i_TerawattStick], m_Maps[i].m_bInvertY[i_TerawattStick]);
				XY.push_back(maVector2d(tempX * m_Maps[i].m_StickSensitivity[i_TerawattStick], tempY * m_Maps[i].m_StickSensitivity[i_TerawattStick]));
			}
			break;
		case e_MOUSE:
			{
				int stick = m_Maps[i].m_Sticks[i_TerawattStick];

				if (m_Maps[i].m_Mouse->IsDirPressed(stick))
				{
					m_Maps[i].m_Mouse->GetAnalogDir(stick, tempX, tempY, tempZ);
					InvertAnalogDir(tempX, tempY, m_Maps[i].m_bInvertX[i_TerawattStick], m_Maps[i].m_bInvertY[i_TerawattStick]);
					XY.push_back(maVector2d(tempX * m_Maps[i].m_StickSensitivity[i_TerawattStick], tempY * m_Maps[i].m_StickSensitivity[i_TerawattStick]));
					if (tempZ)
					{
						InvertAnalogDir(tempZ, m_Maps[i].m_bInvertZ[i_TerawattStick]);
						Z.push_back(tempZ * m_Maps[i].m_StickSensitivity[i_TerawattStick]);
					}
				}
			}
			break;
		case e_GAMEPAD:
			{
				int stick = m_Maps[i].m_Sticks[i_TerawattStick];

				if (m_Maps[i].m_Gamepad->IsDirPressed(stick))
				{
					m_Maps[i].m_Gamepad->GetAnalogDir(stick, tempX, tempY, tempZ);
					InvertAnalogDir(tempX, tempY, m_Maps[i].m_bInvertX[i_TerawattStick], m_Maps[i].m_bInvertY[i_TerawattStick]);
					XY.push_back(maVector2d(tempX * m_Maps[i].m_StickSensitivity[i_TerawattStick], tempY * m_Maps[i].m_StickSensitivity[i_TerawattStick]));
					if (tempZ)
					{
						InvertAnalogDir(tempZ, m_Maps[i].m_bInvertZ[i_TerawattStick]);
						Z.push_back(tempZ * m_Maps[i].m_StickSensitivity[i_TerawattStick]);
					}
				}

				tempDigitalDir = GetGamepadDigitalDir(m_Maps[i].m_Gamepad, m_Maps[i].m_DigitalMaps[i_TerawattStick]);
				if (inDigitalDir::e_NONE != tempDigitalDir)
				{	
					tempX = XYRangedMod[tempDigitalDir].GetX();
					tempY = XYRangedMod[tempDigitalDir].GetY();
					InvertAnalogDir(tempX, tempY, m_Maps[i].m_bInvertX[i_TerawattStick], m_Maps[i].m_bInvertY[i_TerawattStick]);
					XY.push_back(maVector2d(tempX * m_Maps[i].m_StickSensitivity[i_TerawattStick], tempY * m_Maps[i].m_StickSensitivity[i_TerawattStick]));
				}
			}
			break;
		case e_JOYSTICK:
			{
				int stick = m_Maps[i].m_Sticks[i_TerawattStick];

				if (m_Maps[i].m_Joystick->IsDirPressed(stick))
				{
					m_Maps[i].m_Joystick->GetAnalogDir(stick, tempX, tempY, tempZ);
					InvertAnalogDir(tempX, tempY, m_Maps[i].m_bInvertX[i_TerawattStick], m_Maps[i].m_bInvertY[i_TerawattStick]);
					XY.push_back(maVector2d(tempX * m_Maps[i].m_StickSensitivity[i_TerawattStick], tempY * m_Maps[i].m_StickSensitivity[i_TerawattStick]));
					if (tempZ)
					{
						InvertAnalogDir(tempZ, m_Maps[i].m_bInvertZ[i_TerawattStick]);
						Z.push_back(tempZ * m_Maps[i].m_StickSensitivity[i_TerawattStick]);
					}
				}
			}
			break;
		default:
			DBG_WARNING("Unsupported Device Type #" << m_Maps[i].m_Type);
			break;
		}
	}

	//sum up the Z values, then clamp the values to the axial range
	tempZ = 0;
	for (i = 0; i < Z.size(); ++i)
	{
		tempZ += Z[i];
	}

	if (tempZ < -inDeviceMgr::GetRange())
	{
		tempZ = -inDeviceMgr::GetRange();
	}
	else if (tempZ > inDeviceMgr::GetRange())
	{
		tempZ = inDeviceMgr::GetRange();
	}
	o_z = tempZ;

	tempX = 0;
	tempY = 0;
	o_x = 0;
	o_y = 0;
	//determine x and y
	if (1 < XY.size())
	{
		//util function for averaging any number of directions by averaging the vectors
		AverageDir(XY, tempX, tempY, tempDir);
		if (-1 != tempDir)
		{
			o_x = tempX;
			o_y = tempY;
		}
	}
	else if (1 == XY.size())
	{
		//only one direction and pressure value, so just send them back
		o_x = XY[0].GetX();
		o_y = XY[0].GetY();
	}
}

//------------------------------------------------------------------------
//	GetAnalogAxes returns a representation of the position of the joystick
//	along the x and y axes from -1 to 1, great for when you want the
//	direction and intensity of the joystick in terms of x and y
//	North and West (ie Up and Left on a joystick) are negative, South and
//	East are positive
//------------------------------------------------------------------------
void inPCVJoy::GetAnalogAxes(int i_TerawattStick, float& o_x, float& o_y) const
{
	float fIntensity, fDirection = this->GetAnalogDir( i_TerawattStick, fIntensity );

	// No movement
	if( inDigitalDir::e_NONE == fDirection )
	{
		o_y = 0;
		o_x = 0;
	}
	// Between N and W
	else if( fDirection >= lc_fDirW )
	{
		o_y = ( fDirection - lc_fDirW ) / maConstants::c_fPI_Div_2;
		o_x = 1 - o_y;

		o_y *= -1;
		o_x *= -1;
	}
	// Between N and E
	else if( fDirection <= lc_fDirE )
	{
		o_x = fDirection / maConstants::c_fPI_Div_2;
		o_y = 1 - o_x;

		o_y *= -1;
	}
	// Between S and W
	else if( fDirection < lc_fDirW && fDirection >= lc_fDirS )
	{
		o_x = ( fDirection - lc_fDirS ) / maConstants::c_fPI_Div_2;
		o_y = 1 - o_x;

		o_x *= -1;
	}
	// Between S and E
	else if( fDirection < lc_fDirS && fDirection > lc_fDirE )
	{
		o_y = ( fDirection - lc_fDirE ) / maConstants::c_fPI_Div_2;
		o_x = 1 - o_y;
	}

	o_y *= fIntensity;
	o_x *= fIntensity;
}

//------------------------------------------------------------------------
//	ConsumePressed consumes the terawatt key pressed.  Any subsequent calls
//	to IsPressed will return false
//------------------------------------------------------------------------
void inPCVJoy::ConsumePressed(int i_TerawattButton)
{
	m_PressedConsumed[i_TerawattButton] = true;
}

//------------------------------------------------------------------------
//	ConsumeHeld consumes the terawatt key held.  Any subsequent calls
//	to IsHeld will return false
//------------------------------------------------------------------------
void inPCVJoy::ConsumeHeld(int i_TerawattButton)
{
	m_HeldConsumed[i_TerawattButton] = true;
}

//------------------------------------------------------------------------
//	ConsumeReleased consumes the terawatt key released.  Any subsequent calls
//	to IsReleased will return false
//------------------------------------------------------------------------
void inPCVJoy::ConsumeReleased(int i_TerawattButton)
{
	m_ReleasedConsumed[i_TerawattButton] = true;
}

//------------------------------------------------------------------------
//	ConsumeDir consumes the terawatt stick dir.  Any subsequent calls
//	to IsDirPressed will return false
//------------------------------------------------------------------------
void inPCVJoy::ConsumeDir(int i_TerawattStick)
{
	m_DirConsumed[i_TerawattStick] = true;
}

//------------------------------------------------------------------------
//	IsPressedConsumed returns true if the pressed state was consumed this loop
//------------------------------------------------------------------------
bool inPCVJoy::IsPressedConsumed(int i_TerawattButton) const
{
	// Can't possibly consume all key presses at one time.
	if (inPCVJoy::e_ANYBUTTON == i_TerawattButton)
		return false;

	std::map<int,bool>::const_iterator cit;
	cit = m_PressedConsumed.find(i_TerawattButton);
	if ( cit == m_PressedConsumed.end() )
	{
//		DBG_WARNING1( "Virtual joystick button %d not found.", i_TerawattButton );
		return true;
	}
	return cit->second;
}

//------------------------------------------------------------------------
//	IsHeldConsumed returns true if the held state was consumed this loop
//------------------------------------------------------------------------
bool inPCVJoy::IsHeldConsumed(int i_TerawattButton) const
{
	// Can't possibly consume all key holds at one time.
	if (inPCVJoy::e_ANYBUTTON == i_TerawattButton)
		return false;

	std::map<int,bool>::const_iterator cit;
	cit = m_HeldConsumed.find(i_TerawattButton);
	if ( cit == m_HeldConsumed.end() )
	{
//		DBG_WARNING1( "Virtual joystick button %d not found.", i_TerawattButton );
		return true;
	}
	return cit->second;
}

//------------------------------------------------------------------------
//	IsReleasedConsumed returns true if the released state was consumed this loop
//------------------------------------------------------------------------
bool inPCVJoy::IsReleasedConsumed(int i_TerawattButton) const
{
	// Can't possibly consume all key releases at one time.
	if (inPCVJoy::e_ANYBUTTON == i_TerawattButton)
		return false;

	std::map<int,bool>::const_iterator cit;
	cit = m_ReleasedConsumed.find(i_TerawattButton);
	if ( cit == m_ReleasedConsumed.end() )
	{
//		DBG_WARNING1( "Virtual joystick button %d not found.", i_TerawattButton );
		return true;
	}
	return cit->second;
}

//------------------------------------------------------------------------
//	IsDirConsumed returns true if the dir state was consumed this loop
//------------------------------------------------------------------------
bool inPCVJoy::IsDirConsumed(int i_TerawattStick) const
{
	// Can't possibly consume all joystick movements at one time.
	if (inPCVJoy::e_ANYBUTTON == i_TerawattStick)
		return false;

	std::map<int,bool>::const_iterator cit;
	cit = m_DirConsumed.find(i_TerawattStick);
	return cit->second;
}

//------------------------------------------------------------------------
//	IsAxisInverted returns whether the given stick on the given device is
//	currently set to be inverted
//------------------------------------------------------------------------
bool inPCVJoy::IsXAxisInverted(inDevice *i_Device, int i_TerawattStick)
{
	return m_Maps[FindDeviceIndex(i_Device, i_TerawattStick)].m_bInvertX[i_TerawattStick];
}

bool inPCVJoy::IsYAxisInverted(inDevice *i_Device, int i_TerawattStick)
{
	return m_Maps[FindDeviceIndex(i_Device, i_TerawattStick)].m_bInvertY[i_TerawattStick];
}

bool inPCVJoy::IsZAxisInverted(inDevice *i_Device, int i_TerawattStick)
{
	return m_Maps[FindDeviceIndex(i_Device, i_TerawattStick)].m_bInvertZ[i_TerawattStick];
}

//------------------------------------------------------------------------
//	InvertAxis sets the invert parameter for the given stick on the given
//	device
//------------------------------------------------------------------------
void inPCVJoy::InvertXAxis(inDevice *i_Device, int i_TerawattStick, bool i_Invert)
{
	m_Maps[FindDeviceIndex(i_Device, i_TerawattStick)].m_bInvertX[i_TerawattStick] = i_Invert;
}

void inPCVJoy::InvertYAxis(inDevice *i_Device, int i_TerawattStick, bool i_Invert)
{
	m_Maps[FindDeviceIndex(i_Device, i_TerawattStick)].m_bInvertY[i_TerawattStick] = i_Invert;
}

void inPCVJoy::InvertZAxis(inDevice *i_Device, int i_TerawattStick, bool i_Invert)
{
	m_Maps[FindDeviceIndex(i_Device, i_TerawattStick)].m_bInvertZ[i_TerawattStick] = i_Invert;
}

//------------------------------------------------------------------------
//	RemoveMapping removes the the given mapping
//------------------------------------------------------------------------
void inPCVJoy::RemoveMapping(inDevice *i_Device, int i_TerawattButton, int i_ActualButton)
{
	int Index = FindDeviceIndex(i_Device);

	//need to check if the actual button is already mapped to a terawatt button
	//if so we overwrite the old terawatt button
	typedef std::multimap<int, int>::iterator It;
	It i;
	for (i = m_Maps[Index].m_Buttons.begin(); i != m_Maps[Index].m_Buttons.end(); ++i)
	{
		if (i_TerawattButton == i->first && i_ActualButton == i->second)
		{
			m_Maps[Index].m_Buttons.erase(i);
			break;
		}
	}
}

//------------------------------------------------------------------------
//	MapButtons maps the given TerawattButton(s) to the given ActualButtons(s)
//	on the suppied inDevice. The i_Device pointer will be added to m_Maps
//	if it is not already listed there.
//------------------------------------------------------------------------
void inPCVJoy::MapButtons(inDevice *i_Device, int i_TerawattButton, int i_ActualButton)
{
	int Index = FindDeviceIndex(i_Device);

	//we don't prevent the same terawattbutton being mapped to the same actualbutton or vice versa
	//as this can limit mappings we use behind the scenes
	//it is the responsibility of the user configure butons screen to prevent this behavior as needed
	m_Maps[Index].m_Buttons.insert(std::make_pair(i_TerawattButton, i_ActualButton));

	std::pair<int, bool> pair;
	pair.second = false;
	m_PressedConsumed[i_TerawattButton] = false;
	m_HeldConsumed[i_TerawattButton] = false;
	m_ReleasedConsumed[i_TerawattButton] = false;
}

void inPCVJoy::MapButtons(inDevice *i_Device, const std::vector<int>& i_TerawattButton,
								   const std::vector<int>& i_ActualButton)
{
	DBG_ASSERT(i_TerawattButton.size() == i_ActualButton.size(), 
		"MapButtons arrays not of equal length, TerawattLen: " << i_TerawattButton.size() << ", ActualLen: " << i_ActualButton.size()); 

	int i;
	for (i = 0; i < i_TerawattButton.size() && i < i_ActualButton.size(); ++i)
	{
		MapButtons(i_Device, i_TerawattButton[i], i_ActualButton[i]);
	}
}

//------------------------------------------------------------------------
//	MapStick maps the given TerawattStick to the given ActualStick
//	on the suppied inDevice. The i_Device pointer will be added to m_Maps
//	if it is not already listed there.
//------------------------------------------------------------------------
void inPCVJoy::MapStick(inDevice *i_Device, int i_TerawattStick, int i_ActualStick)
{
	m_Maps[FindDeviceIndex(i_Device, i_TerawattStick)].m_Sticks[i_TerawattStick] = i_ActualStick;

	m_DirConsumed[i_TerawattStick] = false;
}

//------------------------------------------------------------------------
//	RemoveStick removes the the given TerawattStick, all mappings are thus
//  removed.
//------------------------------------------------------------------------
void inPCVJoy::RemoveStick(inDevice *i_Device, int i_TerawattStick, int i_ActualStick)
{
	std::vector<DeviceMap>::iterator it = m_Maps.begin() + FindDeviceIndex(i_Device,i_TerawattStick);
	m_Maps.erase(it);
	DBG_ASSERT(m_DirConsumed.end() != m_DirConsumed.find(i_TerawattStick), 
		"Invalid stick already removed!");
	m_DirConsumed.erase(m_DirConsumed.find(i_TerawattStick));
	std::map<int, bool>::iterator it_map = m_HeldConsumed.find(i_TerawattStick);
	if ( m_HeldConsumed.end() != it_map )
		m_HeldConsumed.erase(it_map);
	it_map = m_PressedConsumed.find(i_TerawattStick);
	if ( m_PressedConsumed.end() != it_map )
		m_PressedConsumed.erase(it_map);
	it_map = m_ReleasedConsumed.find(i_TerawattStick);
	if ( m_ReleasedConsumed.end() != it_map )
		m_ReleasedConsumed.erase(it_map);
}

//------------------------------------------------------------------------
//	MapDigitalDir maps the given TerawattStick to the given ActualButtons(s)
//	on the suppied inDevice. The i_Device pointer will be added to m_Maps
//	if it is not already listed there.
//------------------------------------------------------------------------
void inPCVJoy::MapDigitalDir(inDevice *i_Device, int i_TerawattStick, int i_North,
									  int i_East, int i_South, int i_West)
{
	//we can't allow the same actual button to mean two directions at once in the same DigitalMap
	//beyond that we'll say its the clients problem if they map say E to East in one and North in some other
	if (i_North == i_East || i_North == i_South || i_North == i_West ||
		i_East == i_South || i_East == i_West ||
		i_South == i_West
		)
	{
		return;
	}

	DigitalMap DMap;
	DMap.m_North = i_North;
	DMap.m_East = i_East;
	DMap.m_South = i_South;
	DMap.m_West = i_West;

	// do not call finddeviceindex directly in the index brackets of m_Maps
	// because it will cause a crash in release
	// NOTE: try doing it with the .NET VC++ version and see if it works.
	int index = FindDeviceIndex(i_Device, i_TerawattStick);
	m_Maps[index].m_DigitalMaps[i_TerawattStick] = DMap;
}

//------------------------------------------------------------------------
//	SetStickSensitivity sets the sensitivty to use on the given device and stick
//	this is like in FPS's where you change the mouse sensitivity
//------------------------------------------------------------------------
void inPCVJoy::SetStickSensitivity(inDevice *i_Device, int i_TerawattStick, float i_StickSensitivity)
{
	// do not call finddeviceindex directly in the index brackets of m_Maps
	// because it will cause a crash in release
	// NOTE: try doing it with the .NET VC++ version and see if it works.
	int index = FindDeviceIndex(i_Device, i_TerawattStick);
	m_Maps[index].m_StickSensitivity[i_TerawattStick] = i_StickSensitivity;
}

//------------------------------------------------------------------------
//	Think resets all consumed maps
//------------------------------------------------------------------------
void inPCVJoy::Think()
{ 
	ResetConsumed();
}

//------------------------------------------------------------------------
//	FindActualButton will find all actual buttons mapped to the given 
//	TerawattButton in the given DeviceMap and return them in the 
//	o_ActualButton vector
//------------------------------------------------------------------------
bool inPCVJoy::FindActualButtons(const DeviceMap& i_Map, int i_TerawattButton, 
										 std::vector<int>& o_ActualButton) const
{
	if (0 > i_TerawattButton)
	{
		//this is our check if any button capability
		o_ActualButton.push_back(i_TerawattButton);
		return true;
	}

	bool RetVal = false;
	typedef std::multimap<int, int>::const_iterator It;
	std::pair<It, It> PairIt = i_Map.m_Buttons.equal_range(i_TerawattButton);
	for (It i = PairIt.first; i != PairIt.second; ++i)
	{
		RetVal = true;
		o_ActualButton.push_back(i->second);
	}

	return RetVal;
}

//------------------------------------------------------------------------
//	FindDeviceIndex returns the index into the m_Maps vector that holds the
//	DeviceMap containing i_Device. It will add a new DeviceMap to the m_Maps
//	vector if i_Device is not yet listed, and then return the index of this
//	new DeviceMap.
//	it will also accept a terawattstick as a parameter and then handle 
//	the task of properly sizing the various stick vectors.
//------------------------------------------------------------------------
int inPCVJoy::FindDeviceIndex(inDevice *i_Device)
{

	DeviceMap DMap;

	if (inKeyboard *k = dynamic_cast<inKeyboard*>(i_Device))
	{
		DMap.m_Keyboard = k;
		DMap.m_Type = e_KEYBOARD;
	}
	else if (inMouse *m = dynamic_cast<inMouse*>(i_Device))
	{
		DMap.m_Mouse = m;
		DMap.m_Type = e_MOUSE;
	}
	else if (inGamepad *g = dynamic_cast<inGamepad*>(i_Device))
	{
		DMap.m_Gamepad = g;
		DMap.m_Type = e_GAMEPAD;
	}
	else if (inJoystick *j = dynamic_cast<inJoystick*>(i_Device))
	{
		DMap.m_Joystick = j;
		DMap.m_Type = e_JOYSTICK;
	}

	std::vector<DeviceMap>::iterator It = std::find(m_Maps.begin(), m_Maps.end(), DMap);

	if (It != m_Maps.end())
	{
		return It - m_Maps.begin();
	}

	m_Maps.push_back(DMap);

	return (m_Maps.size() - 1);
}

int inPCVJoy::FindDeviceIndex(inDevice *i_Device, int i_TerawattStick)
{
	DBG_ASSERT(0 <= i_TerawattStick && i_TerawattStick < m_NumSticks, "Invalid TerawattStick #" << i_TerawattStick);

	return FindDeviceIndex(i_Device);
}
//------------------------------------------------------------------------
//	InvertDigitalDir will remap io_Dir by inverting it according to i_bInvertX
//	and i_bInvertY
//------------------------------------------------------------------------
void inPCVJoy::InvertDigitalDir(inDigitalDir::DigitalDir& io_Dir, bool i_bInvertX, bool i_bInvertY) const
{
	if (inDigitalDir::e_NONE == io_Dir)
	{
		//don't need to do anything
		return;
	}

	if (i_bInvertX)
	{
		if (inDigitalDir::e_NE == io_Dir)
		{
			io_Dir = inDigitalDir::e_NW;
		}
		else if (inDigitalDir::e_E == io_Dir)
		{
			io_Dir = inDigitalDir::e_W;
		}
		else if (inDigitalDir::e_SE == io_Dir)
		{
			io_Dir = inDigitalDir::e_SW;
		}
		else if (inDigitalDir::e_SW == io_Dir)
		{
			io_Dir = inDigitalDir::e_SE;
		}
		else if (inDigitalDir::e_W == io_Dir)
		{
			io_Dir = inDigitalDir::e_E;
		}
		else if (inDigitalDir::e_NW == io_Dir)
		{
			io_Dir = inDigitalDir::e_NE;
		}
	}

	if (i_bInvertY)
	{
		if (inDigitalDir::e_N == io_Dir)
		{
			io_Dir = inDigitalDir::e_S;
		}
		else if (inDigitalDir::e_NE == io_Dir)
		{
			io_Dir = inDigitalDir::e_SE;
		}
		else if (inDigitalDir::e_SE == io_Dir)
		{
			io_Dir = inDigitalDir::e_NE;
		}
		else if (inDigitalDir::e_S == io_Dir)
		{
			io_Dir = inDigitalDir::e_N;
		}
		else if (inDigitalDir::e_SW == io_Dir)
		{
			io_Dir = inDigitalDir::e_NW;
		}
		else if (inDigitalDir::e_NW == io_Dir)
		{
			io_Dir = inDigitalDir::e_SW;
		}
	}
}

//------------------------------------------------------------------------
//	InvertAnalogDir will remap the directions by inverting them according to i_bInvertX
//	and i_bInvertY
//------------------------------------------------------------------------
void inPCVJoy::InvertAnalogDir(float& io_Axis, bool i_bInvertAxis) const
{
	if (i_bInvertAxis)
	{
		io_Axis = -io_Axis;
	}
}

void inPCVJoy::InvertAnalogDir(float& io_Dir, bool i_bInvertX, bool i_bInvertY) const
{
	if (i_bInvertX || i_bInvertY)
	{
		float X = sin(io_Dir);
		float Y = cos(io_Dir);
		InvertAnalogDir(X, i_bInvertX);
		InvertAnalogDir(Y, i_bInvertY);

		io_Dir = atan2f(X, Y);
		if (0 > io_Dir)
		{
			io_Dir += maConstants::c_fPI_Times_2;
		}
	}
}

void inPCVJoy::InvertAnalogDir(int& io_Axis, bool i_bInvertAxis) const
{
	if (i_bInvertAxis)
	{
		io_Axis = -io_Axis;
	}
}

void inPCVJoy::InvertAnalogDir(int& io_X, int& io_Y, bool i_bInvertX, bool i_bInvertY) const
{
	InvertAnalogDir(io_X, i_bInvertX);
	InvertAnalogDir(io_Y, i_bInvertY);
}

void inPCVJoy::InvertAnalogDir(int& io_X, int& io_Y, int& io_Z, bool i_bInvertX, bool i_bInvertY, bool i_bInvertZ) const
{
	InvertAnalogDir(io_X, i_bInvertX);
	InvertAnalogDir(io_Y, i_bInvertY);
	InvertAnalogDir(io_Z, i_bInvertZ);
}

//------------------------------------------------------------------------
//	GetKeyboardDigitalDir returns the DigitalDir of the given keyboard
//------------------------------------------------------------------------
inDigitalDir::DigitalDir inPCVJoy::GetKeyboardDigitalDir(inKeyboard* i_Keyboard, const inPCVJoy::DigitalMap& i_Map) const
{
	if (-1 == i_Map.m_North)
	{
		return inDigitalDir::e_NONE;
	}

	int X = 0;
	int Y = 0;
	if ( i_Keyboard->IsPressed(static_cast<inKeys::Keys>(i_Map.m_North) ) || 
		i_Keyboard->IsHeld(static_cast<inKeys::Keys>(i_Map.m_North)) )
	{
		Y += 1;
	}
	if (i_Keyboard->IsPressed(static_cast<inKeys::Keys>(i_Map.m_East)) || 
		i_Keyboard->IsHeld(static_cast<inKeys::Keys>(i_Map.m_East)))
	{
		X += 1;
	}
	if (i_Keyboard->IsPressed(static_cast<inKeys::Keys>(i_Map.m_South)) || 
		i_Keyboard->IsHeld(static_cast<inKeys::Keys>(i_Map.m_South)))
	{
		Y -= 1;
	}
	if (i_Keyboard->IsPressed(static_cast<inKeys::Keys>(i_Map.m_West)) || 
		i_Keyboard->IsHeld(static_cast<inKeys::Keys>(i_Map.m_West)))
	{
		X -= 1;
	}

	if (1 == Y)
	{
		if (1 == X)
		{
			return inDigitalDir::e_NE;
		}
		else if (-1 == X)
		{
			return inDigitalDir::e_NW;
		}
		else
		{
			return inDigitalDir::e_N;
		}
	}
	else if (-1 == Y)
	{
		if (1 == X)
		{
			return inDigitalDir::e_SE;
		}
		else if (-1 == X)
		{
			return inDigitalDir::e_SW;
		}
		else
		{
			return inDigitalDir::e_S;
		}
	}
	else
	{
		if (1 == X)
		{
			return inDigitalDir::e_E;
		}
		else if (-1 == X)
		{
			return inDigitalDir::e_W;
		}
		else
		{
			return inDigitalDir::e_NONE;
		}
	}
}

//------------------------------------------------------------------------
//	GetGamepadDigitalDir returns the DigitalDir of the given gamepad
//------------------------------------------------------------------------
inDigitalDir::DigitalDir inPCVJoy::GetGamepadDigitalDir(inGamepad* i_Gamepad, const DigitalMap& i_Map) const
{
	if (-1 == i_Map.m_North)
	{
		return inDigitalDir::e_NONE;
	}

	int X = 0;
	int Y = 0;
	if (i_Gamepad->IsPressed(i_Map.m_North) || i_Gamepad->IsHeld(i_Map.m_North) )
	{
		Y += 1;
	}
	if (i_Gamepad->IsPressed(i_Map.m_East) || i_Gamepad->IsHeld(i_Map.m_East))
	{
		X += 1;
	}
	if (i_Gamepad->IsPressed(i_Map.m_South) || i_Gamepad->IsHeld(i_Map.m_South))
	{
		Y -= 1;
	}
	if (i_Gamepad->IsPressed(i_Map.m_West) || i_Gamepad->IsHeld(i_Map.m_West))
	{
		X -= 1;
	}

	if (1 == Y)
	{
		if (1 == X)
		{
			return inDigitalDir::e_NE;
		}
		else if (-1 == X)
		{
			return inDigitalDir::e_NW;
		}
		else
		{
			return inDigitalDir::e_N;
		}
	}
	else if (-1 == Y)
	{
		if (1 == X)
		{
			return inDigitalDir::e_SE;
		}
		else if (-1 == X)
		{
			return inDigitalDir::e_SW;
		}
		else
		{
			return inDigitalDir::e_S;
		}
	}
	else
	{
		if (1 == X)
		{
			return inDigitalDir::e_E;
		}
		else if (-1 == X)
		{
			return inDigitalDir::e_W;
		}
		else
		{
			return inDigitalDir::e_NONE;
		}
	}
}

//------------------------------------------------------------------------
//	AverageDir takes the given vectors of (x,y) 2dvector values and returns 
//	averaged x, y values as well as float Dir in radians
//------------------------------------------------------------------------
void inPCVJoy::AverageDir(std::vector<maVector2d> i_XY, int& o_X, int& o_Y, float& o_Dir) const
{
	int i;
	maVector2d Total = i_XY[0];
	for (i = 1; i < i_XY.size(); ++i)
	{
		Total += i_XY[i];
	}

	o_X = Total.GetX();
	o_Y = Total.GetY();

	//clamp x and y values
	if (o_X < -inDeviceMgr::GetRange())
	{
		o_X = -inDeviceMgr::GetRange();
	}
	else if (o_X > inDeviceMgr::GetRange())
	{
		o_X = inDeviceMgr::GetRange();
	}
	if (o_Y < -inDeviceMgr::GetRange())
	{
		o_Y = -inDeviceMgr::GetRange();
	}
	else if (o_Y > inDeviceMgr::GetRange())
	{
		o_Y = inDeviceMgr::GetRange();
	}

	o_Dir = atan2f(o_X, o_Y);
	if (0 > o_Dir)
	{
		o_Dir += maConstants::c_fPI_Times_2;
	}
	else if (!o_Dir)
	{
		o_Dir = -1;
	}
}

//------------------------------------------------------------------------
//	ResetConsumed sets all of the consumed flags to false 
//------------------------------------------------------------------------
void inPCVJoy::ResetConsumed()
{
	std::map<int,bool>::iterator it;

	for (it = m_PressedConsumed.begin(); it != m_PressedConsumed.end(); ++it )
		it->second = false;

	for (it = m_HeldConsumed.begin(); it != m_HeldConsumed.end(); ++it )
		it->second = false;

	for (it = m_ReleasedConsumed.begin(); it != m_ReleasedConsumed.end(); ++it )
		it->second = false;

	for (it = m_DirConsumed.begin(); it != m_DirConsumed.end(); ++it )
		it->second = false;

}
