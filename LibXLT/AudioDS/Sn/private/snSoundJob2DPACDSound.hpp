/****************************************************************************\
**  snSoundJob2DPACDSound.hpp
**
**      snSoundJob2DPACDSound.hpp implements the windows portion of the
**	snSoundJob2DPAC.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef SN_SOUNDJOB2DPACDSOUND_HPP
#error snSoundJob2DPACDSound.hpp multiply included
#endif
#define SN_SOUNDJOB2DPACDSOUND_HPP

#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif

#ifndef MA_VECTOR3D_HPP
#include "Core/ma/maVector3d.hpp"
#endif

#ifndef MA_ROTATION_HPP
#include "Core/ma/maRotation.hpp"
#endif

#include <dsound.h>


//================================================================================
//	Forward references
//================================================================================
class	gfFileBin;
class	snSoundJob;


//================================================================================
//================================================================================
class snSoundJob2DPAC
{
	public:
	//----------------------------------------------------------------------------
	//	Constructors and Destructors
	//----------------------------------------------------------------------------

		//========================================================================
		//	default and copy constructors
		//========================================================================
		snSoundJob2DPAC();
		snSoundJob2DPAC( const snSoundJob2DPAC& i_CopyFrom );

		//========================================================================
		//========================================================================
		virtual ~snSoundJob2DPAC();


	//----------------------------------------------------------------------------
	//	Functions
	//----------------------------------------------------------------------------

		//========================================================================
		//	Load()
		//========================================================================
		virtual void	Load();

		//========================================================================
		//	Free()
		//========================================================================
		virtual void	Free();

		//========================================================================
		//	Reload()
		//
		//		Reload the raw sound data
		//========================================================================
		virtual void	Reload();

		//========================================================================
		//	Unload()
		//
		//		Pause the sound and unload the raw sound data
		//========================================================================
		virtual void	Unload(bool i_bReloadOnStart = false);

		//========================================================================
		//	Start()
		//
		//	Start playing a sound
		//
		//		1. make sure everything's ready
		//		2. start the sound playing (looped or not)
		//		3. the Think() function takes care of it from there
		//========================================================================
		virtual void	Start( float i_SimulationTime );

		//========================================================================
		//	Restart()
		//
		//	Restart playing a sound
		//========================================================================
		virtual void	Restart( float i_SimulationTime );

		//========================================================================
		//	Stop()
		//========================================================================
		virtual void	Stop();

		//========================================================================
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
		//		6. do effects
		//========================================================================
		virtual void Think( float i_SimulationTime );

		//========================================================================
		//	ThinkEffects()
		//
		//	Take care of a sound's effects that is active each game loop.
		//
		//		1. do FadeChanging (if applicable)
		//		2. do PanChanging (if applicable)
		//		3. do FrequencyChanging (if applicable)
		//		4. do other effects (if applicable)
		//========================================================================
		virtual void ThinkEffects( float i_SimulationTime );

		//========================================================================
		//	ThinkLocalize()														
		//																		
		//		Adjusts volume and pan of sound based on sound position
		//	and (usually the camera) position.										
		//========================================================================
		virtual void ThinkLocalize(  float i_SimulationTime );

		//========================================================================
		//	Pause()
		//========================================================================
		virtual void Pause();

		//========================================================================
		//	Resume()
		//========================================================================
		virtual void Resume( float i_SimulationTime );

		//========================================================================
		//	SetFadeTo()
		//
		//		Fade the sound to a specific level over time.  This level is a 
		//	percentage from 0.0 to 1.0.
		//========================================================================
		void SetFadeTo( float i_TargetLevel, float i_Seconds, float i_SimulationTime );

		//========================================================================
		//	AddToFade()
		//
		//		Add a value to the Fade.  This value cannot exceed the Fade
		//	percentage from -1.0 to 1.0.
		//========================================================================
		void AddToFade( float i_TargetLevel, float i_Seconds, float i_SimulationTime );

		//========================================================================
		//	SetPanTo()
		//
		//		Pan the sound to a specific channel over time.  This channel is a
		//	percentage from -1.0 (all left) to 0.0 (center) to 1.0 (all right).
		//========================================================================
		void SetPanTo( float i_TargetLevel, float i_Seconds, float i_SimulationTime );

		//========================================================================
		//	AddToPan()
		//
		//		Add a value to the Pan.  This value cannot exceed the pan
		//	percentage from -1.0 to 1.0.
		//========================================================================
		void AddToPan( float i_TargetLevel, float i_Seconds, float i_SimulationTime );

		//========================================================================
		//	SetFrequencyTo()
		//
		//	The sound system provides a way to manipulate frequencies based on a
		//	percentage rather than dealing with the frequency values themselves.
		//	Given a percent, where 1.0 is 100% (normal playback), 0.5 is 50% 
		//	(half speed), and 2.0 is 200% (double speed) we can calculate the 
		//	frequency.  the valid percent is 0.0 to 10.0.
		//========================================================================
		void SetFrequencyTo( float i_TargetLevel, float i_Seconds, float i_SimulationTime );

		//========================================================================
		//	AddToFrequency()
		//
		//		Add a value to the frequency factor.  This level is a factor 
		//	from -10.0 to 10.0.
		//========================================================================
		void AddToFrequency( float i_TargetLevel, float i_Seconds, float i_SimulationTime );

		//========================================================================
		//	IsPlaying()
		//
		//	return true if the sound job is currently playing.
		//========================================================================
		virtual bool	IsPlaying() const;


	//----------------------------------------------------------------------------
	//	Accessor Functions
	//----------------------------------------------------------------------------

		//========================================================================
		//	SetFilename()
		//========================================================================
		inline void		SetFilename( const fsLocator& Filename );

		//========================================================================
		//	GetFilename()
		//========================================================================
		inline const fsLocator &	GetFilename() const;

		//========================================================================
		//	SetVolume()
		//
		//	The volume can range from 0.0 (silent) to 100.0 (full-on)
		//========================================================================
		void		SetVolume( const float i_NewVolume );

		//========================================================================
		//	GetVolume()
		//
		//	return: The volume can range from 0.0 (silent) to 100.0 (full-on)
		//========================================================================
		float		GetVolume() const;

		//========================================================================
		//	SetVolumeFactor()
		//
		//	The volume factor (percentage) can range from 0.0 (0%) to 1.0 (100 %)
		//========================================================================
		void		SetVolumeFactor( const float i_Value );

		//========================================================================
		//	GetVolumeFactor()
		//
		//	return: The volume factor can range from 0.0 (0%) to 1.0 (100 %)
		//========================================================================
		float		GetVolumeFactor() const;

		//========================================================================
		//	SetVolumeTypeFactor()
		//
		//	The volume factor (percentage) can range from 0.0 (0%) to 1.0 (100 %)
		//========================================================================
		void		SetVolumeTypeFactor( const float i_Value );

		//========================================================================
		//	GetVolumeTypeFactor()
		//
		//	return: The volume factor can range from 0.0 (0%) to 1.0 (100 %)
		//========================================================================
		float		GetVolumeTypeFactor() const;

		//========================================================================
		//	SetVolumeGlobalFactor()
		//
		//	The volume factor (percentage) can range from 0.0 (0%) to 1.0 (100 %)
		//========================================================================
		void		SetVolumeGlobalFactor( const float i_Value );

		//========================================================================
		//	GetVolumeGlobalFactor()
		//
		//	return: The volume factor can range from 0.0 (0%) to 1.0 (100 %)
		//========================================================================
		float		GetVolumeGlobalFactor() const;

		//========================================================================
		//	GetVolumeFinal()
		//
		//	The final volume value based on the base volume and all factors.
		//========================================================================
		float		GetVolumeFinal() const;

		//========================================================================
		//	SetFrequency()
		//
		//	The sound system provides a way to manipulate frequencies based on a
		//	percentage rather than dealing with the frequency values themselves.
		//	Given a percent, where 1.0 is 100% (normal playback), 0.5 is 50% 
		//	(half speed), and 2.0 is 200% (double speed) we can calculate the 
		//	frequency.  the valid percent is 0.0 to 10.0.
		//========================================================================
		void		SetFrequency( float i_NewFrequency );

		//========================================================================
		//	GetFrequency()
		//
		//	Get the sound Frequency.  This level is a factor from 0.0 to 10.0.
		//========================================================================
		float		GetFrequency() const;

		//========================================================================
		//	SetFrequencyFactor()
		//
		//	The factor can range from 0.0 (0%) to 1.0 (100 %)
		//========================================================================
		void		SetFrequencyFactor( float i_NewFrequencyFactor );

		//========================================================================
		//	GetFrequencyFactor()
		//
		//	return: The factor can range from 0.0 (0%) to 1.0 (100 %)
		//========================================================================
		float		GetFrequencyFactor() const;

		//========================================================================
		//	SetPan()
		//
		//	The pan can range from -1.0f (100% to the left) to 0.0f (dead center)
		//	to 1.0f (100% to the right).
		//========================================================================
		void		SetPan( float i_NewPan );

		//========================================================================
		//	GetPan()
		//
		//	return: The pan can range from -1.0f (100% to the left) to 0.0f 
		//	(dead center) to 1.0f (100% to the right).
		//========================================================================
		float		GetPan() const;

		//========================================================================
		//	GetPanPosition()
		//
		//	return: the pan modifing factor.  It can range from -1.0 to 1.0
		//========================================================================
		float		GetPanPosition() const;

		//========================================================================
		//	SetFadeFactor()
		//
		//	this factor ranges from 0.0 (silence) to 1.0 (full). 
		//========================================================================
		void		SetFadeFactor( float Value );

		//========================================================================
		//	GetFadeFactor()
		//
		//	return: this factor ranges from 0.0 (silence) to 1.0 (full). 
		//========================================================================
		float		GetFadeFactor() const;

		//========================================================================
		//	SetLoops()
		//
		//	Set the number of loops to play a sound.
		//	-1 (or any negative number) means infinite.
		//
		//	Note: currently it only signifies whether a sound is looping or not.
		//	it doesn't do counting.
		//========================================================================
		void		SetLoops( const int Value );

		//========================================================================
		//	GetLoops()
		//	
		//	Get the number of loops to play a sound.
		//
		//	Note: currently it only signifies whether a sound is looping or not.
		//	it doesn't do counting.
		//========================================================================
		int			GetLoops() const;

		//========================================================================
		//	SetLoopCurrent()
		//
		//	The current loop count.
		//========================================================================
		void		SetLoopCurrent( const int Value );

		//========================================================================
		//	GetLoopCurrent()
		//
		//	return: The current loop count.
		//========================================================================
		int			GetLoopCurrent() const;

		//========================================================================
		//	SetRestorePosition()
		//
		//	The last position we read from the sound buffer.
		//========================================================================
		void			SetRestorePosition( const unsigned long Value );

		//========================================================================
		//	GetRestorePosition()
		//
		//	return: The last position we read from the sound buffer.
		//========================================================================
		unsigned long	GetRestorePosition() const;

		//========================================================================
		//	SetPosition()
		//
		//	physical location of sound in world (used by localize)
		//========================================================================
		void			SetPosition( const maVector3d& Value );

		//========================================================================
		//	GetPosition()
		//
		//	physical location of sound in world (used by localize)
		//========================================================================
		maVector3d		GetPosition() const;

		//========================================================================
		//	SetListenerPosition()
		//
		//	physical location of listener in world (used by localize)
		//========================================================================
		void			SetListenerPosition( const maVector3d& i_Position );

		//========================================================================
		//	GetListenerPosition()
		//
		//	physical location of listener in world (used by localize)
		//========================================================================
		maVector3d		GetListenerPosition() const;

		//========================================================================
		//	SetListenerOrientation()
		//
		//	physical orientation of listener in world (used by localize)
		//========================================================================
		void			SetListenerOrientation( const maRotation& i_Value );

		//========================================================================
		//	GetListenerOrientation()
		//
		//	physical location of listener in world (used by localize)
		//========================================================================
		maRotation		GetListenerOrientation() const;

		//========================================================================
		//	SetSoundDistance()
		//
		//	Distance the sound plays at full volume.
		//========================================================================
		void			SetSoundDistance( float i_Distance );

		//========================================================================
		//	GetSoundDistance()
		//
		//	Distance the sound plays at full volume.
		//========================================================================
		float			GetSoundDistance() const;

		//========================================================================
		//	SetFalloffDistance()
		//
		//	Distance that the sound falls off at.  The sound starts falling off
		//	after it goes past the SoundDistance.
		//========================================================================
		void			SetFalloffDistance( float i_Distance );

		//========================================================================
		//	GetFalloffDistance()
		//
		//	Distance that the sound falls off at.  The sound starts falling off
		//	after it goes past the SoundDistance.
		//========================================================================
		float			GetFalloffDistance() const;

		//========================================================================
		//	SetStartPos()
		//
		//	The first position of data we read from the sound buffer.
		//========================================================================
		void		SetStartPos( const int i_Position );

		//========================================================================
		//	GetStartPos()
		//
		//	return: The first position of data we read from the sound buffer.
		//========================================================================
		int			GetStartPos() const;

		//========================================================================
		//	SetStartTime()
		//
		//	the time the sound started.
		//========================================================================
		void		SetStartTime( const float Value );

		//========================================================================
		//	GetStartTime()
		//
		//	return: the time the sound started.
		//========================================================================
		float		GetStartTime() const;

		//========================================================================
		//	SetDataSize()
		//========================================================================
		void		SetDataSize( const int Value );

		//========================================================================
		//	GetDataSize()
		//========================================================================
		int			GetDataSize() const;

		//========================================================================
		//	SetBufferSize()
		//========================================================================
		void		SetBufferSize( const int Value );

		//========================================================================
		//	GetBufferSize()
		//========================================================================
		int			GetBufferSize() const;

		//========================================================================
		//	SetFormat()
		//========================================================================
		void		SetFormat( const WAVEFORMATEX& Value );

		//========================================================================
		//	GetFormat()
		//========================================================================
		WAVEFORMATEX			GetFormat() const;

		//========================================================================
		//	SetBuffer()
		//========================================================================
		void		SetBuffer( IDirectSoundBuffer * i_pBuffer );

		//========================================================================
		//	GetBuffer()
		//========================================================================
		IDirectSoundBuffer * 	GetBuffer();

		
		//========================================================================
		//	SetParentSoundJob()
		//
		//	set the ParentSoundJob
		//========================================================================
		void		SetParentSoundJob( snSoundJob *i_pParentSoundJob)	{m_pParentSoundJob = i_pParentSoundJob;}

		//========================================================================
		//	GetParentSoundJob()
		//
		//	return: ParentSoundJob
		//========================================================================
		snSoundJob *GetParentSoundJob() const	{return m_pParentSoundJob;}
		
		//========================================================================
		//	SetPositionInSoundByPercent()
		//
		//	i_Percent - the percent of the sound to jump to
		//
		//	return the current percentage
		//========================================================================
		float SetPositionInSoundByPercent( float i_fPercent );

		//========================================================================
		//	GetPositionInSoundByPercent()
		//
		//	Get the current percent of the playing sound.
		//========================================================================
		float GetPositionInSoundByPercent();

		//========================================================================
		//	GetTimeLength()
		//
		//	Get the length of the sound in seconds.
		//========================================================================
		float GetTimeLength();

	//----------------------------------------------------------------------------
	//	Overloaded Operators
	//----------------------------------------------------------------------------

		//========================================================================
		//	Assignment (=)
		//========================================================================
		virtual snSoundJob2DPAC& operator = ( const snSoundJob2DPAC& i_SoundData );


	//----------------------------------------------------------------------------
	//	Flag Accessor functions
	//----------------------------------------------------------------------------

		//========================================================================
		//	Loaded
		//========================================================================
		bool	IsLoaded() const							{ return m_Flag.bLoaded; }
		void	SetLoaded( const bool Value )				{ m_Flag.bLoaded = Value; }

		//========================================================================
		//	Paused
		//========================================================================
		bool	IsPaused() const							{ return m_Flag.bPaused; }
		void	SetPaused( const bool Value )				{ m_Flag.bPaused = Value; }

		//========================================================================
		//	Finished
		//========================================================================
		bool	IsFinished() const							{ return m_Flag.bFinished; }
		void	SetFinished( const bool Value )				{ m_Flag.bFinished = Value; }

		//========================================================================
		//	Looping
		//========================================================================
		bool	IsLooping() const							{ return m_Flag.bLooping; }
		void	SetLooping( const bool Value )				{ m_Flag.bLooping = Value; }

		//========================================================================
		//	FrequencyChanging
		//========================================================================
		bool	IsFrequencyChanging() const					{ return m_Flag.bFrequencyChanging; }
		void	SetFrequencyChanging( const bool Value )	{ m_Flag.bFrequencyChanging = Value; }

		//========================================================================
		//	PanChanging
		//========================================================================
		bool	IsPanChanging() const						{ return m_Flag.bPanChanging; }
		void	SetPanChanging( const bool Value )			{ m_Flag.bPanChanging = Value; }

		//========================================================================
		//	FadeChanging
		//========================================================================
		bool	IsFadeChanging() const						{ return m_Flag.bFadeChanging; }
		void	SetFadeChanging( const bool Value )			{ m_Flag.bFadeChanging = Value; }

		//========================================================================
		//	StopOnFadeFinish
		//========================================================================
		bool	IsStopOnFadeFinish() const					{ return m_Flag.bStopOnFadeFinish; }
		void	SetStopOnFadeFinish( const bool Value )		{ m_Flag.bStopOnFadeFinish = Value; }

		//========================================================================
		//	FreeOnFadeFinish
		//========================================================================
		bool	IsFreeOnFadeFinish() const					{ return m_Flag.bFreeOnFadeFinish; }
		void	SetFreeOnFadeFinish( const bool Value )		{ m_Flag.bFreeOnFadeFinish = Value; }

		//========================================================================
		//	DeleteWhenFinished
		//========================================================================
		bool	IsDeleteWhenFinished() const				{ return m_Flag.bDeleteWhenFinished; }
		void	SetDeleteWhenFinished( const bool Value )	{ m_Flag.bDeleteWhenFinished = Value; }

		//========================================================================
		//	Localize
		//========================================================================
		bool	IsLocalize() const							{ return m_Flag.bLocalize; }
		void	SetLocalize( const bool Value )				{ m_Flag.bLocalize = Value; 
																if (!Value) m_VolumeLocalizeFactor = 1.0f;}

		//========================================================================
		//	ShouldStart
		//========================================================================
		bool	IsShouldStart() const						{ return m_Flag.bShouldStart; }
		void	SetShouldStart( const bool Value )			{ m_Flag.bShouldStart = Value; }

		//========================================================================
		//	VolumeChanged
		//========================================================================
		bool	IsVolumeChanged() const						{ return m_Flag.bVolumeChanged; }
		void	SetVolumeChanged( const bool Value )		{ m_Flag.bVolumeChanged = Value; }

		//========================================================================
		// SetLocalize2D
		//========================================================================
		bool	IsLocalize2D() const						{ return m_Flag.bLocalize2D; }
		void	SetLocalize2D( const bool Value )			{ m_Flag.bLocalize2D = Value; }

	private:

		//========================================================================
		//	CreateBuffer()
		//========================================================================
		virtual void	CreateBuffer();

		//========================================================================
		//	LoadBuffer()
		//========================================================================
		virtual void	LoadBuffer( gfFileBin * pFileStream );

		//========================================================================
		//	Init()
		//========================================================================
		void	Init();

		//========================================================================
		//	read the wav header and fill in m_Format, m_DataSize, m_StartPos,
		//	and m_BufferSize.
		//========================================================================
		int read_wav_header(gfFileBin& io_SoundFile);

	private:
		//========================================================================
		//========================================================================
		struct
		{
			bool	bLoaded				:1;
			bool	bPaused				:1;
			bool	bFinished			:1;
			bool	bLooping			:1;
			bool	bFrequencyChanging	:1;
			bool	bPanChanging		:1;
			bool	bFadeChanging		:1;
			bool	bStopOnFadeFinish	:1;
			bool	bFreeOnFadeFinish	:1;
			bool	bDeleteWhenFinished	:1;
			bool	bLocalize			:1;
			bool	bShouldStart		:1;
			bool	bVolumeChanged		:1;
			bool    bLocalize2D			:1;
		} m_Flag;

		//	Volume
		//
		//		VolumeFactor		- scalar used by sound effect functions (like fade)
		//		VolumeTypeFactor	- scalar used by sound types (i.e. tank sounds)
		//		VolumeGlobalFactor	- scalar used by preferences
		//
		float	m_Volume;					// from 0.0 (off) to 100.0 (full volume)
		float	m_VolumeFactor;				// from 0.0 to 1.0 scalar
		float	m_VolumeLocalizeFactor;		// from 0.0 to 1.0 scalar
		float	m_VolumeTypeFactor;			// from 0.0 to 1.0 scalar
		float	m_VolumeGlobalFactor;		// from 0.0 to 1.0 scalar

		//	Frequency
		//
		unsigned long	m_FrequencyOriginal;	// ranges from 100 to 100,000

		float	m_FrequencyFactor;			// factor from 0.0 to 10.0
		float	m_FrequencyStartFactor;		// factor at start
		float	m_FrequencyTargetFactor;	// target factor from 0.0 to 10.0
		float	m_FrequencyStartTime;		// when did we start changing
		float	m_FrequencyTime;			// how long should we change

		//	Pan information
		//
		float	m_Pan;				// ranges from -1.0 ( full left ) to 1.0 ( full right )

		float	m_PanPosition;		// factor from -1.0 to 1.0
		float	m_PanStartPosition;	// volume factor at start
		float	m_PanStartTime;		// how long have we Pand so far
		float	m_PanTime;			// how long to Pan

		//	Fade information
		//
		float	m_FadeFactor;		// factor from 0.0 to 1.0
		float	m_FadeStartFactor;	// volume factor at start
		float	m_FadeStartTime;	// how long have we faded so far
		float	m_FadeTime;			// how long to fade
		bool	m_bNewFadeSet;		//	has someone set the fadeto?
		float	m_NewFadeFactor;	// factor from 0.0 to 1.0
		float	m_NewFadeTime;		// how long to fade

		//	Looping variables
		//
		int		m_Loops;			// How many loops this sound should perform, 0 is forever
		int		m_LoopCurrent;		// How many loops this sound should perform, 0 is forever

		float	m_PlayTime;			//	Play time - how long to play for
		float	m_fTotalTime;		// total time of the sound file

		maVector3d	m_Position;				//	Sound Position
		maVector3d	m_ListenerPosition;		//	Position of listener
		maRotation	m_ListenerOrientation;	//	Orientation of listener
		float		m_SoundDistance;		//	distance the sound plays at full volume
		float		m_FalloffDistance;		//	distance the sound cuts off at

		//	Saved info on the begining of the file.  Replaces Push and poping the file
		//	Also save the start time
		//
		unsigned long	m_StartPos;
		float			m_StartTime;

		// the position to which the play cursor should be repositioned
		// or from which sound streams should be restreamed
		//
		unsigned long	m_RestorePosition;

		// the size of the sound's wav data ( will be > m_BufferSize for streaming sounds )
		//
		int				m_DataSize;
		int				m_BufferSize;	// the size of the directsound buffer

		unsigned long	m_Status;		// buffer status

		// the wav file header info struct
		//
		WAVEFORMATEX	m_Format;

		fsLocator		m_Filename;

		IDirectSoundBuffer*	m_pBuffer;
		snSoundJob*			m_pParentSoundJob;
};


//------------------------------------------------------------------------
//	In-line functions
//------------------------------------------------------------------------


//========================================================================
//	SetFilename()
//========================================================================
void	
snSoundJob2DPAC::SetFilename( const fsLocator& Filename )
{
	m_Filename = Filename;
}


//========================================================================
//	GetFilename()
//========================================================================
const fsLocator &
snSoundJob2DPAC::GetFilename() const
{
	return m_Filename;
}


