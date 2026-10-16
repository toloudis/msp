/****************************************************************************\
**  inKeyboardPACWin.hpp
**
**      inKeyboardPACWin.hpp defines the Windows PAC component of inKeyboard.
**	Native scan codes are the set 1 scan codes Windows puts in bits 16-23
**	of a key message's lParam, plus 0x100 when the extended key bit (24)
**	is set. This is what Qt's QKeyEvent::nativeScanCode() returns.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef IN_KEYBOARDPACWIN_HPP
#error inKeyboardPACWin.hpp multiply included
#endif
#define IN_KEYBOARDPACWIN_HPP

#ifndef IN_KEYS_HPP
#include "Input/in/inKeys.hpp"
#endif

namespace inKeyboardPAC
{
	//========================================================================
	//	NativeScanCodeToKey returns false for keys that inKeys doesn't have
	//========================================================================
	bool NativeScanCodeToKey(unsigned int i_NativeScanCode, inKeys::Keys& o_Key);
}
