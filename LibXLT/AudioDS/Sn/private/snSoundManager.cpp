/****************************************************************************\
**  snSoundManager.hpp
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
**	Copyright(C) 2003-2 - All Rights Reserved
\****************************************************************************/
#include "AudioDS/sn/snSoundManager.hpp"

#include "Core/dbg/dbgMsg.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/it/itStringUtil.hpp"
#include "Core/ma/maRotation.hpp"
#include "AudioDS/sn/snExceptionX.hpp"
#include "AudioDS/sn/snSoundJob.hpp"
#include "AudioDS/sn/snSoundJob2D.hpp"
#include "AudioDS/sn/private/snSoundJob2DPAC.hpp"
#include "AudioDS/sn/snSoundJob2DStreamed.hpp"
//#include "snSoundJob3D.hpp"
//#include "snSoundJob3DStreamed.hpp"
//#include "snSoundJobMIDI.hpp"
#include "AudioDS/sn/private/snSoundManagerPAC.hpp"
#include "AudioDS/sn/snSoundJobMP3.hpp"
#include "AudioDS/sn/snSoundJobRedbook.hpp"
#include "AudioDS/sn/snSoundUtil.hpp"

#include <algorithm>
#include <list>


//============================================================================
//============================================================================
namespace snSoundManager
{

namespace
{

//------------------------------------------------------------------------
//	Data
//------------------------------------------------------------------------

float	m_VolumeGlobalFactor_Old;							// store the old value when muting
float	m_VolumeGlobalFactor;								// from 0.0 to 1.0 scalar
float	m_VolumeTypeFactors[ snSoundManager::SOUNDTYPES ];	// from 0.0 to 1.0 scalar

struct
{
	bool	bInitialized		:1;
	bool	b2DLocalizeAs2D		:1;  // false when 2d sounds should localize mono.
} l_Flag;

//	sound lists
//
std::list<snSoundJob *>	l_PreloadedSounds;
std::list<snSoundJob *> l_SoundJobs;

int l_nSoundMemoryTotal = 0;
void print_sound_info( const snSoundJob* i_pSoundJob )
{
	std::string filename;
	const snSoundJob2D* sound = dynamic_cast<const snSoundJob2D*>(i_pSoundJob);
	if ( sound )
	{
		fsFileUtil::LocatorToANSIFilename( sound->GetFilename(), filename );
		l_nSoundMemoryTotal += sound->GetDataSize();
		DBG_WARNING( "Sound " << filename.c_str() << ": Size(" << (sound->GetDataSize() / 1024) << " KB)");
	}
}

void dump_sound_list()
{
	l_nSoundMemoryTotal = 0;

	DBG_WARNING("Preloaded sound memory usage**********************************************");
	std::for_each( l_PreloadedSounds.begin(), l_PreloadedSounds.end(), print_sound_info );
	DBG_WARNING("Sound memory usage**********************************************");
	std::for_each( l_SoundJobs.begin(), l_SoundJobs.end(), print_sound_info );

	float total = ((float)l_nSoundMemoryTotal) / 1024.0f;  //KB
	total /= 1024.0f; //MB
	DBG_WARNING("Sound memory usage*********************TOTAL********************");
	DBG_WARNING("Sound Memory Usage Total:  " << total << " MB");
	DBG_WARNING("Sound memory usage**********************************************");
}


//========================================================================
//	clone_sound_job
//========================================================================
snSoundJob* 
clone_sound_job( snSoundJob* i_Sound )
{
	try
	{
		snSoundJob* pSoundJob = NULL;
		
		//	found the sound, in the list, so let's clone it and
		//	return a pointer to the new sound job.
		//
		snSoundJobRedbook* pRedbook = dynamic_cast<snSoundJobRedbook *>( i_Sound );
		if ( pRedbook )
		{
			pSoundJob = snSoundUtil::CloneSoundJobRedbook( *pRedbook );
		}
		else
		{
			snSoundJob2DStreamed* pStreamed_2d = dynamic_cast<snSoundJob2DStreamed *>( i_Sound );
			if ( pStreamed_2d )
			{
				pSoundJob = snSoundUtil::CloneSoundJob2DStreamed( *pStreamed_2d );
			}
			else
			{
				snSoundJob2D *pSound_2d = dynamic_cast<snSoundJob2D*>( i_Sound );
				if ( pSound_2d )
				{
					pSoundJob = snSoundUtil::CloneSoundJob2DStatic(*pSound_2d);
				}
				else
				{
					// others?
				}
			}
		}
		return pSoundJob;
	}
	catch ( const snSoundCreateFailedX& i_Ex )
	{
		std::string filename;
		fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);
		DBG_ERROR("Not enough sound memory to load " << filename.c_str() << "." );
		dump_sound_list();
		throw; // bring down the game
	}
}

}	// namespace

//----------------------------------------------------------------------------
//	snSoundManager Functions
//----------------------------------------------------------------------------


//========================================================================
//	Initialize()
//
//		Initialize the sound manager
//========================================================================
void 
Initialize()
{
	Init();
}


//========================================================================
//	DeInitialize()
//
//		Deinitialize the sound manager
//========================================================================
void 
DeInitialize()
{
	Delete();
	CleanUp();
}


//========================================================================
//	DumpSoundList()
//
//	Displays all sound effects infomation.
//========================================================================
void DumpSoundList()
{
	dump_sound_list();
}

//========================================================================
//	Load()
//
//		Load all static sounds and sound jobs
//========================================================================
void	
Load()
{
	PreloadSoundsLoad();
	SoundJobsLoad();
}


//========================================================================
//	Free()
//
//		Free all static sounds and sound jobs
//========================================================================
void	
Free()
{
	PreloadSoundsFree();
	SoundJobsFree();
}


//========================================================================
//	Delete()
//
//		Delete all static sounds and sound jobs
//========================================================================
void	
Delete()
{
	PreloadSoundsDelete();
	SoundJobsDelete();
}


//========================================================================
//	Think()
//
//		Let all the sound jobs think
//========================================================================
void	
Think( float i_SimulationTime )
{
	// platform specific Think().
	snSoundManagerPAC::Think( i_SimulationTime );

	//	iterate through each item
	//
	std::list<snSoundJob *>::iterator it	= l_SoundJobs.begin();
	std::list<snSoundJob *>::iterator end	= l_SoundJobs.end();

	while ( it != end )
	{
		snSoundJob *	pSound	= *it;

		pSound->Think( i_SimulationTime );

		if (   ( pSound->IsFinished() )
			&& ( pSound->IsDeleteWhenFinished() ) )
		{
			std::list<snSoundJob *>::iterator tempIt	= it;
			++it;
			l_SoundJobs.erase( tempIt );
			delete pSound;
		}
		else
		{
			++it;
		}
	}
}


//========================================================================
//	Stop()
//
//		Stop all the sound jobs
//========================================================================
void	
Stop()
{
	SoundJobsStop();
}


//========================================================================
//	Pause()
//
//		Pause all the sound jobs
//========================================================================
void	
Pause()
{
	SoundJobsPause();
}


//========================================================================
//	Resume()
//
//		Resume all the sound jobs
//========================================================================
void	
Resume( float i_SimulationTime )
{
	SoundJobsResume( i_SimulationTime );
}


//========================================================================
//	LoadSound()
//
//	Load a sound and handle any exceptions
//========================================================================
void	
LoadSound( snSoundJob * i_pSound )
{
	try 
	{
		i_pSound->Load();
	} 
	catch ( const fsDirectoryDoesntExistX& ex ) 
	{
		std::string filename;
		fsFileUtil::LocatorToANSIFilename(ex.GetLocator(), filename);
		DBG_WARNING( "Directory does not exist for sound " << filename.c_str() );
	}
	catch ( const fsFileDoesntExistX& ex ) 
	{
		std::string filename;
		fsFileUtil::LocatorToANSIFilename(ex.GetLocator(), filename);
		DBG_WARNING( "File does not exist for sound " << filename.c_str() );
	}
}


//========================================================================
//	CalculateSoundTypeIndex()
//
//	Calculate the sound type index based on sound type mask.
//========================================================================
int 
CalculateSoundTypeIndex( const int i_TypeMask )
{
	int soundTypeIndex;

	soundTypeIndex = (int)(log( (float)i_TypeMask ) / log( 2.0f ) + 0.001f);	// 0.001f because 3.000 != 3

	DBG_ASSERT( soundTypeIndex >= 0 && soundTypeIndex < snSoundManager::SOUNDTYPES, "SoundType invalid for setting volume" );

	if ( soundTypeIndex < 0 )
		soundTypeIndex = 0;
	if ( soundTypeIndex >= snSoundManager::SOUNDTYPES )
		soundTypeIndex = snSoundManager::SOUNDTYPES - 1;

	return soundTypeIndex;
}


//========================================================================
//	UpdateSoundJobVolumeGlobalFactor()
//
//	Update the volume global factor for a specific sound based on
//	the standard.
//========================================================================
void	
UpdateSoundJobVolumeGlobalFactor( snSoundJob * i_pSoundJob )
{
	i_pSoundJob->SetVolumeGlobalFactor( m_VolumeGlobalFactor );
}


//========================================================================
//	UpdateSoundJobVolumeTypeFactor()
//
//	Update the volume Type factor for a specific sound based on
//	the standard.
//========================================================================
void	
UpdateSoundJobVolumeTypeFactor( snSoundJob * i_pSoundJob )
{
	if ( i_pSoundJob->GetTypeMask() == 0x0 )
	{
		i_pSoundJob->SetVolumeTypeFactor( 1.0f );
		return;
	}

	//	Calculate the sound type index
	//
	float	factor;
	int		soundTypeIndex;
	soundTypeIndex	= CalculateSoundTypeIndex( i_pSoundJob->GetTypeMask() );

	factor = m_VolumeTypeFactors[ soundTypeIndex ];
	i_pSoundJob->SetVolumeTypeFactor( factor );
}


//========================================================================
//	UpdateSoundJobVolumeFactors()
//
//		Update the volume factors for a specific sound based on the standard.
//========================================================================
void	
UpdateSoundJobVolumeFactors( snSoundJob * i_pSoundJob )
{
	DBG_ASSERT( i_pSoundJob != NULL, "Null sound job passed to volume factor update function" );

	if ( i_pSoundJob )
	{
		UpdateSoundJobVolumeGlobalFactor( i_pSoundJob );
		UpdateSoundJobVolumeTypeFactor( i_pSoundJob );
	}
}


//========================================================================
//	UpdateSoundJobsVolumeGlobalFactor()
//
//	Update the volume global factor for a specific sound group.
//========================================================================
void	
UpdateSoundJobsVolumeGlobalFactor()
{
	//	First, update the preloaded sounds
	//
	std::list<snSoundJob *>::iterator it	= l_PreloadedSounds.begin();
	std::list<snSoundJob *>::iterator end	= l_PreloadedSounds.end();

	while ( it != end )
	{
		snSoundJob *	pSound	= *it;

		DBG_ASSERT( pSound != NULL, "item in Sound Manager Sound Job list has a NULL pointer to a Sound Job" );

		if ( pSound )
		{
			UpdateSoundJobVolumeGlobalFactor( pSound );
		}

		++it;
	}

	//	Next, update the sound jobs
	//
	it	= l_SoundJobs.begin();
	end	= l_SoundJobs.end();

	while ( it != end )
	{
		snSoundJob *	pSound	= *it;

		DBG_ASSERT( pSound != NULL, "item in Sound Manager Sound Job list has a NULL pointer to a Sound Job" );

		if ( pSound )
		{
			UpdateSoundJobVolumeGlobalFactor( pSound );
		}

		++it;
	}
}


//========================================================================
//	UpdateSoundJobVolumeTypeFactor()
//
//	Update the volume Type factor for a specific sound group.
//========================================================================
void	
UpdateSoundJobsVolumeTypeFactor( const int i_eSoundType )
{
	//	First, update the preloaded sounds
	//
	std::list<snSoundJob *>::iterator it	= l_PreloadedSounds.begin();
	std::list<snSoundJob *>::iterator end	= l_PreloadedSounds.end();

	while ( it != end )
	{
		snSoundJob *	pSound	= *it;

		DBG_ASSERT( pSound != NULL, "item in Sound Manager Sound Job list has a NULL pointer to a Sound Job" );

		if (   ( pSound )
			&& ( pSound->GetTypeMask() == i_eSoundType ) )
		{
			UpdateSoundJobVolumeTypeFactor( pSound );
		}

		++it;
	}

	//	Next, update the sound jobs
	//
	it	= l_SoundJobs.begin();
	end	= l_SoundJobs.end();

	while ( it != end )
	{
		snSoundJob *	pSound	= *it;

		DBG_ASSERT( pSound != NULL, "item in Sound Manager Sound Job list has a NULL pointer to a Sound Job" );

		float temp;
		temp = pSound->GetVolumeTypeFactor();
		temp = pSound->GetVolumeGlobalFactor();

		if (   ( pSound )
			&& ( pSound->GetTypeMask() == i_eSoundType ) )
		{
			UpdateSoundJobVolumeTypeFactor( pSound );
		}

		++it;
	}
}


//========================================================================
//	GetSoundTypeVolumeGlobalFactor()
//
//	Get the default volume global factor for a specific sound type.
//========================================================================
const float	
GetSoundTypeVolumeGlobalFactor()
{
	return m_VolumeGlobalFactor;
}


//========================================================================
//	GetSoundTypeVolumeTypeFactor()
//
//	Get the default volume type factor for a specific sound type.
//========================================================================
const float	
GetSoundTypeVolumeTypeFactor( const int i_eSoundType )
{
	int soundTypeIndex;
	soundTypeIndex	= CalculateSoundTypeIndex( i_eSoundType );

	return m_VolumeTypeFactors[ soundTypeIndex ];
}


//========================================================================
//	SetSoundTypeVolumeGlobalFactor()
//
//	Set the default volume global factor for a specific sound type.
//========================================================================
void	
SetSoundTypeVolumeGlobalFactor( const float i_Factor )
{
	DBG_ASSERT( i_Factor >= 0.0f && i_Factor <= 1.0f, "Sound volume factor out of range" );

	//	Update the default factor
	//
	m_VolumeGlobalFactor		= i_Factor;

	if ( m_VolumeGlobalFactor < 0.0f )
		m_VolumeGlobalFactor = 0.0f;
	if ( m_VolumeGlobalFactor > 1.0f )
		m_VolumeGlobalFactor = 1.0f;

	//	Update all the currently loaded sounds
	//
	UpdateSoundJobsVolumeGlobalFactor();
}


//========================================================================
//	SetSoundTypeVolumeTypeFactor()
//
//	Set the default volume type factor for a specific sound type.
//========================================================================
void
SetSoundTypeVolumeTypeFactor( const float i_Factor, const int i_eSoundType )
{
	//	Calculate the sound type index
	//
	int soundTypeIndex;
	soundTypeIndex	= CalculateSoundTypeIndex( i_eSoundType );

	DBG_ASSERT( i_Factor >= 0.0f && i_Factor <= 1.0f, "Sound volume factor out of range" );

	//	Update the default factor
	//
	if (i_Factor == m_VolumeTypeFactors[soundTypeIndex])
	{
		//don't need to update, already at that value
		return;
	}
	m_VolumeTypeFactors[ soundTypeIndex ]	= i_Factor;

	if ( m_VolumeTypeFactors[ soundTypeIndex ] < 0.0f )
		m_VolumeTypeFactors[ soundTypeIndex ] = 0.0f;
	if ( m_VolumeTypeFactors[ soundTypeIndex ] > 1.0f )
		m_VolumeTypeFactors[ soundTypeIndex ] = 1.0f;

	//	Update all the currently loaded sounds
	//
	UpdateSoundJobsVolumeTypeFactor( i_eSoundType );
}


//========================================================================
//	SetupSoundJob()
//
//	Set up the sound job with default values (factors, etc.)  This can
//	be called if the user creates a sound *not* using the Sound Manager.
//	If the sound is created using the SM then this is automatically called.
//========================================================================
void	
SetupSoundJob( snSoundJob * i_pSoundJob )
{
	UpdateSoundJobVolumeFactors( i_pSoundJob );
}


//========================================================================
//	PreloadSoundsLoad()
//
//		Load all Preload sounds
//========================================================================
void	
PreloadSoundsLoad()
{
	//	iterate through each item
	//
	std::list<snSoundJob *>::iterator it	= l_PreloadedSounds.begin();
	std::list<snSoundJob *>::iterator end	= l_PreloadedSounds.end();

	while ( it != end )
	{
		snSoundJob *	pSound	= *it;

		LoadSound( pSound );

		++it;
	}
}


//========================================================================
//	PreloadSoundsFree()
//
//		Free all Preload sounds
//========================================================================
void	
PreloadSoundsFree()
{
	//	iterate through each item
	//
	std::list<snSoundJob *>::iterator it	= l_PreloadedSounds.begin();
	std::list<snSoundJob *>::iterator end	= l_PreloadedSounds.end();

	while ( it != end )
	{
		snSoundJob *	pSound	= *it;

		pSound->Free();

		++it;
	}
}


//========================================================================
//	PreloadSoundsDelete()
//
//		Delete all Preload sounds
//========================================================================
void	
PreloadSoundsDelete()
{
	//	iterate through each item
	//
	envSTLHelpers::DeleteContainer(l_PreloadedSounds);
/*
	std::list<snSoundJob *>::iterator it	= l_PreloadedSounds.begin();
	std::list<snSoundJob *>::iterator end	= l_PreloadedSounds.end();

	while ( it != end )
	{
		delete *it;
		++it;
	}

	l_PreloadedSounds.clear();*/
}


//========================================================================
//	AddPreloadSound()
//
//		Add a Preload sound
//========================================================================
void	
AddPreloadSound( snSoundJob * i_pSound )
{
	l_PreloadedSounds.push_back( i_pSound );
}


//========================================================================
//	RemovePreloadSound( snSoundJob * )
//
//		Remove a sound based on a pointer
//========================================================================
snSoundJob *	
RemovePreloadSound( const snSoundJob * i_pPreloadSoundToRemove )
{
	snSoundJob *	pSound;

	pSound	= FindPreloadSound( i_pPreloadSoundToRemove );

	if ( pSound )
	{
		//	Found it, so let's remove it.
		//
		l_PreloadedSounds.remove( pSound );

		//	Give the user the sound back so they can deal with it
		//
		return pSound;
	}

	return NULL;
}


//========================================================================
//	RemovePreloadSound( itString & )
//
//		Remove a sound based on a string
//========================================================================
snSoundJob *	
RemovePreloadSound( const itString & i_PreloadSoundToRemove )
{
	snSoundJob *	pSound;

	pSound	= FindPreloadSound( i_PreloadSoundToRemove );

	if ( pSound )
	{
		//	Found it, so let's remove it.
		//
		l_PreloadedSounds.remove( pSound );

		//	Give the user the sound back so they can deal with it
		//
		return pSound;
	}

	return NULL;
}


//========================================================================
//	DeletePreloadSound( snSoundJob * )
//
//		Delete a sound based on a pointer
//========================================================================
void 
DeletePreloadSound( const snSoundJob * i_pPreloadSoundToDelete )
{
	//	iterate through each item
	//
	std::list<snSoundJob *>::iterator it	= l_PreloadedSounds.begin();
	std::list<snSoundJob *>::iterator end	= l_PreloadedSounds.end();

	while ( it != end )
	{
		snSoundJob *	pSound	= *it;

		DBG_ASSERT( pSound != NULL, "item in Sound Manager Preload Sound list has a NULL pointer to a Sound Job" );

		if ( i_pPreloadSoundToDelete == pSound )
		{
			l_PreloadedSounds.erase( it );
			delete pSound;
			break;
		}

		++it;
	}
}


//========================================================================
//	DeletePreloadSound( itString & )
//
//		Delete a sound based on a string
//========================================================================
void	
DeletePreloadSound( const itString & i_PreloadSoundToDelete )
{
	//	iterate through each item
	//
	std::list<snSoundJob *>::iterator it	= l_PreloadedSounds.begin();
	std::list<snSoundJob *>::iterator end	= l_PreloadedSounds.end();

	while ( it != end )
	{
		snSoundJob *	pSound	= *it;

		DBG_ASSERT( pSound != NULL, "item in Sound Manager Preload Sound list has a NULL pointer to a Sound Job" );

		if ( i_PreloadSoundToDelete == pSound->GetName() )
		{
			l_PreloadedSounds.erase( it );
			delete pSound;
			break;
		}

		++it;
	}
}


//========================================================================
//	FindPreloadSound( snSoundJob * )
//
//		Find a sound based on a pointer
//========================================================================
snSoundJob *	
FindPreloadSound( const snSoundJob * i_pPreloadSoundToFind )
{
	//	iterate through each item
	//
	std::list<snSoundJob *>::iterator it	= l_PreloadedSounds.begin();
	std::list<snSoundJob *>::iterator end	= l_PreloadedSounds.end();

	while ( it != end )
	{
		snSoundJob *	pSound	= *it;

		DBG_ASSERT( pSound != NULL, "item in Sound Manager Preload Sound list has a NULL pointer to a Sound Job" );

		if ( i_pPreloadSoundToFind == pSound )
		{
			//	found the sound, so return a pointer to it
			//
			return pSound;
		}

		++it;
	}

	return NULL;
}


//========================================================================
//	FindPreloadSound( itString & )
//
//		Find a sound based on a string
//========================================================================
snSoundJob *	
FindPreloadSound( const itString & i_PreloadSoundToFind )
{
	//	iterate through each item
	//
	std::list<snSoundJob *>::iterator it	= l_PreloadedSounds.begin();
	std::list<snSoundJob *>::iterator end	= l_PreloadedSounds.end();

	while ( it != end )
	{
		snSoundJob *	pSound	= *it;

		DBG_ASSERT( pSound != NULL, "item in Sound Manager Preload Sound list has a NULL pointer to a Sound Job" );

		if ( i_PreloadSoundToFind == pSound->GetName() )
		{
			//	found the sound, so return a pointer to it
			//
			return pSound;
		}

		++it;
	}

	return NULL;
}


//========================================================================
//	CreatePreloadSound2DStatic()
//
//		Create a sound that is a 2-D static sound
//	and add it to the preload sound list.  A pointer is given to the sound
//	so it can be configured.
//========================================================================
snSoundJob2D *	
CreatePreloadSound2DStatic( const fsLocator& i_Filename,
							const itString& i_Name )
{
	snSoundJob2D * pSound;
	pSound	= snSoundUtil::CreateSoundJob2DStatic( i_Filename );
	pSound->SetName( i_Name );

	// Add the sound to the preload sound list
	//
	AddPreloadSound( pSound );

	SetupSoundJob( pSound );
	pSound->SetLocalize2D( Is2dLocalizeAs2D() );

	return pSound;
}


//========================================================================
//	CreatePreloadSound3DStatic()
//
//		Create a sound that is a 3-D static sound
//	and add it to the preload sound list.  A pointer is given to the sound
//	so it can configured.
//========================================================================
snSoundJob3D *	
CreatePreloadSound3DStatic( const fsLocator& i_Filename,
							const itString& i_Name )
{
	snSoundJob3D * pSound;
	pSound	= snSoundUtil::CreateSoundJob3DStatic( i_Filename );
	//pSound->SetName( i_Name );

	// Add the sound to the preload sound list
	//
//	AddPreloadSound( pSound );

//	SetupSoundJob( pSound );

	return pSound;
}


//========================================================================
//	ClonePreloadSound( itString & )
//
//	Clone a sound based on a string and add it to the sound job list.  
//	Return the cloned sound job *or* return NULL is the sound isn't in 
//	the pre-load list
//========================================================================
snSoundJob *	
ClonePreloadSound( const itString & i_PreloadSoundToClone )
{
	//	iterate through each item
	//
	std::list<snSoundJob *>::iterator it	= l_PreloadedSounds.begin();
	std::list<snSoundJob *>::iterator end	= l_PreloadedSounds.end();

	while ( it != end )
	{
		snSoundJob *	pSound	= *it;

		DBG_ASSERT( pSound != NULL, "item in Sound Manager Preload Sound list has a NULL pointer to a Sound Job" );

		//	Determine the type of sound to create
		//
		if ( i_PreloadSoundToClone == pSound->GetName() )
		{
			pSound	= clone_sound_job(pSound);
			if ( pSound )
			{
				AddSoundJob( pSound );
			}
			else
			{
				DBG_WARNING( "Cannot Clone Sound " << itStringUtil::GetStdString( i_PreloadSoundToClone ).c_str() << ": unknown type"  );
			}

			return pSound;
		}

		++it;
	}

	DBG_WARNING( "Cannot Clone Sound " << itStringUtil::GetStdString( i_PreloadSoundToClone ).c_str()  << ": does not exist in Preload List" );
	return NULL;
}

//========================================================================
//	ClonePreloadSound( snSoundJob* )
//
//	Clone a sound based on a pointer and add it to the sound job list.  
//	Return the cloned sound job *or* return NULL is the sound isn't in 
//	the pre-load list
//========================================================================
snSoundJob *ClonePreloadSound( const snSoundJob * i_pPreloadSoundToFind )
{
	//	iterate through each item
	//
	std::list<snSoundJob *>::iterator it;
	it = std::find(l_PreloadedSounds.begin(), l_PreloadedSounds.end(), i_pPreloadSoundToFind);

	if ( it != l_PreloadedSounds.end() )
	{
		snSoundJob *pSound = clone_sound_job(*it);
		if ( pSound )
		{
			AddSoundJob( pSound );
			return pSound;
		}
		else
		{
			DBG_WARNING( "Cannot Clone Sound " << itStringUtil::GetStdString( i_pPreloadSoundToFind->GetName() ).c_str() << ": unknown type" );
		}
	}
	else
	{
		DBG_WARNING( "Cannot Clone Sound " << itStringUtil::GetStdString( i_pPreloadSoundToFind->GetName() ).c_str() << ": does not exist in Preload List" );
	}

	return NULL;
}


//========================================================================
//	return the number of preloaded sounds
//========================================================================
const int		
GetPreloadSoundCount()
{
	//	iterate through each item
	//
	int	counter = 0;
	std::list<snSoundJob *>::iterator it	= l_PreloadedSounds.begin();
	std::list<snSoundJob *>::iterator end	= l_PreloadedSounds.end();

	while ( it != end )
	{
		counter++;
		++it;
	}

	return counter;
}


//========================================================================
//	SoundJobsLoad()
//
//		Load all sound jobs
//========================================================================
void	
SoundJobsLoad()
{
	//	iterate through each item
	//
	std::list<snSoundJob *>::iterator it	= l_SoundJobs.begin();
	std::list<snSoundJob *>::iterator end	= l_SoundJobs.end();

	while ( it != end )
	{
		snSoundJob *	pSound	= *it;

		DBG_ASSERT( pSound != NULL, "item in Sound Manager Sound Job list has a NULL pointer to a Sound Job" );

		LoadSound( pSound );

		++it;
	}
}


//========================================================================
//	SoundJobsFree()
//
//		Free all sound jobs
//========================================================================
void	
SoundJobsFree()
{
	//	iterate through each item
	//
	std::list<snSoundJob *>::iterator it	= l_SoundJobs.begin();
	std::list<snSoundJob *>::iterator end	= l_SoundJobs.end();

	while ( it != end )
	{
		snSoundJob *	pSound	= *it;

		DBG_ASSERT( pSound != NULL, "item in Sound Manager Sound Job list has a NULL pointer to a Sound Job" );

		pSound->Free();

		++it;
	}
}


//========================================================================
//	SoundJobsDelete()
//
//		Delete all sound jobs
//========================================================================
void	
SoundJobsDelete()
{
	//	iterate through each item
	//
	std::list<snSoundJob *>::iterator it	= l_SoundJobs.begin();
	std::list<snSoundJob *>::iterator end	= l_SoundJobs.end();

	int size = l_SoundJobs.size();

	while ( it != end )
	{
		snSoundJob *	pSound	= *it;

		delete *it;
		++it;
	}

	l_SoundJobs.clear();
}


//========================================================================
//	SoundJobsStop()
//
//		Stop all sound jobs
//========================================================================
void	
SoundJobsStop()
{
	//	iterate through each item
	//
	std::list<snSoundJob *>::iterator it	= l_SoundJobs.begin();
	std::list<snSoundJob *>::iterator end	= l_SoundJobs.end();

	while ( it != end )
	{
		snSoundJob *	pSound	= *it;

		DBG_ASSERT( pSound != NULL, "item in Sound Manager Sound Job list has a NULL pointer to a Sound Job" );

		pSound->Stop();

		++it;
	}
}


//========================================================================
//	SoundJobsPause()
//
//		Pause all sound jobs
//========================================================================
void	
SoundJobsPause()
{
	//	iterate through each item
	//
	std::list<snSoundJob *>::iterator it	= l_SoundJobs.begin();
	std::list<snSoundJob *>::iterator end	= l_SoundJobs.end();

	while ( it != end )
	{
		snSoundJob *	pSound	= *it;

		DBG_ASSERT( pSound != NULL, "item in Sound Manager Sound Job list has a NULL pointer to a Sound Job" );

		pSound->Pause();

		++it;
	}
}


//========================================================================
//	SoundJobsResume()
//
//		Resume all sound jobs
//========================================================================
void	
SoundJobsResume( float i_SimulationTime )
{
	//	iterate through each item
	//
	std::list<snSoundJob *>::iterator it	= l_SoundJobs.begin();
	std::list<snSoundJob *>::iterator end	= l_SoundJobs.end();

	while ( it != end )
	{
		snSoundJob *	pSound	= *it;

		DBG_ASSERT( pSound != NULL, "item in Sound Manager Sound Job list has a NULL pointer to a Sound Job" );

		pSound->Resume( i_SimulationTime );

		++it;
	}
}


//========================================================================
//	SetSoundJobsListenerPosition()
//
//	physical location of listener in world (used by localize)
//
//	Note: this only affects sounds with the "localize" set to true
//========================================================================
void
SetSoundJobsListenerPosition( const maVector3d& i_Position )
{
	//	iterate through each item
	//
	std::list<snSoundJob *>::iterator it	= l_SoundJobs.begin();
	std::list<snSoundJob *>::iterator end	= l_SoundJobs.end();

	while ( it != end )
	{
		snSoundJob *	pSound	= *it;

		DBG_ASSERT( pSound != NULL, "item in Sound Manager Sound Job list has a NULL pointer to a Sound Job" );

		snSoundJob2D * pSound2D = dynamic_cast<snSoundJob2D *>(pSound);
		if ( pSound2D )
		{
			if ( pSound2D->IsLocalize() )
			{
				pSound2D->SetListenerPosition( i_Position );
			}
		}

		++it;
	}
}


//========================================================================
//	GetSoundJobsListenerPosition()
//
//	physical location of listener in world (used by localize)
//
//	Note: this only affects sounds with the "localize" set to true
//	Note: this returns the orientation of the FIRST localized job it hits
//========================================================================
maVector3d		
GetSoundJobsListenerPosition()
{
	//	iterate through each item
	//
	std::list<snSoundJob *>::iterator it	= l_SoundJobs.begin();
	std::list<snSoundJob *>::iterator end	= l_SoundJobs.end();

	while ( it != end )
	{
		snSoundJob *	pSound	= *it;

		DBG_ASSERT( pSound != NULL, "item in Sound Manager Sound Job list has a NULL pointer to a Sound Job" );

		snSoundJob2D * pSound2D = dynamic_cast<snSoundJob2D *>(pSound);
		if ( pSound2D )
		{
			if ( pSound2D->IsLocalize() )
			{
				return pSound2D->GetListenerPosition();
			}
		}

		++it;
	}

	return maVector3d( 0.0f, 0.0f, 0.0f );
}


//========================================================================
//	SetSoundJobsListenerOrientation()
//
//	physical orientation of listener in world (used by localize)
//
//	Note: this only affects sounds with the "localize" set to true
//========================================================================
void
SetSoundJobsListenerOrientation( const maRotation& i_Value )
{
	//	iterate through each item
	//
	std::list<snSoundJob *>::iterator it	= l_SoundJobs.begin();
	std::list<snSoundJob *>::iterator end	= l_SoundJobs.end();

	while ( it != end )
	{
		snSoundJob *	pSound	= *it;

		DBG_ASSERT( pSound != NULL, "item in Sound Manager Sound Job list has a NULL pointer to a Sound Job" );

		snSoundJob2D * pSound2D = dynamic_cast<snSoundJob2D *>(pSound);
		if ( pSound2D )
		{
			if ( pSound2D->IsLocalize() )
			{
				pSound2D->SetListenerOrientation( i_Value );
			}
		}

		++it;
	}
}


//========================================================================
//	GetSoundJobsListenerOrientation()
//
//	physical location of listener in world (used by localize)
//
//	Note: this only affects sounds with the "localize" set to true
//	Note: this returns the orientation of the FIRST localized job it hits
//========================================================================
maRotation
GetSoundJobsListenerOrientation()
{
	//	iterate through each item
	//
	std::list<snSoundJob *>::iterator it	= l_SoundJobs.begin();
	std::list<snSoundJob *>::iterator end	= l_SoundJobs.end();

	while ( it != end )
	{
		snSoundJob *	pSound	= *it;

		DBG_ASSERT( pSound != NULL, "item in Sound Manager Sound Job list has a NULL pointer to a Sound Job" );

		snSoundJob2D * pSound2D = dynamic_cast<snSoundJob2D *>(pSound);
		if ( pSound2D )
		{
			if ( pSound2D->IsLocalize() )
			{
				return pSound2D->GetListenerOrientation();
			}
		}

		++it;
	}

	return maRotation( maVector3d( 0.0f,0.0f,0.0f ), 0.0f );
}


//========================================================================
//	AddSoundJob()
//
//		Add a sound job 
//========================================================================
void	
AddSoundJob( snSoundJob * i_pSoundJob )
{
	//	if it's not already in the list add it
	//
	if ( FindSoundJob( i_pSoundJob ) == NULL )
	{
		l_SoundJobs.push_back( i_pSoundJob );
	}
}


//========================================================================
//	RemoveSoundJob( snSoundJob * )
//
//		Remove a soundjob based on a pointer
//========================================================================
snSoundJob *	
RemoveSoundJob( const snSoundJob * i_pSoundJobToRemove )
{
	snSoundJob * pSoundJob;

	pSoundJob	= FindSoundJob( i_pSoundJobToRemove );

	if ( pSoundJob )
	{
		// remove it
		//
		l_SoundJobs.remove( pSoundJob );
	}

	return pSoundJob;
}


//========================================================================
//	RemoveSoundJob( itString & )
//
//		Remove a soundjob based on a pointer
//========================================================================
snSoundJob *	
RemoveSoundJob( const itString & i_SoundJobToRemove )
{
	snSoundJob * pSoundJob;

	pSoundJob	= FindSoundJob( i_SoundJobToRemove );

	if ( pSoundJob )
	{
		// remove it
		//
		l_SoundJobs.remove( pSoundJob );
	}

	return pSoundJob;
}


//========================================================================
//	DeleteSoundJob( snSoundJob * )
//
//		Delete a soundjob based on a pointer
//========================================================================
void	
DeleteSoundJob( const snSoundJob * i_pSoundJobToDelete )
{
	snSoundJob * pSoundJob;

	pSoundJob	= FindSoundJob( i_pSoundJobToDelete );

	if ( pSoundJob )
	{
		// remove it
		//
		l_SoundJobs.remove( pSoundJob );
		delete pSoundJob;
	}
}


//========================================================================
//	DeleteSoundJob( itString & )
//
//		Delete a soundjob based on a pointer
//========================================================================
void	
DeleteSoundJob( const itString & i_SoundJobToDelete )
{
	snSoundJob * pSoundJob;

	pSoundJob	= FindSoundJob( i_SoundJobToDelete );

	if ( pSoundJob )
	{
		// remove it
		//
		l_SoundJobs.remove( pSoundJob );
		delete pSoundJob;
	}
}


//========================================================================
//	FindSoundJob( snSoundJob * )
//
//		Find a sound job based on a pointer
//========================================================================
snSoundJob *	
FindSoundJob( const snSoundJob * i_pSoundJobToFind )
{
	//	iterate through each item
	//
	std::list<snSoundJob *>::iterator it	= l_SoundJobs.begin();
	std::list<snSoundJob *>::iterator end	= l_SoundJobs.end();

	while ( it != end )
	{
		snSoundJob *	pSound	= *it;

		DBG_ASSERT( pSound != NULL, "item in Sound Manager Sound Job list has a NULL pointer to a Sound Job" );

		if ( i_pSoundJobToFind == pSound )
		{
			//	found the sound job, so return a pointer to it
			//
			return pSound;
		}

		++it;
	}

	return NULL;
}


//========================================================================
//	FindSoundJob( itString & )
//
//		Find a sound job based on a pointer
//========================================================================
snSoundJob *	
FindSoundJob( const itString & i_SoundJobToFind )
{
	//	iterate through each item
	//
	std::list<snSoundJob *>::iterator it	= l_SoundJobs.begin();
	std::list<snSoundJob *>::iterator end	= l_SoundJobs.end();

	while ( it != end )
	{
		snSoundJob *	pSound	= *it;

		DBG_ASSERT( pSound != NULL, "item in Sound Manager Sound Job list has a NULL pointer to a Sound Job" );

		if ( i_SoundJobToFind == pSound->GetName() )
		{
			//	found the sound job, so return a pointer to it
			//
			return pSound;
		}

		++it;
	}

	return NULL;
}

//========================================================================
//	FindSoundJobs( itString & )
//
//		Finds all sound jobs based on a name
//========================================================================
void	
FindSoundJobs( const itString & i_SoundJobToFind, std::vector<snSoundJob *>& o_Jobs )
{
	//	iterate through each item
	//
	std::list<snSoundJob *>::iterator it	= l_SoundJobs.begin();
	std::list<snSoundJob *>::iterator end	= l_SoundJobs.end();

	while ( it != end )
	{
		snSoundJob *	pSound	= *it;

		DBG_ASSERT( pSound != NULL, "item in Sound Manager Sound Job list has a NULL pointer to a Sound Job" );

		if ( i_SoundJobToFind == pSound->GetName() )
		{
			o_Jobs.push_back(pSound);
		}

		++it;
	}
}

//========================================================================
//	FindSoundJob( int )
//
//		Find the first sound job based on a SOUNDTYPE_
//========================================================================
snSoundJob *	FindSoundJob( int i_eSoundType, bool i_bMustBePlaying )
{
	//	iterate through each item
	//
	std::list<snSoundJob *>::iterator it	= l_SoundJobs.begin();
	std::list<snSoundJob *>::iterator end	= l_SoundJobs.end();

	while ( it != end )
	{
		snSoundJob *	pSound	= *it;

		DBG_ASSERT( pSound != NULL, "item in Sound Manager Sound Job list has a NULL pointer to a Sound Job" );

		if ( pSound->IsTypeMaskEqual( i_eSoundType ) )
		{
			if ( i_bMustBePlaying )
			{
				//! if ( pSound->IsPlaying() )
				{
					//	found the sound job AND it is playing, so return a pointer to it
					//
					return pSound;
				}
			}
			else
			{
				//	found the sound job, so return a pointer to it
				//
				return pSound;
			}
		}

		++it;
	}

	return NULL;
}


//========================================================================
//	CreateSoundJob2DStatic()
//
//	Create a sound job that is a 2-D static sound
//	and add it to the sound job list.  A pointer is given to the sound
//	so it can be played or its features can be altered.
//
//	If the flag, i_bCreateFromPreloadIfPossible, is set for the function
//	it will try to clone it from the preload list if it exists.
//========================================================================
snSoundJob2D *	
CreateSoundJob2DStatic( const fsLocator&	i_Filename, 
					    const itString&		i_Name,
						const bool			i_bCreateFromPreloadIfPossible )
{
	snSoundJob2D * pSound = NULL;

	//	If the flag, i_bCreateFromPreloadIfPossible, is set for the function
	//	it will try to clone it from the preload list if it exists.
	//
	if ( i_bCreateFromPreloadIfPossible )
	{
		pSound	= dynamic_cast<snSoundJob2D *>(snSoundManager::ClonePreloadSound( i_Name ));
	}

	if ( !pSound )
	{
		pSound	= snSoundUtil::CreateSoundJob2DStatic( i_Filename );
		pSound->SetName( i_Name );

		// Add the sound to the sound job list
		//
		AddSoundJob( pSound );
	}

	SetupSoundJob( pSound );
	pSound->SetLocalize2D( Is2dLocalizeAs2D() );

	return pSound;
}


//========================================================================
//	CreateSoundJob2DStreamed()
//
//		Create a sound job that is a 2-D streamed sound
//	and add it to the sound job list.  A pointer is given to the sound
//	so it can be played or its features can be altered.
//========================================================================
snSoundJob2DStreamed *	
CreateSoundJob2DStreamed( const fsLocator& i_Filename )
{
	snSoundJob2DStreamed * pSound;
	pSound	= snSoundUtil::CreateSoundJob2DStreamed( i_Filename );

	// Add the sound to the sound job list
	//
	AddSoundJob( pSound );

	SetupSoundJob( pSound );
	pSound->SetLocalize2D( Is2dLocalizeAs2D() );

	return pSound;
}


//========================================================================
//	CreateSoundJob3DStatic()
//
//		Create a sound job that is a 3-D static sound
//	and add it to the sound job list.  A pointer is given to the sound
//	so it can be played or its features can be altered.
//========================================================================
snSoundJob3D *	
CreateSoundJob3DStatic( const fsLocator& i_Filename, const itString& i_Name )
{
	snSoundJob3D * pSound;
	pSound	= snSoundUtil::CreateSoundJob3DStatic( i_Filename );
	//pSound->SetName( i_Name );

	// Add the sound to the sound job list
	//
//	AddSoundJob( pSound );

//	SetupSoundJob( pSound );

	return pSound;
}


//========================================================================
//	CreateSoundJob3DStreamed()
//
//		Create a sound job that is a 3-D streamed sound
//	and add it to the sound job list.  A pointer is given to the sound
//	so it can be played or its features can be altered.
//========================================================================
snSoundJob3DStreamed *	
CreateSoundJob3DStreamed( const fsLocator& i_Filename )
{
	snSoundJob3DStreamed * pSound;
	pSound	= snSoundUtil::CreateSoundJob3DStreamed( i_Filename );

	// Add the sound to the sound job list
	//
//	AddSoundJob( pSound );

//	SetupSoundJob( pSound );

	return pSound;
}


//========================================================================
//	CreateSoundJobMIDI()
//
//		Create a sound job that is a MIDI sound
//	and add it to the sound job list.  A pointer is given to the sound
//	so it can be played or its features can be altered.
//========================================================================
snSoundJobMIDI *	
CreateSoundJobMIDI( const fsLocator& i_Filename )
{
	snSoundJobMIDI * pSound;
	pSound	= snSoundUtil::CreateSoundJobMIDI( i_Filename );

	// Add the sound to the sound job list
	//
//	AddSoundJob( pSound );

//	SetupSoundJob( pSound );

	return pSound;
}


//========================================================================
//	CreateSoundJobMP3()
//
//		Create a sound job that is a MP3 sound
//	and add it to the sound job list.  A pointer is given to the sound
//	so it can be played or its features can be altered.
//========================================================================
snSoundJobMP3 *	
CreateSoundJobMP3( const fsLocator& i_Filename )
{
	snSoundJobMP3 * pSound;
	pSound	= snSoundUtil::CreateSoundJobMP3( i_Filename );

	// Add the sound to the sound job list
	//
	AddSoundJob( pSound );

	SetupSoundJob( pSound );

	return pSound;
}


//========================================================================
//	CreateSoundJobStaticRedbook()
//
//		Create a sound job that is a Redbook track
//	and add it to the sound job list.  A pointer is given to the sound
//	so it can be played or its features can be altered.
//========================================================================
snSoundJobRedbook *	
CreateSoundJobRedbook( const fsLocator& i_Filename )
{
	snSoundJobRedbook * pSound;
	pSound	= snSoundUtil::CreateSoundJobRedbook( i_Filename );

	// Add the sound to the sound job list
	//
	AddSoundJob( pSound );

	//SetupSoundJob( pSound );

	return pSound;
}


//========================================================================
//	Play2DStaticFromPreload()
//
//	Create/Find a sound job from the preload list and start it.
//
//	If the name is not found it will only create a new sound if the 
//	i_bPlayIfNotFound flag is set to true.
//
//	The sound job that is played will be returned otherwise NULL.
//========================================================================
snSoundJob2D *			
Play2DStaticFromPreload(	const	fsLocator& i_Filename,
							float	i_SimulationTime,
							bool	i_bDeleteWhenFinished, 
							int		i_eExclusiveFlag, 
							int		i_eSoundType, 
							bool	i_bLocalize,
							bool	i_bPlayIfNotFound = true )
{
	snSoundJob2D *	pSound = NULL;

	//	We aren't in any exclusive mode, so just play the sound
	//
	pSound	= dynamic_cast<snSoundJob2D *>( ClonePreloadSound( i_Filename.GetLastName() ) );

	if ( pSound )
	{
		//	The sound is pre-loaded (and now cloned), so let's load and play it
		//
		LoadSound( pSound );

		pSound->SetDeleteWhenFinished( i_bDeleteWhenFinished );
		pSound->SetLocalize( i_bLocalize );

		pSound->Start( i_SimulationTime );
	}
	else
	{
		if ( i_bPlayIfNotFound )
		{
			//	The sound wasn't pre-loaded, so let's create a new sound job and 
			//	play it.
			//
			pSound	= CreateSoundJob2DStatic( i_Filename, i_Filename.GetLastName() );

			if ( pSound )
			{
				pSound->Load();

				pSound->SetDeleteWhenFinished( i_bDeleteWhenFinished );
				pSound->SetLocalize( i_bLocalize );

				pSound->Start( i_SimulationTime );
			}
		}
	}

	return pSound;
}


//========================================================================
//	Play2DStaticFromPreload()
//
//	Create/Find a sound job from the preload list and start it.
//
//	If the name is not found it will only create a new sound if the 
//	i_bPlayIfNotFound flag is set to true.
//
//	The sound job that is played will be returned otherwise NULL.
//========================================================================
snSoundJob2D *			
Play2DStaticFromPreload(	const	itString& i_Name,
							float	i_SimulationTime,
							bool	i_bDeleteWhenFinished, 
							int		i_eExclusiveFlag, 
							int		i_eSoundType, 
							bool	i_bLocalize )
{
	snSoundJob2D *	pSound;

	//	We aren't in any exclusive mode, so just play the sound
	//
	pSound	= dynamic_cast<snSoundJob2D *>( ClonePreloadSound( i_Name ) );

	if ( pSound )
	{
		//	The sound is pre-loaded (and now cloned), so let's load and play it
		//
		pSound->Load();

		pSound->SetDeleteWhenFinished( i_bDeleteWhenFinished );
		pSound->SetLocalize( i_bLocalize );

		pSound->Start( i_SimulationTime );
	}

	return pSound;
}


//========================================================================
//	Play2DStatic()
//
//		Play a sound job that is a 2-D static sound
//	and add it to the sound job list.  A pointer is given to the sound
//	so it can be played or its features can be altered.
//========================================================================
snSoundJob2D *			
Play2DStatic(	const	itString& i_Name,
				float	i_SimulationTime,
				bool	i_bDeleteWhenFinished, 
				int		i_eExclusiveFlag, 
				int		i_eSoundType, 
				bool	i_bLocalize )
{
	snSoundJob2D * pSound;

	//
	//	TO DO:
	//		set DELETE WHEN DONE flag
	//		set ISPLAYING flag
	//		set LOCALIZE flag
	//		implement localize
	//		copy this play to other play functions
	//

	//	First check if we are playing a sound based on name or type.
	//
	if ( i_eSoundType == SOUNDTYPE_NONE )
	{
		//	We are finding a sound by name, so first, check the mode of play
		//
		if ( i_eExclusiveFlag == SOUNDMODE_NOTEXCLUSIVE )
		{
			pSound = Play2DStaticFromPreload( i_Name, 
											i_SimulationTime,
											i_bDeleteWhenFinished, 
											i_eExclusiveFlag, 
											i_eSoundType, 
											i_bLocalize );
		}
		else
		{
			//	First check to see if the sound already exists.  
			//
			pSound	= dynamic_cast<snSoundJob2D *>( FindSoundJob( i_Name ) );

			if ( pSound )
			{
				// if so, do we jump out or restart it?
				//
				if ( i_eExclusiveFlag == SOUNDMODE_EXCLUSIVE_RESTART )
				{
					pSound->Load();
			
					pSound->SetDeleteWhenFinished( i_bDeleteWhenFinished );
					pSound->SetLocalize( i_bLocalize );

					pSound->Restart( i_SimulationTime );
				}
				else
				{
					//	The sound job exists AND is exclusive.
					//	Determine if the sound is currently playing
					//	if not, start it, otherwise do nothing
					//!
					//if ( !pSound->IsPlaying() )
					//{
						pSound->Start( i_SimulationTime );
					//}
				}
			}
			else
			{
				pSound = Play2DStaticFromPreload( i_Name, 
												i_SimulationTime,
												i_bDeleteWhenFinished, 
												i_eExclusiveFlag, 
												i_eSoundType, 
												i_bLocalize );
			}
		}
	}
	else
	{
		if ( i_eExclusiveFlag == SOUNDMODE_NOTEXCLUSIVE )
		{
			pSound = Play2DStaticFromPreload( i_Name, 
											i_SimulationTime,
											i_bDeleteWhenFinished, 
											i_eExclusiveFlag, 
											i_eSoundType, 
											i_bLocalize );
		}
		else
		{
			//	we must be exclusive, so check for a sound of the type
			//
			pSound	= dynamic_cast<snSoundJob2D *>( FindSoundJob( i_eSoundType, true ) );

			if ( pSound )
			{
				//	There is already a sound of this type and it is playing
				//
				if ( i_eExclusiveFlag == SOUNDMODE_EXCLUSIVE )
				{
					//	do nothing
					return NULL;
				}
				else
				{
					snSoundJob * pUniqueSoundJob;

					//	First check to see if the sound already exists.  
					//
					pUniqueSoundJob	= dynamic_cast<snSoundJob2D *>( FindSoundJob( i_Name ) );
					
					//	if the currently playing sound point and this sound pointer are the same
					//	restart one of them.  If they are NOT, then we need to stop the old sound
					//	and start the new one
					//
					if ( pSound == pUniqueSoundJob )
					{
						pSound->SetDeleteWhenFinished( i_bDeleteWhenFinished );
						pSound->SetLocalize( i_bLocalize );

						pSound->Start( i_SimulationTime );
					}
					else
					{
						pSound->Stop();

						if ( pUniqueSoundJob )
						{
							pSound->SetDeleteWhenFinished( i_bDeleteWhenFinished );
							pSound->SetLocalize( i_bLocalize );

							pUniqueSoundJob->Start( i_SimulationTime );
						}
						else
						{
							pSound = Play2DStaticFromPreload( i_Name, 
															i_SimulationTime,
															i_bDeleteWhenFinished, 
															i_eExclusiveFlag, 
															i_eSoundType, 
															i_bLocalize );
						}
					}
				}

			}
			else
			{
				pSound = Play2DStaticFromPreload( i_Name, 
												i_SimulationTime,
												i_bDeleteWhenFinished, 
												i_eExclusiveFlag, 
												i_eSoundType, 
												i_bLocalize );
			}
		}
	}

	return pSound;
}


//========================================================================
//	Play2DStatic()
//
//		Play a sound job that is a 2-D static sound
//	and add it to the sound job list.  A pointer is given to the sound
//	so it can be played or its features can be altered.
//========================================================================
snSoundJob2D *			
Play2DStatic(	const	fsLocator& i_Filename,
				float	i_SimulationTime,
				bool	i_bDeleteWhenFinished, 
				int		i_eExclusiveFlag, 
				int		i_eSoundType, 
				bool	i_bLocalize )
{
	snSoundJob2D * pSound;

	//
	//	TO DO:
	//		set DELETE WHEN DONE flag
	//		set ISPLAYING flag
	//		set LOCALIZE flag
	//		implement localize
	//		copy this play to other play functions
	//

	//	First check if we are playing a sound based on name or type.
	//
	if ( i_eSoundType == SOUNDTYPE_NONE )
	{
		//	We are finding a sound by name, so first, check the mode of play
		//
		if ( i_eExclusiveFlag == SOUNDMODE_NOTEXCLUSIVE )
		{
			pSound = Play2DStaticFromPreload( i_Filename, 
											i_SimulationTime,
											i_bDeleteWhenFinished, 
											i_eExclusiveFlag, 
											i_eSoundType, 
											i_bLocalize );
		}
		else
		{
			//	First check to see if the sound already exists.  
			//
			pSound	= dynamic_cast<snSoundJob2D *>( FindSoundJob( i_Filename.GetLastName() ) );

			if ( pSound )
			{
				// if so, do we jump out or restart it?
				//
				if ( i_eExclusiveFlag == SOUNDMODE_EXCLUSIVE_RESTART )
				{
					pSound->Load();

					pSound->SetDeleteWhenFinished( i_bDeleteWhenFinished );
					pSound->SetLocalize( i_bLocalize );

					pSound->Restart( i_SimulationTime );
				}
				else
				{
					//	The sound job exists AND is exclusive.
					//	Determine if the sound is currently playing
					//	if not, start it, otherwise do nothing
					//!
					//if ( !pSound->IsPlaying() )
					//{
						pSound->SetDeleteWhenFinished( i_bDeleteWhenFinished );
						pSound->SetLocalize( i_bLocalize );

						pSound->Start( i_SimulationTime );
					//}
				}
			}
			else
			{
				pSound = Play2DStaticFromPreload( i_Filename, 
												i_SimulationTime,
												i_bDeleteWhenFinished, 
												i_eExclusiveFlag, 
												i_eSoundType, 
												i_bLocalize );
			}
		}
	}
	else
	{
		if ( i_eExclusiveFlag == SOUNDMODE_NOTEXCLUSIVE )
		{
			pSound = Play2DStaticFromPreload( i_Filename, 
											i_SimulationTime,
											i_bDeleteWhenFinished, 
											i_eExclusiveFlag, 
											i_eSoundType, 
											i_bLocalize );
		}
		else
		{
			//	we must be exclusive, so check for a sound of the type
			//
			pSound	= dynamic_cast<snSoundJob2D *>( FindSoundJob( i_eSoundType, true ) );

			if ( pSound )
			{
				//	There is already a sound of this type and it is playing
				//
				if ( i_eExclusiveFlag == SOUNDMODE_EXCLUSIVE )
				{
					//	do nothing
					return NULL;
				}
				else
				{
					snSoundJob * pUniqueSoundJob;

					//	First check to see if the sound already exists.  
					//
					pUniqueSoundJob	= dynamic_cast<snSoundJob2D *>( FindSoundJob( i_Filename.GetLastName() ) );
					
					//	if the currently playing sound point and this sound pointer are the same
					//	restart one of them.  If they are NOT, then we need to stop the old sound
					//	and start the new one
					//
					if ( pSound == pUniqueSoundJob )
					{
						pSound->SetDeleteWhenFinished( i_bDeleteWhenFinished );
						pSound->SetLocalize( i_bLocalize );

						pSound->Start( i_SimulationTime );
					}
					else
					{
						pSound->Stop();

						if ( pUniqueSoundJob )
						{
							pSound->SetDeleteWhenFinished( i_bDeleteWhenFinished );
							pSound->SetLocalize( i_bLocalize );

							pUniqueSoundJob->Start( i_SimulationTime );
						}
						else
						{
							pSound = Play2DStaticFromPreload( i_Filename, 
															i_SimulationTime,
															i_bDeleteWhenFinished, 
															i_eExclusiveFlag, 
															i_eSoundType, 
															i_bLocalize );
						}
					}
				}

			}
			else
			{
				pSound = Play2DStaticFromPreload( i_Filename, 
												i_SimulationTime,
												i_bDeleteWhenFinished, 
												i_eExclusiveFlag, 
												i_eSoundType, 
												i_bLocalize );
			}
		}
	}

	return pSound;
}


//========================================================================
//	Play2DStreamed()
//
//		Play a sound job that is a 2-D streamed sound
//	and add it to the sound job list.  A pointer is given to the sound
//	so it can be played or its features can be altered.
//========================================================================
snSoundJob2DStreamed *	
Play2DStreamed(	const	fsLocator& i_Filename,
				float	i_SimulationTime,
				bool	i_bDeleteWhenFinished, 
				int		i_eExclusiveFlag, 
				int		i_eSoundType, 
				bool	i_bLocalize )
{
	snSoundJob2DStreamed * pSound;
	pSound	= CreateSoundJob2DStreamed( i_Filename );

	pSound->Load();

	pSound->SetDeleteWhenFinished( i_bDeleteWhenFinished );
	pSound->SetLocalize( i_bLocalize );

	pSound->Start( i_SimulationTime );
	return pSound;
}


//========================================================================
//	Play3DStatic()
//
//		Play a sound job that is a 3-D static sound
//	and add it to the sound job list.  A pointer is given to the sound
//	so it can be played or its features can be altered.
//========================================================================
snSoundJob3D *			
Play3DStatic(	const	fsLocator& i_Filename,
				float	i_SimulationTime,
				bool	i_bDeleteWhenFinished, 
				int		i_eExclusiveFlag, 
				int		i_eSoundType )
{
//	snSoundJob3D * pSound;
//	pSound	= CreateSoundJob3DStatic( i_Filename );
//
//	pSound->Load();
//	pSound->SetDeleteWhenFinished( i_bDeleteWhenFinished );
//	pSound->SetLocalize( i_bLocalize );
//	pSound->Start( i_SimulationTime );
	return NULL;
}


//========================================================================
//	Play3DStatic()
//
//		Play a sound job that is a 3-D static sound
//	and add it to the sound job list.  A pointer is given to the sound
//	so it can be played or its features can be altered.
//========================================================================
snSoundJob3D *			
Play3DStatic(	const	itString& i_Filename,
				float	i_SimulationTime,
				bool	i_bDeleteWhenFinished, 
				int		i_eExclusiveFlag, 
				int		i_eSoundType )
{
//	snSoundJob3D * pSound;
//	pSound	= CreateSoundJob3DStatic( i_Filename );
//
//	pSound->Load();
//	pSound->SetDeleteWhenFinished( i_bDeleteWhenFinished );
//	pSound->SetLocalize( i_bLocalize );
//	pSound->Start( i_SimulationTime );
	return NULL;
}


//========================================================================
//	Play3DStreamed()
//
//		Play a sound job that is a 3-D streamed sound
//	and add it to the sound job list.  A pointer is given to the sound
//	so it can be played or its features can be altered.
//========================================================================
snSoundJob3DStreamed *	
Play3DStreamed(	const	fsLocator& i_Filename,
				float	i_SimulationTime,
				bool	i_bDeleteWhenFinished, 
				int		i_eExclusiveFlag, 
				int		i_eSoundType )
{
//	snSoundJob3DStreamed * pSound;
//	pSound	= CreateSoundJob3DStreamed( i_Filename );
//
//	pSound->Load();
//	pSound->SetDeleteWhenFinished( i_bDeleteWhenFinished );
//	pSound->SetLocalize( i_bLocalize );
//	pSound->Start( i_SimulationTime );
	return NULL;
}


//========================================================================
//	PlayMIDI()
//
//		Play a sound job that is a MIDI sound
//	and add it to the sound job list.  A pointer is given to the sound
//	so it can be played or its features can be altered.
//========================================================================
snSoundJobMIDI *		
PlayMIDI(	const fsLocator& i_Filename,
			float	i_SimulationTime )
{
//	snSoundJobMIDI * pSound;
//	pSound	= CreateSoundJobMIDI( i_Filename );
//
//	pSound->Load();
//	pSound->Start( i_SimulationTime );
	return NULL;
}


//========================================================================
//	PlayMP3()
//
//		Play a sound job that is a MP3 sound
//	and add it to the sound job list.  A pointer is given to the sound
//	so it can be played or its features can be altered.
//========================================================================
snSoundJobMP3 *		
PlayMP3(	const fsLocator& i_Filename,
			float	i_SimulationTime )
{
	snSoundJobMP3 * pSound;
	pSound	= CreateSoundJobMP3( i_Filename );

	pSound->Load();
	pSound->Start( i_SimulationTime );
	return pSound;
}


//========================================================================
//	PlayStaticRedbook()
//
//		Play a sound job that is a Redbook track
//	and add it to the sound job list.  A pointer is given to the sound
//	so it can be played or its features can be altered.
//========================================================================
snSoundJobRedbook *		
PlayRedbook( const fsLocator& i_Filename )
{
	snSoundJobRedbook * pSound;
	pSound	= CreateSoundJobRedbook( i_Filename );

	pSound->Load();
//	pSound->SetDeleteWhenFinished( i_bDeleteWhenFinished );
//	pSound->SetLocalize( i_bLocalize );
	pSound->Start();
	return pSound;
}


//========================================================================
//	return the number of sound jobs
//========================================================================
const int		
GetSoundJobCount()
{
	//	iterate through each item
	//
	int	counter = 0;
	std::list<snSoundJob *>::iterator it	= l_SoundJobs.begin();
	std::list<snSoundJob *>::iterator end	= l_SoundJobs.end();

	while ( it != end )
	{
		counter++;
		++it;
	}

	return counter;
}


//========================================================================
//	ReloadSounds()
//
//		Reload all raw sound data from all lists
//========================================================================
void 
ReloadSounds()
{
	//	iterate through each item
	//
	std::list<snSoundJob *>::iterator it	= l_PreloadedSounds.begin();
	std::list<snSoundJob *>::iterator end	= l_PreloadedSounds.end();

	while ( it != end )
	{
		snSoundJob *	pSound	= *it;

		DBG_ASSERT( pSound != NULL, "item in Sound Manager Sound Job list has a NULL pointer to a Sound Job" );

		pSound->Reload();

		++it;
	}

	//	iterate through each item
	//
	it	= l_SoundJobs.begin();
	end	= l_SoundJobs.end();

	while ( it != end )
	{
		snSoundJob *	pSound	= *it;

		DBG_ASSERT( pSound != NULL, "item in Sound Manager Sound Job list has a NULL pointer to a Sound Job" );

		pSound->Reload();

		++it;
	}
}


//========================================================================
//	UnloadSounds()
//
//		Unload all raw sound data from all lists
//========================================================================
void 
UnloadSounds()
{
	//	iterate through each item
	//
	std::list<snSoundJob *>::iterator it	= l_PreloadedSounds.begin();
	std::list<snSoundJob *>::iterator end	= l_PreloadedSounds.end();

	while ( it != end )
	{
		snSoundJob *	pSound	= *it;

		DBG_ASSERT( pSound != NULL, "item in Sound Manager Sound Job list has a NULL pointer to a Sound Job" );

		pSound->Unload();

		++it;
	}

	//	iterate through each item
	//
	it	= l_SoundJobs.begin();
	end	= l_SoundJobs.end();

	while ( it != end )
	{
		snSoundJob *	pSound	= *it;

		DBG_ASSERT( pSound != NULL, "item in Sound Manager Sound Job list has a NULL pointer to a Sound Job" );

		pSound->Unload();

		++it;
	}
}

//========================================================================
//========================================================================
bool IsMuted()
{
	return (m_VolumeGlobalFactor_Old != 0.0f);
}
void Mute(bool i_bMute)
{
	if (i_bMute)
	{
		//	store the current global factor
		//
		if (m_VolumeGlobalFactor != 0.0f)
			m_VolumeGlobalFactor_Old = m_VolumeGlobalFactor;
		SetSoundTypeVolumeGlobalFactor( 0.0f );
	}
	else
	{
		if (m_VolumeGlobalFactor_Old != 0.0f)
			SetSoundTypeVolumeGlobalFactor( m_VolumeGlobalFactor_Old );

		m_VolumeGlobalFactor_Old = 0.0f;
	}
}


//========================================================================
//	Initialized		- has the sound manager been initialized yet
//========================================================================
bool	IsInitialized()							{ return l_Flag.bInitialized; };
void	SetInitialized( const bool Value )		{ l_Flag.bInitialized	= Value; };


//========================================================================
// 2DLocalizeAs2D -- true by default, set to false to localize all created
// 2d sounds as mono.
//========================================================================
void Set2DLocalizeAs2D( const bool Value )	{ l_Flag.b2DLocalizeAs2D = Value; };
bool Is2dLocalizeAs2D()						{ return l_Flag.b2DLocalizeAs2D; }

//========================================================================
//	Init()
//
//	Initialize this classes data
//========================================================================
void	
Init()
{
	int i;
	for ( i = 0 ; i < snSoundManager::SOUNDTYPES ; i++ )
	{
		m_VolumeTypeFactors[ i ]	= 1.0f;
	}

	m_VolumeGlobalFactor		= 1.0f;
	m_VolumeGlobalFactor_Old	= 0.0f;

	snSoundManagerPAC::Init();
}


//========================================================================
//	CleanUp()
//========================================================================
void 
CleanUp() throw ()
{
	snSoundManagerPAC::CleanUp();
}

}	// namespace

