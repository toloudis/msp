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

#ifdef IN_GAMEPAD_HPP
#error inGamepad.hpp multiply included
#endif
#define IN_GAMEPAD_HPP

#include <vector>

#ifndef IN_DEVICE_HPP
#include "Input/in/inDevice.hpp"
#endif

#ifndef IN_DIGITALDIR_HPP
#include "Input/in/inDigitalDir.hpp"
#endif

class inGamepadPAC;

class inGamepad : public inDevice
{
public:
	enum { e_ANYBUTTON = -1 };

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
	inGamepad(int i_Range, float i_DeadZone, float i_Saturation, inGamepadPAC* i_PAC);

	//========================================================================
	//	Destructor
	//========================================================================
	~inGamepad();

	//========================================================================
	//	Think gives the device a chance to update its state once per frame
	//========================================================================
	void Think();

	//========================================================================
	//	IsPressed
	//========================================================================
	bool IsPressed(int i_Button) const;
	bool IsPressed(int i_Button, float& o_AnalogPressure) const;

	//========================================================================
	//	IsHeld
	//========================================================================
	bool IsHeld(int i_Button) const;
	bool IsHeld(int i_Button, float& o_AnalogPressure) const;

	//========================================================================
	//	IsReleased
	//========================================================================
	bool IsReleased(int i_Button) const;

	//========================================================================
	//	IsDirPressed returns 0 if there is no direction being pressed on 
	//	i_Stick, else returns 1
	//========================================================================
	bool IsDirPressed(int i_Stick) const;

	//========================================================================
	//	GetDigitalDir returns the current digital direction for i_Stick
	//========================================================================
	inDigitalDir::DigitalDir GetDigitalDir(int i_Stick) const;

	//========================================================================
	//	GetAnalogDir returns i_Sticks' direction in various configurations
	//	the float returned and o_Twist are direction in radians, and 
	//	o_AnalogPressure is a value from 0 - 1 representing how far the stick
	//	is being pushed
	//========================================================================
	float GetAnalogDir(int i_Stick) const;
	float GetAnalogDir(int i_Stick, float& o_AnalogPressure) const;
	float GetAnalogDir(int i_Stick, float& o_AnalogPressure, float& o_Twist) const;
	void GetAnalogDir(int i_Stick, int& o_X, int& o_Y) const;
	void GetAnalogDir(int i_Stick, int& o_X, int& o_Y, int& o_Z) const;

	inline inGamepadPAC* GetPAC() const;

private:

	enum { e_NumSticks = 2 };
	enum { e_PadBufferSize = 128 };

	inDigitalDir::DigitalDir m_DigitalDir[e_NumSticks];
	float m_Dir[e_NumSticks];
	float m_AnalogPressure[e_NumSticks];
	float m_Twist[e_NumSticks];
	int m_X[e_NumSticks];
	int m_Y[e_NumSticks];
	int m_Z[e_NumSticks];

	bool m_PastState[e_PadBufferSize];
	bool m_CurState[e_PadBufferSize];
	float m_ButtonPressure[e_PadBufferSize];
	inGamepadPAC *m_pPAC;
	int m_Range;
};

//========================================================================
//========================================================================
inline inGamepadPAC* inGamepad::GetPAC() const
{
	return m_pPAC;
}
