/*****************************************************************************
**  snSoundUtil.cpp
**
**      snSoundUtil contains functions related to sound parameter conversion
**	for the sn package.
**
**	StudioGPU
**	Copyright(C) 2003-2 - All Rights Reserved
\****************************************************************************/

#ifdef SN_SOUNDUTIL_HPP
#error snSoundUtil.hpp multiply included
#endif
#define SN_SOUNDUTIL_HPP


#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif

//----------------------------------------------------------------------------
//	Forward declarations
//----------------------------------------------------------------------------
class itString;
class snSoundJob;
class snSoundJob2D;
class snSoundJob2DStreamed;
class snSoundJob3D;
class snSoundJob3DStreamed;
class snSoundJobMIDI;
class snSoundJobMP3;
class snSoundJobRedbook;


//----------------------------------------------------------------------------
//	Sound types
//----------------------------------------------------------------------------
namespace
{
	enum SoundFormatType
	{
		e_TYPE_STATIC	= 0x1,
		e_TYPE_STREAM	= 0x2,
		e_TYPE_2D		= 0x3,
		e_TYPE_3D		= 0x4,
		e_TYPE_REDBOOK	= 0x5,
		e_TYPE_MIDI		= 0x6,
		e_TYPE_MP3		= 0x7,
	};
}


//----------------------------------------------------------------------------
//	snSoundUtil
//----------------------------------------------------------------------------
namespace snSoundUtil
{
//----------------------------------------------------------------------------
//	Sound Job Creation
//----------------------------------------------------------------------------

	//========================================================================
	//	CreateSoundJob2DStatic()
	//========================================================================
	snSoundJob2D *			CreateSoundJob2DStatic( const fsLocator& i_Filename );

	//========================================================================
	//	CreateSoundJob2DStreamed()
	//========================================================================
	snSoundJob2DStreamed *	CreateSoundJob2DStreamed( const fsLocator& i_Filename );

	//========================================================================
	//	CreateSoundJob3DStatic()
	//========================================================================
	snSoundJob3D *			CreateSoundJob3DStatic( const fsLocator& i_Filename );

	//========================================================================
	//	CreateSoundJob3DStreamed()
	//========================================================================
	snSoundJob3DStreamed *	CreateSoundJob3DStreamed( const fsLocator& i_Filename );

	//========================================================================
	//	CreateSoundJobMIDI()
	//========================================================================
	snSoundJobMIDI *		CreateSoundJobMIDI( const fsLocator& i_Filename );

	//========================================================================
	//	CreateSoundJobMP3()
	//========================================================================
	snSoundJobMP3 *		CreateSoundJobMP3( const fsLocator& i_Filename );

	//========================================================================
	//	CreateSoundJobStaticRedbook()
	//========================================================================
	snSoundJobRedbook *		CreateSoundJobRedbook( const fsLocator& i_Filename );

	//========================================================================
	//	CloneSoundJob2DStatic()
	//========================================================================
	snSoundJob2D *			CloneSoundJob2DStatic( const snSoundJob2D& i_SoundJob );

	//========================================================================
	//	CloneSoundJob2DStreamed()
	//========================================================================
	snSoundJob2DStreamed *	CloneSoundJob2DStreamed( const snSoundJob2DStreamed& i_SoundJob );

	//========================================================================
	//	CloneSoundJob3DStatic()
	//========================================================================
	snSoundJob3D *			CloneSoundJob3DStatic( const snSoundJob3D& i_SoundJob );

	//========================================================================
	//	CloneSoundJob3DStreamed()
	//========================================================================
	snSoundJob3DStreamed *	CloneSoundJob3DStreamed( const snSoundJob3DStreamed& i_SoundJob );

	//========================================================================
	//	CloneSoundJobMIDI()
	//========================================================================
	snSoundJobMIDI *		CloneSoundJobMIDI( const snSoundJobMIDI& i_SoundJob );

	//========================================================================
	//	CloneSoundJobMP3()
	//========================================================================
	snSoundJobMP3 *			CloneSoundJobMP3( const snSoundJobMP3& i_SoundJob );

	//========================================================================
	//	CloneSoundJobStaticRedbook()
	//========================================================================
	snSoundJobRedbook *		CloneSoundJobRedbook( const snSoundJobRedbook& i_SoundJob );


//----------------------------------------------------------------------------
//	Conversions
//----------------------------------------------------------------------------

	//========================================================================
	//	ConvertVolume()
	//
	//	Given a percent from 0 to 100, return the correct volume value.  The
	//	percentage is LINEAR, so the coverted volume will not be a percentage
	//	on the curve, but a percentage of the actual volume.
	//========================================================================
	int	ConvertVolume( float i_SoundPercent );

	//========================================================================
	//	ConvertPan()
	//
	//	Given a percent from -1.0 to 1.0, return the correct Pan value.  The
	//	percentage is LINEAR, so the coverted Pan will not be a percentage
	//	on the curve, but a percentage of the actual Pan.
	//========================================================================
	int	ConvertPan( float i_SoundPercent );

	//========================================================================
	//	ConvertFrequency()
	//
	//	Given a multiplier from -10.0 to 10.0, return the correct Frequency 
	//	value.  The ORIGINAL sound frequency is passed in and a conversion
	//	is made.  So if the multiplier if 1.0 the sound is doubled, 2.0 is
	//	tripled, -1.0 is halved, and 0.0 is back to the original.
	//========================================================================
	int	ConvertFrequency( float i_SoundPercent, float i_SoundFrequency );


//----------------------------------------------------------------------------
//	Type specific functions
//----------------------------------------------------------------------------

	//========================================================================
	//	()
	//========================================================================
	

//----------------------------------------------------------------------------
//	Base functions
//----------------------------------------------------------------------------

	//========================================================================
	//	Don't call Init() and CleanUp() yourself; they are called 
	//	by the package Init and Cleanup.
	//========================================================================
	void Init();
	void CleanUp() throw();
}
