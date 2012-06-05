/*****************************************************************************
**  inDeviceMgr.cpp
**
**      inDeviceMgr defines the class inDeviceMgr, which is responsible for
**		enumerating or deleting all available devices,
**		as well as calling each devices' Think() function during its own
**		Think()
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Input/in/inDeviceMgr.hpp"

#include <vector>

#include "Core/env/envSTLHelpers.hpp"
#include "Input/in/private/inDeviceMgrPAC.hpp"
#include "Input/in/inPCVJoy.hpp"

//#include "Profiler/profile.h"


namespace inDeviceMgr
{

namespace
{
inPCVJoy *l_PCVJoy = 0;
inTablet *l_pTablet = 0;
std::vector<inKeyboard *> l_Keyboards;
std::vector<inMouse *> l_Mouses;
std::vector<inGamepad *> l_Gamepads;
std::vector<inDevice*> l_UserDefinedDevices(0);  // Initially, there are none

int l_NumKeyboards = 0;
int l_NumMouses = 0;
int l_NumGamepads = 0;
int l_Range = 1000;
float l_DeadZone = 0.05f;
float l_Saturation = 0.95f;
}

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
void Init(int i_NumSticksOnVJoystick)
{
	inDeviceMgrPAC::Init(l_Range, l_DeadZone, l_Saturation, 
							l_Keyboards, l_Mouses, l_Gamepads, //l_Joysticks, 
							l_NumKeyboards, l_NumMouses, l_NumGamepads);//, l_NumJoysticks);

	l_PCVJoy = new inPCVJoy(i_NumSticksOnVJoystick);
	l_pTablet = new inTablet();
}

void Init(int i_Range, float i_DeadZone, float i_Saturation, int i_NumSticksOnVJoystick)
{
	if (100 <= i_Range)
	{
		l_Range = i_Range;
	}

	if (0 > i_DeadZone)
	{
		l_DeadZone = 0;
	}
	else if (0.49f < i_DeadZone)
	{
		l_DeadZone = 0.49f;
	}
	else
	{
		l_DeadZone = i_DeadZone;
	}

	if (0.51f > i_Saturation)
	{
		l_Saturation = 0.51f;
	}
	else if (1.0f < i_Saturation)
	{
		l_Saturation = 1.0f;
	}
	else
	{
		l_Saturation = i_Saturation;
	}

	Init(i_NumSticksOnVJoystick);
}
//========================================================================
//	CleanUp should be called after you are done with the inDeviceMgr.
//========================================================================
void CleanUp() throw()
{
	inDeviceMgrPAC::CleanUp();

	envSTLHelpers::DeleteContainer(l_Keyboards);
	envSTLHelpers::DeleteContainer(l_Mouses);
	envSTLHelpers::DeleteContainer(l_Gamepads);

	delete l_PCVJoy;
	delete l_pTablet;

	// user defined devices are not deleted here, only by the user
}

//========================================================================
//	Think calls Think on every device currently being managed
//========================================================================
void Think()
{ //PROFILE_FUNC();
	int i;
	for (i = 0; i < l_Keyboards.size(); ++i)
	{
		if (l_Keyboards[i])
		{
			l_Keyboards[i]->Think();
		}
	}

	for (i = 0; i < l_Mouses.size(); ++i)
	{
		if (l_Mouses[i])
		{
			l_Mouses[i]->Think();
		}
	}

	for (i = 0; i < l_Gamepads.size(); ++i)
	{
		if (l_Gamepads[i])
		{
			l_Gamepads[i]->Think();
		}
	}

	for (i = 0; i < l_UserDefinedDevices.size(); ++i)
	{
		if (l_UserDefinedDevices[i])
		{
			l_UserDefinedDevices[i]->Think();
		}
	}

	if ( l_PCVJoy )
		l_PCVJoy->Think();

	if ( l_pTablet )
		l_pTablet->Think();
}

//========================================================================
//	GetRange returns the range that x, y, z values are returned in
//========================================================================
int GetRange()
{
	return l_Range;
}

//========================================================================
//	GetVirtualJoystick returns the inVirtualJoystick meant to be global
//	to the game
//========================================================================
inVirtualJoystick *GetVirtualJoystick()
{
	return l_PCVJoy;
}

//========================================================================
//	GetPCVJoy returns the inPCVJoy meant to be global to the game
//========================================================================
inPCVJoy *GetPCVJoy()
{
	return l_PCVJoy;
}

//========================================================================
//	GetTablet returns the inTablet that registers all of the digitizer events
//========================================================================
inTablet *GetTablet()
{
	return l_pTablet;
}


//========================================================================
//	Get(Device) returns the specified device type at the given index
//	returns a NULL pointer if there is none
//========================================================================
inKeyboard *GetKeyboard(int i_Index)
{
#if ENV_WINDOWS
	if (l_Keyboards.empty()) return NULL;
	DBG_ASSERT(0 <= i_Index && i_Index < l_Keyboards.size(), "Invalid index #" << i_Index);

	return l_Keyboards[i_Index];
#else
	return NULL;
#endif
}

inMouse *GetMouse(int i_Index)
{
#if ENV_WINDOWS
	DBG_ASSERT(0 <= i_Index && i_Index < l_Mouses.size(), "Invalid index #" << i_Index);

	return l_Mouses[i_Index];
#else
	return NULL;
#endif
}

inGamepad *GetGamepad(int i_Index)
{
	DBG_ASSERT(0 <= i_Index && i_Index < l_Gamepads.size(), "Invalid index #" << i_Index);

	return l_Gamepads[i_Index];
}

inDevice* GetUserDefinedDevice( int i_Index )
{
	DBG_ASSERT(0 <= i_Index && i_Index < l_UserDefinedDevices.size(), "Invalid index #" << i_Index);
	return l_UserDefinedDevices[i_Index];
}

//========================================================================
//	GetNum(Device) returns the number of the specified type of device
//	currently being managed.
//	For systems such as Dreamcast that have multiple ports, any of which
//	may or not have a device attached, will instead return the total
//	number of available ports
//========================================================================
int GetNumKeyboards()
{
	return l_NumKeyboards;
}

int GetNumMouses()
{
	return l_NumMouses;
}

int GetNumGamepads()
{
	return l_NumGamepads;
}

int GetNumUserDefinedDevices()
{
	return l_UserDefinedDevices.size();
}

//========================================================================
// AddUserDefinedDevice will add the given inDevice to the device mgr where
// it will be managed until the user calls DeleteUserDefinedDevice.
// The user still owns the memory and is responsible for cleanup.
// It returns the index of that device
//========================================================================
int AddUserDefinedDevice( inDevice* i_Device )
{
	DBG_ASSERT(NULL != i_Device, "Invalid NULL Device!");

	l_UserDefinedDevices.push_back(i_Device);
	return l_UserDefinedDevices.size()-1;
}

//========================================================================
//	Delete(Device) removes the specified device type at the given index
//	from being managed. This does not move devices indexed above it down
//	to avoid device indexing confusion.
//========================================================================
void DeleteKeyboard(int i_Index)
{
	DBG_ASSERT(0 <= i_Index && i_Index < l_Keyboards.size(), "Invalid index #" << i_Index);

	if (l_Keyboards[i_Index])
	{
		inDeviceMgrPAC::UpdateNumOnDelete(l_NumKeyboards);
	}

	delete l_Keyboards[i_Index];
	l_Keyboards[i_Index] = NULL;
}

void DeleteMouse(int i_Index)
{
	DBG_ASSERT(0 <= i_Index && i_Index < l_Mouses.size(), "Invalid index #" << i_Index);

	if (l_Mouses[i_Index])
	{
		inDeviceMgrPAC::UpdateNumOnDelete(l_NumMouses);
	}

	delete l_Mouses[i_Index];
	l_Mouses[i_Index] = NULL;
}

void DeleteGamepad(int i_Index)
{
	DBG_ASSERT(0 <= i_Index && i_Index < l_Gamepads.size(), "Invalid index #" << i_Index);

	if (l_Gamepads[i_Index])
	{
		inDeviceMgrPAC::UpdateNumOnDelete(l_NumGamepads);
	}

	delete l_Gamepads[i_Index];
	l_Gamepads[i_Index] = NULL;
}

void DeleteUserDefinedDevice( int i_Index )
{
	DBG_ASSERT(0 <= i_Index && i_Index < l_UserDefinedDevices.size(), "Invalid index #" << i_Index);

	// We don't delete the memory, the user owns that, just remove the instance from 
	// the vector
	l_UserDefinedDevices.erase(l_UserDefinedDevices.begin()+i_Index);
}

//============================================================================
// We need to store the hwnd of the viewport window for later use
//============================================================================
void SetWindowHandle(void* i_Hwnd)
{
	inDeviceMgrPAC::SetWindowHandle(i_Hwnd);
}
void* GetWindowHandle()
{
	return inDeviceMgrPAC::GetWindowHandle();
}

}
