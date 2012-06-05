/****************************************************************************\
**  snSoundJobMP3PAC.hpp
**
**      snSoundJobMP3PAC.hpp forwards calls from the snSoundJobMP3 to the
**	correct PAC component.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef SN_SOUNDJOBMP3PAC_HPP
#error snSoundJobMP3PAC.hpp multiply included
#endif
#define SN_SOUNDJOBMP3PAC_HPP

#ifndef ENV_PLATFORM_HPP
#include "Core/env/envPlatform.hpp"
#endif

#ifndef SN_PLATFORM_HPP
#include "AudioDS/sn/snPlatform.hpp"
#endif

// PAC components should support the following interface:
/*
class snSoundJobMP3PAC
{
		snSoundJobMP3PAC();
		snSoundJobMP3PAC( const snSoundJobMP3PAC& i_CopyFrom );

		~snSoundJobMP3PAC();

		virtual void Load();
		virtual void Free();
		virtual void Start();
		virtual void Start( const int i_TrackNum );
		virtual void Stop();
		virtual void Think( float i_SimulationTime );
		virtual void Pause();
		virtual void Resume();

		void		SetLoops( const int i_Value );
		int			GetLoops() const;

		fsLocator &	GetFilename()
		void		SetFilename( fsLocator& )

		bool	IsLoaded() const			
		void	SetLoaded( const bool Value )

		bool	IsPaused() const			
		void	SetPaused( const bool Value )

		bool	IsLooping() const
		void	SetLooping( const bool Value )	
}*/


#if ENV_WINDOWS
	#if (SN_SOUNDSYSTEM == SN_MILES)
		#include "AudioDS/sn/private/snSoundJobMP3PACMiles.hpp"
	#elif SND_DSHOW
		#include "AudioDS/sn/private/snSoundJobMP3PACDShow.hpp"
	#else
		#include "AudioDS/sn/private/snSoundJobMP3PACXAudio.hpp"
	#endif
#else
	#if ENV_OS == ENV_XBOXOS
		#include "AudioDS/sn/private/snSoundJobMP3PACXbox.hpp"
	#else
		#error snSoundJobMP3PAC not defined for this platform
	#endif
#endif
