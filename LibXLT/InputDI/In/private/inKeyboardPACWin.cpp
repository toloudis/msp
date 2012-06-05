/****************************************************************************\
**  inKeyboardPACWin.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "InputDI/in/private/inKeyboardPACWin.hpp"

#include "Core/app/private/appApplicationPACWin.hpp"
#include "Core/env/envInitX.hpp"
#include "Input/in/inKeys.hpp"
#include "InputDI/in/private/inDirectInputWin.hpp"


//============================================================================
//============================================================================
namespace
{

int DIKToTerawattMap[256] = {
	-1, //left blank in dik
	inKeys::e_ESC, //DIK_ESCAPE
	inKeys::e_1, //DIK_1
	inKeys::e_2, //DIK_2
	inKeys::e_3, //DIK_3
	inKeys::e_4, //DIK_4
	inKeys::e_5, //DIK_5
	inKeys::e_6, //DIK_6
	inKeys::e_7, //DIK_7
	inKeys::e_8, //DIK_8
	inKeys::e_9, //DIK_9
	inKeys::e_0, //DIK_0
	inKeys::e_DASH, //DIK_MINUS
	inKeys::e_EQUALS, //DIK_EQUALS
	inKeys::e_BACKSPACE, //DIK_BACK
	inKeys::e_TAB, //DIK_TAB
	inKeys::e_Q, //DIK_Q
	inKeys::e_W, //DIK_W
	inKeys::e_E, //DIK_E
	inKeys::e_R, //DIK_R
	inKeys::e_T, //DIK_T
	inKeys::e_Y, //DIK_Y
	inKeys::e_U, //DIK_U
	inKeys::e_I, //DIK_I
	inKeys::e_O, //DIK_O
	inKeys::e_P, //DIK_P
	inKeys::e_LBRACKET, //DIK_LBRACKET
	inKeys::e_RBRACKET, //DIK_RBRACKET
	inKeys::e_ENTER, //DIK_RETURN   (ENTER ON MAIN KEYBOARD)
	inKeys::e_LCTRL, //DIK_LCONTROL
	inKeys::e_A, //DIK_A
	inKeys::e_S, //DIK_S
	inKeys::e_D, //DIK_D
	inKeys::e_F, //DIK_F
	inKeys::e_G, //DIK_G
	inKeys::e_H, //DIK_H
	inKeys::e_J, //DIK_J
	inKeys::e_K, //DIK_K
	inKeys::e_L, //DIK_L
	inKeys::e_SEMICOLON, //DIK_SEMICOLON
	inKeys::e_APOSTROPHE, //DIK_APOSTROPHE
	inKeys::e_TILDE, //DIK_GRAVE
	inKeys::e_LSHIFT, //DIK_LSHIFT
	inKeys::e_BACKSLASH, //DIK_BACKSLASH
	inKeys::e_Z, //DIK_Z
	inKeys::e_X, //DIK_X
	inKeys::e_C, //DIK_C
	inKeys::e_V, //DIK_V
	inKeys::e_B, //DIK_B
	inKeys::e_N, //DIK_N
	inKeys::e_M, //DIK_M
	inKeys::e_COMMA, //DIK_COMMA
	inKeys::e_PERIOD, //DIK_PERIOD
	inKeys::e_SLASH, //DIK_SLASH
	inKeys::e_RSHIFT, //DIK_RSHIFT
	inKeys::e_NUMPADMULTIPLY, //DIK_MULTIPLY
	inKeys::e_LALT, //DIK_LMENU		(LEFT ALT)
	inKeys::e_SPACE, //DIK_SPACE
	inKeys::e_CAPSLOCK, //DIK_CAPITAL	(CAPS LOCK)
	inKeys::e_F1, //DIK_F1
	inKeys::e_F2, //DIK_F2
	inKeys::e_F3, //DIK_F3
	inKeys::e_F4, //DIK_F4
	inKeys::e_F5, //DIK_F5
	inKeys::e_F6, //DIK_F6
	inKeys::e_F7, //DIK_F7
	inKeys::e_F8, //DIK_F8
	inKeys::e_F9, //DIK_F9
	inKeys::e_F10, //DIK_F10
	inKeys::e_NUMLOCK, //DIK_NUMLOCK
	inKeys::e_SCROLL, //DIK_SCROLL
	inKeys::e_NUMPAD7, //DIK_NUMPAD7
	inKeys::e_NUMPAD8, //DIK_NUMPAD8
	inKeys::e_NUMPAD9, //DIK_NUMPAD9
	inKeys::e_NUMPADMINUS, //DIK_SUBTRACT
	inKeys::e_NUMPAD4, //DIK_NUMPAD4
	inKeys::e_NUMPAD5, //DIK_NUMPAD5
	inKeys::e_NUMPAD6, //DIK_NUMPAD6
	inKeys::e_NUMPADPLUS, //DIK_ADD
	inKeys::e_NUMPAD1, //DIK_NUMPAD1
	inKeys::e_NUMPAD2, //DIK_NUMPAD2
	inKeys::e_NUMPAD3, //DIK_NUMPAD3
	inKeys::e_NUMPAD0, //DIK_NUMPAD0
	inKeys::e_NUMPADDECIMAL, //DIK_DECIMAL
	-1, //left blank in dik
	-1, //left blank in dik
	-1, //DIK_OEM_102		(GERMAN/UK)
	inKeys::e_F11, //DIK_F11
	inKeys::e_F12, //DIK_F12
	-1, //left blank in dik
	-1, //left blank in dik
	-1, //left blank in dik
	-1, //left blank in dik
	-1, //left blank in dik
	-1, //left blank in dik
	-1, //left blank in dik
	-1, //left blank in dik
	-1, //left blank in dik
	-1, //left blank in dik
	-1, //left blank in dik
	-1, //DIK_F13
	-1, //DIK_F14
	-1, //DIK_F15
	-1, //left blank in dik
	-1, //left blank in dik
	-1, //left blank in dik
	-1, //left blank in dik
	-1, //left blank in dik
	-1, //left blank in dik
	-1, //left blank in dik
	-1, //left blank in dik
	-1, //left blank in dik
	-1, //DIK_KANA			(JAPANESE)
	-1, //left blank in dik
	-1, //left blank in dik
	-1, //DIK_ABNT_C1		(PORTUGUESE)
	-1, //left blank in dik
	-1, //left blank in dik
	-1, //left blank in dik
	-1, //left blank in dik
	-1, //left blank in dik
	-1, //DIK_CONVERT		(JAPANESE)
	-1, //left blank in dik
	-1, //DIK_NOCONVERT		(JAPANESE)
	-1, //left blank in dik
	-1, //DIK_YEN			(JAPANESE)
	-1, //left blank in dik
	-1, //left blank in dik
	-1, //left blank in dik
	-1, //left blank in dik
	-1, //left blank in dik
	-1, //left blank in dik
	-1, //left blank in dik
	-1, //left blank in dik
	-1, //left blank in dik
	-1, //left blank in dik
	-1, //left blank in dik
	-1, //left blank in dik
	-1, //left blank in dik
	-1, //left blank in dik
	-1, //DIK_ABNT_C2		(PORTUGUESE)
	-1, //DIK_NUMPADEQUALS	(NEC PC98)
	-1, //left blank in dik
	-1, //left blank in dik
	-1, //DIK_PREVTRACK OR DIK_CIRCUMFLEX	(JAPANESE)
	-1, //DIK_AT			(JAPANESE)
	-1, //DIK_COLON			(JAPANESE)
	-1, //DIK_UNDERLINE		(JAPANESE)
	-1, //DIK_KANJI			(JAPANESE)
	-1, //DIK_STOP			(JAPANESE)
	-1, //DIK_AX			(JAPANESE)
	-1, //DIK_UNLABELED		(JAPANESE)
	-1, //left blank in dik
	-1, //DIK_NEXTTRACK
	-1, //left blank in dik
	-1, //left blank in dik
	inKeys::e_NUMPADENTER, //DIK_NUMPADENTER
	inKeys::e_RCTRL, //DIK_RCONTROL
	-1, //left blank in dik
	-1, //left blank in dik
	-1, //DIK_MUTE
	-1, //DIK_CALCULATOR
	-1, //DIK_PLAYPAUSE
	-1, //left blank in dik
	-1, //DIK_MEDIASTOP
	-1, //left blank in dik
	-1, //left blank in dik
	-1, //left blank in dik
	-1, //left blank in dik
	-1, //left blank in dik
	-1, //left blank in dik
	-1, //left blank in dik
	-1, //left blank in dik
	-1, //left blank in dik
	-1, //DIK_VOLUMEDOWN
	-1, //left blank in dik
	-1, //DIK_VOLUMEUP
	-1, //left blank in dik
	-1, //DIK_WEBHOME
	-1, //DIK_NUMPADCOMMA	(NEC PC98)
	-1, //left blank in dik
	inKeys::e_NUMPADDIVIDE, //DIK_DIVIDE
	-1, //left blank in dik
	inKeys::e_PRINT, //DIK_SYSRQ
	inKeys::e_RALT, //DIK_RMENU		(RIGHT ALT)
	-1, //left blank in dik
	-1, //left blank in dik
	-1, //left blank in dik
	-1, //left blank in dik
	-1, //left blank in dik
	-1, //left blank in dik
	-1, //left blank in dik
	-1, //left blank in dik
	-1, //left blank in dik
	-1, //left blank in dik
	-1, //left blank in dik
	-1, //left blank in dik
	inKeys::e_PAUSE, //DIK_PAUSE
	-1, //left blank in dik
	inKeys::e_HOME, //DIK_HOME
	inKeys::e_UP, //DIK_UP
	inKeys::e_PAGEUP, //DIK_PRIOR		(PAGE UP)
	-1, //left blank in dik
	inKeys::e_LEFT, //DIK_LEFT
	-1, //left blank in dik
	inKeys::e_RIGHT, //DIK_RIGHT
	-1, //left blank in dik
	inKeys::e_END, //DIK_END
	inKeys::e_DOWN, //DIK_DOWN
	inKeys::e_PAGEDOWN, //DIK_NEXT			(PAGE DOWN)
	inKeys::e_INSERT, //DIK_INSERT
	inKeys::e_DELETE, //DIK_DELETE
	-1, //left blank in dik
	-1, //left blank in dik
	-1, //left blank in dik
	-1, //left blank in dik
	-1, //left blank in dik
	-1, //left blank in dik
	-1, //left blank in dik
	inKeys::e_LWIN, //DIK_LWIN
	inKeys::e_RWIN, //DIK_RWIN
	-1, //DIK_APPS
	//these -1s fill out the array to prevent a bounds problem
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1
};

//DWORD	l_SetCooperativeFlags = DISCL_FOREGROUND | DISCL_EXCLUSIVE;
DWORD	l_SetCooperativeFlags = DISCL_FOREGROUND | DISCL_NONEXCLUSIVE;

}

//------------------------------------------------------------------------
//	Constructor
//------------------------------------------------------------------------
inKeyboardPAC::inKeyboardPAC()
{
	HRESULT hresult;

	hresult = inDirectInputWin::g_lpInput->CreateDevice(	GUID_SysKeyboard, 
															&m_lpDevice, 
															NULL);
	if (DI_OK != hresult)
	{
		throw envInitX("in");
	}

	hresult = m_lpDevice->SetDataFormat(&c_dfDIKeyboard);
	if (DI_OK != hresult)
	{
		throw envInitX("in");
	}

	DBG_ASSERT( appApplicationPAC::GetHWND() != NULL, "Application doesn't have a valid HWND yet." );

	hresult = m_lpDevice->SetCooperativeLevel(appApplicationPAC::GetHWND(), l_SetCooperativeFlags);
	if (E_HANDLE == hresult)
	{
		// E_HANDLE The HWND parameter is not a valid top-level window that belongs to the process. 

		if ( DISCL_EXCLUSIVE & l_SetCooperativeFlags )
		{
			DBG_WARNING("setting keyboard to exclusive mode failed -- keyboard set to non-exclusive");
			DBG_WARNING("    inPackage::Init() must NOT be called until after (or in) appPostStartEvent");
			DBG_WARNING("    to ensure a valid HWND for device initialization!!!!!" );
		}
		else
		{
			DBG_WARNING("tried to set keyboard cooperative level and it failed");
			//throw envInitX("in");
		}
	}
	hresult = m_lpDevice->Acquire();
}

//------------------------------------------------------------------------
//	Destructor
//------------------------------------------------------------------------
inKeyboardPAC::~inKeyboardPAC()
{
	m_lpDevice->Unacquire();
	ULONG refcount = m_lpDevice->Release();
	if (refcount != 0)
	{
		DBG_WARNING("Keyboard device ref count is not 0 but " << refcount);
	}
}

//------------------------------------------------------------------------
//	Think gives the device a chance to update its state once per frame
//	i_State will be updated with the current state of the keyboard
//------------------------------------------------------------------------
void inKeyboardPAC::Think(bool o_State[])
{
	HRESULT hresult = m_lpDevice->GetDeviceState(256, m_State);
	if (DIERR_INPUTLOST == hresult || DIERR_NOTACQUIRED == hresult)
	{
		//we try to reacquire the device here
		hresult = m_lpDevice->SetCooperativeLevel(appApplicationPAC::GetHWND(), l_SetCooperativeFlags);
		hresult = m_lpDevice->Acquire();
		hresult = m_lpDevice->GetDeviceState(256, m_State);
		if (DIERR_INPUTLOST == hresult || DIERR_NOTACQUIRED == hresult)
		{
			//we'll pick up the input on the next think, more than likely an alt tab has occurred
			return;
		}
	}

	if (DI_OK != hresult)
	{
		DBG_LOG("Keyboard problem: hresult is " << hresult);
		return;
	}

	int i;
	for (i = 0; i < 256; ++i)
	{
		if (-1 != DIKToTerawattMap[i])
		{
			o_State[DIKToTerawattMap[i]] = (m_State[i] & 0x80) ? 1 : 0;
		}
	}
}

//------------------------------------------------------------------------
//	SetNonExclusiveKeyboard will initialize keyboard devices in NON_EXCLUSIVE
//	which results in the app receiving charevents in general from Windows
//	Generally for games this is a bad thing as it makes it easier for windows
//	to tak focus away due to an accidental keypress (i.e. oops! i hit the WIN
//	key). This function is here for rapid prototyping purposes since the
//	default configuration for keyboard is now EXCLUSIVE, preventing needless
//	key events, but as a side effect appCharEvents cease to function properly
//	NOTE: u must call this before the inPackage::Init() to take effect
//------------------------------------------------------------------------
void inKeyboardPAC::SetNonExclusiveKeyboard()
{
	l_SetCooperativeFlags = DISCL_FOREGROUND | DISCL_NONEXCLUSIVE;
}

//------------------------------------------------------------------------
//	SetNonExclusiveKeyboard will initialize keyboard devices in EXCLUSIVE
//	which results in the app NOT receiving charevents in general from Windows.
//	NOTE: u must call this before the inPackage::Init() to take effect
//------------------------------------------------------------------------
void inKeyboardPAC::SetExclusiveKeyboard()
{
	l_SetCooperativeFlags = DISCL_FOREGROUND | DISCL_EXCLUSIVE;
}