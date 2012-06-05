/*****************************************************************************
**  inDeviceMgr.hpp
**
**      inDeviceMgr defines the class inDeviceMgr, which is responsible for
**		enumerating or deleting all available devices,
**		as well as calling each devices' Think() function during its own
**		Think()
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef IN_DEVICEMGR_HPP
#error inDeviceMgr.hpp multiply included
#endif
#define IN_DEVICEMGR_HPP

#ifndef IN_KEYBOARD_HPP
#include "Input/in/inKeyboard.hpp"
#endif

#ifndef IN_MOUSE_HPP
#include "Input/in/inMouse.hpp"
#endif

#ifndef IN_GAMEPAD_HPP
#include "Input/in/inGamepad.hpp"
#endif

#ifndef IN_TABLET_HPP
#include "Input/in/inTablet.hpp"
#endif

//========================================================================
//	Forward References
//========================================================================
class inVirtualJoystick;
class inPCVJoy;

namespace inDeviceMgr
{
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
	void Init(int i_NumSticksOnVJoystick);
	void Init(int i_Range, float i_DeadZone, float i_Saturation, int i_NumSticksOnVJoystick);

	//========================================================================
	//	CleanUp should be called after you are done with the inDeviceMgr.
	//========================================================================
	void CleanUp() throw();

	//========================================================================
	//	Think calls Think on every device currently being managed
	//========================================================================
	void Think();

	//========================================================================
	//	GetRange returns the range that x, y, z values are returned in
	//========================================================================
	int GetRange();

	//========================================================================
	//	GetVirtualJoystick returns the inVirtualJoystick meant to be global
	//	to the game
	//========================================================================
	inVirtualJoystick *GetVirtualJoystick();

	//========================================================================
	//	GetPCVJoy returns the inPCVJoy meant to be global to the game
	//========================================================================
	inPCVJoy *GetPCVJoy();

	//========================================================================
	//	GetTablet returns the inTablet that registers all of the digitizer events
	//========================================================================
	inTablet *GetTablet();

	//========================================================================
	//	Get(Device) returns the specified device type at the given index
	//	returns a NULL pointer if there is none
	//========================================================================
	inKeyboard *GetKeyboard(int i_Index = 0);
	inMouse *GetMouse(int i_Index = 0);
	inGamepad *GetGamepad(int i_Index = 0);

	//========================================================================
	//	GetNum(Device) returns the number of the specified type of device
	//	currently being managed.
	//	For systems such as Dreamcast that have multiple ports, any of which
	//	may or not have a device attached, will instead return the total
	//	number of available ports
	//========================================================================
	int GetNumKeyboards();
	int GetNumMouses();
	int GetNumGamepads();
	int GetNumUserDefinedDevices();

	//========================================================================
	// AddUserDefinedDevice will add the given inDevice to the device mgr where
	// it will be managed until the user calls DeleteUserDefinedDevice.
	// The user still owns the memory and is responsible for cleanup.
	// It returns the index of that device
	//========================================================================
	int AddUserDefinedDevice( inDevice* i_Device );

	//========================================================================
	//	Delete(Device) removes the specified device type at the given index
	//	from being managed. This does not move devices indexed above it down
	//	to avoid device indexing confusion.
	//========================================================================
	void DeleteKeyboard(int i_Index);
	void DeleteMouse(int i_Index);
	void DeleteGamepad(int i_Index);
	void DeleteUserDefinedDevice( int i_Index );

	//========================================================================
	// We need to store the hwnd of the viewport window for later use
	//========================================================================
	void SetWindowHandle(void* i_Hwnd);
	void* GetWindowHandle();
}
