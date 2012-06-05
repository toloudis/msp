/****************************************************************************\
**  inKeyboardPAC.hpp
**
**      inKeyboardPAC.hpp forwards calls from the inKeyboard component to the
**	correct PAC component.
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
	#include "InputDI/in/private/inKeyboardPACWin.hpp"
#else
	#if ENV_OS == ENV_PS2OS
		#include "InputDI/in/private/inKeyboardPACPS2.hpp"
	#else
		#if ENV_OS == ENV_XBOXOS
			#include "InputDI/in/private/inKeyboardPACXbox.hpp"
		#else
			#error inKeyboardPAC not defined for this platform
		#endif
	#endif
#endif
