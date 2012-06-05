/****************************************************************************\
**  snSoundJobMP3PACDShow.hpp
**
**      snSoundJobMP3PACDShow.hpp implements the windows portion of the
**	snSoundJobMP3PAC.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef SN_SOUNDDATAMP3PACDSHOW_HPP
#error snSoundJobMP3PACDShow.hpp multiply included
#endif
#define SN_SOUNDDATAMP3PACDSHOW_HPP

#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif

//#include <windows.h>
//#include <mmsystem.h>


//================================================================================
//	Forward references
//================================================================================
//class fsFileStream;
class snSoundJob;


//================================================================================
//================================================================================
class snSoundJobMP3PAC
{
	public:
	//----------------------------------------------------------------------------
	//	Constructors and Destructors
	//----------------------------------------------------------------------------

		//========================================================================
		//	default and copy constructors
		//========================================================================
		snSoundJobMP3PAC();
		snSoundJobMP3PAC( const snSoundJobMP3PAC& i_CopyFrom );

		//========================================================================
		//========================================================================
		~snSoundJobMP3PAC();


	//----------------------------------------------------------------------------
	//	Functions
	//----------------------------------------------------------------------------

		//========================================================================
		//	Load()
		//========================================================================
		virtual void Load();

		//========================================================================
		//	Free()
		//========================================================================
		virtual void Free();

		//========================================================================
		//	Start()
		//========================================================================
		virtual void Start( float i_SimulationTime );

		//========================================================================
		//	Stop()
		//========================================================================
		virtual void Stop();

		//========================================================================
		//	Think()
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
		//	Pause()
		//========================================================================
		virtual void Pause();

		//========================================================================
		//	Resume()
		//========================================================================
		virtual void Resume( float i_SimulationTime );

		//========================================================================
		//	Volume
		//
		//	The volume can range from 0.0 (silent) to 1.0 (full-on)
		//========================================================================
		void		SetVolume( const float i_NewVolume );
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
		//	VolumeGlobalFactor
		//
		//	The volume factor (percentage) can range from 0.0 (0%) to 1.0 (100 %)
		//========================================================================
		void		SetVolumeGlobalFactor( const float i_Value );
		float		GetVolumeGlobalFactor() const;

		//========================================================================
		//	SetVolumeTypeFactor()
		//
		//	The volume factor (percentage) can range from 0.0 (0%) to 1.0 (100 %)
		//========================================================================
		void		SetVolumeTypeFactor( const float i_Value );

		//========================================================================
		//	GetVolumeTypeFactor()
		//
		//	The volume factor (percentage) can range from 0.0 (0%) to 1.0 (100 %)
		//========================================================================
		float		GetVolumeTypeFactor() const;

		//========================================================================
		//	GetVolumeFinal()
		//
		//	The final volume value based on the base volume and all factors.
		//========================================================================
		float		GetVolumeFinal() const;

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
		//	SetLoops()
		//
		//	Set the number of loops to play a sound.
		//	-1 (or any negative number) means infinite.
		//
		//	Note: currently it only signifies whether a sound is looping or not.
		//	it doesn't do counting.
		//========================================================================
		void		SetLoops( const int i_Value );

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
		//	SetParentSoundJob()
		//
		//	set the ParentSoundJob
		//========================================================================
		void		SetParentSoundJob( snSoundJob *i_ParentSoundJob)	{m_ParentSoundJob = i_ParentSoundJob;}

		//========================================================================
		//	GetParentSoundJob()
		//
		//	return: ParentSoundJob
		//========================================================================
		snSoundJob *GetParentSoundJob() const	{return m_ParentSoundJob;}
		
	//----------------------------------------------------------------------------
	//	Flag Accessor functions
	//----------------------------------------------------------------------------

		//========================================================================
		//	Loaded
		//========================================================================
		bool	IsLoaded() const						{ return m_Flag.bLoaded; }
		void	SetLoaded( const bool Value )			{ m_Flag.bLoaded = Value; }

		//========================================================================
		//	Paused
		//========================================================================
		bool	IsPaused() const						{ return m_Flag.bPaused; }
		void	SetPaused( const bool Value )			{ m_Flag.bPaused = Value; }

		//========================================================================
		//	Looping
		//========================================================================
		bool	IsLooping() const							{ return m_Flag.bLooping; }
		void	SetLooping( const bool Value )				{ m_Flag.bLooping = Value; }

		//========================================================================
		//	DeleteWhenFinished
		//========================================================================
		bool	IsDeleteWhenFinished() const				{ return m_Flag.bDeleteWhenFinished; }
		void	SetDeleteWhenFinished( const bool Value )	{ m_Flag.bDeleteWhenFinished = Value; }

		//========================================================================
		//	Finished
		//========================================================================
		bool	IsFinished() const							{ return m_Flag.bFinished; }
		void	SetFinished( const bool Value )				{ m_Flag.bFinished = Value; }

		//========================================================================
		//	FadeChanging
		//========================================================================
		bool	IsFadeChanging() const						{ return m_Flag.bFadeChanging; }
		void	SetFadeChanging( const bool Value )			{ m_Flag.bFadeChanging = Value; }

		//========================================================================
		//	VolumeChanged
		//========================================================================
		bool	IsVolumeChanged() const						{ return m_Flag.bVolumeChanged; }
		void	SetVolumeChanged( const bool Value )		{ m_Flag.bVolumeChanged = Value; }


	private:
		//========================================================================
		//	HandleEvent()
		//========================================================================
		void	HandleEvent( float i_SimulationTime );

		//========================================================================
		//	Init()
		//========================================================================
		void	Init();

	private:
		//========================================================================
		//========================================================================
		struct
		{
			bool	bLoaded				:1;
			bool	bPaused				:1;
			bool	bLooping			:1;
			bool	bDeleteWhenFinished	:1;
			bool	bFinished			:1;
			bool	bFadeChanging		:1;
			bool	bVolumeChanged		:1;
		} m_Flag;

		fsLocator		m_Filename;

		//	Looping variables
		//
		int		m_Loops;			// How many loops this sound should perform, 0 is forever
		int		m_LoopCurrent;		// current loop count (decrement)

		//	Volume
		//
		//		VolumeFactor		- scalar used by sound effect functions (like fade)
		//		VolumeTypeFactor	- scalar used by sound types (i.e. tank sounds)
		//		VolumeGlobalFactor	- scalar used by preferences
		//
		float	m_Volume;					// from 0.0 (off) to 100.0 (full volume)
		float	m_VolumeFactor;				// from 0.0 to 1.0 scalar
		//float	m_VolumeLocalizeFactor;		// from 0.0 to 1.0 scalar
		float	m_VolumeTypeFactor;			// from 0.0 to 1.0 scalar
		float	m_VolumeGlobalFactor;		// from 0.0 to 1.0 scalar

		//	Fade information
		//
		float	m_FadeFactor;		// factor from 0.0 to 1.0
		float	m_FadeStartFactor;	// volume factor at start
		float	m_FadeStartTime;	// how long have we faded so far
		float	m_FadeTime;			// how long to fade

		//	time
		//
		float	m_LastStateCheck;
		snSoundJob *m_ParentSoundJob;
};


//------------------------------------------------------------------------
//	In-line functions
//------------------------------------------------------------------------


//========================================================================
//	SetFilename()
//========================================================================
void	
snSoundJobMP3PAC::SetFilename( const fsLocator& Filename )
{
	m_Filename = Filename;
}


//========================================================================
//	GetFilename()
//========================================================================
const fsLocator &
snSoundJobMP3PAC::GetFilename() const
{
	return m_Filename;
}

