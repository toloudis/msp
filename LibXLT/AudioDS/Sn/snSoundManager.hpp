/****************************************************************************\
**	snSoundManager.hpp
**
**	The sound manager (SM) is a singleton that is in charge of managing the 
**	sounds of a game.  It handles the creating, playing, and organizing for
**	all the sounds in the game no matter what type they are.
**	
**	The basic foundation of the SM is two lists:  one to hold sounds that a
**	game may want to preload in memory at the beginning of a game and then
**	clone copies of it during play for faster access and the other is a
**	list that holds the currently playing sounds.  The playing sounds will
**	be allowed to think when the SM thinks.
**
**	Note:  The SM does NOT cover interactive music.
**	
**	StudioGPU
**	Copyright(C) 2003-10 - All Rights Reserved
\****************************************************************************/
#ifdef SN_SOUNDMANAGER_HPP
#error snSoundManager.hpp multiply included
#endif
#define SN_SOUNDMANAGER_HPP

#include <vector>


//============================================================================
//============================================================================
class snSoundJob;
class snSoundJob2D;
class snSoundJob2DStreamed;
class snSoundJob3D;
class snSoundJob3DStreamed;
class snSoundJobMIDI;
class snSoundJobMP3;
class snSoundJobRedbook;
class fsLocator;
class itString;
class maVector3d;
class maRotation;


//============================================================================
//============================================================================
namespace snSoundManager
{
//----------------------------------------------------------------------------
//	Sound types
//----------------------------------------------------------------------------
	enum
	{
		SOUNDTYPE_NONE		= 0x00,
		SOUNDTYPE_EFFECT	= 0x01,
		SOUNDTYPE_VOICEOVER	= 0x02,
		SOUNDTYPE_AMBIENT	= 0x04,
		SOUNDTYPE_MUSIC		= 0x08,
		SOUNDTYPE_OTHER		= 0x10,
		SOUNDTYPE_INTERFACE	= 0x20,
		SOUNDTYPE_ALL		= 0xFF,
	};

	const int SOUNDTYPES	= 7;	// number of valid sound type flags above ( don't count _ALL )

//----------------------------------------------------------------------------
//	Sound play modes
//----------------------------------------------------------------------------
	enum
	{
		SOUNDMODE_EXCLUSIVE			= 0x1,
		SOUNDMODE_EXCLUSIVE_RESTART	= 0x2,
		SOUNDMODE_NOTEXCLUSIVE		= 0x3,
	};

//----------------------------------------------------------------------------
//	snSoundManager Functions
//----------------------------------------------------------------------------

	//------------------------------------------------------------------------
	//	Initialize()
	//
	//		Initialize the sound manager
	//------------------------------------------------------------------------
	void Initialize();

	//------------------------------------------------------------------------
	//	DeInitialize()
	//
	//		Deinitialize the sound manager
	//------------------------------------------------------------------------
	void DeInitialize();

	//------------------------------------------------------------------------
	//	DumpSoundList()
	//
	//	Displays all sound effects infomation.
	//------------------------------------------------------------------------
	void DumpSoundList();

	//------------------------------------------------------------------------
	//	Load()
	//
	//		Load all static sounds and sound jobs
	//------------------------------------------------------------------------
	void Load();

	//------------------------------------------------------------------------
	//	Free()
	//
	//		Free all static sounds and sound jobs
	//------------------------------------------------------------------------
	void Free();

	//------------------------------------------------------------------------
	//	Delete()
	//
	//		Delete all static sounds and sound jobs
	//------------------------------------------------------------------------
	void Delete();

	//------------------------------------------------------------------------
	//	Think()
	//
	//		Let all the sound jobs think
	//------------------------------------------------------------------------
	void Think( float i_SimulationTime );

	//------------------------------------------------------------------------
	//	Stop()
	//
	//		Stop all the sound jobs
	//------------------------------------------------------------------------
	void Stop();

	//------------------------------------------------------------------------
	//	Pause()
	//
	//		Pause all the sound jobs
	//------------------------------------------------------------------------
	void Pause();

	//------------------------------------------------------------------------
	//	Resume()
	//
	//		Resume all the sound jobs
	//------------------------------------------------------------------------
	void Resume( float i_SimulationTime );

	//------------------------------------------------------------------------
	//	LoadSound()
	//
	//	Load a sound and handle any exceptions
	//------------------------------------------------------------------------
	void LoadSound( snSoundJob * i_pSound );

	//------------------------------------------------------------------------
	//	GetSoundTypeVolumeGlobalFactor()
	//
	//	Get the default volume global factor for a specific sound type.
	//------------------------------------------------------------------------
	const float GetSoundTypeVolumeGlobalFactor();

	//------------------------------------------------------------------------
	//	GetSoundTypeVolumeTypeFactor()
	//
	//	Get the default volume type factor for a specific sound type.
	//------------------------------------------------------------------------
	const float GetSoundTypeVolumeTypeFactor( const int i_eSoundType );

	//------------------------------------------------------------------------
	//	SetSoundTypeVolumeGlobalFactor()
	//
	//	Set the default volume global factor for a specific sound type.
	//------------------------------------------------------------------------
	void SetSoundTypeVolumeGlobalFactor( const float i_Factor );

	//------------------------------------------------------------------------
	//	SetSoundTypeVolumeTypeFactor()
	//
	//	Set the default volume type factor for a specific sound type.
	//------------------------------------------------------------------------
	void SetSoundTypeVolumeTypeFactor( const float i_Factor, const int i_eSoundType );

	//------------------------------------------------------------------------
	//	SetupSoundJob()
	//
	//	Set up the sound job with default values (factors, etc.)  This can
	//	be called if the user creates a sound *not* using the Sound Manager.
	//	If the sound is created using the SM then this is automatically called.
	//------------------------------------------------------------------------
	void SetupSoundJob( snSoundJob * i_pSoundJob );


//============================================================================
//	PreloadSound List Functions
//============================================================================

	//------------------------------------------------------------------------
	//	PreloadSoundsLoad()
	//
	//		Load all Preload sounds
	//------------------------------------------------------------------------
	void PreloadSoundsLoad();

	//------------------------------------------------------------------------
	//	PreloadSoundsFree()
	//
	//		Free all Preload sounds
	//------------------------------------------------------------------------
	void PreloadSoundsFree();

	//------------------------------------------------------------------------
	//	PreloadSoundsDelete()
	//
	//		Delete all Preload sounds
	//------------------------------------------------------------------------
	void PreloadSoundsDelete();

	//------------------------------------------------------------------------
	//	AddPreloadSound()
	//
	//		Add a Preload sound
	//------------------------------------------------------------------------
	void AddPreloadSound( snSoundJob * i_pSound );

	//------------------------------------------------------------------------
	//	RemovePreloadSound( snSoundJob * )
	//
	//		Remove a sound based on a pointer
	//------------------------------------------------------------------------
	snSoundJob * RemovePreloadSound( const snSoundJob * i_pPreloadSoundToRemove );

	//------------------------------------------------------------------------
	//	RemovePreloadSound( itString & )
	//
	//		Remove a sound based on a string
	//------------------------------------------------------------------------
	snSoundJob * RemovePreloadSound( const itString & i_PreloadSoundToRemove );

	//------------------------------------------------------------------------
	//	DeletePreloadSound( snSoundJob * )
	//
	//		Delete a sound based on a pointer
	//------------------------------------------------------------------------
	void DeletePreloadSound( const snSoundJob * i_pPreloadSoundToDelete );

	//------------------------------------------------------------------------
	//	DeletePreloadSound( itString & )
	//
	//		Delete a sound based on a string
	//------------------------------------------------------------------------
	void DeletePreloadSound( const itString & i_PreloadSoundToDelete );

	//------------------------------------------------------------------------
	//	FindPreloadSound( snSoundJob * )
	//
	//		Find a sound based on a pointer
	//------------------------------------------------------------------------
	snSoundJob * FindPreloadSound( const snSoundJob * i_pPreloadSoundToFind );

	//------------------------------------------------------------------------
	//	FindPreloadSound( itString & )
	//
	//	Find a sound based on a string
	//------------------------------------------------------------------------
	snSoundJob * FindPreloadSound( const itString & i_PreloadSoundToFind );

	//------------------------------------------------------------------------
	//	CreatePreloadSound2DStatic()
	//
	//	Create a sound that is a 2-D static sound
	//	and add it to the preload sound list.  A pointer is given to the sound
	//	so it can be preconfigured.
	//------------------------------------------------------------------------
	snSoundJob2D * CreatePreloadSound2DStatic( const fsLocator& i_Filename,
												const itString& i_Name );

	//------------------------------------------------------------------------
	//	CreatePreloadSound3DStatic()
	//
	//	Create a sound that is a 3-D static sound
	//	and add it to the preload sound list.  A pointer is given to the sound
	//	so it can be preconfigured.
	//------------------------------------------------------------------------
	snSoundJob3D * CreatePreloadSound3DStatic( const fsLocator& i_Filename,
												const itString& i_Name );

	//------------------------------------------------------------------------
	//	ClonePreloadSound( itString & )
	//
	//	Clone a sound based on a string and add it to the sound job list.  
	//	Return the cloned sound job *or* return NULL is the sound isn't in 
	//	the pre-load list
	//------------------------------------------------------------------------
	snSoundJob * ClonePreloadSound( const itString & i_PreloadSoundToClone );

	//------------------------------------------------------------------------
	//	ClonePreloadSound( snSoundJob* )
	//
	//	Clone a sound based on a pointer and add it to the sound job list.  
	//	Return the cloned sound job *or* return NULL is the sound isn't in 
	//	the pre-load list
	//------------------------------------------------------------------------
	snSoundJob * ClonePreloadSound( const snSoundJob * i_pPreloadSoundToFind );

	//------------------------------------------------------------------------
	//	return the number of preloaded sounds
	//------------------------------------------------------------------------
	const int GetPreloadSoundCount();


//============================================================================
//	SoundJob List Functions
//============================================================================

	//------------------------------------------------------------------------
	//	SoundJobsLoad()
	//
	//		Load all sound jobs
	//------------------------------------------------------------------------
	void SoundJobsLoad();

	//------------------------------------------------------------------------
	//	SoundJobsFree()
	//
	//		Free all sound jobs
	//------------------------------------------------------------------------
	void SoundJobsFree();

	//------------------------------------------------------------------------
	//	SoundJobsDelete()
	//
	//		Delete all sound jobs
	//------------------------------------------------------------------------
	void SoundJobsDelete();

	//------------------------------------------------------------------------
	//	SoundJobsStop()
	//
	//		Stop all sound jobs
	//------------------------------------------------------------------------
	void SoundJobsStop();

	//------------------------------------------------------------------------
	//	SoundJobsPause()
	//
	//		Pause all sound jobs
	//------------------------------------------------------------------------
	void SoundJobsPause();

	//------------------------------------------------------------------------
	//	SoundJobsResume()
	//
	//		Resume all sound jobs
	//------------------------------------------------------------------------
	void SoundJobsResume( float i_SimulationTime );

	//------------------------------------------------------------------------
	//	SetSoundJobsListenerPosition()
	//
	//	physical location of listener in world (used by localize)
	//
	//	Note: this only affects sounds with the "localize" set to true
	//------------------------------------------------------------------------
	void SetSoundJobsListenerPosition( const maVector3d& i_Position );

	//------------------------------------------------------------------------
	//	GetSoundJobsListenerPosition()
	//
	//	physical location of listener in world (used by localize)
	//
	//	Note: this only affects sounds with the "localize" set to true
	//	Note: this returns the orientation of the FIRST localized job it hits
	//------------------------------------------------------------------------
	maVector3d GetSoundJobsListenerPosition();

	//------------------------------------------------------------------------
	//	SetSoundJobsListenerOrientation()
	//
	//	physical orientation of listener in world (used by localize)
	//
	//	Note: this only affects sounds with the "localize" set to true
	//------------------------------------------------------------------------
	void SetSoundJobsListenerOrientation( const maRotation& i_Value );

	//------------------------------------------------------------------------
	//	GetSoundJobsListenerOrientation()
	//
	//	physical location of listener in world (used by localize)
	//
	//	Note: this only affects sounds with the "localize" set to true
	//	Note: this returns the orientation of the FIRST localized job it hits
	//------------------------------------------------------------------------
	maRotation GetSoundJobsListenerOrientation();

	//------------------------------------------------------------------------
	//	AddSoundJob()
	//
	//		Add a sound job 
	//------------------------------------------------------------------------
	void AddSoundJob( snSoundJob * i_pSoundJob );

	//------------------------------------------------------------------------
	//	RemoveSoundJob( snSoundJob * )
	//
	//		Remove a soundjob based on a pointer
	//------------------------------------------------------------------------
	snSoundJob * RemoveSoundJob( const snSoundJob * i_pSoundJobToRemove );

	//------------------------------------------------------------------------
	//	RemoveSoundJob( itString & )
	//
	//		Remove a soundjob based on a pointer
	//------------------------------------------------------------------------
	snSoundJob * RemoveSoundJob( const itString & i_SoundJobToRemove );

	//------------------------------------------------------------------------
	//	DeleteSoundJob( snSoundJob * )
	//
	//		Delete a soundjob based on a pointer
	//------------------------------------------------------------------------
	void DeleteSoundJob( const snSoundJob * i_pSoundJobToDelete );

	//------------------------------------------------------------------------
	//	DeleteSoundJob( itString & )
	//
	//		Delete a soundjob based on a pointer
	//------------------------------------------------------------------------
	void DeleteSoundJob( const itString & i_SoundJobToDelete );

	//------------------------------------------------------------------------
	//	FindSoundJob( snSoundJob * )
	//
	//		Find a sound job based on a pointer
	//------------------------------------------------------------------------
	snSoundJob * FindSoundJob( const snSoundJob * i_pSoundJobToFind );

	//------------------------------------------------------------------------
	//	FindSoundJob( itString & )
	//
	//		Find a sound job based on a pointer
	//------------------------------------------------------------------------
	snSoundJob * FindSoundJob( const itString & i_SoundJobToFind );

	//------------------------------------------------------------------------
	//	FindSoundJobs( itString & )
	//
	//		Finds all sound jobs based on a name
	//------------------------------------------------------------------------
	void FindSoundJobs( const itString & i_SoundJobToFind, std::vector<snSoundJob *>& o_Jobs );

	//------------------------------------------------------------------------
	//	FindSoundJob( int )
	//
	//		Find the first sound job based on a SOUNDTYPE_
	//------------------------------------------------------------------------
	snSoundJob * FindSoundJob( int i_eSoundType, bool i_bMustBePlaying = true );

	//------------------------------------------------------------------------
	//	CreateSoundJob2DStatic()
	//
	//		Create a sound job that is a 2-D static sound
	//	and add it to the sound job list.  A pointer is given to the sound
	//	so it can be played or its features can be altered.
	//
	//	If the flag, i_bCreateFromPreloadIfPossible, is set for the function
	//	it will try to clone it from the preload list if it exists.
	//------------------------------------------------------------------------
	snSoundJob2D * CreateSoundJob2DStatic( const fsLocator&	i_Filename,
											const itString&		i_Name,
											const bool			i_bCreateFromPreloadIfPossible = true );

	//------------------------------------------------------------------------
	//	CreateSoundJob2DStreamed()
	//
	//		Create a sound job that is a 2-D streamed sound
	//	and add it to the sound job list.  A pointer is given to the sound
	//	so it can be played or its features can be altered.
	//------------------------------------------------------------------------
	snSoundJob2DStreamed * CreateSoundJob2DStreamed( const fsLocator& i_Filename );

	//------------------------------------------------------------------------
	//	CreateSoundJob3DStatic()
	//
	//		Create a sound job that is a 3-D static sound
	//	and add it to the sound job list.  A pointer is given to the sound
	//	so it can be played or its features can be altered.
	//------------------------------------------------------------------------
	snSoundJob3D * CreateSoundJob3DStatic( const fsLocator& i_Filename,
											const itString& i_Name );

	//------------------------------------------------------------------------
	//	CreateSoundJob3DStreamed()
	//
	//		Create a sound job that is a 3-D streamed sound
	//	and add it to the sound job list.  A pointer is given to the sound
	//	so it can be played or its features can be altered.
	//------------------------------------------------------------------------
	snSoundJob3DStreamed * CreateSoundJob3DStreamed( const fsLocator& i_Filename );

	//------------------------------------------------------------------------
	//	CreateSoundJobMIDI()
	//
	//		Create a sound job that is a MIDI sound
	//	and add it to the sound job list.  A pointer is given to the sound
	//	so it can be played or its features can be altered.
	//------------------------------------------------------------------------
	snSoundJobMIDI * CreateSoundJobMIDI( const fsLocator& i_Filename );

	//------------------------------------------------------------------------
	//	CreateSoundJobMP3()
	//
	//		Create a sound job that is a MP3 sound
	//	and add it to the sound job list.  A pointer is given to the sound
	//	so it can be played or its features can be altered.
	//------------------------------------------------------------------------
	snSoundJobMP3 * CreateSoundJobMP3( const fsLocator& i_Filename );

	//------------------------------------------------------------------------
	//	CreateSoundJobStaticRedbook()
	//
	//		Create a sound job that is a Redbook track
	//	and add it to the sound job list.  A pointer is given to the sound
	//	so it can be played or its features can be altered.
	//------------------------------------------------------------------------
	snSoundJobRedbook * CreateSoundJobRedbook( const fsLocator& i_Filename );

	//------------------------------------------------------------------------
	//	Play2DStatic()
	//
	//	Play a sound job that is a 2-D static sound
	//	and add it to the sound job list.  A pointer is given to the sound
	//	so it can be played or its features can be altered.
	//
	//	If the sound is not pre-loaded, this function will not play a sound
	//	and return NULL.
	//------------------------------------------------------------------------
	snSoundJob2D * Play2DStatic(	const	itString& i_Name,
									float	i_SimulationTime,
									bool	i_bDeleteWhenFinished = true, 
									int		i_eExclusiveFlag = SOUNDMODE_NOTEXCLUSIVE, 
									int		i_eSoundType = SOUNDTYPE_EFFECT, 
									bool	i_bLocalize = false );

	//------------------------------------------------------------------------
	//	Play2DStatic()
	//
	//	Play a sound job that is a 2-D static sound
	//	and add it to the sound job list.  A pointer is given to the sound
	//	so it can be played or its features can be altered.
	//
	//	If the sound is not pre-loaded, this function will not play a sound
	//	and return NULL.
	//------------------------------------------------------------------------
	snSoundJob2D * Play2DStatic(	const	fsLocator& i_Filename,
									float	i_SimulationTime,
									bool	i_bDeleteWhenFinished = true, 
									int		i_eExclusiveFlag = SOUNDMODE_NOTEXCLUSIVE, 
									int		i_eSoundType = SOUNDTYPE_EFFECT, 
									bool	i_bLocalize = false );

	//------------------------------------------------------------------------
	//	Play2DStreamed()
	//
	//	Play a sound job that is a 2-D streamed sound
	//	and add it to the sound job list.  A pointer is given to the sound
	//	so it can be played or its features can be altered.
	//------------------------------------------------------------------------
	snSoundJob2DStreamed * Play2DStreamed(	const	fsLocator& i_Filename,
											float	i_SimulationTime,
											bool	i_bDeleteWhenFinished = true, 
											int		i_eExclusiveFlag = SOUNDMODE_NOTEXCLUSIVE, 
											int		i_eSoundType = SOUNDTYPE_EFFECT, 
											bool	i_bLocalize = false );

	//------------------------------------------------------------------------
	//	Play3DStatic()
	//
	//	Play a sound job that is a 3-D static sound
	//	and add it to the sound job list.  A pointer is given to the sound
	//	so it can be played or its features can be altered.
	//------------------------------------------------------------------------
	snSoundJob3D * Play3DStatic(	const	fsLocator& i_Filename,
									float	i_SimulationTime,
									bool	i_bDeleteWhenFinished = true, 
									int		i_eExclusiveFlag = SOUNDMODE_NOTEXCLUSIVE, 
									int		i_eSoundType = SOUNDTYPE_EFFECT );

	//------------------------------------------------------------------------
	//	Play3DStatic()
	//
	//	Play a sound job that is a 3-D static sound
	//	and add it to the sound job list.  A pointer is given to the sound
	//	so it can be played or its features can be altered.
	//------------------------------------------------------------------------
	snSoundJob3D * Play3DStatic(	const	itString& i_Filename,
									float	i_SimulationTime,
									bool	i_bDeleteWhenFinished = true, 
									int		i_eExclusiveFlag = SOUNDMODE_NOTEXCLUSIVE, 
									int		i_eSoundType = SOUNDTYPE_EFFECT );

	//------------------------------------------------------------------------
	//	Play3DStreamed()
	//
	// Play a sound job that is a 3-D streamed sound
	//	and add it to the sound job list.  A pointer is given to the sound
	//	so it can be played or its features can be altered.
	//------------------------------------------------------------------------
	snSoundJob3DStreamed * Play3DStreamed(	const	fsLocator& i_Filename,
											float	i_SimulationTime,
											bool	i_bDeleteWhenFinished = true, 
											int		i_eExclusiveFlag = SOUNDMODE_NOTEXCLUSIVE, 
											int		i_eSoundType = SOUNDTYPE_EFFECT );

	//------------------------------------------------------------------------
	//	PlayMIDI()
	//
	// Play a sound job that is a MIDI sound
	//	and add it to the sound job list.  A pointer is given to the sound
	//	so it can be played or its features can be altered.
	//------------------------------------------------------------------------
	snSoundJobMIDI * PlayMIDI( const fsLocator& i_Filename,
							   float i_SimulationTime );

	//------------------------------------------------------------------------
	//	PlayMP3()
	//
	// Play a sound job that is a MP3 sound
	//	and add it to the sound job list.  A pointer is given to the sound
	//	so it can be played or its features can be altered.
	//------------------------------------------------------------------------
	snSoundJobMP3 * PlayMP3( const fsLocator& i_Filename,
							 float i_SimulationTime );

	//------------------------------------------------------------------------
	//	PlayRedbook()
	//
	// Play a sound job that is a Redbook track
	//	and add it to the sound job list.  A pointer is given to the sound
	//	so it can be played or its features can be altered.
	//------------------------------------------------------------------------
	snSoundJobRedbook * PlayRedbook( const fsLocator& i_Filename );

	//------------------------------------------------------------------------
	//	UpdateSoundJobVolumeFactors()
	//
	//		Update the volume factors for a specific sound based on the standard.
	//------------------------------------------------------------------------
	void UpdateSoundJobVolumeFactors( snSoundJob * i_pSoundJob );

	//------------------------------------------------------------------------
	//	return the number of sound jobs
	//------------------------------------------------------------------------
	const int GetSoundJobCount();


//============================================================================
//	Functions that affect all sound lists
//============================================================================

	//------------------------------------------------------------------------
	//	ReloadSounds()
	//
	//		Reload all raw sound data from all lists
	//------------------------------------------------------------------------
	void ReloadSounds();

	//------------------------------------------------------------------------
	//	UnloadSounds()
	//
	//		Unload all raw sound data from all lists
	//------------------------------------------------------------------------
	void UnloadSounds();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	bool IsMuted();
	void Mute(bool i_bMute);


//============================================================================
//	Flags
//============================================================================

	//------------------------------------------------------------------------
	//	Initialized		- has the sound manager been initialized yet
	//------------------------------------------------------------------------
	bool	IsInitialized();
	void SetInitialized( const bool Value );

	//------------------------------------------------------------------------
	// 2DLocalizeAs2D -- true by default, set to false to localize all created
	// 2d sounds as mono.
	//------------------------------------------------------------------------
	void Set2DLocalizeAs2D( const bool Value );
	bool Is2dLocalizeAs2D(); 


//============================================================================
//	Package called functions
//============================================================================

	//------------------------------------------------------------------------
	//	Don't call Init() and CleanUp() yourself; they are called 
	//	by the package Init and Cleanup.
	//------------------------------------------------------------------------
	void Init();
	void CleanUp() throw();
}

