/*
	File:		QTSound.c
	
	Contains:	Sound code for QuickTime CreateMovie sample
	
	Written by:	Scott Kuechle
				(based heavily on QuickTime sample code in Inside Macintosh: QuickTime)

	Copyright:	© 1998 by Apple Computer, Inc. All rights reserved
	
	Change History (most recent first)
	
		<2>		9/28/98		rtm		changes for Metrowerks compiler
		<1>		6/26/98		srk		first file


*/

/************************************************************
*                                                           *
*    INCLUDE FILES                                          *
*                                                           *
*************************************************************/
#include "mnmQuickTimeSound.hpp"


#if !defined(_MSC_VER) && _WIN32
	#include <Win32Headers.mch>
	#define TARGET_OS_WIN32		1
#else
	#include <ConditionalMacros.h>
#endif

#if TARGET_OS_WIN32
	#include <QTML.h>
	#define	STRICT
	#include <windows.h>
#endif

#include <Resources.h>
#include <FixMath.h>
#include <Sound.h>

//#include "CreateMovie.h"

#include "Core/dbg/dbgLog.hpp"

/************************************************************
*                                                           *
*    TYPE DEFINITIONS                                       *
*                                                           *
*************************************************************/

typedef SndCommand *SndCmdPtr;

#if PRAGMA_STRUCT_ALIGN
	#pragma options align=mac68k
#elif PRAGMA_STRUCT_PACKPUSH
	#pragma pack(push, 2)
#elif PRAGMA_STRUCT_PACK
	#pragma pack(2)
#endif

	/* 'snd ' resource format 1 - see the Sound Manager chapter of Inside Macintosh: Sound
		for the details */
	typedef struct 
	{
		short format;
		short numSynths;
	} Snd1Header, *Snd1HdrPtr, **Snd1HdrHndl;

	/* 'snd ' resource format 2 - see the Sound Manager chapter of Inside Macintosh: Sound
		for the details */

	typedef struct 
	{
		short format;
		short refCount;
	} Snd2Header, *Snd2HdrPtr, **Snd2HdrHndl;


#if PRAGMA_STRUCT_ALIGN
	#pragma options align=reset
#elif PRAGMA_STRUCT_PACKPUSH
	#pragma pack(pop)
#elif PRAGMA_STRUCT_PACK
	#pragma pack()
#endif

/************************************************************
*                                                           *
*    CONSTANTS                                              *
*                                                           *
*************************************************************/

#define	kOurSoundResourceID 128

#define	kSoundSampleDuration 1
#define	kSyncSample 0
#define	kTrackStart	0
#define	kMediaStart	0
/* 
for the following constants, please consult the Macintosh
Audio Compression and Expansion Toolkit
*/
#define kMACEBeginningNumberOfBytes 6
#define kMACE31MonoPacketSize 2
#define kMACE31StereoPacketSize 4
#define kMACE61MonoPacketSize 1
#define kMACE61StereoPacketSize 2


/************************************************************
*                                                           *
*    FUNCTION PROTOTYPES                                    *
*                                                           *
*************************************************************/

/************************************************************
*                                                           *
*    QTSound_GetSndHdrOffset()                              *
*                                                           *
*    Returns an pointer to the first sound command in the   *
*    sound resource                                         *
*                                                           *
*************************************************************/
static long QTSound_GetSndHdrOffset (Handle sndHandle)
{
	short howManyCmds;
	long sndOffset = 0;
	Ptr sndPtr;

	if (sndHandle == nil)
	{
		return 0;
	}
	sndPtr = *sndHandle;
	if (sndPtr == nil)
	{
		return 0;
	}

	if ((*(SndListPtr)sndPtr).format == firstSoundFormat) 
	{
		short synths = ((SndListPtr)sndPtr)->numModifiers;
		sndPtr += ( sizeof(Snd1Header) + (sizeof(ModRef) * synths) );
	}
	else 
	{
		sndPtr += sizeof(Snd2Header);
	}

	howManyCmds = *(short *)sndPtr;

	sndPtr += sizeof(howManyCmds);
	/* 
	sndPtr is now at the first sound command--cruise all
	commands and find the first soundCmd or bufferCmd
	*/
	while (howManyCmds > 0) 
	{
		switch (((SndCmdPtr)sndPtr)->cmd) 
		{
			case (soundCmd + dataOffsetFlag):
			case (bufferCmd + dataOffsetFlag):
				sndOffset = ((SndCmdPtr)sndPtr)->param2;
				howManyCmds = 0;	/* done, get out of loop */
			break;
			
			default:/* catch any other type of commands */
				sndPtr += sizeof(SndCommand);
				howManyCmds--;
			break;
		}
	}/* done with all commands */

	return sndOffset;
}/* of GetSndHdrOffset */ 


/************************************************************
*                                                           *
*    QTSound_CreateSoundDescription()                       *
*                                                           *
*    Creates a SoundDescription structure for a given sound *
*    sample                                                 *
*                                                           *
*************************************************************/

static void QTSound_CreateSoundDescription (Handle sndHandle,
											SoundDescriptionHandle sndDesc,
											long *sndDataOffset,
											long *numSamples,
											long *sndDataSize )
{
	long sndHdrOffset = 0;
	long sampleDataOffset;
	SoundHeaderPtr sndHdrPtr = nil;
	long numFrames;
	long samplesPerFrame;
	long bytesPerFrame;
	SoundDescriptionPtr sndDescPtr;

		*sndDataOffset = 0;
		*numSamples = 0;
		*sndDataSize = 0;

		SetHandleSize( (Handle)sndDesc, sizeof(SoundDescription) );
		//CheckError(MemError(),"SetHandleSize error");
		
		sndHdrOffset = QTSound_GetSndHdrOffset (sndHandle);
		if (sndHdrOffset == 0)
		{
			//CheckError(-1, "QTSound_GetSndHdrOffset error");
		}

		/* we can use pointers since we don't move memory */
		sndHdrPtr = (SoundHeaderPtr) (*sndHandle + sndHdrOffset);
		sndDescPtr = *sndDesc;

		sndDescPtr->descSize = sizeof (SoundDescription);
		/* total size of sound description structure */
		sndDescPtr->resvd1 = 0;
		sndDescPtr->resvd2 = 0;
		sndDescPtr->dataRefIndex = 1;
		sndDescPtr->compressionID = 0;
		sndDescPtr->packetSize = 0;
		sndDescPtr->version = 0;
		sndDescPtr->revlevel = 0;
		sndDescPtr->vendor = 0; 

		switch (sndHdrPtr->encode) 
		{
			case stdSH:
				sndDescPtr->dataFormat = kRawCodecType;
				/* uncompressed offset-binary data */
				sndDescPtr->numChannels = 1;
				/* number of channels of sound */
				sndDescPtr->sampleSize = 8;
				/* number of bits per sample */
				sndDescPtr->sampleRate = sndHdrPtr->sampleRate;
				/* sample rate */
				*numSamples = sndHdrPtr->length;
				*sndDataSize = *numSamples;
				bytesPerFrame = 1; 
				samplesPerFrame = 1;
				sampleDataOffset = (Ptr)&sndHdrPtr->sampleArea - (Ptr)sndHdrPtr;
			break;

			case extSH:
			{
				ExtSoundHeaderPtr extSndHdrP;

					extSndHdrP = (ExtSoundHeaderPtr)sndHdrPtr;
					sndDescPtr->dataFormat = kRawCodecType;
					/* uncompressed offset-binary data */

					/* we typecast a long to a short here, and it should really be fixed */
					sndDescPtr->numChannels = (short)extSndHdrP->numChannels;
					/* number of channels of sound */
					sndDescPtr->sampleSize = extSndHdrP->sampleSize;
					/* number of bits per sample */
					sndDescPtr->sampleRate = extSndHdrP->sampleRate; 
					/* sample rate */
					numFrames = extSndHdrP->numFrames;
					*numSamples = numFrames;
					bytesPerFrame = extSndHdrP->numChannels * ( extSndHdrP->sampleSize / 8);
					samplesPerFrame = 1;
					*sndDataSize = numFrames * bytesPerFrame;
					sampleDataOffset = (Ptr)(&extSndHdrP->sampleArea) - (Ptr)extSndHdrP;
			}
			break;

			case cmpSH:
			{
				CmpSoundHeaderPtr cmpSndHdrP;

				cmpSndHdrP = (CmpSoundHeaderPtr)sndHdrPtr;
				/* we typecast a long to a short here, and it should really be fixed */

				sndDescPtr->numChannels = (short)cmpSndHdrP->numChannels;
				/* number of channels of sound */
				sndDescPtr->sampleSize = cmpSndHdrP->sampleSize;
				/* number of bits per sample before compression */
				sndDescPtr->sampleRate = cmpSndHdrP->sampleRate;
				/* sample rate */
				numFrames = cmpSndHdrP->numFrames; 
				sampleDataOffset =(Ptr)(&cmpSndHdrP->sampleArea) - (Ptr)cmpSndHdrP;
				
				switch (cmpSndHdrP->compressionID) 
				{
					case threeToOne:
						sndDescPtr->dataFormat = kMACE3Compression;
						/* compressed 3:1 data */
						samplesPerFrame = kMACEBeginningNumberOfBytes; 
						*numSamples = numFrames * samplesPerFrame;
						
						switch (cmpSndHdrP->numChannels) 
						{
							case 1:
								bytesPerFrame = cmpSndHdrP->numChannels 
													* kMACE31MonoPacketSize;
							break;
							
							case 2:
								bytesPerFrame = cmpSndHdrP->numChannels 
													* kMACE31StereoPacketSize;
							break;
							
							default: 
								//CheckError(-1, "Corrupt sound data" );
							break;
						}
						
					*sndDataSize = numFrames * bytesPerFrame;
					break;
					
					case sixToOne:
						sndDescPtr->dataFormat = kMACE6Compression; 
						/* compressed 6:1 data */
						samplesPerFrame = kMACEBeginningNumberOfBytes; 
						*numSamples = numFrames * samplesPerFrame;
						
						switch (cmpSndHdrP->numChannels) 
						{
							case 1:
								bytesPerFrame = cmpSndHdrP->numChannels 
													* kMACE61MonoPacketSize; 
							break;
							
							case 2:
								bytesPerFrame = cmpSndHdrP->numChannels 
													* kMACE61StereoPacketSize; 
							break;
							
							default:
								//CheckError(-1, "Corrupt sound data" );
							break;
						}
						
						*sndDataSize = (*numSamples) * bytesPerFrame;
					break;
					
					default:
						//CheckError(-1, "Corrupt sound data" );
					break;
					}
					
				} /* switch cmpSndHdrP->compressionID:*/
				
				break;  /* of cmpSH: */

				default:
					//CheckError(-1, "Corrupt sound data" );
				break;

		} /* switch sndHdrPtr->encode */
		
	*sndDataOffset = sndHdrOffset + sampleDataOffset; 
} 


/************************************************************
*                                                           *
*    QTSound_CreateMySoundTrack()                           *
*                                                           *
*    Creates a QuickTime movie sound track & media data     *
*                                                           *
*************************************************************/

namespace mnmQuickTimeSound
{

void QTSound_CreateMySoundTrack(Movie theMovie, const char* i_SoundFile)
{
	Track theTrack;
	Media theMedia;
	Handle sndHandle = nil;
	SoundDescriptionHandle sndDesc = nil;
	long sndDataOffset;
	long sndDataSize;
	long numSamples;
	OSErr err = noErr;

//#if TARGET_OS_WIN32
//
//	char path[MAX_PATH+1];
//	short resID;
//	FSSpec fsspec;
//
//	fsspec.vRefNum = 0;
//	fsspec.parID = 0;
//	//GetModuleFileName( NULL, path, MAX_PATH+1);
//
//	char sound_file[512];
//	strcpy(sound_file, i_SoundFile);
//	NativePathNameToFSSpec(sound_file, &fsspec, 0);
//
//	//QTNewDataReferenceFromFullPathCFString
//
//	/* open our application resource file so we
//		can access the Macintosh 'snd ' resource */ 
//	resID = FSpOpenResFile(&fsspec, fsRdPerm);
//	//CheckError(ResError(), "FSpOpenResFile error" );
//	DBG_LOG2("(%d) %s", resID, sound_file);
//
//	CFStringRef srcAudioFilePath;
//	srcAudioFilePath = CFStringCreateWithCString(NULL, sound_file, kCFStringEncodingMacRoman);
//	OSType audioFileDataRefType;
//    err = QTNewDataReferenceFromFullPathCFString(
//                           srcAudioFilePath,
//                           kQTNativeDefaultPathStyle,
//                           0,
//                           &sndHandle,
//                           &audioFileDataRefType );
//#endif

	// Get the sound
//	sndHandle = GetResource('snd ', resID); //kOurSoundResourceID);
	//CheckError(ResError(), "GetResource error" );
	//if (sndHandle == nil)
	//{
	//	return;
	//}

	// Allocate memory for the sound description
	//sndDesc = (SoundDescriptionHandle) NewHandle(4);
	//CheckError(MemError(), "NewHandle error" );

	// Create the sound description that correctly describes the sound samples
	// obtained from the 'snd ' resource.
	//QTSound_CreateSoundDescription(sndHandle, 
	//							   sndDesc, 
	//							   &sndDataOffset, 
	//							   &numSamples, 
	//							   &sndDataSize);

//OSErr error; 
//Handle dataRef; 
//OSType dataRefType; 
//
//short fileID = movieInDataForkResID; 
//short flags = 0; 
//error = NewMovieFromDataRef(&theMovie, flags, 
//        &fileID, sndHandle, audioFileDataRefType);
//return;

	FSSpec soundfileSpec;
	char sound_file[512];
	strcpy(sound_file, i_SoundFile);
	NativePathNameToFSSpec(sound_file, &soundfileSpec, 0L);

	//
	//	TODO what memory needs to be cleaned up?
	//

	Movie audioMovie = NULL;
	short audioMovieRefNum = 0;
	short audioMovieResId = 0;
	err = OpenMovieFile( &soundfileSpec, &audioMovieRefNum, fsRdPerm );
	err = NewMovieFromFile( &audioMovie, audioMovieRefNum, &audioMovieResId, NULL, newMovieActive, NULL );
	err = CloseMovieFile( audioMovieRefNum );

	Track	audioCopyTrack = NULL;
	Media	audioCopyMedia = NULL;
	SetMovieTimeScale( audioMovie, GetMovieTimeScale(theMovie) );
	audioCopyTrack = GetMovieTrack(audioMovie, 1);
	audioCopyMedia = GetTrackMedia(audioCopyTrack);

	long duration = GetMovieDuration(audioMovie);

	// 1. Create the track
	//theTrack = NewMovieTrack(theMovie,						/* movie specifier */
	//						  0,							/* width */
	//						  0,							/* height */
	//						  kFullVolume);					/* track volume */
	//CheckError(GetMoviesError(), "NewMovieTrack error" );

	// 2. Create the media for the track
	//theMedia = NewTrackMedia(theTrack,							/* track specifier */
	//						  SoundMediaType,					/* type of media */
	//						  FixRound((**sndDesc).sampleRate),	/* time coordinate system */
	//						  nil,								/* data reference */
	//						  0);								/* data reference type */
	//CheckError(GetMoviesError(), "NewTrackMedia error" );

	// 3. Establish a media-editing session
	//err = BeginMediaEdits(theMedia);
	//CheckError( err, "BeginMediaEdits error" );
	
	// 3a. Add Samples to the media
	//err = AddMediaSample(theMedia,							/* media specifier */		
	//					  sndHandle,						/* handle to sample data - dataIn */
	//					  sndDataOffset,					/* specifies offset into data reffered to by dataIn handle */
	//					  sndDataSize,						/* number of bytes of sample data to be added */
	//					  kSoundSampleDuration,				/* duration of each sound sample */
	//					  (SampleDescriptionHandle)sndDesc,	/* sample description handle */
	//					  numSamples,						/* number of samples */
	//					  kSyncSample,						/* control flag indicating self-contained samples */
	//					  nil);								/* returns a time value where sample was insterted */
	//CheckError( err, "AddMediaSample error" );

	// 3b. End media-editing session
	//err = EndMediaEdits(theMedia);
	//CheckError( err, "EndMediaEdits error" );

	err = BeginMediaEdits(theMedia);

	Track destTrack = NULL;
	err = AddEmptyTrackToMovie( audioCopyTrack, theMovie, NULL, NULL, &destTrack );
	err = InsertTrackSegment( audioCopyTrack, destTrack, 0, duration, 0);

	// 4. Inserts a reference to a media segment into the track
	err = InsertMediaIntoTrack(theTrack,						/* track specifier */ 
							    kTrackStart,					/* track start time */
							    kMediaStart,					/* media start time */
							    GetMediaDuration(audioCopyMedia),	/* media duration */
							    fixed1);						/* media rate ((Fixed) 0x00010000L) */
	//CheckError( err, "InsertMediaIntoTrack error" );
	
	err = EndMediaEdits(theMedia);

	//if (sndDesc != nil)
	//{
	//	DisposeHandle( (Handle)sndDesc);
	//}
} 

} // namespace