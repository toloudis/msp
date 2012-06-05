/****************************************************************************\
**  inDeviceMgrPAC.hpp
**
**      inDeviceMgrPAC.hpp forwards calls from the inDeviceMgr component to the
**	correct PAC component.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef IN_DEVICEMGRPAC_HPP
#error inDeviceMgrPAC.hpp multiply included
#endif
#define IN_DEVICEMGRPAC_HPP

#ifndef ENV_PLATFORM_HPP
#include "Core/env/envPlatform.hpp"
#endif

#if ENV_WINDOWS
	#include "InputDI/in/private/inDeviceMgrPACWin.hpp"
#else
	#if ENV_OS == ENV_PS2OS
		#include "InputDI/in/private/inDeviceMgrPACPS2.hpp"
	#else
		#if ENV_OS == ENV_XBOXOS
			#include "InputDI/in/private/inDeviceMgrPACXbox.hpp"
		#else
			#error inDeviceMgrPAC not defined for this platform
		#endif
	#endif
#endif
