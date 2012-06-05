/****************************************************************************\
**  appApplicationPAC.hpp
**
**      appApplicationPAC.hpp forwards calls from the appApplication to the
**	correct PAC component.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef APP_APPLICATIONPAC_HPP
#error appApplicationPAC.hpp multiply included
#endif
#define APP_APPLICATIONPAC_HPP

#ifndef ENV_PLATFORM_HPP
#include "Core/env/envPlatform.hpp"
#endif

// PAC components should support the following interface:
//
/*class appApplicationPAC
{
		appApplicationPAC();
		~appApplicationPAC();
		void Run(appApplication* i_App);
		void Exit();
		void SetWindowSize(int i_X, int i_Y, int i_Width, int i_Height);
		void SetWindowTitle(const itString& i_Title);
		static void Init();
		static void CleanUp() throw();
		void DialogLoopTasks();
		void CreateMainWindow();
}*/

#if ENV_WINDOWS
	#include "Core/app/private/appApplicationPACWin.hpp"
#else
	#if ENV_OS == ENV_PS2OS
		#include "Core/app/private/appApplicationPACPS2.hpp"
	#else
		#if ENV_OS == ENV_XBOXOS
			#include "Core/app/private/appApplicationPACXbox.hpp"
		#else
			#error appApplicationPAC not defined for this platform
		#endif
	#endif
#endif
