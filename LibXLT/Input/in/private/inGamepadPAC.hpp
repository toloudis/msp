/****************************************************************************\
**  inGamepadPAC.hpp
**
**      inGamepadPAC.hpp forwards calls from the inGamepad component to the
**	correct PAC component.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef IN_GAMEPADPAC_HPP
#error inGamepadPAC.hpp multiply included
#endif
#define IN_GAMEPADPAC_HPP

#ifndef ENV_PLATFORM_HPP
#include "Core/env/envPlatform.hpp"
#endif

#if ENV_WINDOWS
	#include "InputDI/in/private/inGamepadPACWin.hpp"
#else
	#if ENV_OS == ENV_PS2OS
		#include "InputDI/in/private/inGamepadPACPS2.hpp"
	#else
		#if ENV_OS == ENV_XBOXOS
			#include "InputDI/in/private/inGamepadPACXbox.hpp"
		#else
			#error inGamepadPAC not defined for this platform
		#endif
	#endif
#endif
