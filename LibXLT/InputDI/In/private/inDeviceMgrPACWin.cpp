/****************************************************************************\
**  inDeviceMgrPACWin.cpp
**
**      inDeviceMgrPACWin.cpp defines the windows PAC component of the 
**		inDeviceMgr namespace.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "InputDI/in/private/inDeviceMgrPACWin.hpp"

#include "Core/app/private/appApplicationPACWin.hpp"
#include "Core/env/envInitX.hpp"
#include "InputDI/in/private/inDirectInputWin.hpp"
#include "InputDI/in/private/inGamepadPACWin.hpp"

#pragma comment(lib, c_g2dD3DLIBRARYINPUT)
#pragma comment(lib, "dxguid.lib")


//============================================================================
//============================================================================
namespace inDeviceMgrPAC
{
namespace
{

int l_Range;
float l_DeadZone;
float l_Saturation;
HWND l_Hwnd;

BOOL CALLBACK DIEnumKeyboardsProc(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef)
{
	inKeyboard *k = new inKeyboard;
	k->SetDeviceName(itString(lpddi->tszInstanceName));
	//DBG_LOG("Keyboard named: " << dbgLog::UnicodetoANSI(k->GetDeviceName().GetString(), k->GetDeviceName().GetLength()).c_str());
	//DBG_LOG("Keyboard named: " << lpddi->tszProductName);
	//DBG_LOG("Keyboard named: " << lpddi->tszInstanceName);
	static_cast<std::vector<inKeyboard*>*>(pvRef)->push_back(k);

	return DIENUM_CONTINUE;
}

BOOL CALLBACK DIEnumMousesProc(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef)
{
	inMouse *m = new inMouse;
	m->SetDeviceName(itString(lpddi->tszInstanceName));
	//DBG_LOG("Mouse named: " << dbgLog::UnicodetoANSI(m->GetDeviceName().GetString(), m->GetDeviceName().GetLength()).c_str());
	//DBG_LOG("Mouse named: " << lpddi->tszProductName);
	//DBG_LOG("Mouse named: " << lpddi->tszInstanceName);
	static_cast<std::vector<inMouse*>*>(pvRef)->push_back(m);

	return DIENUM_CONTINUE;
}

BOOL CALLBACK DIEnumGamepadsProc(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef)
{
	inGamepadPAC *gp_pac = new inGamepadPAC(l_Range, l_DeadZone, l_Saturation, lpddi);
	inGamepad* new_gamepad = new inGamepad(l_Range, l_DeadZone, l_Saturation, gp_pac);
	new_gamepad->SetDeviceName(itString(lpddi->tszInstanceName));
	//DBG_LOG("Gamepad named: " << dbgLog::UnicodetoANSI(new_gamepad->GetDeviceName().GetString(), new_gamepad->GetDeviceName().GetLength()).c_str());
	//DBG_LOG("Gamepad named: " << lpddi->tszProductName);
	//DBG_LOG("Gamepad named: " << lpddi->tszInstanceName);
	static_cast<std::vector<inGamepad*>*>(pvRef)->push_back(new_gamepad);

	return DIENUM_CONTINUE;
}

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
void Init(int i_Range, float i_DeadZone, float i_Saturation, 
			std::vector<inKeyboard *>& o_Keyboards, std::vector<inMouse *>& o_Mouses, 
			std::vector<inGamepad *>& o_Gamepads,
			int& o_NumKeyboards, int& o_NumMouses, int& o_NumGamepads)
{
	l_Range = i_Range;
	l_DeadZone = i_DeadZone;
	l_Saturation = i_Saturation;

	HRESULT hresult;
	hresult = DirectInput8Create(	appApplicationPAC::GetHINSTANCE(), 
									DIRECTINPUT_VERSION, 
									IID_IDirectInput8A,
									(void**)&inDirectInputWin::g_lpInput, 
									NULL);
	if (DI_OK != hresult)
		throw envInitX("in");

	//enumerate Keyboards
	hresult = inDirectInputWin::g_lpInput->EnumDevices(	DI8DEVTYPE_KEYBOARD, 
														(LPDIENUMDEVICESCALLBACK)&DIEnumKeyboardsProc, 
														&o_Keyboards, 
														DIEDFL_ATTACHEDONLY);
	if (DI_OK != hresult)
		throw envInitX("in");

	o_NumKeyboards = o_Keyboards.size();


	//enumerate Mouses
	hresult = inDirectInputWin::g_lpInput->EnumDevices(	DI8DEVCLASS_POINTER, 
														(LPDIENUMDEVICESCALLBACK)&DIEnumMousesProc, 
														&o_Mouses, 
														DIEDFL_ATTACHEDONLY);
	if (DI_OK != hresult)
		throw envInitX("in");

	o_NumMouses = o_Mouses.size();

	//enumerate Gamepads
	hresult = inDirectInputWin::g_lpInput->EnumDevices(	DI8DEVCLASS_GAMECTRL, 
														(LPDIENUMDEVICESCALLBACK)&DIEnumGamepadsProc, 
														&o_Gamepads, 
														DIEDFL_ATTACHEDONLY);
	if (DI_OK != hresult)
		throw envInitX("in");

	o_NumGamepads = o_Gamepads.size();
}

//========================================================================
//	CleanUp should be called after you are done with the inDeviceMgr.
//========================================================================
void CleanUp() throw()
{
	inDirectInputWin::g_lpInput->Release();
}

//========================================================================
//	this is used by Delete(Device) to properly update the number of devices
//	since this behavior is platform dependent
//========================================================================
void UpdateNumOnDelete(int& io_Num)
{
	--io_Num;
}


//============================================================================
// We need to store the hwnd of the viewport window for later use
//============================================================================
void SetWindowHandle(void* i_Hwnd)
{
	l_Hwnd = (HWND)i_Hwnd;
}

HWND GetWindowHandle()
{
	return l_Hwnd;
}

}