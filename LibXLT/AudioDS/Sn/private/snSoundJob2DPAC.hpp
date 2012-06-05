/****************************************************************************\
**  snSoundJob2DPAC.hpp
**
**      snSoundJob2DPAC.hpp forwards calls from the snSoundJob2D to the
**	correct PAC component.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef SN_SOUNDJOB2DPAC_HPP
#error snSoundJob2DPAC.hpp multiply included
#endif
#define SN_SOUNDJOB2DPAC_HPP

#ifndef ENV_PLATFORM_HPP
#include "Core/env/envPlatform.hpp"
#endif

#ifndef SN_PLATFORM_HPP
#include "AudioDS/sn/snPlatform.hpp"
#endif

// PAC components should support the following interface:
/*
class snSoundJob2DPAC
{
		snSoundJob2DPAC();
		snSoundJob2DPAC( const snSoundJob2DPAC& i_CopyFrom );

		~snSoundJob2DPAC();

		void	Load();
		void	Free();
		void	Reload();
		void	Unload();
		virtual void	Start( float i_SimulationTime );
		virtual void	Stop();
		virtual void Think( float i_SimulationTime );
		virtual void ThinkEffects( float i_SimulationTime );
		virtual void Pause();
		virtual void Resume( float i_SimulationTime );

		void SetFadeTo( float i_TargetLevel, float i_Seconds,  float i_SimulationTime );
		void AddToFade( float i_TargetLevel, float i_Seconds,  float i_SimulationTime );

		void SetPanTo( float i_TargetLevel, float i_Seconds,  float i_SimulationTime );
		void AddToPan( float i_TargetLevel, float i_Seconds,  float i_SimulationTime );

		void SetFrequencyTo( float i_TargetLevel, float i_Seconds,  float i_SimulationTime );
		void AddToFrequency( float i_TargetLevel, float i_Seconds,  float i_SimulationTime );

		inline void		SetFilename( const fsLocator& Filename );
		inline fsLocator &	GetFilename() const;

		void		SetVolume( const float i_NewVolume );
		float		GetVolume() const;

		void		SetVolumeFactor( const float i_Value );
		float		GetVolumeFactor() const;

		void		SetVolumeTypeFactor( const float i_Value );
		float		GetVolumeTypeFactor() const;

		void		SetVolumeGlobalFactor( const float i_Value );
		float		GetVolumeGlobalFactor() const;

		void		SetFrequency( float i_NewFrequency );
		float		GetFrequency() const;

		void		SetPan( float i_NewPan );
		float		GetPan() const;

		void		SetFadeFactor( float Value );
		float		GetFadeFactor() const;

		void		SetLoops( const int Value );
		int			GetLoops() const;

		void		SetLoopCurrent( const int Value );
		int			GetLoopCurrent() const;

		void			SetPosition( const maVector3d& Value );
		maVector3d		GetPosition() const;
		void			SetListenerPosition( const maVector3d& i_Position );
		maVector3d		GetListenerPosition() const;
		void			SetListenerOrientation( const maRotation& i_Value );
		maRotation		GetListenerOrientation() const;
		void			SetSoundDistance( float i_Distance );
		float			GetSoundDistance() const;
		void			SetFalloffDistance( float i_Distance );
		float			GetFalloffDistance() const;

		bool	IsLoaded() const					
		void	SetLoaded( const bool Value )		

		bool	IsPaused() const					
		void	SetPaused( const bool Value )		

		bool	IsFinished() const					
		void	SetFinished( const bool Value )		

		bool	IsLooping() const					
		void	SetLooping( const bool Value )		

		bool	IsFrequencyChanging() const			
		void	SetFrequencyChanging( const bool Value )	

		bool	IsPanChanging() const						
		void	SetPanChanging( const bool Value )			

		bool	IsFadeChanging() const						
		void	SetFadeChanging( const bool Value )			

		bool	IsStopOnFadeFinish() const					
		void	SetStopOnFadeFinish( const bool Value )		

		bool	IsFreeOnFadeFinish() const					
		void	SetFreeOnFadeFinish( const bool Value )		

		bool	IsDeleteWhenFinished() const					
		void	SetDeleteWhenFinished( const bool Value )		

		bool	IsLocalize() const							
		void	SetLocalize( const bool Value )				

		float SetPositionInSoundByPercent( float i_Percent );
		float GetPositionInSoundByPercent();
		float GetTimeLength();
}*/


#if ENV_WINDOWS
	#if	(SN_SOUNDSYSTEM == SN_DIRECTSOUND)
		#include "AudioDS/sn/private/snSoundJob2DPACDSound.hpp"
	#elif (SN_SOUNDSYSTEM == SN_MILES)
		#include "AudioDS/sn/private/snSoundJob2DPACMiles.hpp"
	#else
		#error snSoundJob2DPAC not defined for this platform
	#endif
#else
	#if ENV_OS == ENV_XBOXOS
		#include "AudioDS/sn/private/snSoundJob2DPACXbox.hpp"
	#else
		#error snSoundJob2DPAC not defined for this platform
	#endif
#endif
