/****************************************************************************\
**  snSoundJob2D.hpp
**
**      snSoundJob2D.hpp defines the snSoundJob2D class
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef SN_SOUNDJOB2D_HPP
#error snSoundJob2D.hpp multiply included
#endif
#define SN_SOUNDJOB2D_HPP

#ifndef SN_SOUNDJOB_HPP
#include "AudioDS/sn/snSoundJob.hpp"
#endif


//============================================================================
//	Forward References
//============================================================================
class	snSoundJob2DPAC;
class	fsLocator;
class	maVector3d;
class	maRotation;


//============================================================================
//============================================================================
class snSoundJob2D : public snSoundJob
{
	protected:
		//========================================================================
		//	constructor called by children
		//========================================================================
		snSoundJob2D( snSoundJob2DPAC * i_pSoundData );

	public:
	//----------------------------------------------------------------------------
	//	Constructors and Destructors
	//----------------------------------------------------------------------------

		//========================================================================
		//	default and copy constructors
		//========================================================================
		snSoundJob2D();
		snSoundJob2D( const snSoundJob2D& i_CopyFrom );

		//========================================================================
		//========================================================================
		virtual ~snSoundJob2D();


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
		//	Reload()
		//
		//		Reload the raw sound data and resume the sound
		//========================================================================
		virtual void Reload();

		//========================================================================
		//	Unload()
		//
		//		Pause the sound and unload the raw sound data
		//========================================================================
		virtual void Unload(bool i_bReloadOnStart = false);

		//========================================================================
		//	Start()
		//========================================================================
		virtual void Start( float i_SimulationTime );

		//========================================================================
		//	Restart()
		//========================================================================
		virtual void Restart( float i_SimulationTime );

		//========================================================================
		//	Stop()
		//========================================================================
		virtual void Stop();

		//========================================================================
		//	Think()
		//========================================================================
		virtual void Think( float i_SimulationTime );

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
	//	Overloaded Operators
	//----------------------------------------------------------------------------

		//========================================================================
		//	Assignment (=)
		//========================================================================
		snSoundJob2D& operator = ( const snSoundJob2D& i_SoundJob );


	//----------------------------------------------------------------------------
	//	Access Functions
	//----------------------------------------------------------------------------

		//========================================================================
		//	GetDataSize()
		//
		//	Returns the size of sound data.
		//========================================================================
		int			GetDataSize() const;

		//========================================================================
		//	Filename
		//========================================================================
		void				SetFilename( const fsLocator & Filename );
		const fsLocator &	GetFilename() const;

		//========================================================================
		//	Volume
		//
		//	The volume can range from 0.0 (silent) to 1.0 (full-on)
		//========================================================================
		void		SetVolume( const float i_NewVolume );
		float		GetVolume() const;

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
		//	SetFrequency()
		//
		//		Change the sound Frequency to a specific level.  This 
		//	level is a factor from -10.0 to 10.0.
		//
		//	The ORIGINAL sound frequency will be converted based on this factor
		//	or multiplier.  So if the multiplier if 1.0 the sound is doubled, 2.0
		//	is tripled, -1.0 is halved, and 0.0 is back to the original.
		//========================================================================
		void		SetFrequency( float i_Value );

		//========================================================================
		//	GetFrequency()
		//
		//	Get the sound Frequency.  This level is a factor from -10.0 to 10.0.
		//========================================================================
		float		GetFrequency() const;

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
		void		SetPan( float i_Value );

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
		void		SetFadeFactor( float i_Value );

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
		//	SetLoopCurrent()
		//
		//	The current loop count.
		//========================================================================
		void		SetLoopCurrent( const int i_Value );

		//========================================================================
		//	GetLoopCurrent()
		//
		//	return: The current loop count.
		//========================================================================
		int			GetLoopCurrent() const;

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

		//========================================================================
		//	DeleteWhenFinished
		//========================================================================
		inline virtual void		SetDeleteWhenFinished( const bool i_bValue );
		inline virtual bool		IsDeleteWhenFinished() const;

		//========================================================================
		//	Finished
		//========================================================================
		inline virtual bool		IsFinished() const;
		inline virtual void		SetFinished( const bool Value );

		//========================================================================
		//	Localize
		//========================================================================
		inline virtual void		SetLocalize( const bool i_bValue );
		inline virtual bool		IsLocalize() const;

		//========================================================================
		// Loacalize2D
		//========================================================================
		inline virtual void		SetLocalize2D( const bool i_bValue );
		inline virtual bool		IsLocalize2D() const;

		//========================================================================
		//	StopOnFadeFinish
		//========================================================================
		inline virtual bool	IsStopOnFadeFinish() const;
		inline virtual void	SetStopOnFadeFinish( const bool Value );

		//========================================================================
		//	Paused
		//========================================================================
		bool	IsPaused() const;

	public:
		//========================================================================
		//	GetPAC()
		//
		//	GetPAC is for Terawatt PAC components which need to interact with
		//	parts of the private implemenation of the snSoundJob2D.  You
		//	shouldn't need to call this function yourself unless you are
		//	implementing Terawatt PAC stuff.  If you find
		//	yourself wanting to, it means that there was something not
		//	addressed in the Sn design and you should fix it.
		//========================================================================
		snSoundJob2DPAC *	GetPAC() const;

		//========================================================================
		//	SetPAC()
		//
		//	SetPAC is for Terawatt PAC components which needs to set the PAC.
		//========================================================================
		void SetPAC( snSoundJob2DPAC * pData );

	private:
		//========================================================================
		//	InitData()
		//========================================================================
		virtual void	InitData();

		//========================================================================
		//	Init()
		//========================================================================
		void	Init();


	//----------------------------------------------------------------------------
	//	Data
	//----------------------------------------------------------------------------

		//	Sound Data
		//
		snSoundJob2DPAC * m_pPAC;
};
