/****************************************************************************\
**  snSoundSystemPAC.hpp
**
**      snSoundSystemPAC.hpp forwards calls from the snSoundSystem to the
**	correct PAC component.
**
**	StudioGPU
**	Copyright(C) 2003-2 - All Rights Reserved
\****************************************************************************/

#ifdef SN_SOUNDSYSTEMPAC_HPP
#error snSoundSystemPAC.hpp multiply included
#endif
#define SN_SOUNDSYSTEMPAC_HPP

#ifndef ENV_PLATFORM_HPP
#include "Core/env/envPlatform.hpp"
#endif

#ifndef SN_PLATFORM_HPP
#include "AudioDS/sn/snPlatform.hpp"
#endif

// PAC components should support the following interface:
/*
{
		//========================================================================
		//	Initialize()
		//========================================================================
		void	Initialize();

		//========================================================================
		//	DeInitialize()
		//========================================================================
		void	DeInitialize();

		//========================================================================
		//	Don't call Init() and CleanUp() yourself; they are called 
		//	by the package Init and Cleanup.
		//========================================================================
		void Init();
		void CleanUp() throw();

		//========================================================================
		//	Initialized
		//========================================================================
		bool	IsInitialized() const					{ return snSoundSystemPAC::IsInitialized; }
		void	SetInitialized( const bool Value )		{ snSoundSystemPAC::SetInitialized( Value ); }
}*/

#if ENV_WINDOWS
	#if	(SN_SOUNDSYSTEM == SN_DIRECTSOUND)
		#include "AudioDS/sn/private/snSoundSystemPACDSound.hpp"
	#elif (SN_SOUNDSYSTEM == SN_MILES)
		#include "AudioDS/sn/private/snSoundSystemPACMiles.hpp"
	#else
		#error snSoundSystemPAC not defined for this platform
	#endif
#else
	#if ENV_OS == ENV_XBOXOS
		#include "AudioDS/sn/private/snSoundSystemPACXbox.hpp"
	#else
		#error snSoundSystemPAC not defined for this platform
	#endif
#endif
