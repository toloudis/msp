/****************************************************************************\
**  inKeyboardPAC.hpp
**
**      inKeyboardPAC.hpp forwards calls from the inKeyboard component to the
**	correct PAC component.
**
**	The PAC turns the platform's native scan codes into inKeys::Keys:
**
**	namespace inKeyboardPAC
**	{
**		// returns false for keys that inKeys doesn't have
**		bool NativeScanCodeToKey(unsigned int i_NativeScanCode, inKeys::Keys& o_Key);
**	}
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef IN_KEYBOARDPAC_HPP
#error inKeyboardPAC.hpp multiply included
#endif
#define IN_KEYBOARDPAC_HPP

#ifndef ENV_PLATFORM_HPP
#include "Core/env/envPlatform.hpp"
#endif

#if ENV_WINDOWS
	#include "Input/in/private/inKeyboardPACWin.hpp"
#else
	#error inKeyboardPAC not defined for this platform
#endif
