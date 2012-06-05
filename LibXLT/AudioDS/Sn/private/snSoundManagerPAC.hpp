/****************************************************************************\
**  snSoundManagerPAC.hpp
**
**      snSoundManagerPAC.hpp forwards calls from the snSoundManager to the
**	correct PAC component.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef SN_SOUNDMANAGERPAC_HPP
#error snSoundManagerPAC.hpp multiply included
#endif
#define SN_SOUNDMANAGERPAC_HPP

#ifndef ENV_PLATFORM_HPP
#include "Core/env/envPlatform.hpp"
#endif

//#ifndef SN_PLATFORM_HPP
//#include "AudioDS/sn/snPlatform.hpp"
//#endif


// PAC components should support the following interface:
/*
class snSoundManagerPAC
{
		snSoundManagerPAC();
		snSoundManagerPAC( const snSoundManagerPAC& i_CopyFrom );

		~snSoundManagerPAC();
}
*/


#if ENV_WINDOWS
	#include "AudioDS/sn/private/snSoundManagerPACWin.hpp"
#else
	#if ENV_OS == ENV_XBOXOS
		#include "AudioDS/sn/private/snSoundManagerPACXbox.hpp"
	#else
		#error snSoundManagerPAC not defined for this platform
	#endif
#endif
