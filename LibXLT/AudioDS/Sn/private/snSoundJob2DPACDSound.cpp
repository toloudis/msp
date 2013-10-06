/****************************************************************************\
**	snSoundJob2DPACDSound.cpp
**
**		snSoundJob2DPACDSound.cpp implements the DirectSound portion of the
**	snSoundJob2DPAC.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "AudioDS/sn/private/snSoundJob2DPACDSound.hpp"

#include "AudioDS/sn/private/snDSoundGlobalWin.hpp"
#include "AudioDS/sn/private/snSoundSystemPAC.hpp"
#include "AudioDS/sn/private/snSoundUtilPAC.hpp"
#include "AudioDS/sn/snExceptionX.hpp"
#include "AudioDS/sn/snSoundSystem.hpp"

#include "Core/app/appTime.hpp"
#include "Core/gf/gfFileBin.hpp"

#include <iomanip>
#include <string>
#include <dsound.h>


//========================================================================
//	library pragmas
//========================================================================
#pragma comment(lib,"dsound.lib")


//------------------------------------------------------------------------
//	default and copy constructors
//------------------------------------------------------------------------
snSoundJob2DPAC::snSoundJob2DPAC()
{
	Init();
}


//------------------------------------------------------------------------
//------------------------------------------------------------------------
snSoundJob2DPAC::snSoundJob2DPAC( const snSoundJob2DPAC& i_CopyFrom )
{
	*this = i_CopyFrom;
}


//------------------------------------------------------------------------
//------------------------------------------------------------------------
snSoundJob2DPAC::~snSoundJob2DPAC()
{
	Free();
}


//------------------------------------------------------------------------
//	Load()
//------------------------------------------------------------------------
void	
snSoundJob2DPAC::Load()
{
	if ( !IsLoaded() )
	{
		DBG_ASSERT( snSoundSystem::IsInitialized(), "Sound system is not initialized" );

		CreateBuffer();

		if ( m_pBuffer )
		{
			SetLoaded( true );

			//	Grab the 'original' frequency
			//
			m_pBuffer->GetFrequency( &m_FrequencyOriginal );
		}
	}
}


//------------------------------------------------------------------------
//	Free()
//------------------------------------------------------------------------
void	
snSoundJob2DPAC::Free()
{
	if ( IsLoaded() )
	{
		if ( m_pBuffer )
		{
			m_pBuffer->Stop();
			m_pBuffer->Release();
			m_pBuffer = NULL;
		}

		SetLoaded( false );
	}
	else
		DBG_ASSERT(m_pBuffer == NULL, "Buffer should be deallocated by now");
}


//------------------------------------------------------------------------
//	Reload()
//
//		Reload the raw sound data
//------------------------------------------------------------------------
void	
snSoundJob2DPAC::Reload()
{
	if ( !IsLoaded() )
	{
		this->Load();
		this->SetShouldStart(true);
		SetVolumeChanged(true);
	}
}


//------------------------------------------------------------------------
//	Unload()
//
//		Pause the sound and unload the raw sound data
//------------------------------------------------------------------------
void	
snSoundJob2DPAC::Unload(bool i_bReloadOnStart/* = false*/)
{
	if ( IsLoaded() )
	{
		this->Free();
		this->Pause();
	}
}


//------------------------------------------------------------------------
//	Start()
//
//	Start playing a sound
//
//		1. make sure everything's ready
//		2. start the sound playing (looped or not)
//		3. the Think() function takes care of it from there
//------------------------------------------------------------------------
void 
snSoundJob2DPAC::Start( float i_SimulationTime )
{
	//	verify everything is ready to go
	//
	if (	( !IsLoaded() )
		||	( !snSoundSystem::IsInitialized() ) )
	{
		return;
	}

	//	Set a flag so we will start next Think
	if ( m_pBuffer )
	{
		SetPaused( false );
		SetFinished( false );
		this->SetStartTime( i_SimulationTime );
		this->SetShouldStart( true );
	}
}


//------------------------------------------------------------------------
//	Restart()
//
//	Restart playing a sound
//------------------------------------------------------------------------
void	
snSoundJob2DPAC::Restart( float i_SimulationTime )
{
	SetRestorePosition( 0 );
	Start( i_SimulationTime );
}


//------------------------------------------------------------------------
//	Stop()
//------------------------------------------------------------------------
void	
snSoundJob2DPAC::Stop()
{
	if (	( !IsLoaded() )
		||	( !snSoundSystem::IsInitialized() ) )
	{
		return;
	}

	if ( m_pBuffer )
	{
		HRESULT			Error;

		Error = m_pBuffer->Stop();
		if (Error)
		{
			snDSoundGlobal::PrintDSError( Error );
		}

		if ( Error = m_pBuffer->GetStatus( &m_Status ) )
		{
			snDSoundGlobal::PrintDSError( Error );
		}

		SetFinished( true );

		//	If we are supposed to delete it when it's done all we can do
		//	is free it up and set the flag saying we are finished.
		//
		if ( IsDeleteWhenFinished() )
		{
			Free();
		}

		m_RestorePosition	= 0;
		m_Flag.bShouldStart = false;
	}
}


//------------------------------------------------------------------------
//	Think()
//
//	Take care of a sound that is active each game loop.
//
//		1. check if it is finished or not (it may be on a sound list
//		   somewhere)
//		2. check if everything is set up correctly
//		3. check on the status
//		4. based on the status do things
//		5. check to see if we have a time limit
//		6. do FadeChanging (if applicable)
//		7. do PanChanging (if applicable)
//		7. do FrequencyChanging (if applicable)
//		8. do other effects (if applicable)
//------------------------------------------------------------------------
void	
snSoundJob2DPAC::Think( float i_SimulationTime )
{
	if ( IsFinished() )
	{
		return;
	}

	if (	!m_pBuffer
		||	( !snSoundSystem::IsInitialized() ) )
	{
		Stop();
		return;
	}

	//	Everything, is ok so far...
	//
	HRESULT			Error;

	if ( Error = m_pBuffer->GetStatus( &m_Status ) )
	{
		snDSoundGlobal::PrintDSError( Error );
		return;
	}

	// if we lost our directsound buffer, restore it
	//
	if ( m_Status & DSBSTATUS_BUFFERLOST )
	{
		m_pBuffer->Release();
		m_pBuffer = NULL;
		CreateBuffer();

		Start( i_SimulationTime );
	}
	else
	if ( !m_Status && !IsPaused() && !m_Flag.bShouldStart )
	{
		// if we got a non-playing status and we're not paused, kill the sound
		//
		Stop();
		return;
	}

	unsigned long status;
	m_pBuffer->GetStatus(&status);
	
	//	If the sound has played long enough, then stop it
	//
	if ( m_PlayTime > 0.0f )
	{
		// we need to query the buffer directly because we can not count on the 
		// m_StartTime to be correct
		if ( !IsShouldStart() && !IsPaused() && (status & DSBSTATUS_PLAYING) )
//		if ( (i_SimulationTime - m_StartTime) > m_PlayTime )
		{
			//	We have played long enough
			//
			Stop();
			return;
		}
	}

	//	If some function has updated some aspect of the volume, 
	//	recalculate it here.
	//
	if ( IsVolumeChanged() )
	{
		SetVolume( GetVolume() );
	}

	//	Do the effects
	//
	ThinkEffects( i_SimulationTime );

	//	Localize the effect
	//
	if ( this->IsLocalize() )
	{
		ThinkLocalize( i_SimulationTime );
	}

	//	Start the sound, if necessary.
	//	We do this in the Think so that the various volumes and so on will be set correctly
	//	BEFORE any of the sound is played.
	//
	if ( m_pBuffer && m_Flag.bShouldStart )
	{
		HRESULT			Error;

		//	Play the sound
		//
		m_pBuffer->SetCurrentPosition( m_RestorePosition );
		while ( Error = m_pBuffer->Play( 0, 0, (IsLooping() ? DSBPLAY_LOOPING : 0) ) )
		{
			snDSoundGlobal::PrintDSError( Error );

			// if the buffer was lost, recreate and reload it
			//
			if ( Error == DSERR_BUFFERLOST )
			{
				m_pBuffer->Release();
				m_pBuffer = NULL;
				CreateBuffer();
			}
			else
			{
				return;
			}
		}

		this->SetStartTime( i_SimulationTime );

		SetPaused( false );
		SetFinished( false );
	
		m_Flag.bShouldStart = false;
	}
}


//------------------------------------------------------------------------
//	ThinkEffects()
//
//	Take care of a sound's effects that is active each game loop.
//
//		1. do FadeChanging (if applicable)
//		2. do PanChanging (if applicable)
//		3. do FrequencyChanging (if applicable)
//		4. do other effects (if applicable)
//------------------------------------------------------------------------
void	
snSoundJob2DPAC::ThinkEffects( float i_SimulationTime )
{
	// update the volume with the result of a linterp of time to volume
	//
	if ( IsFadeChanging() )
	{
		if (   ( m_VolumeFactor == m_FadeFactor )
			|| ( ( m_FadeStartTime + m_FadeTime ) < i_SimulationTime ) )
		{
			// This fading is done.
			//
			SetFadeChanging( false );
			m_VolumeFactor	= m_FadeFactor;

			//	Officially set the volume
			//
			SetVolume( GetVolume() );

			//	if flag is set, Stop the sound
			//
			if (   ( IsStopOnFadeFinish() )
				|| ( IsFreeOnFadeFinish() ) )
			{
				//	if flag is set, Free the sound
				//
				if ( IsFreeOnFadeFinish() )
				{
					SetDeleteWhenFinished( true );
				}

				Stop();

				return;
			}
		}
		else
		{
			//	calculate the elapsed time so far
			//
			float elapsed = i_SimulationTime - m_FadeStartTime;

			//DBG_ASSERT( elapsed >= 0.0f, "elapsed time shouldn't be negative" );

			//	Set the volume factor based on this time
			//
			float timeRatio;
			
			if ( m_FadeTime == 0.0f )
			{
				timeRatio = 1.0f;
			}
			else
			{
				timeRatio = ( elapsed / m_FadeTime );

				if ( timeRatio > 1.0f ) timeRatio = 1.0f;
			}

			m_VolumeFactor = m_FadeStartFactor 
				+ ( (m_FadeFactor - m_FadeStartFactor) * timeRatio );

			//	Officially set the volume
			//
			SetVolume( GetVolume() );
		}

		//	if the fade has been set (or reset) adjust the values
		//
		if ( m_bNewFadeSet )
		{
			m_FadeStartFactor	= m_VolumeFactor;
			m_FadeStartTime		= i_SimulationTime;

			m_FadeFactor		= m_NewFadeFactor;
			m_FadeTime			= m_NewFadeTime;
			m_bNewFadeSet		= false;
		}
	}

	// update the pan position with the result of a linterp of time to pan position
	//
	if ( IsPanChanging() )
	{
		if (   ( m_Pan == m_PanPosition )
			|| ( ( m_PanStartTime + m_PanTime ) < i_SimulationTime ) )
		{
			// This sound is done.
			//
			SetPanChanging( false );
			m_Pan	= m_PanPosition;

			//	Now officially set the pan
			//
			SetPan( GetPan() );
		}
		else
		{
			//	calculate the elapsed time so far
			//
			float elapsed = i_SimulationTime - m_PanStartTime;

			//	Set the volume factor based on this time
			//
			if ( m_PanTime > 0 )
			{
				m_Pan	= m_PanStartPosition
					+ ( (m_PanPosition - m_PanStartPosition) * ( elapsed / m_PanTime ) );
			}
			else
			{
				m_Pan = m_PanPosition;
			}

			//	Officially set the pan
			//
			SetPan( GetPan() );
		}
	}

	// update the frequency with the result of a linterp of time to frequency
	//
	if ( IsFrequencyChanging() )
	{
		if (   ( m_FrequencyFactor == m_FrequencyTargetFactor )
			|| ( ( m_FrequencyStartTime + m_FrequencyTime ) < i_SimulationTime ) )
		{
			// This sound is done.
			//
			SetFrequencyChanging( false );
			m_FrequencyFactor	= m_FrequencyTargetFactor;

			//	Officially set the Frequency
			//
			SetFrequency( GetFrequency() );
		}
		else
		{
			//	calculate the elapsed time so far
			//
			float elapsed;
			elapsed = i_SimulationTime - m_FrequencyStartTime;

			if ( m_FrequencyTime > 0.0f )
			{
				//	Set the Frequency factor based on this time
				//
				m_FrequencyFactor = m_FrequencyStartFactor 
					+ ( (m_FrequencyTargetFactor - m_FrequencyStartFactor) * ( elapsed / m_FrequencyTime ) );

				if ( m_FrequencyFactor < 0.0f )
					m_FrequencyFactor = 0.0f;

				//	Officially set the Frequency
				//
				SetFrequency( GetFrequency() );
			}
			else
			{
				//	Officially set the Frequency
				//
				m_FrequencyFactor = m_FrequencyTargetFactor;
				SetFrequency( GetFrequency() );
			}
		}
	}
}


//------------------------------------------------------------------------
//	ThinkLocalize()														
//																		
//		Adjusts volume and pan of sound based on sound position
//	and (usually the camera) listener position.
//------------------------------------------------------------------------
void 
snSoundJob2DPAC::ThinkLocalize( float i_SimulationTime )
{
	//	Note:
	//	
	//		i_ListenerPos	- position of the camera or player
	//		m_Position		- position of the sound
	//
	maVector3d	diff;
	diff = m_ListenerPosition - m_Position;

	float	dist	= diff.Length();

	//	if the listener is outside the area this sound is heard, then turn it off.
	//
	if ( dist >= m_FalloffDistance )
	{
		m_VolumeLocalizeFactor = 0.0f;	// 0.0 to 1.0

		//	Officially set the volume
		//
		SetVolume( GetVolume() );
		return;
	}

	//	Calculate the 
	// Based on the distance:
	//	if the sound is within the sounddistance then play at full volume
	//	if the sound is between sounddistance and falloffdistance, play the sound scaled
	//	if the sound is outside of the falloffdistance, the sound is silent.
	//
	if ( dist <= m_SoundDistance )
	{
		m_VolumeLocalizeFactor = 1.0f;
	}
	else
	{
		float deltaFalloff	= m_FalloffDistance - m_SoundDistance;
		float deltaSound	= dist - m_SoundDistance;
		
		m_VolumeLocalizeFactor = (deltaFalloff - deltaSound) / deltaFalloff;
	}

	//	Now that the volume is set based on distance, calculate
	//	the pan based on direction.
	//
	if (!dist || !IsLocalize2D())
	{
		//handle this special case of diff = 0 to prevent divbyzero error
		this->SetPanTo(0.0f, 0.0f, i_SimulationTime);
	}
	else
	{
		diff /= dist;
		m_ListenerOrientation.RotateVector(diff);
		float right_val = diff.m_X;

		if ( right_val < -1.0f ) right_val = -1.0f;
		if ( right_val > 1.0f ) right_val = 1.0f;

		if( right_val < 0 )
			right_val *= -right_val;
		else
			right_val *= right_val;

		this->SetPanTo(right_val, 0.0f, i_SimulationTime);
	}

	//	Officially set the volume
	//
	SetVolume( GetVolume() );

	//	Officially set the pan
	//
	SetPan( GetPan() );
}


//------------------------------------------------------------------------
//	Pause()
//------------------------------------------------------------------------
void	
snSoundJob2DPAC::Pause()
{
	if (	( !IsLoaded() )
		||	( IsPaused() )
		||	( !snSoundSystem::IsInitialized() ) )
	{
		return;
	}

	if ( m_pBuffer )
	{
		HRESULT			Error;

		Error = m_pBuffer->Stop();
		if (Error)
		{
			snDSoundGlobal::PrintDSError( Error );
		}

		//	Save the current position
		//
		m_pBuffer->GetCurrentPosition( &m_RestorePosition, NULL );

		SetPaused( true );
	}
}


//------------------------------------------------------------------------
//	Resume()
//------------------------------------------------------------------------
void	
snSoundJob2DPAC::Resume( float i_SimulationTime )
{
	if (	( !IsLoaded() )
		||	( !IsPaused() )
		||	( !snSoundSystem::IsInitialized() ) )
	{
		return;
	}

	//	Start up the sound
	//
	Start( i_SimulationTime );
}


//------------------------------------------------------------------------
//	SetFadeTo()
//
//		Fade the sound to a specific level.  This level is a percentage
//	from 0.0 to 1.0.
//------------------------------------------------------------------------
void 
snSoundJob2DPAC::SetFadeTo( float i_TargetLevel, float i_Seconds, float i_SimulationTime )
{
	DBG_ASSERT( ((i_TargetLevel >= 0.0f) && (i_TargetLevel <= 1.0f)), "Target Level out of range - " << i_TargetLevel );
	if ( i_TargetLevel < 0.0f ) i_TargetLevel = 0.0f;
	if ( i_TargetLevel > 1.0f ) i_TargetLevel = 1.0f;

	if ((i_Seconds == 0.0f) && (i_SimulationTime == 0.0f))
	{
		m_VolumeFactor		= i_TargetLevel;
		m_FadeStartFactor	= m_VolumeFactor;
		m_FadeFactor		= i_TargetLevel;
		m_FadeTime			= i_Seconds;
		m_FadeStartTime		= i_SimulationTime;
		m_NewFadeFactor		= i_TargetLevel;
		m_NewFadeTime		= i_Seconds;
		SetFadeChanging( false );
		return;
	}

	if ( !IsFadeChanging() )
	{
		//	if not currently doing a fade change, then set the actual parameters so
		//	the fade will happen right away.
		//
		m_FadeStartFactor	= m_VolumeFactor;
		m_FadeFactor		= i_TargetLevel;
		m_FadeTime			= i_Seconds;
		m_FadeStartTime		= i_SimulationTime;
	}
	else
	{
		//	if already fading then store the new fade values and wait until the
		//	next fade calculation until applying.
		//
		m_NewFadeFactor		= i_TargetLevel;
		m_NewFadeTime		= i_Seconds;
		m_bNewFadeSet		= true;

		//DBG_LOG4( "StartFactor=%.2f   StartTime=%.2f   FadeTime=%.2f   FadeFactor=%.2f", 
		//	m_FadeStartFactor,
		//	m_FadeStartTime,
		//	m_FadeTime,
		//	m_FadeFactor );
	}

	SetFadeChanging( true );
}


//------------------------------------------------------------------------
//	AddToFade()
//
//		Add a value to the Fade.  This value cannot exceed the Fade
//	percentage from 0.0 to 1.0.
//------------------------------------------------------------------------
void 
snSoundJob2DPAC::AddToFade( float i_TargetLevel, float i_Seconds, float i_SimulationTime )
{
	DBG_ASSERT( ((i_TargetLevel >= 0.0f) && (i_TargetLevel <= 1.0f)), "Target Level out of range - " << i_TargetLevel );

	SetFadeChanging( true );

	m_FadeStartFactor	= m_VolumeFactor;
	m_FadeFactor		= m_FadeFactor + i_TargetLevel;
	if ( m_FadeFactor > 1.0f ) m_FadeFactor = 1.0f;
	if ( m_FadeFactor < 0.0f ) m_FadeFactor = 0.0f;
	m_FadeTime			= i_Seconds;
	m_FadeStartTime		= i_SimulationTime;
}


//------------------------------------------------------------------------
//	SetPanTo()
//
//		Pan the sound to a specific channel over time.  This channel is a
//	percentage from -1.0 (all left) to 0.0 (center) to 1.0 (all right).
//------------------------------------------------------------------------
void 
snSoundJob2DPAC::SetPanTo( float i_TargetLevel, float i_Seconds, float i_SimulationTime )
{
	DBG_ASSERT( ((i_TargetLevel >= -1.0f) && (i_TargetLevel <= 1.0f)), "Target Level out of range - " << i_TargetLevel );
	if ( i_TargetLevel < -1.0f ) i_TargetLevel = -1.0f;
	if ( i_TargetLevel >  1.0f ) i_TargetLevel = 1.0f;

	SetPanChanging( true );

	m_PanStartPosition	= m_Pan;
	m_PanPosition		= i_TargetLevel;
	if ( m_PanPosition >  1.0f ) m_PanPosition =  1.0f;
	if ( m_PanPosition < -1.0f ) m_PanPosition = -1.0f;
	m_PanTime			= i_Seconds;
	m_PanStartTime		= i_SimulationTime;

	if( i_Seconds = 0.0f )
		m_Pan = i_TargetLevel;
}


//------------------------------------------------------------------------
//	AddToPan()
//
//		Add a value to the Pan.  This value cannot exceed the pan
//	percentage from -1.0 to 1.0.
//------------------------------------------------------------------------
void 
snSoundJob2DPAC::AddToPan( float i_TargetLevel, float i_Seconds, float i_SimulationTime )
{
	DBG_ASSERT( ((i_TargetLevel >= -1.0f) && (i_TargetLevel <= 1.0f)), "Target Level out of range - " << i_TargetLevel );

	SetPanChanging( true );

	m_PanStartPosition	= m_Pan;
	m_PanPosition		= i_TargetLevel;
	m_PanPosition		= m_PanPosition + i_TargetLevel;
	if ( m_PanPosition >  1.0f ) m_PanPosition =  1.0f;
	if ( m_PanPosition < -1.0f ) m_PanPosition = -1.0f;
	m_PanTime			= i_Seconds;
	m_PanStartTime		= i_SimulationTime;
}


//------------------------------------------------------------------------
//	SetFrequencyTo()
//
//	The sound system provides a way to manipulate frequencies based on a
//	percentage rather than dealing with the frequency values themselves.
//	Given a percent, where 1.0 is 100% (normal playback), 0.5 is 50% 
//	(half speed), and 2.0 is 200% (double speed) we can calculate the 
//	frequency.  the valid percent is 0.0 to 10.0.
//------------------------------------------------------------------------
void 
snSoundJob2DPAC::SetFrequencyTo( float i_TargetLevel, float i_Seconds, float i_SimulationTime )
{
	DBG_ASSERT( ((i_TargetLevel >= 0.0f) && (i_TargetLevel <= 10.0f)), "Target Level out of range - " << i_TargetLevel );

	SetFrequencyChanging( true );

	m_FrequencyStartFactor	= m_FrequencyFactor;
	m_FrequencyFactor		= m_FrequencyFactor;
	m_FrequencyTargetFactor	= i_TargetLevel;
	if ( m_FrequencyTargetFactor >  10.0f ) m_FrequencyTargetFactor =  10.0f;
	if ( m_FrequencyTargetFactor < -10.0f )	m_FrequencyTargetFactor = -10.0f;
	m_FrequencyTime			= i_Seconds;
	m_FrequencyStartTime	= i_SimulationTime;
}


//------------------------------------------------------------------------
//	AddToFrequency()
//
//		Add a value to the frequency factor.  This level is a factor 
//	from -10.0 to 10.0.
//------------------------------------------------------------------------
void 
snSoundJob2DPAC::AddToFrequency( float i_TargetLevel, float i_Seconds, float i_SimulationTime )
{
	DBG_ASSERT( ((i_TargetLevel >= -10.0f) && (i_TargetLevel <= 10.0f)), "Target Level out of range - " << i_TargetLevel );

	SetFrequencyChanging( true );

	m_FrequencyStartFactor	= m_FrequencyFactor;
	m_FrequencyFactor		= m_FrequencyStartFactor;
	m_FrequencyTargetFactor	= m_FrequencyFactor + i_TargetLevel;
	if ( m_FrequencyTargetFactor >  10.0f ) m_FrequencyTargetFactor =  10.0f;
	if ( m_FrequencyTargetFactor < -10.0f )	m_FrequencyTargetFactor = -10.0f;
	m_FrequencyTime			= i_Seconds;
	m_FrequencyStartTime	= i_SimulationTime;
}


//------------------------------------------------------------------------
//	IsPlaying()
//
//	return true if the sound job is currently playing.
//------------------------------------------------------------------------
bool	
snSoundJob2DPAC::IsPlaying() const 
{
	return ( (m_Flag.bShouldStart) || (( m_Status & DSBSTATUS_PLAYING ) != 0) );
}


//------------------------------------------------------------------------
//	SetVolume()
//
//	The volume can range from 0.0 (silent) to 1.0 (full-on)
//------------------------------------------------------------------------
void	
snSoundJob2DPAC::SetVolume( const float i_NewVolume )
{
	//	if the volume requested is within range, change the volume.
	//	if not, we need to still re-calculate the volume because some of the factors
	//	may have changed.
	//
	if ( ( i_NewVolume >= 0.0f ) && ( i_NewVolume <= 1.0f ) )
	{
		m_Volume = i_NewVolume;
	}
	else
	{
		// TODO: Should we assert here?
		//DBG_ASSERT(false, "Invalid volume setting (%.2f).  Must be 0.0 to 1.0.  Ignoring setting.", i_NewVolume );
		DBG_WARNING("Invalid volume (" << i_NewVolume << "), ignoring." );
	}

	// scale volume according to fade in/ fade out
	//
	float ScaledVolume = GetVolumeFinal();

	// convert to 100ths of dBs for directsound
	//
	//	Note: ScaledVolume is a LINEAR volume so it will be properly converted
	//	to an exponential value.
	//
	int NewValue = snSoundUtilPAC::ConvertVolume( ScaledVolume );

	if ( m_pBuffer )
	{
		int nVolumeErr = m_pBuffer->SetVolume( NewValue );

		if (	nVolumeErr != DS_OK )
		{
			switch( nVolumeErr )
			{
				case DSERR_CONTROLUNAVAIL  :
					DBG_WARNING( "gsSoundDS: DSERR_CONTROLUNAVAIL" );
					break;
				case DSERR_GENERIC  :
					DBG_WARNING( "gsSoundDS: DSERR_GENERIC" );
					break;
				case DSERR_INVALIDPARAM   :
					DBG_WARNING( "gsSoundDS: DSERR_INVALIDPARAM" );
					break;
				case DSERR_PRIOLEVELNEEDED  :
					DBG_WARNING( "gsSoundDS: DSERR_PRIOLEVELNEEDED " );
					break;
			}
		}
	}

	//	reset the value
	//
	SetVolumeChanged( false );
}


//------------------------------------------------------------------------
//	GetVolume()
//
//	return: The volume can range from 0.0 (silent) to 1.0 (full-on)
//------------------------------------------------------------------------
float
snSoundJob2DPAC::GetVolume() const
{
	return m_Volume;
}


//------------------------------------------------------------------------
//	SetVolumeFactor()
//
//	The volume factor (percentage) can range from 0.0 (0%) to 1.0 (100 %)
//------------------------------------------------------------------------
void	
snSoundJob2DPAC::SetVolumeFactor( const float i_Value )
{
	DBG_ASSERT( ((i_Value >= 0.0f) && (i_Value <= 1.0f)), "Value out of range - " << i_Value );

	m_VolumeFactor	= i_Value;

	if ( m_VolumeFactor < 0.0f )
	{
		DBG_WARNING("Invalid volume factor (" << i_Value << "), resetting to 0.0" );
		m_VolumeFactor = 0.0f;
	}

	if ( m_VolumeFactor > 1.0f )
	{
		DBG_WARNING("Invalid volume factor (" << i_Value << "), resetting to 1.0" );
		m_VolumeFactor = 1.0f;
	}

	SetVolumeChanged( true );
}


//------------------------------------------------------------------------
//	GetVolumeFactor()
//
//	return: The volume factor can range from 0.0 (0%) to 1.0 (100 %)
//------------------------------------------------------------------------
float		
snSoundJob2DPAC::GetVolumeFactor() const
{
	return m_VolumeFactor;
}


//------------------------------------------------------------------------
//	SetVolumeTypeFactor()
//
//	The volume factor (percentage) can range from 0.0 (0%) to 1.0 (100 %)
//------------------------------------------------------------------------
void	
snSoundJob2DPAC::SetVolumeTypeFactor( const float i_Value )
{
	DBG_ASSERT( ((i_Value >= 0.0f) && (i_Value <= 1.0f)), "Value out of range - " << i_Value );

	m_VolumeTypeFactor	= i_Value;

	if ( m_VolumeTypeFactor < 0.0f )
	{
		DBG_WARNING("Invalid volume type factor (" << i_Value << "), resetting to 0.0" );
		m_VolumeTypeFactor = 0.0f;
	}

	if ( m_VolumeTypeFactor > 1.0f )
	{
		DBG_WARNING("Invalid volume factor (" <<  i_Value << "), resetting to 1.0" );
		m_VolumeTypeFactor = 1.0f;
	}

	SetVolumeChanged( true );
}


//------------------------------------------------------------------------
//	GetVolumeTypeFactor()
//
//	return: The volume factor can range from 0.0 (0%) to 1.0 (100 %)
//------------------------------------------------------------------------
float		
snSoundJob2DPAC::GetVolumeTypeFactor() const
{
	return m_VolumeTypeFactor;
}


//------------------------------------------------------------------------
//	SetVolumeGlobalFactor()
//
//	The volume factor (percentage) can range from 0.0 (0%) to 1.0 (100 %)
//------------------------------------------------------------------------
void	
snSoundJob2DPAC::SetVolumeGlobalFactor( const float i_Value )
{
	DBG_ASSERT( ((i_Value >= 0.0f) && (i_Value <= 1.0f)), "Value out of range - " << i_Value );

	m_VolumeGlobalFactor	= i_Value;

	if ( m_VolumeGlobalFactor < 0.0f )
	{
		DBG_WARNING("Invalid volume factor (" << i_Value << "), resetting to 0.0" );
		m_VolumeGlobalFactor = 0.0f;
	}

	if ( m_VolumeGlobalFactor > 1.0f )
	{
		DBG_WARNING("Invalid volume factor (" << i_Value << "), resetting to 1.0" );
		m_VolumeGlobalFactor = 1.0f;
	}

	SetVolumeChanged( true );
}


//------------------------------------------------------------------------
//	GetVolumeGlobalFactor()
//
//	return: The volume factor can range from 0.0 (0%) to 1.0 (100 %)
//------------------------------------------------------------------------
float		
snSoundJob2DPAC::GetVolumeGlobalFactor() const
{
	return m_VolumeGlobalFactor;
}


//------------------------------------------------------------------------
//	GetVolumeFinal()
//
//	The final volume value based on the base volume and all factors.
//------------------------------------------------------------------------
float		
snSoundJob2DPAC::GetVolumeFinal() const
{
	float ScaledVolume;

	ScaledVolume = m_Volume * GetVolumeFactor();

	// scale the volume according to the current effects or music volume settings
	//
	ScaledVolume *= m_VolumeLocalizeFactor;
	ScaledVolume *= GetVolumeTypeFactor();
	ScaledVolume *= GetVolumeGlobalFactor();

	return ScaledVolume;
}


//------------------------------------------------------------------------
//	SetFrequency()
//
//	The sound system provides a way to manipulate frequencies based on a
//	percentage rather than dealing with the frequency values themselves.
//	Given a percent, where 1.0 is 100% (normal playback), 0.5 is 50% 
//	(half speed), and 2.0 is 200% (double speed) we can calculate the 
//	frequency.  the valid percent is 0.0 to 10.0.
//------------------------------------------------------------------------
void	
snSoundJob2DPAC::SetFrequency( float i_NewFrequency )
{
	//	if the Frequency requested is within range, change the Frequency.
	//	if not, we need to still re-calculate the Frequency because some of the factors
	//	may have changed.
	//
	if ( ( i_NewFrequency >= 100.0f ) && ( i_NewFrequency <= 100000.0f ) )
	{
		m_FrequencyOriginal = (long)i_NewFrequency;
	}

	// scale Frequency according to fade in/ fade out
	//
	//float ScaledFrequency = m_Frequency * GetFrequencyFactor();
	float ScaledFrequency = (float)m_FrequencyOriginal;

	// convert to 100ths of dBs for directsound
	//
	//	Note: ScaledFrequency is a LINEAR Frequency so it will be properly converted
	//	to an exponential value.
	//
	int NewValue = snSoundUtilPAC::ConvertFrequency( m_FrequencyFactor, ScaledFrequency );

	if ( m_pBuffer )
	{
		int nFrequencyErr = m_pBuffer->SetFrequency( NewValue );

		if (	nFrequencyErr != DS_OK )
		{
			switch( nFrequencyErr )
			{
				case DSERR_CONTROLUNAVAIL  :
					DBG_WARNING( "gsSoundDS: DSERR_CONTROLUNAVAIL" );
					break;
				case DSERR_GENERIC  :
					DBG_WARNING( "gsSoundDS: DSERR_GENERIC" );
					break;
				case DSERR_INVALIDPARAM   :
					DBG_WARNING( "gsSoundDS: DSERR_INVALIDPARAM" );
					break;
				case DSERR_PRIOLEVELNEEDED  :
					DBG_WARNING( "gsSoundDS: DSERR_PRIOLEVELNEEDED " );
					break;
			}
		}
	}
}


//------------------------------------------------------------------------
//	GetFrequency()
//
//	Get the sound Frequency.  This level is a factor from 0.0 to 10.0.
//------------------------------------------------------------------------
float
snSoundJob2DPAC::GetFrequency() const
{
	return (float)m_FrequencyOriginal;
}


//------------------------------------------------------------------------
//	SetFrequencyFactor()
//
//	The factor can range from 0.0 (0%) to 1.0 (100 %)
//------------------------------------------------------------------------
void		
snSoundJob2DPAC::SetFrequencyFactor( float i_NewFrequencyFactor )
{
	DBG_ASSERT( ((i_NewFrequencyFactor >= 0.0f) && (i_NewFrequencyFactor <= 1.0f)), "Value out of range - " << std::setw(6) << std::setprecision(2) << i_NewFrequencyFactor );

	m_FrequencyFactor	= i_NewFrequencyFactor;

	if ( m_FrequencyFactor < 0.0f )
		m_FrequencyFactor = 0.0f;

	if ( m_FrequencyFactor > 1.0f )
		m_FrequencyFactor = 1.0f;
}


//------------------------------------------------------------------------
//	GetFrequencyFactor()
//
//	return: The factor can range from 0.0 (0%) to 1.0 (100 %)
//------------------------------------------------------------------------
float		
snSoundJob2DPAC::GetFrequencyFactor() const
{
	return m_FrequencyFactor;
}


//------------------------------------------------------------------------
//	SetPan()
//
//	The pan can range from -1.0f (100% to the left) to 0.0f (dead center)
//	to 1.0f (100% to the right).
//------------------------------------------------------------------------
void	
snSoundJob2DPAC::SetPan( float i_NewPan )
{
	//	if the pan requested is within range, change it.
	//	if not, we need to still re-calculate the pan because some of the factors
	//	may have changed.
	//
	if ( ( i_NewPan >= -1.0f ) && ( i_NewPan <= 1.0f ) )
	{
		m_Pan = i_NewPan;
	}

	// convert to 100ths of dBs for directsound
	//
	//	Note: Pan is a LINEAR value so it will be properly converted
	//	to an exponential value.
	//
	int NewValue = snSoundUtilPAC::ConvertPan( m_Pan );

	if ( m_pBuffer )
	{
		int nErr = m_pBuffer->SetPan( NewValue );

		if (	nErr != DS_OK )
		{
			switch( nErr )
			{
				case DSERR_CONTROLUNAVAIL  :
					DBG_WARNING( "gsSoundDS: DSERR_CONTROLUNAVAIL" );
					break;
				case DSERR_GENERIC  :
					DBG_WARNING( "gsSoundDS: DSERR_GENERIC" );
					break;
				case DSERR_INVALIDPARAM   :
					DBG_WARNING( "gsSoundDS: DSERR_INVALIDPARAM" );
					break;
				case DSERR_PRIOLEVELNEEDED  :
					DBG_WARNING( "gsSoundDS: DSERR_PRIOLEVELNEEDED " );
					break;
				default:
					DBG_WARNING( "gsSoundDS: DSERR_ Unknown " );
					break;
			}
		}
	}
}


//------------------------------------------------------------------------
//	GetPan()
//
//	return: The pan can range from -1.0f (100% to the left) to 0.0f 
//	(dead center) to 1.0f (100% to the right).
//------------------------------------------------------------------------
float
snSoundJob2DPAC::GetPan() const
{
	return m_Pan;
}


//------------------------------------------------------------------------
//	GetPanPosition()
//
//	return: the pan modifing factor.  It can range from -1.0 to 1.0
//------------------------------------------------------------------------
float
snSoundJob2DPAC::GetPanPosition() const
{
	return m_PanPosition;
}


//------------------------------------------------------------------------
//	SetFadeFactor()
//
//	this factor ranges from 0.0 (silence) to 1.0 (full). 
//------------------------------------------------------------------------
void	
snSoundJob2DPAC::SetFadeFactor( float Value )
{
	m_FadeFactor = Value;
}


//------------------------------------------------------------------------
//	GetFadeFactor()
//
//	return: this factor ranges from 0.0 (silence) to 1.0 (full). 
//------------------------------------------------------------------------
float
snSoundJob2DPAC::GetFadeFactor() const
{
	return m_FadeFactor;
}


//------------------------------------------------------------------------
//	SetLoops()
//
//	Set the number of loops to play a sound.
//	-1 (or any negative number) means infinite.
//
//	Note: currently it only signifies whether a sound is looping or not.
//	it doesn't do counting.
//------------------------------------------------------------------------
void	
snSoundJob2DPAC::SetLoops( const int Value )
{
	m_Loops = Value;

	if ( Value != 0 )
	{
		SetLooping( true );
	}
	else
	{
		SetLooping( false );
	}
}


//------------------------------------------------------------------------
//	GetLoops()
//	
//	Get the number of loops to play a sound.
//
//	Note: currently it only signifies whether a sound is looping or not.
//	it doesn't do counting.
//------------------------------------------------------------------------
int
snSoundJob2DPAC::GetLoops() const
{
	return m_Loops;
}


//------------------------------------------------------------------------
//	SetLoopCurrent()
//
//	The current loop count.
//------------------------------------------------------------------------
void	
snSoundJob2DPAC::SetLoopCurrent( const int Value )
{
	m_LoopCurrent = Value;
}


//------------------------------------------------------------------------
//	GetLoopCurrent()
//
//	return: The current loop count.
//------------------------------------------------------------------------
int
snSoundJob2DPAC::GetLoopCurrent() const
{
	return m_LoopCurrent;
}


//------------------------------------------------------------------------
//	SetRestorePosition()
//
//	The last position we read from the sound buffer.
//------------------------------------------------------------------------
void	
snSoundJob2DPAC::SetRestorePosition( const unsigned long Value )
{
	m_RestorePosition = Value;
}


//------------------------------------------------------------------------
//	GetRestorePosition()
//
//	return: The last position we read from the sound buffer.
//------------------------------------------------------------------------
unsigned long
snSoundJob2DPAC::GetRestorePosition() const
{
	return m_RestorePosition;
}


//------------------------------------------------------------------------
//	SetPosition()
//
//	physical location of sound in world (used by localize)
//------------------------------------------------------------------------
void
snSoundJob2DPAC::SetPosition( const maVector3d& Value )
{
	m_Position	= Value;
}


//------------------------------------------------------------------------
//	GetPosition()
//
//	physical location of sound in world (used by localize)
//------------------------------------------------------------------------
maVector3d		
snSoundJob2DPAC::GetPosition() const
{
	return m_Position;
}


//------------------------------------------------------------------------
//	SetListenerPosition()
//
//	physical location of listener in world (used by localize)
//------------------------------------------------------------------------
void
snSoundJob2DPAC::SetListenerPosition( const maVector3d& i_Position )
{
	m_ListenerPosition	= i_Position;
}


//------------------------------------------------------------------------
//	GetListenerPosition()
//
//	physical location of listener in world (used by localize)
//------------------------------------------------------------------------
maVector3d		
snSoundJob2DPAC::GetListenerPosition() const
{
	return m_ListenerPosition;
}


//------------------------------------------------------------------------
//	SetListenerOrientation()
//
//	physical orientation of listener in world (used by localize)
//------------------------------------------------------------------------
void
snSoundJob2DPAC::SetListenerOrientation( const maRotation& i_Value )
{
	m_ListenerOrientation	= i_Value;
}


//------------------------------------------------------------------------
//	GetListenerOrientation()
//
//	physical location of listener in world (used by localize)
//------------------------------------------------------------------------
maRotation
snSoundJob2DPAC::GetListenerOrientation() const
{
	return m_ListenerOrientation;
}


//------------------------------------------------------------------------
//	SetSoundDistance()
//
//	Distance that the sound falls off at.
//------------------------------------------------------------------------
void
snSoundJob2DPAC::SetSoundDistance( float i_Distance )
{
	m_SoundDistance	= i_Distance;
}


//------------------------------------------------------------------------
//	GetSoundDistance()
//
//	Distance that the sound falls off at.
//------------------------------------------------------------------------
float
snSoundJob2DPAC::GetSoundDistance() const
{
	return m_SoundDistance;
}


//------------------------------------------------------------------------
//	SetFalloffDistance()
//
//	Distance that the sound falls off at.  The sound starts falling off
//	after it goes past the SoundDistance.
//------------------------------------------------------------------------
void
snSoundJob2DPAC::SetFalloffDistance( float i_Distance )
{
	m_FalloffDistance	= i_Distance;
}


//------------------------------------------------------------------------
//	GetFalloffDistance()
//
//	Distance that the sound falls off at.  The sound starts falling off
//	after it goes past the SoundDistance.
//------------------------------------------------------------------------
float
snSoundJob2DPAC::GetFalloffDistance() const
{
	return m_FalloffDistance;
}


//------------------------------------------------------------------------
//	SetStartPos()
//
//	The first position of data we read from the sound buffer.
//------------------------------------------------------------------------
void	
snSoundJob2DPAC::SetStartPos( const int i_Position )
{
	m_StartPos = i_Position;
}


//------------------------------------------------------------------------
//	GetStartPos()
//
//	return: The first position of data we read from the sound buffer.
//------------------------------------------------------------------------
int
snSoundJob2DPAC::GetStartPos() const
{
	return m_StartPos;
}


//------------------------------------------------------------------------
//	SetStartTime()
//
//	the time the sound started.
//------------------------------------------------------------------------
void	
snSoundJob2DPAC::SetStartTime( const float Value )
{
	m_StartTime = Value;
}


//------------------------------------------------------------------------
//	GetStartTime()
//
//	return: the time the sound started.
//------------------------------------------------------------------------
float
snSoundJob2DPAC::GetStartTime() const
{
	return m_StartTime;
}


//------------------------------------------------------------------------
//	SetDataSize()
//------------------------------------------------------------------------
void	
snSoundJob2DPAC::SetDataSize( const int Value )
{
	m_DataSize = Value;
}


//------------------------------------------------------------------------
//	GetDataSize()
//------------------------------------------------------------------------
int
snSoundJob2DPAC::GetDataSize() const
{
	return m_DataSize;
}


//------------------------------------------------------------------------
//	SetBufferSize()
//------------------------------------------------------------------------
void	
snSoundJob2DPAC::SetBufferSize( const int Value )
{
	m_BufferSize = Value;
}


//------------------------------------------------------------------------
//	GetBufferSize()
//------------------------------------------------------------------------
int
snSoundJob2DPAC::GetBufferSize() const
{
	return m_BufferSize;
}


//------------------------------------------------------------------------
//	SetFormat()
//------------------------------------------------------------------------
void	
snSoundJob2DPAC::SetFormat( const WAVEFORMATEX& Value )
{
	m_Format = Value;
}


//------------------------------------------------------------------------
//	GetFormat()
//------------------------------------------------------------------------
WAVEFORMATEX
snSoundJob2DPAC::GetFormat() const
{
	return m_Format;
}


//------------------------------------------------------------------------
//	SetBuffer()
//------------------------------------------------------------------------
void		
snSoundJob2DPAC::SetBuffer( IDirectSoundBuffer * i_pBuffer )
{
	m_pBuffer	= i_pBuffer;
}


//------------------------------------------------------------------------
//	SetPositionInSoundByPercent()
//
//	i_Percent - the percent of the sound to jump to
//
//	return the current percentage
//------------------------------------------------------------------------
float snSoundJob2DPAC::SetPositionInSoundByPercent( float i_fPercent )
{
	DBG_ASSERT( (i_fPercent >= 0.0f && i_fPercent <= 1.0f), "Invalid Percent" );

	float bytes = m_DataSize * i_fPercent; //m_Format.cbSize * i_fPercent;
	if ( m_pBuffer != 0 )
		m_pBuffer->SetCurrentPosition( (int)bytes );

	return i_fPercent;
}

//------------------------------------------------------------------------
//	GetPositionInSoundByPercent()
//
//	Get the current percent of the playing sound.
//------------------------------------------------------------------------
float snSoundJob2DPAC::GetPositionInSoundByPercent()
{
	DWORD playBytes;
	if ( m_pBuffer == 0 )
		return 1.0f;

	HRESULT result = m_pBuffer->GetCurrentPosition(&playBytes,NULL);
	if (result != DS_OK)
	{
		if (result == DSERR_INVALIDPARAM)
			DBG_ERROR("Invalid Param");
		if (result == DSERR_PRIOLEVELNEEDED)
			DBG_ERROR("Priority level needed");
	}

	int size = m_DataSize; //m_Format.cbSize;
	DBG_ASSERT( size > 0, "Size of sound is zero");

	float percent = ((float)playBytes / (float)size);
	//DBG_LOG3("playbytes %9d size %9d  percent =%9.4f", playBytes, size, percent );

	DBG_ASSERT( (percent >= 0.0f && percent <= 1.0f), "Invalid Percent calculated" );

	return percent;
}

//------------------------------------------------------------------------
//	GetTimeLength()
//
//	Get the length of the sound in seconds.
//------------------------------------------------------------------------
float snSoundJob2DPAC::GetTimeLength()
{
	return m_fTotalTime;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
snSoundJob2DPAC&
snSoundJob2DPAC::operator = ( const snSoundJob2DPAC& i_SoundData )
{
	m_Flag					= i_SoundData.m_Flag;
	m_Volume				= i_SoundData.m_Volume;
	m_VolumeFactor			= i_SoundData.m_VolumeFactor;
	m_VolumeLocalizeFactor	= i_SoundData.m_VolumeLocalizeFactor;
	m_VolumeTypeFactor		= i_SoundData.m_VolumeTypeFactor;
	m_VolumeGlobalFactor	= i_SoundData.m_VolumeGlobalFactor;
	m_FrequencyOriginal		= i_SoundData.m_FrequencyOriginal;
	m_FrequencyFactor		= i_SoundData.m_FrequencyFactor;
	m_FrequencyStartFactor	= i_SoundData.m_FrequencyStartFactor;
	m_FrequencyTargetFactor	= i_SoundData.m_FrequencyTargetFactor;
	m_FrequencyStartTime	= i_SoundData.m_FrequencyStartTime;
	m_FrequencyTime			= i_SoundData.m_FrequencyTime;
	m_Pan					= i_SoundData.m_Pan;
	m_PanPosition			= i_SoundData.m_PanPosition;
	m_PanStartTime			= i_SoundData.m_PanStartTime;
	m_PanTime				= i_SoundData.m_PanTime;
	m_FadeFactor			= i_SoundData.m_FadeFactor;
	m_FadeStartFactor		= i_SoundData.m_FadeStartFactor;
	m_FadeStartTime			= i_SoundData.m_FadeStartTime;
	m_FadeTime				= i_SoundData.m_FadeTime;
	m_Loops					= i_SoundData.m_Loops;
	m_LoopCurrent			= i_SoundData.m_LoopCurrent;
	m_PlayTime				= i_SoundData.m_PlayTime;
	m_Position				= i_SoundData.m_Position;
	m_ListenerPosition		= i_SoundData.m_ListenerPosition;
	m_ListenerOrientation	= i_SoundData.m_ListenerOrientation;
	m_SoundDistance			= i_SoundData.m_SoundDistance;
	m_FalloffDistance		= i_SoundData.m_FalloffDistance;
	m_StartPos				= i_SoundData.m_StartPos;
	m_StartTime				= i_SoundData.m_StartTime;
	m_RestorePosition		= i_SoundData.m_RestorePosition;
	m_DataSize				= i_SoundData.m_DataSize;
	m_BufferSize			= i_SoundData.m_BufferSize;
	m_Format				= i_SoundData.m_Format;
	m_Filename				= i_SoundData.m_Filename;

	(snDSoundGlobal::g_pDirectSound)->DuplicateSoundBuffer( i_SoundData.m_pBuffer, &(m_pBuffer) );

	return(*this);
}


//------------------------------------------------------------------------
//	GetBuffer()
//------------------------------------------------------------------------
IDirectSoundBuffer * 	
snSoundJob2DPAC::GetBuffer()
{
	return m_pBuffer;
}


//------------------------------------------------------------------------
//	CreateBuffer()
//------------------------------------------------------------------------
void	
snSoundJob2DPAC::CreateBuffer()
{
	gfFileBin SoundFile( m_Filename, fsFileStream::e_ReadOnly );
	//read_wav_header(SoundFile);
	int HeaderSize = 0;

	// find and read the wav format info and the size
	// of the wav data contained in this sound file
	//
	memset( &m_Format, 0, sizeof( m_Format ) );

	SoundFile.SetFilePos( 0x14, fsFileStream::e_Beginning );
	SoundFile.Read( 16, &m_Format );
	HeaderSize += 16;
	SoundFile.SetFilePos( sizeof( int ), fsFileStream::e_Current );
	HeaderSize += sizeof( m_DataSize );
	SoundFile.Read( sizeof( m_DataSize ), &m_DataSize );

	m_StartPos		= (unsigned long)SoundFile.GetFilePos();
	m_BufferSize	= m_DataSize;

	//	Create the Sound Buffer
	//
	DSBUFFERDESC	BufferDesc;
	memset( &BufferDesc, 0, sizeof( BufferDesc ) );
	BufferDesc.dwSize			= sizeof( BufferDesc );
	BufferDesc.lpwfxFormat		= &m_Format;

	//	Grab the correct capabilities and set the buffer size
	//
	BufferDesc.dwFlags			|= ( DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_STATIC  );
	BufferDesc.dwBufferBytes	=  m_DataSize;

	//	Create the sound buffer
	//
	snDSoundGlobal::CreateSoundBuffer( &BufferDesc, &m_pBuffer );
	if ( m_pBuffer == NULL )
	{
		//	Error: there was a problem creating the sound buffer
		//
		throw snSoundCreateFailedX( m_Filename );
	}

	LoadBuffer( &SoundFile );

	m_fTotalTime = (float)((float)(m_DataSize) / (float)(m_Format.nAvgBytesPerSec));
	DBG_TRACE( "Total sound time = " << std::setw(6) << std::setprecision(2) << m_fTotalTime );
}


//------------------------------------------------------------------------
//	LoadBuffer()
//------------------------------------------------------------------------
void	
snSoundJob2DPAC::LoadBuffer( gfFileBin * pFileStream )
{
	HRESULT			Error;
	unsigned long	DataSize;
	void*			pData = NULL;

	//	Lock the buffer
	//
	Error = m_pBuffer->Lock( 0, m_BufferSize, &pData, &DataSize, NULL, NULL, 0 );
	if ( Error )
	{
		snDSoundGlobal::PrintDSError( Error );
		return;
	}

	//	Read into it
	//
	if ( !pFileStream->Read( m_DataSize, pData ) )
	{
		m_pBuffer->Unlock( pData, DataSize, NULL, 0 );
		m_pBuffer->Release();
		m_pBuffer	= NULL;
		return;
	}

	//	Read into it
	//
	Error = m_pBuffer->Unlock( pData, DataSize, NULL, 0 );
	if ( Error )
	{
		snDSoundGlobal::PrintDSError( Error );
		return;
	}
}


//------------------------------------------------------------------------
//	Init()
//
//	Initialize this classes data
//------------------------------------------------------------------------
void	
snSoundJob2DPAC::Init()
{
	SetLoaded( false );
	SetPaused( false );
	SetFinished( false );
	SetLooping( false );
	SetFadeChanging( false );
	SetStopOnFadeFinish( false );
	SetFreeOnFadeFinish( false );
	SetDeleteWhenFinished( false );
	SetLocalize( false );
	SetShouldStart( false );
	SetVolumeChanged( false );

	m_Volume				= 1.0f;
	m_VolumeFactor			= 1.0f;
	m_VolumeLocalizeFactor	= 1.0f;
	m_VolumeGlobalFactor	= 1.0f;
	m_VolumeTypeFactor		= 1.0f;

	m_FrequencyOriginal		= 0;
	m_FrequencyFactor		= 1.0f;
	m_FrequencyStartFactor	= 1.0f;
	m_FrequencyTargetFactor	= 1.0f;
	m_FrequencyStartTime	= 0.0f;
	m_FrequencyTime			= 0.0f;

	m_Pan					= 0.0f;
	m_PanPosition			= 0.0f;
	m_PanStartPosition		= 0.0f;
	m_PanStartTime			= 0.0f;
	m_PanTime				= 0.0f;

	m_FadeFactor			= 1.0f;
	m_FadeStartFactor		= 0;
	m_FadeTime				= 0;
	m_FadeStartTime			= 0;
	m_bNewFadeSet			= false;

	m_Loops					= 1;
	m_LoopCurrent			= 0;

	m_StartPos				= 0;
	m_StartTime				= 0;
	m_PlayTime				= -1.0f;
	m_RestorePosition		= 0;

	m_Status				= 0;

	m_Position				= maVector3d( 0.0f, 0.0f, 0.0f );
	m_ListenerPosition		= maVector3d( 0.0f, 0.0f, 0.0f );
	m_ListenerOrientation	= maRotation( maVector3d( 0.0f, 1.0f, 0.0f ), 0.0f );
	m_SoundDistance			= 20.0f;
	m_FalloffDistance		= 40.0f;

	m_DataSize				= 0;
	m_BufferSize			= 0;
	m_pBuffer				= NULL;
	memset( &m_Format, 0, sizeof( m_Format) );
}

//------------------------------------------------------------------------
//	read the wav header and fill in m_Format, m_DataSize, m_StartPos,
//	and m_BufferSize.
//------------------------------------------------------------------------
int snSoundJob2DPAC::read_wav_header(gfFileBin& io_SoundFile)
{
	int HeaderSize = 0;

	// find and read the wav format info and the size
	// of the wav data contained in this sound file
	//
	memset( &m_Format, 0, sizeof( m_Format ) );

	io_SoundFile.SetFilePos( 0x14, fsFileStream::e_Beginning );
	io_SoundFile.Read( 16, &m_Format );
	HeaderSize += 16;
	io_SoundFile.SetFilePos( sizeof( int ), fsFileStream::e_Current );
	HeaderSize += sizeof( m_DataSize );
	io_SoundFile.Read( sizeof( m_DataSize ), &m_DataSize );

	m_StartPos		= (unsigned long)io_SoundFile.GetFilePos();
	m_BufferSize	= m_DataSize;

//	//------------------------------------------------------------------------------
//
//    HMMIO         m_hmmio;       // MM I/O handle for the WAVE
//    MMCKINFO      m_ck;          // Multimedia RIFF chunk
//    MMCKINFO      m_ckRiff;      // Use in opening a WAVE file
//    DWORD         m_dwSize;      // The size of the wave file
//    MMIOINFO      m_mmioinfoOut;
//    DWORD         m_dwFlags;
//    BOOL          m_bIsReadingFromMemory;
//    BYTE*         m_pbData;
//    BYTE*         m_pbDataCur;
//    ULONG         m_ulDataSize;
//    CHAR*         m_pResourceBuffer;
//    MMCKINFO        ckIn;           // chunk info. for general use.
//    PCMWAVEFORMAT   pcmWaveFormat;  // Temp PCM structure to load in.
//
//    if( ( 0 != mmioDescend( m_hmmio, &m_ckRiff, NULL, 0 ) ) )
//        return -1; //DXUT_ERR( L"mmioDescend", E_FAIL );
//
//    // Check to make sure this is a valid wave file
//    if( (m_ckRiff.ckid != FOURCC_RIFF) ||
//        (m_ckRiff.fccType != mmioFOURCC('W', 'A', 'V', 'E') ) )
//        return  -1; //DXUT_ERR( L"mmioFOURCC", E_FAIL );
//
//    // Search the input file for for the 'fmt ' chunk.
//    ckIn.ckid = mmioFOURCC('f', 'm', 't', ' ');
//    if( 0 != mmioDescend( m_hmmio, &ckIn, &m_ckRiff, MMIO_FINDCHUNK ) )
//        return  -1; //DXUT_ERR( L"mmioDescend", E_FAIL );
//
//    // Expect the 'fmt' chunk to be at least as large as <PCMWAVEFORMAT>;
//    // if there are extra parameters at the end, we'll ignore them
//       if( ckIn.cksize < (LONG) sizeof(PCMWAVEFORMAT) )
//           return  -1; //DXUT_ERR( L"sizeof(PCMWAVEFORMAT)", E_FAIL );
//
//    // Read the 'fmt ' chunk into <pcmWaveFormat>.
//    if( mmioRead( m_hmmio, (HPSTR) &pcmWaveFormat,
//                  sizeof(pcmWaveFormat)) != sizeof(pcmWaveFormat) )
//        return  -1; //DXUT_ERR( L"mmioRead", E_FAIL );
//
//    // Allocate the waveformatex, but if its not pcm format, read the next
//    // word, and thats how many extra bytes to allocate.
//    if( pcmWaveFormat.wf.wFormatTag == WAVE_FORMAT_PCM )
//    {
//        //m_Format = (WAVEFORMATEX*)new CHAR[ sizeof(WAVEFORMATEX) ];
//        //if( NULL == m_Format )
//        //    return  -1; //DXUT_ERR( L"m_Format", E_FAIL );
//
//        // Copy the bytes from the pcm structure to the waveformatex structure
//        memcpy( &m_Format, &pcmWaveFormat, sizeof(pcmWaveFormat) );
//        m_Format.cbSize = 0;
//    }
//    else
//    {
//        //// Read in length of extra bytes.
//        //WORD cbExtraBytes = 0L;
//        //if( mmioRead( m_hmmio, (CHAR*)&cbExtraBytes, sizeof(WORD)) != sizeof(WORD) )
//        //    return  -1; //DXUT_ERR( L"mmioRead", E_FAIL );
//
//        //m_Format = (WAVEFORMATEX*)new CHAR[ sizeof(WAVEFORMATEX) + cbExtraBytes ];
//        //if( NULL == m_Format )
//        //    return  -1; //DXUT_ERR( L"new", E_FAIL );
//
//        //// Copy the bytes from the pcm structure to the waveformatex structure
//        //memcpy( m_Format, &pcmWaveFormat, sizeof(pcmWaveFormat) );
//        //m_Format->cbSize = cbExtraBytes;
//
//        //// Now, read those extra bytes into the structure, if cbExtraAlloc != 0.
//        //if( mmioRead( m_hmmio, (CHAR*)(((BYTE*)&(m_Format->cbSize))+sizeof(WORD)),
//        //              cbExtraBytes ) != cbExtraBytes )
//        //{
//        //    delete m_Format;
//        //    return  -1; //DXUT_ERR( L"mmioRead", E_FAIL );
//        //}
//    }
//
//    // Ascend the input file out of the 'fmt ' chunk.
//    if( 0 != mmioAscend( m_hmmio, &ckIn, 0 ) )
//    {
//		//delete m_Format;
//        return  -1; //DXUT_ERR( L"mmioAscend", E_FAIL );
//    }
//
//    // Seek to the data
//    if( -1 == mmioSeek( m_hmmio, m_ckRiff.dwDataOffset + sizeof(FOURCC),
//                    SEEK_SET ) )
//        return -1; //DXUT_ERR( L"mmioSeek", E_FAIL );
//
//    // Search the input file for the 'data' chunk.
//    m_ck.ckid = mmioFOURCC('d', 'a', 't', 'a');
//    if( 0 != mmioDescend( m_hmmio, &m_ck, &m_ckRiff, MMIO_FINDCHUNK ) )
//        return -1; //DXUT_ERR( L"mmioDescend", E_FAIL );
//
//	m_DataSize = m_ck.cksize;
//	m_StartPos = 
////	read the wav header and fill in m_Format, m_DataSize, m_StartPos,
////	and m_BufferSize.

	return S_OK;
}
