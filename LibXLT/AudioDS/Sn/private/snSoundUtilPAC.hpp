/****************************************************************************\
**  snSoundUtilPAC.hpp
**
**      snSoundUtilPAC.hpp forwards calls from the snSoundUtil to the
**	correct PAC component.
**
**	StudioGPU
**	Copyright(C) 2003-2 - All Rights Reserved
\****************************************************************************/

#ifdef SN_SOUNDUTILPAC_HPP
#error snSoundUtilPAC.hpp multiply included
#endif
#define SN_SOUNDUTILPAC_HPP

#ifndef ENV_PLATFORM_HPP
#include "Core/env/envPlatform.hpp"
#endif

#ifndef SN_PLATFORM_HPP
#include "AudioDS/sn/snPlatform.hpp"
#endif


// PAC components should support the following interface:
//
/*
namespace snSoundUtilPAC
{
	int	ConvertVolume( float i_SoundPercent );
	int	ConvertPan( float i_SoundPercent );
	int	ConvertFrequency( float i_SoundPercent, float i_SoundFrequency );

	void Init();
	void CleanUp() throw();
}
*/

#if ENV_WINDOWS
	#if	(SN_SOUNDSYSTEM == SN_DIRECTSOUND)
		#include "AudioDS/sn/private/snSoundUtilPACDSound.hpp"
	#elif (SN_SOUNDSYSTEM == SN_MILES)
		#include "AudioDS/sn/private/snSoundUtilPACMiles.hpp"
	#else
		#error snSoundUtilPAC not defined for this platform
	#endif
#else
	#if ENV_OS == ENV_XBOXOS
		#include "AudioDS/sn/private/snSoundUtilPACXbox.hpp"
	#else
		#error snSoundUtilPAC not defined for this platform
	#endif
#endif

