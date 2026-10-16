/****************************************************************************\
**  inKeyboardPACWin.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Input/in/private/inKeyboardPACWin.hpp"


//============================================================================
//============================================================================
namespace inKeyboardPAC
{
namespace
{

enum { e_EXTENDED = 0x100 };

}

//========================================================================
//	NativeScanCodeToKey returns false for keys that inKeys doesn't have
//========================================================================
bool NativeScanCodeToKey(unsigned int i_NativeScanCode, inKeys::Keys& o_Key)
{
	switch (i_NativeScanCode)
	{
	case 0x01: o_Key = inKeys::e_ESC; return true;
	case 0x02: o_Key = inKeys::e_1; return true;
	case 0x03: o_Key = inKeys::e_2; return true;
	case 0x04: o_Key = inKeys::e_3; return true;
	case 0x05: o_Key = inKeys::e_4; return true;
	case 0x06: o_Key = inKeys::e_5; return true;
	case 0x07: o_Key = inKeys::e_6; return true;
	case 0x08: o_Key = inKeys::e_7; return true;
	case 0x09: o_Key = inKeys::e_8; return true;
	case 0x0A: o_Key = inKeys::e_9; return true;
	case 0x0B: o_Key = inKeys::e_0; return true;
	case 0x0C: o_Key = inKeys::e_DASH; return true;
	case 0x0D: o_Key = inKeys::e_EQUALS; return true;
	case 0x0E: o_Key = inKeys::e_BACKSPACE; return true;
	case 0x0F: o_Key = inKeys::e_TAB; return true;
	case 0x10: o_Key = inKeys::e_Q; return true;
	case 0x11: o_Key = inKeys::e_W; return true;
	case 0x12: o_Key = inKeys::e_E; return true;
	case 0x13: o_Key = inKeys::e_R; return true;
	case 0x14: o_Key = inKeys::e_T; return true;
	case 0x15: o_Key = inKeys::e_Y; return true;
	case 0x16: o_Key = inKeys::e_U; return true;
	case 0x17: o_Key = inKeys::e_I; return true;
	case 0x18: o_Key = inKeys::e_O; return true;
	case 0x19: o_Key = inKeys::e_P; return true;
	case 0x1A: o_Key = inKeys::e_LBRACKET; return true;
	case 0x1B: o_Key = inKeys::e_RBRACKET; return true;
	case 0x1C: o_Key = inKeys::e_ENTER; return true;
	case 0x1D: o_Key = inKeys::e_LCTRL; return true;
	case 0x1E: o_Key = inKeys::e_A; return true;
	case 0x1F: o_Key = inKeys::e_S; return true;
	case 0x20: o_Key = inKeys::e_D; return true;
	case 0x21: o_Key = inKeys::e_F; return true;
	case 0x22: o_Key = inKeys::e_G; return true;
	case 0x23: o_Key = inKeys::e_H; return true;
	case 0x24: o_Key = inKeys::e_J; return true;
	case 0x25: o_Key = inKeys::e_K; return true;
	case 0x26: o_Key = inKeys::e_L; return true;
	case 0x27: o_Key = inKeys::e_SEMICOLON; return true;
	case 0x28: o_Key = inKeys::e_APOSTROPHE; return true;
	case 0x29: o_Key = inKeys::e_TILDE; return true;
	case 0x2A: o_Key = inKeys::e_LSHIFT; return true;
	case 0x2B: o_Key = inKeys::e_BACKSLASH; return true;
	case 0x2C: o_Key = inKeys::e_Z; return true;
	case 0x2D: o_Key = inKeys::e_X; return true;
	case 0x2E: o_Key = inKeys::e_C; return true;
	case 0x2F: o_Key = inKeys::e_V; return true;
	case 0x30: o_Key = inKeys::e_B; return true;
	case 0x31: o_Key = inKeys::e_N; return true;
	case 0x32: o_Key = inKeys::e_M; return true;
	case 0x33: o_Key = inKeys::e_COMMA; return true;
	case 0x34: o_Key = inKeys::e_PERIOD; return true;
	case 0x35: o_Key = inKeys::e_SLASH; return true;
	case 0x36: o_Key = inKeys::e_RSHIFT; return true;
	case 0x37: o_Key = inKeys::e_NUMPADMULTIPLY; return true;
	case 0x38: o_Key = inKeys::e_LALT; return true;
	case 0x39: o_Key = inKeys::e_SPACE; return true;
	case 0x3A: o_Key = inKeys::e_CAPSLOCK; return true;
	case 0x3B: o_Key = inKeys::e_F1; return true;
	case 0x3C: o_Key = inKeys::e_F2; return true;
	case 0x3D: o_Key = inKeys::e_F3; return true;
	case 0x3E: o_Key = inKeys::e_F4; return true;
	case 0x3F: o_Key = inKeys::e_F5; return true;
	case 0x40: o_Key = inKeys::e_F6; return true;
	case 0x41: o_Key = inKeys::e_F7; return true;
	case 0x42: o_Key = inKeys::e_F8; return true;
	case 0x43: o_Key = inKeys::e_F9; return true;
	case 0x44: o_Key = inKeys::e_F10; return true;
	// Windows reports Pause as 0x45 and Num Lock as extended 0x45
	case 0x45: o_Key = inKeys::e_PAUSE; return true;
	case 0x46: o_Key = inKeys::e_SCROLL; return true;
	case 0x47: o_Key = inKeys::e_NUMPAD7; return true;
	case 0x48: o_Key = inKeys::e_NUMPAD8; return true;
	case 0x49: o_Key = inKeys::e_NUMPAD9; return true;
	case 0x4A: o_Key = inKeys::e_NUMPADMINUS; return true;
	case 0x4B: o_Key = inKeys::e_NUMPAD4; return true;
	case 0x4C: o_Key = inKeys::e_NUMPAD5; return true;
	case 0x4D: o_Key = inKeys::e_NUMPAD6; return true;
	case 0x4E: o_Key = inKeys::e_NUMPADPLUS; return true;
	case 0x4F: o_Key = inKeys::e_NUMPAD1; return true;
	case 0x50: o_Key = inKeys::e_NUMPAD2; return true;
	case 0x51: o_Key = inKeys::e_NUMPAD3; return true;
	case 0x52: o_Key = inKeys::e_NUMPAD0; return true;
	case 0x53: o_Key = inKeys::e_NUMPADDECIMAL; return true;
	case 0x54: o_Key = inKeys::e_PRINT; return true;	// Alt+Print Screen (SysRq)
	case 0x57: o_Key = inKeys::e_F11; return true;
	case 0x58: o_Key = inKeys::e_F12; return true;

	case e_EXTENDED | 0x1C: o_Key = inKeys::e_NUMPADENTER; return true;
	case e_EXTENDED | 0x1D: o_Key = inKeys::e_RCTRL; return true;
	case e_EXTENDED | 0x35: o_Key = inKeys::e_NUMPADDIVIDE; return true;
	case e_EXTENDED | 0x36: o_Key = inKeys::e_RSHIFT; return true;
	case e_EXTENDED | 0x37: o_Key = inKeys::e_PRINT; return true;
	case e_EXTENDED | 0x38: o_Key = inKeys::e_RALT; return true;
	case e_EXTENDED | 0x45: o_Key = inKeys::e_NUMLOCK; return true;
	case e_EXTENDED | 0x47: o_Key = inKeys::e_HOME; return true;
	case e_EXTENDED | 0x48: o_Key = inKeys::e_UP; return true;
	case e_EXTENDED | 0x49: o_Key = inKeys::e_PAGEUP; return true;
	case e_EXTENDED | 0x4B: o_Key = inKeys::e_LEFT; return true;
	case e_EXTENDED | 0x4D: o_Key = inKeys::e_RIGHT; return true;
	case e_EXTENDED | 0x4F: o_Key = inKeys::e_END; return true;
	case e_EXTENDED | 0x50: o_Key = inKeys::e_DOWN; return true;
	case e_EXTENDED | 0x51: o_Key = inKeys::e_PAGEDOWN; return true;
	case e_EXTENDED | 0x52: o_Key = inKeys::e_INSERT; return true;
	case e_EXTENDED | 0x53: o_Key = inKeys::e_DELETE; return true;
	case e_EXTENDED | 0x5B: o_Key = inKeys::e_LWIN; return true;
	case e_EXTENDED | 0x5C: o_Key = inKeys::e_RWIN; return true;
	}

	return false;
}

}
