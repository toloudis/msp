/****************************************************************************\
**  inGamepadPACWin.hpp
**
**      inGamepadPACWin.hpp defines the PAC component of inGamepad.
**
**		NOTICE!!!!!!!!
**		Be sure to include both dinput.lib and dxguid.lib in any projects using
**		this package or you will have link problems
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef IN_GAMEPADPACWIN_HPP
#error inGamepadPACWin.hpp multiply included
#endif
#define IN_GAMEPADPACWIN_HPP

#ifndef IN_DITYPES_HPP
#include "InputDI/in/private/inDITypes.hpp"
#endif

#include <vector>


class inGamepadPAC
{
public:
	//========================================================================
	//	Constructor i_Ranges set the
	//	range of values for the axes, so i_Range = 1000 sets a range of
	//	-1000 to 1000. i_DeadZone is a percentage representing how far you
	//	can press the gamepad before values start being counted. i_Saturation
	//	is a percentage representing when to start counting the value returned
	//	as at the full range. The ranges returned are unaffected by dead zone
	//	and saturation. If deadzone plus saturation is greater than 99%, the
	//	axes will be unable to return data. For this reason, deadzone will be 
	//	truncated to the range 0-49% and saturation to 51-100%
	//========================================================================
	inGamepadPAC(int i_Range, float i_DeadZone, float i_Saturation, LPCDIDEVICEINSTANCE i_DeviceInstance);

	//========================================================================
	//	Destructor
	//========================================================================
	~inGamepadPAC();

	//========================================================================
	//	Think gives the device a chance to update its state once per frame
	//	i_State will be updated with the current state of the mouse
	//========================================================================
	void Think(	int i_NumSticks,
				int i_NumButtons,
				float o_Dir[],				//	stick direction in radians (i_NumSticks)
				float o_AnalogPressure[],	//	stick offset from center (radius) (i_NumSticks)
				float o_Twist[],			//	twist (i_NumSticks)
				int o_X[],					//	X axis of stick (i_NumSticks)
				int o_Y[],					//	Y axis of stick (i_NumSticks)
				int o_Z[],					//	Z axis of stick (i_NumSticks)
				bool o_CurState[],			//	0 up, 1 down, for each button (i_NumButtons)
				float o_ButtonPressure[]);	//	0.0f - released, 1.0f - maximum (i_NumButtons)

private:

	inDIDevicePtr m_lpDevice;
	int m_Range;
};