/****************************************************************************\
**  inMousePAC.hpp
**
**      inMousePAC.hpp forwards calls from the inMouse component to the
**	correct PAC component.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef IN_MOUSEPAC_HPP
#error inMousePAC.hpp multiply included
#endif
#define IN_MOUSEPAC_HPP

#ifndef ENV_PLATFORM_HPP
#include "Core/env/envPlatform.hpp"
#endif

#if ENV_WINDOWS
	#include "InputDI/in/private/inMousePACWin.hpp"
#else
	#if ENV_OS == ENV_PS2OS
		#include "InputDI/in/private/inMousePACPS2.hpp"
	#else
		#if ENV_OS == ENV_XBOXOS
			#include "InputDI/in/private/inMousePACXbox.hpp"
		#else
			#error inMousePAC not defined for this platform
		#endif
	#endif
#endif
