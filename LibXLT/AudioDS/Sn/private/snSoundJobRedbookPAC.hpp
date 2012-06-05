/****************************************************************************\
**  snSoundJobRedbookPAC.hpp
**
**      snSoundJobRedbookPAC.hpp forwards calls from the snSoundJobRedbook to the
**	correct PAC component.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef SN_SOUNDJOBREDBOOKPAC_HPP
#error snSoundJobRedbookPAC.hpp multiply included
#endif
#define SN_SOUNDJOBREDBOOKPAC_HPP

#ifndef ENV_PLATFORM_HPP
#include "Core/env/envPlatform.hpp"
#endif

//#ifndef SN_PLATFORM_HPP
//#include "AudioDS/sn/snPlatform.hpp"
//#endif

// PAC components should support the following interface:
/*
class snSoundJobRedbookPAC
{
		snSoundJobRedbookPAC();
		snSoundJobRedbookPAC( const snSoundJobRedbookPAC& i_CopyFrom );

		~snSoundJobRedbookPAC();

		virtual void Load();
		virtual void Free();
		virtual void Start();
		virtual void Start( const int i_TrackNum );
		virtual void Stop();
		virtual void Think( float i_SimulationTime );
		virtual void Pause();
		virtual void Resume();

		virtual int GetTrack();
		virtual void SetTrack( const int i_TrackNum );

		virtual int GetNumberOfTracks();
		virtual void NextTrack();
		virtual void PreviousTrack();

		bool	IsLoaded() const						{ return m_Flag.bLoaded; }
		void	SetLoaded( const bool Value )			{ m_Flag.bLoaded = Value; }

		bool	IsPaused() const						{ return m_Flag.bPaused; }
		void	SetPaused( const bool Value )			{ m_Flag.bPaused = Value; }

		bool	IsLooping() const							{ return m_Flag.bLooping; }
		void	SetLooping( const bool Value )				{ m_Flag.bLooping = Value; }
}*/


#if ENV_WINDOWS
	#include "AudioDS/sn/private/snSoundJobRedbookPACWin.hpp"
#else
	#if ENV_OS == ENV_XBOXOS
		#include "AudioDS/sn/private/snSoundJobRedbookPACXbox.hpp"
	#else
		#error snSoundJobRedbookPAC not defined for this platform
	#endif
#endif
