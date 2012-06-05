/*****************************************************************************
**  snSoundUtil.cpp
**
**      snSoundUtil contains functions related to sound parameter conversion
**	for the sn package.
**
**	StudioGPU
**	Copyright(C) 2003-2 - All Rights Reserved
\****************************************************************************/

#include "AudioDS/sn/snSoundUtil.hpp"

#include "AudioDS/sn/snSoundJob2D.hpp"
#include "AudioDS/sn/snSoundJob2DStreamed.hpp"
#include "AudioDS/sn/snSoundJobMP3.hpp"
#include "snSoundUtilPac.hpp"
//#include "Core/it/itString.hpp"


namespace snSoundUtil
{

//========================================================================
//	CreateSoundJob2DStatic()
//========================================================================
snSoundJob2D *	
CreateSoundJob2DStatic( const fsLocator& i_Filename )
{
	snSoundJob2D * pSoundJob;
	pSoundJob = new snSoundJob2D();
	pSoundJob->SetFilename( i_Filename );

	return pSoundJob;
}


//========================================================================
//	CreateSoundJob2DStreamed()
//========================================================================
snSoundJob2DStreamed *	
CreateSoundJob2DStreamed( const fsLocator& i_Filename )
{
	snSoundJob2DStreamed * pSoundJob;
	pSoundJob = new snSoundJob2DStreamed();
	pSoundJob->SetFilename( i_Filename );

	return pSoundJob;
}


//========================================================================
//	CreateSoundJob3DStatic()
//========================================================================
snSoundJob3D *	
CreateSoundJob3DStatic( const fsLocator& i_Filename )
{
	DBG_ASSERT( NULL, "This Sound Job type hasn't been implemented yet" );
	
	return NULL;
}


//========================================================================
//	CreateSoundJob3DStreamed()
//========================================================================
snSoundJob3DStreamed *	
CreateSoundJob3DStreamed( const fsLocator& i_Filename )
{
	DBG_ASSERT( NULL, "This Sound Job type hasn't been implemented yet" );
	
	return NULL;
}


//========================================================================
//	CreateSoundJobMIDI()
//========================================================================
snSoundJobMIDI *	
CreateSoundJobMIDI( const fsLocator& i_Filename )
{
	DBG_ASSERT( NULL, "This Sound Job type hasn't been implemented yet" );
	
	return NULL;
}


//========================================================================
//	CreateSoundJobMP3()
//========================================================================
snSoundJobMP3 *	
CreateSoundJobMP3( const fsLocator& i_Filename )
{
	snSoundJobMP3 * pSoundJob;
	pSoundJob = new snSoundJobMP3();
	pSoundJob->SetFilename( i_Filename );

	return pSoundJob;
}


//========================================================================
//	CreateSoundJobStaticRedbook()
//========================================================================
snSoundJobRedbook *	
CreateSoundJobRedbook( const fsLocator& i_Filename )
{
	DBG_ASSERT( NULL, "This Sound Job type hasn't been implemented yet" );
	
	return NULL;
}


//========================================================================
//	CloneSoundJob2DStatic()
//========================================================================
snSoundJob2D *	
CloneSoundJob2DStatic( const snSoundJob2D& i_SoundJob )
{
	snSoundJob2D * pSoundJob;
	pSoundJob = new snSoundJob2D( i_SoundJob );
	return pSoundJob;
}


//========================================================================
//	CloneSoundJob2DStreamed()
//========================================================================
snSoundJob2DStreamed *	
CloneSoundJob2DStreamed( const snSoundJob2DStreamed& i_SoundJob )
{
	snSoundJob2DStreamed * pSoundJob = NULL;
	pSoundJob = new snSoundJob2DStreamed( i_SoundJob );
	return pSoundJob;
}


//========================================================================
//	CloneSoundJob3DStatic()
//========================================================================
snSoundJob3D *	
CloneSoundJob3DStatic( const snSoundJob3D& i_SoundJob )
{
	DBG_ASSERT( NULL, "This Sound Job type hasn't been implemented yet" );
	
	return NULL;
}


//========================================================================
//	CloneSoundJob3DStreamed()
//========================================================================
//snSoundJob3DStreamed *	
//CloneSoundJob3DStreamed( const snSoundJob3DStreamed& i_SoundJob )
//{
//	DBG_ASSERT( NULL, "This Sound Job type hasn't been implemented yet" );
//	
//	return NULL;
//}


//========================================================================
//	CloneSoundJobMIDI()
//========================================================================
snSoundJobMIDI *	
CloneSoundJobMIDI( const snSoundJobMIDI& i_SoundJob )
{
	DBG_ASSERT( NULL, "This Sound Job type hasn't been implemented yet" );
	
	return NULL;
}


//========================================================================
//	CloneSoundJobMP3()
//========================================================================
snSoundJobMP3 *	
CloneSoundJobMP3( const snSoundJobMP3& i_SoundJob )
{
	DBG_ASSERT( NULL, "This Sound Job type hasn't been implemented yet" );
	
	return NULL;
}


//========================================================================
//	CloneSoundJobStaticRedbook()
//========================================================================
snSoundJobRedbook *	
CloneSoundJobRedbook( const snSoundJobRedbook& i_SoundJob )
{
	DBG_ASSERT( NULL, "This Sound Job type hasn't been implemented yet" );
	
	return NULL;
}


//========================================================================
//	ConvertVolume()
//
//	Given a percent from 0 to 100, return the correct volume value.  The
//	percentage is LINEAR, so the coverted volume will not be a percentage
//	on the curve, but a percentage of the actual volume.
//========================================================================
int	ConvertVolume( float i_SoundPercent )
{
	return snSoundUtilPAC::ConvertVolume( i_SoundPercent );
}


//========================================================================
//	Don't call Init() and CleanUp() yourself; they are called 
//	by the package Init and Cleanup.
//========================================================================
void Init()
{
	snSoundUtilPAC::Init();
}


void CleanUp() throw()
{
	snSoundUtilPAC::CleanUp();
}

}
