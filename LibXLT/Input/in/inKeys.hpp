/*****************************************************************************
**  inKeys.hpp
**
**      inKeys defines a namespace inKeys to hold an enumeration of terawatt
**		keys.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef IN_KEYS_HPP
#error inKeys.hpp multiply included
#endif
#define IN_KEYS_HPP

#ifndef IT_STRING_HPP
#include "Core/it/itString.hpp"
#endif

namespace inKeys
{

enum Keys
{
	e_A = 0,
	e_B,
	e_C,
	e_D,
	e_E,
	e_F,
	e_G,
	e_H,
	e_I,
	e_J,
	e_K,
	e_L,
	e_M,
	e_N,
	e_O,
	e_P,
	e_Q,
	e_R,
	e_S,
	e_T,
	e_U,
	e_V,
	e_W,
	e_X,
	e_Y,
	e_Z,
	e_0,
	e_1,
	e_2,
	e_3,
	e_4,
	e_5,
	e_6,
	e_7,
	e_8,
	e_9,
	e_ESC,
	e_F1,
	e_F2,
	e_F3,
	e_F4,
	e_F5,
	e_F6,
	e_F7,
	e_F8,
	e_F9,
	e_F10,
	e_F11,
	e_F12,
	e_PRINT,
	e_SCROLL,
	e_PAUSE,
	e_TILDE,
	e_DASH,
	e_EQUALS,
	e_BACKSPACE,
	e_TAB,
	e_LBRACKET,
	e_RBRACKET,
	e_BACKSLASH,
	e_CAPSLOCK,
	e_SEMICOLON,
	e_APOSTROPHE,
	e_ENTER,
	e_LSHIFT,
	e_COMMA,
	e_PERIOD,
	e_SLASH,
	e_RSHIFT,
	e_LCTRL,
	e_LWIN,
	e_LALT,
	e_SPACE,
	e_RALT,
	e_RWIN,
	e_RCTRL,
	e_INSERT,
	e_HOME,
	e_PAGEUP,
	e_DELETE,
	e_END,
	e_PAGEDOWN,
	e_UP,
	e_DOWN,
	e_LEFT,
	e_RIGHT,
	e_NUMPAD0,
	e_NUMPAD1,
	e_NUMPAD2,
	e_NUMPAD3,
	e_NUMPAD4,
	e_NUMPAD5,
	e_NUMPAD6,
	e_NUMPAD7,
	e_NUMPAD8,
	e_NUMPAD9,
	e_NUMLOCK,
	e_NUMPADDIVIDE,
	e_NUMPADMULTIPLY,
	e_NUMPADMINUS,
	e_NUMPADPLUS,
	e_NUMPADDECIMAL,
	e_NUMPADENTER,
	e_NUMTERAWATTKEYS	//this should always be last, to facilitate looping over the keyboard
};

extern itString TerawattKeyStrings[e_NUMTERAWATTKEYS];
}