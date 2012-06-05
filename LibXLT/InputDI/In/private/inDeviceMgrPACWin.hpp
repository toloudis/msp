/****************************************************************************\
**  inDeviceMgrPACWin.hpp
**
**      inDeviceMgrPACWin.hpp defines the windows PAC component of the 
**		inDeviceMgr namespace.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef IN_DEVICEMGRPACWIN_HPP
#error inDeviceMgrPACWin.hpp multiply included
#endif
#define IN_DEVICEMGRPACWIN_HPP

#include <vector>

#ifndef IN_KEYBOARD_HPP
#include "Input/in/inKeyboard.hpp"
#endif

#ifndef IN_MOUSE_HPP
#include "Input/in/inMouse.hpp"
#endif

#ifndef IN_GAMEPAD_HPP
#include "Input/in/inGamepad.hpp"
#endif

#include <windows.h>

namespace inDeviceMgrPAC
{
	void Test(inGamepad* i_Test);

	//========================================================================
	//	Init must be called before you use the inDeviceMgr. i_Ranges set the
	//	range of values for the axes, so i_Range = 1000 sets a range of
	//	-1000 to 1000. i_Ranges must be a minimum of 100 to ensure that there
	//	is a range to report. i_DeadZone is a percentage representing how far you
	//	can press the gamepad before values start being counted. i_Saturation
	//	is a percentage representing when to start counting the value returned
	//	as at the full range. The ranges returned are unaffected by dead zone
	//	and saturation. If deadzone plus saturation is greater than 99%, the
	//	axes will be unable to return data. For this reason, deadzone will be 
	//	truncated to the range 0-49% and saturation to 51-100%
	//========================================================================
	void Init(int i_Range, float i_DeadZone, float i_Saturation, 
				std::vector<inKeyboard *>& o_Keyboards, std::vector<inMouse *>& o_Mouses, 
				std::vector<inGamepad *>& o_Gamepads,
				int& o_NumKeyboards, int& o_NumMouses, int& o_NumGamepads);

	//========================================================================
	//	CleanUp should be called after you are done with the inDeviceMgr.
	//========================================================================
	void CleanUp() throw();

	//========================================================================
	//	this is used by Delete(Device) to properly update the number of devices
	//	since it is different based on platform
	//========================================================================
	void UpdateNumOnDelete(int& io_Num);

	//========================================================================
	// We need to store the hwnd of the viewport window for later use
	//========================================================================
	void SetWindowHandle(void* i_Hwnd);
	HWND GetWindowHandle();
}