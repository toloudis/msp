/****************************************************************************\
**  snSoundJob2DStreamedPAC.hpp
**
**      snSoundJob2DStreamedPAC.hpp forwards calls from the snSoundJob2D to the
**	correct PAC component.
**
**	StudioGPU
**	Copyright(C) 2003-2 - All Rights Reserved
\****************************************************************************/

#ifdef SN_SOUNDJOB2DSTREAMEDPAC_HPP
#error snSoundJob2DStreamedPAC.hpp multiply included
#endif
#define SN_SOUNDJOB2DSTREAMEDPAC_HPP

#ifndef ENV_PLATFORM_HPP
#include "Core/env/envPlatform.hpp"
#endif

#ifndef SN_PLATFORM_HPP
#include "AudioDS/sn/snPlatform.hpp"
#endif

// PAC components should support the following interface:
/*
class snSoundJob2DStreamedPAC
{
		virtual void Start( float i_SimulationTime );
		virtual void Restart( float i_SimulationTime );
		virtual void Think( float i_SimulationTime );
		virtual void Pause();
		virtual void Resume( float i_SimulationTime );
		virtual void Reload();
}*/


#if ENV_WINDOWS
	#if	(SN_SOUNDSYSTEM == SN_DIRECTSOUND)
		#include "AudioDS/sn/private/snSoundJob2DStreamedPACDSound.hpp"
	#elif (SN_SOUNDSYSTEM == SN_MILES)
		#include "AudioDS/sn/private/snSoundJob2DStreamedPACMiles.hpp"
	#else
		#error snSoundJob2DStreamedPAC not defined for this platform
	#endif
#else
	#if ENV_OS == ENV_XBOXOS
		#include "AudioDS/sn/private/snSoundJob2DStreamedPACXbox.hpp"
	#else
		#error snSoundJob2DStreamedPAC not defined for this platform
	#endif
#endif
