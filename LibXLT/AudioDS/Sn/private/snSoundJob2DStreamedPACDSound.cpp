/****************************************************************************\
**  snSoundJob2DStreamedPACDSound.cpp
**
**  This class implements the DirectSound portion of the
**	snSoundJob2DStreamedPAC.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "AudioDS/sn/private/snSoundJob2DStreamedPACDSound.hpp"

#include "AudioDS/sn/private/dstrwave.hpp"			// wave i/o functions
#include "AudioDS/sn/private/snDSoundGlobalWin.hpp"
#include "AudioDS/sn/private/snSoundUtilPAC.hpp"
#include "AudioDS/sn/snExceptionX.hpp"
#include "AudioDS/sn/snSoundSystem.hpp"
#include "Core/app/appTime.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/private/fsFileUtilPAC.hpp"
#include "Core/gf/gfFileBin.hpp"
#include "Core/gf/gfFileTranslationMgr.hpp"

#include <string>
#include <dsound.h>


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
namespace
{
const int	NUM_PLAY_NOTIFICATIONS				= 12;	// # of times to add data per buffer size
const float SN_DEFAULT_STREAM_BUFFER_IN_SECONDS	= 6.0f;	// size of buffer in seconds
}


//========================================================================
//	default and copy constructors
//========================================================================
snSoundJob2DStreamedPAC::snSoundJob2DStreamedPAC()
: snSoundJob2DPAC()
{
	Init();
}


//========================================================================
//========================================================================
snSoundJob2DStreamedPAC::snSoundJob2DStreamedPAC( const snSoundJob2DStreamedPAC& i_CopyFrom )
: snSoundJob2DPAC()
{
	*this = i_CopyFrom;
}


//========================================================================
//========================================================================
snSoundJob2DStreamedPAC::~snSoundJob2DStreamedPAC()
{
	Free();
}


//========================================================================
//	Reload()
//
//		Reload the raw sound data
//========================================================================
void	
snSoundJob2DStreamedPAC::Reload()
{
	snSoundJob2DPAC::Reload();
}


//========================================================================
//	Unload()
//
//		Pause the sound and unload the raw sound data
//========================================================================
void	
snSoundJob2DStreamedPAC::Unload()
{
	snSoundJob2DPAC::Unload();
}


//========================================================================
//	Start()
//
//	Start playing a sound
//
//		1. make sure everything's ready
//		2. Set a flag to signal this sound CAN be started
//		3. the Think() function takes care of it from there
//========================================================================
void	
snSoundJob2DStreamedPAC::Start( float i_SimulationTime )
{
	//	verify everything is ready to go
	//
	if (	( !IsLoaded() )
		||	( !snSoundSystem::IsInitialized() ) )
	{
		return;
	}

	if ( this->GetBuffer() )
	{
		this->SetShouldStart( true );
		this->SetPaused( false );
		this->SetFinished( false );
		this->SetDoneReading( false );

		//m_dwLastPlayPosInBuffer	= 0;
		//m_EndOfStream			= 0;
		//m_BytesWritten			= 0;
		//m_StreamUpdateLast		= i_SimulationTime;
		//m_LoopCount				= 0;
		//m_LoopTarget			= 0;
	}
}


//========================================================================
//	Restart()
//
//	Restart playing a sound
//========================================================================
void	
snSoundJob2DStreamedPAC::Restart( float i_SimulationTime )
{
	//	this should re-load the sound
	
	snSoundJob2DPAC::Restart( i_SimulationTime );
}


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
//		6. do FadeChanging (if applicable)
//		7. do PanChanging (if applicable)
//		8. do FrequencyChanging (if applicable)
//		9. do other effects (if applicable)
//========================================================================
void	
snSoundJob2DStreamedPAC::Think( float i_SimulationTime )
{
	if ( this->IsFinished() )
	{
		return;
	}

	if (	!(this->GetBuffer())
		||	( !snSoundSystem::IsInitialized() ) )
	{
		this->Stop();
		return;
	}

	//	Everything, is ok so far...let's check the status of the buffer.
	//
	HRESULT			Error;
	unsigned long	Status;

	if ( Error = this->GetBuffer()->GetStatus( &Status ) )
	{
		snDSoundGlobal::PrintDSError( Error );
		return;
	}

	if ( Status & DSBSTATUS_BUFFERLOST )
	{
		// if we lost our directsound buffer, restore it
		//
		this->GetBuffer()->Release();
		this->SetBuffer( NULL );
		this->CreateBuffer();

		this->Start( i_SimulationTime );
	}
	else
	if ( !Status && !this->IsPaused() && !this->IsShouldStart() )
	{
		// if we got a non-playing status and we're not paused, kill the sound job
		//
		this->Stop();
		return;
	}

	//	If the sound has been started, then update it with more 
	//	data if it's ready.
	//
	if ( !this->IsShouldStart() )
	{
		bool	bDoUpdate	= false;

		//	Get the current position of the sound play cursor
		//
		unsigned long	dwWriteCursor;
		unsigned long	dwPlayPosInBuffer;
		Error = this->GetBuffer()->GetCurrentPosition( &dwPlayPosInBuffer, &dwWriteCursor );
		if (Error != DS_OK)
		{
			DBG_LOG( "couldn't read current postion from buffer " << Error );
			return;
		}

		//	DBG_LOG2( "play cursor = %6lu  write cursor = %6lu", dwPlayPosInBuffer, dwWriteCursor );

		//	Check to see if the play cursor has moved far enough along the file to warrant
		//	reading in another chunk
		//
		//	first check if the buffer has looped yet
		//
		if ( dwPlayPosInBuffer > m_dwLastPlayPosInBuffer )
		{
			//	we haven't looped the buffer yet, so now check if enough sound has played to 
			//	warrant an update to the buffer.
			//
			if ( (dwPlayPosInBuffer - m_dwLastPlayPosInBuffer) >= (m_StreamUpdateBytes) )
			{
				//	not yet
				//
				bDoUpdate	= true;
				m_dwLastPlayPosInBuffer	+= (unsigned long)m_StreamUpdateBytes;
			}
		}
		else
		{
			//	we HAVE looped the buffer, so now check if enough sound has played to 
			//	warrant an update to the buffer.  If so, we'll update from the beginning
			//	of the buffer
			//
			
			//DBG_LOG3( " end(%6lu) + begin(%6lu) >= %8.2f", (m_dwBufferSize - m_dwLastPlayPosInBuffer), dwPlayPosInBuffer, (m_StreamUpdateBytes) );

			if ( ((m_dwBufferSize - m_dwLastPlayPosInBuffer) + dwPlayPosInBuffer) >= (m_StreamUpdateBytes) )
			{
				bDoUpdate	= true;
				m_dwLastPlayPosInBuffer	= 0;
			}
		}

		//	if the sound is finished AND we've looped the buffer, finish it
		//
		if ( this->IsDoneReading() )
		{
			//if ( dwPlayPosInBuffer > m_dwLastPlayPosInBuffer )
			//{
			//	if ( (dwPlayPosInBuffer - m_dwLastPlayPosInBuffer) >= (m_dwNotifySize) )
			//	{
			//		this->SetFinished( true );
			//		Stop();
			//	}
			//}
			//else
			//{
			//	if ( ((m_dwBufferSize - m_dwLastPlayPosInBuffer) + dwPlayPosInBuffer) >= (m_dwNotifySize) )
			//	{
			//		this->SetFinished( true );
			//		Stop();
			//	}
			//}
			
			//DBG_LOG3( "play=%6lu last=%6lu final=%6lu", dwPlayPosInBuffer, m_dwLastPlayPosInBuffer, m_dwFinalWritePosInBuffer );
			if ((   ( dwPlayPosInBuffer > m_dwFinalWritePosInBuffer )
				 && ( m_dwLastPlayPosInBuffer > m_dwFinalWritePosInBuffer ) )
				 ||
			    (   ( dwPlayPosInBuffer < m_dwFinalWritePosInBuffer )
				 && ( m_dwLastPlayPosInBuffer < m_dwFinalWritePosInBuffer ) ) )
			{
				if ( !this->m_Flag.bAlmostDone )
				{
					m_Flag.bAlmostDone = true;
				}
			}

			if ( m_Flag.bAlmostDone )
			{
				if (   ( dwPlayPosInBuffer >= m_dwFinalWritePosInBuffer )
					&& ( m_dwLastPlayPosInBuffer <= m_dwFinalWritePosInBuffer ) )
				{
					this->SetFinished( true );
					Stop();
					m_Flag.bAlmostDone = false;
				}
			}
		}

		//	done reading yes/no?
		//if ( this->IsDoneReading() )
		//	if (bDoUpdate)
		//		DBG_LOG3( "+play cursor = %6lu  last read = %6lu  stream update bytes = %6lu", dwPlayPosInBuffer, m_dwLastPlayPosInBuffer, m_StreamUpdateBytes );
		//	else
		//		DBG_LOG3( " play cursor = %6lu  last read = %6lu  stream update bytes = %6lu", dwPlayPosInBuffer, m_dwLastPlayPosInBuffer, m_StreamUpdateBytes );

		// if it is time for an update to the buffers do it here.
		//
		if ( bDoUpdate )
		{
			//DBG_LOG2( "    Update %lu (%6.2f)", m_dwLastPlayPosInBuffer, m_StreamUpdateBytes );

			// reset the delta since our last update
			//
			m_StreamUpdateLast = i_SimulationTime;

			// A play notification has been received.
			//
			LPBYTE			lpWrite1;
			unsigned long	dwWrite1;
			UINT			cbActual = 0;

			// If the entire file has been read into the buffer, bFoundEnd will be TRUE.
			//
			if ( !this->IsFoundEnd() )
			{
				this->FillBuffer( m_dwNotifySize );
			}
			else
			{	
				// Allow the rest of the bytes to be played and fill here
				// with silence. The next notification will quit the while loop.
				//
				Error = this->GetBuffer()->Lock(
												m_dwNextWriteOffset, 
												m_dwNotifySize, 
												(void **) &lpWrite1, &dwWrite1,
												NULL, NULL, 0 );

				FillMemory(lpWrite1, dwWrite1,(BYTE)(wiWave.pwfx->wBitsPerSample == 8 ? 128 : 0));

				Error = this->GetBuffer()->Unlock((LPVOID)lpWrite1, dwWrite1, NULL, 0 );

				//	Calculate the next write offset.
				//
				m_dwNextWriteOffset += dwWrite1;
				if (m_dwNextWriteOffset >= m_dwBufferSize)
				{
					m_dwNextWriteOffset -= m_dwBufferSize;
				}

				// We don't want to cut off the sound before it's done playing.
				// When SetFinish() is set, the next notification event will post a stop message.
				//
				if ( wiWave.mmckInRIFF.cksize > m_dwNotifySize &&
					 m_dwProgress >= wiWave.mmckInRIFF.cksize - m_dwNotifySize )
				{
					if ( !IsDoneReading() )
					{
						m_dwFinalWritePosInBuffer	= m_dwNextWriteOffset;
						//DBG_LOG3( "+-> play=%6lu last=%6lu final=%6lu", dwPlayPosInBuffer, m_dwLastPlayPosInBuffer, m_dwFinalWritePosInBuffer );
					}
					this->SetDoneReading( true );
				}
				else
				{
					// for short files.
					//
					if (m_dwProgress >= wiWave.mmckInRIFF.cksize)
					{
						if ( !IsDoneReading() )
						{
							m_dwFinalWritePosInBuffer	= m_dwNextWriteOffset;
							//DBG_LOG3( "*-> play=%6lu last=%6lu final=%6lu", dwPlayPosInBuffer, m_dwLastPlayPosInBuffer, m_dwFinalWritePosInBuffer );
						}
						this->SetDoneReading( true );
					}
				}
			}
		}
	}

	//	If some function has updated some aspect of the volume, 
	//	recalculate it here.
	//
	if ( this->IsVolumeChanged() )
	{
		this->SetVolume( GetVolume() );
	}

	//	Do the effects
	//
	this->ThinkEffects( i_SimulationTime );

	//	Localize the effect
	//
	if ( this->IsLocalize() )
	{
		this->ThinkLocalize( i_SimulationTime );
	}

	//	if the buffer hasn't been started then start it
	//
	if ( this->IsShouldStart() )
	{
		this->PlayBuffer( i_SimulationTime );
	}
}


//========================================================================
//	Pause()
//========================================================================
void	
snSoundJob2DStreamedPAC::Pause()
{
	if (	( !IsLoaded() )
		||	( IsPaused() )
		||	( !snSoundSystem::IsInitialized() ) )
	{
		return;
	}

	if ( this->GetBuffer() )
	{
		HRESULT			Error;

		Error = this->GetBuffer()->Stop();
		if (Error)
		{
			snDSoundGlobal::PrintDSError( Error );
		}

		//	Save the current position
		//
		unsigned long restorePosition;
		this->GetBuffer()->GetCurrentPosition( &(restorePosition), NULL );
		this->SetRestorePosition( restorePosition );

		//	Change start time to hold the amount of time elapsed
		//	When it is restarted we will set it back to a time
		//
		this->SetStartTime( appTime::GetTime() - this->GetStartTime() );

		SetPaused( true );
	}
}


//========================================================================
//	Resume()
//========================================================================
void	
snSoundJob2DStreamedPAC::Resume( float i_SimulationTime )
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


//========================================================================
//	CreateBuffer()
//========================================================================
void	
snSoundJob2DStreamedPAC::CreateBuffer()
{
    DSBUFFERDESC dsbd;
    HRESULT      dsRetVal;

    int nChkErr;
	int nRem;

    // This portion of the WAVE I/O is patterned after what's in DSTRWAVE, which
    // was in turn adopted from WAVE.C which is part of the DSSHOW sample.
    //
	WAVEINFOCA	tempWaveInfo;
	tempWaveInfo.pwfx = NULL;

	std::string filename;
	itString	UnicodeFilename;
	fsLocator ExpandedLoc(this->GetFilename());
	gfFileTranslationMgr::ExpandLocator(ExpandedLoc);
	fsFileUtilPAC::LocatorToUnicodeFilename( ExpandedLoc, UnicodeFilename );
	//UnicodeFilename += 0; // not needed anymore, GetString() is NULL-terminated

    if (( nChkErr = WaveUtil::WaveOpenFile( const_cast<const LPWSTR>(UnicodeFilename.GetString()), &tempWaveInfo.hmmio, &tempWaveInfo.pwfx, &tempWaveInfo.mmckInRIFF )) != 0 )
    {
		GlobalFree( tempWaveInfo.pwfx );
		tempWaveInfo.pwfx = NULL;

		//	Error: could not find the file
		//
		throw fsFileDoesntExistX( this->GetFilename() );
    }

    if ( tempWaveInfo.pwfx->wFormatTag != WAVE_FORMAT_PCM )
    {
		WaveUtil::WaveCloseReadFile( &tempWaveInfo.hmmio, &tempWaveInfo.pwfx );

		GlobalFree( tempWaveInfo.pwfx );
		tempWaveInfo.pwfx = NULL;

		//	Error: unsupported sound type
		//
		throw snUnsupportedSoundFileTypeX( this->GetFilename() );
	}

    // Seek to the data chunk. mmck.ckSize will be the size of all the data in the file.
	//
    if ( (nChkErr = WaveUtil::WaveStartDataRead( &tempWaveInfo.hmmio, &tempWaveInfo.mmck, &tempWaveInfo.mmckInRIFF )) != 0 )
    {
        WaveUtil::WaveCloseReadFile( &tempWaveInfo.hmmio, &tempWaveInfo.pwfx );

		GlobalFree( tempWaveInfo.pwfx );
		tempWaveInfo.pwfx = NULL;

		//	Error: corrupt file
		//
		throw snCorruptedFileX( this->GetFilename() );
    }
	
    // Calculate a buffer length in seconds. This should be an integral number of the
	// number of bytes in one notification period. 
	//
	unsigned long	notifySize;
	unsigned long	bufferSize;

	notifySize = (unsigned long)(tempWaveInfo.pwfx->nSamplesPerSec * m_StreamBufferInSeconds * tempWaveInfo.pwfx->nBlockAlign);
	notifySize = notifySize / m_NumberOfPlayNotifications;

	// the notify size should be an intergral multiple of the nBlockAlignvalue.
	//
	if ((nRem = notifySize%(unsigned long)tempWaveInfo.pwfx->nBlockAlign) != 0)
	{
		notifySize += (tempWaveInfo.pwfx->nBlockAlign - nRem);
	}
	bufferSize = notifySize * m_NumberOfPlayNotifications;
	m_StreamUpdateBytes	= (float)notifySize;

    //Create the secondary DirectSoundBuffer object to receive our sound data.
	//
    memset( &dsbd, 0, sizeof( DSBUFFERDESC ));
    dsbd.dwSize = sizeof( DSBUFFERDESC );

    // Use new GetCurrentPosition() accuracy (DirectX 2 feature)
	//
    dsbd.dwFlags =	  DSBCAPS_CTRLPAN 
					| DSBCAPS_CTRLVOLUME 
					| DSBCAPS_CTRLFREQUENCY 
					| DSBCAPS_GETCURRENTPOSITION2;
    dsbd.dwBufferBytes = bufferSize;

    //Set Format properties according to the WAVE file we just opened
	//
    dsbd.lpwfxFormat = tempWaveInfo.pwfx;

	IDirectSoundBuffer * pBuffer;
	dsRetVal = (snDSoundGlobal::g_pDirectSound)->CreateSoundBuffer( &dsbd, &(pBuffer), NULL );						
    if ( dsRetVal != DS_OK )
    {
		GlobalFree( tempWaveInfo.pwfx );

		//	Error: there was a problem creating the sound buffer
		//
		throw snSoundCreateFailedX( this->GetFilename() );
    }

	wiWave			= tempWaveInfo;
	m_dwNotifySize	= notifySize;
	m_dwBufferSize	= bufferSize;
	m_dwNextWriteOffset		= 0;
	m_dwLastPlayPosInBuffer	= 0;
	m_dwProgress	= 0;

	this->SetBuffer( pBuffer );
	this->SetFoundEnd( false );
	this->SetLooping( true );
    this->SetFinished( false );

	// Fill data in the buffer.
	//
    FillBuffer( m_dwBufferSize );	

	// Clean-up 
	//
	GlobalFree( tempWaveInfo.pwfx );
	tempWaveInfo.pwfx = NULL;
}


//========================================================================
//	FillBuffer()
//========================================================================
void	
snSoundJob2DStreamedPAC::FillBuffer( unsigned int i_BytesToRead )
{
    LPBYTE  lpWrite1, lpWrite2;
    unsigned long	dwWriteLen1, dwWriteLen2;
    UINT	uActualBytesWritten;
    int		nChkErr;
    HRESULT	dsRetVal;
	unsigned long	dwBytes = m_dwBufferSize; 

	// This is the initial read. So we fill the entire buffer.(LPVOID)
	// This will not wrap around so the 2nd pointers will be NULL.
	//
    dsRetVal = this->GetBuffer()->Lock( m_dwNextWriteOffset, i_BytesToRead,
									(void **) &lpWrite1, &dwWriteLen1,
									(void **) &lpWrite2, &dwWriteLen2, 0 );
    if ( dsRetVal != DS_OK )
	{
		DBG_LOG( "couldn't lock buffer with hr = " << dsRetVal );
		return;
	}

	if (dwWriteLen1 < m_dwNotifySize)
	{
		DBG_LOG( "Lock returned number of bytes and requested size differ" );
	}
	
	//	ASSERT(dwWriteLen1);
	//	ASSERT( NULL != lpWrite1 );
	//	ASSERT(m_dwNextWriteOffset < m_dwBufferSize);

    nChkErr = WaveUtil::WaveReadFile( wiWave.hmmio, (UINT)dwWriteLen1, lpWrite1,
                            &wiWave.mmck, &uActualBytesWritten );

	m_dwProgress	+= uActualBytesWritten;

	// if the number of bytes written is less than the 
	// amount we requested, we have a short file.
	//
	if (uActualBytesWritten < dwWriteLen1)
	{
		//	Decrement our loop counter.
		//
		if ( this->GetLoops() >= 1 )
		{
			//m_dwLastPlayPosInBuffer	= 0;

			this->SetLoops( this->GetLoops() - 1 );

			if ( this->GetLoops() == 0 )
			{
				this->SetLooping( false );
			}
		}

		if (!(this->IsLooping()))
		{
			// we set the bFoundEnd flag if the length is less than
			// one notify period long which is when the first notification comes in.
			// The next notification will then call send a message to process a stop. 
			//
			if (uActualBytesWritten < m_dwNotifySize)
			{
				this->SetFoundEnd( true );
			}
			
			// Fill in silence for the rest of the buffer.
			//
			FillMemory(lpWrite1+uActualBytesWritten, dwWriteLen1-uActualBytesWritten, 
						(BYTE)(wiWave.pwfx->wBitsPerSample == 8 ? 128 : 0));
		}
		else
		{
			// we are looping.
			//
			UINT uWritten = uActualBytesWritten;	// from previous call above.
			while (uWritten < dwWriteLen1)
			{	
				// this will keep reading in until the buffer is full. For very short files.
				//
				nChkErr = WaveUtil::WaveStartDataRead( &wiWave.hmmio, &wiWave.mmck, &wiWave.mmckInRIFF );
				//ASSERT(nChkErr == 0);	// we've already this before so shouldn't fail.

				nChkErr = WaveUtil::WaveReadFile(wiWave.hmmio, (UINT)dwWriteLen1-uWritten, 
								lpWrite1 + uWritten, &wiWave.mmck, &uActualBytesWritten);
				//ASSERT(nChkErr == 0);	// we've already this before so shouldn't fail.

				uWritten		+= uActualBytesWritten;
				m_dwProgress	+= uActualBytesWritten;
			}
			
		} // else
	}

	// now unlock the buffer.
	//
	dsRetVal = this->GetBuffer()->Unlock( (LPVOID)lpWrite1, dwWriteLen1,		NULL, 0 );
                                                                             
	m_dwNextWriteOffset += dwWriteLen1;	

	// this is a circular buffer. Do mod buffersize.
	//
	if (m_dwNextWriteOffset >= m_dwBufferSize)
	{
		m_dwNextWriteOffset -= m_dwBufferSize;
	}
}


//========================================================================
//	PlayBuffer()
//
//	Start the buffer playing.
//========================================================================
void	
snSoundJob2DStreamedPAC::PlayBuffer( float i_SimulationTime )
{
	//	Play the sound
	//
	HRESULT			Error;

	this->GetBuffer()->SetCurrentPosition( this->GetRestorePosition() );

	//	set the buffer to play.  Streaming buffers are *always* looping
	//
	while ( Error = this->GetBuffer()->Play( 0, 0, DSBPLAY_LOOPING ) )
	{
		snDSoundGlobal::PrintDSError( Error );

		// if the buffer was lost, recreate and reload it
		//
		if ( Error == DSERR_BUFFERLOST )
		{
			this->GetBuffer()->Release();
			this->SetBuffer( NULL );
			this->CreateBuffer();
		}
		else
		{
			//	some other error occurred, jump out
			//
			return;
		}
	}

	//	initialize the relevent variables
	//
	//this->SetPaused( false );
	//this->SetFinished( false );
	this->SetShouldStart( false );
	this->SetStartTime( i_SimulationTime );
	m_dwLastPlayPosInBuffer	= 0;
	m_EndOfStream			= 0;
	m_BytesWritten			= 0;
	m_StreamUpdateLast		= i_SimulationTime;
	m_LoopCount				= 0;
	m_LoopTarget			= 0;
	m_dwNextWriteOffset		= m_dwFinalWritePosInBuffer;
}


//========================================================================
//	Init()
//
//	Initialize this classes data
//========================================================================
void	
snSoundJob2DStreamedPAC::Init()
{
	m_dwLastPlayPosInBuffer	= 0;
	m_EndOfStream			= 0;
	m_BytesWritten			= 0;
	m_StreamUpdateLast		= 0.0f;
	m_LoopCount				= 0;
	m_LoopTarget			= 0;
	m_StreamBufferInSeconds		= SN_DEFAULT_STREAM_BUFFER_IN_SECONDS;
	m_NumberOfPlayNotifications = NUM_PLAY_NOTIFICATIONS;
	m_StreamUpdateRate			= m_StreamBufferInSeconds / m_NumberOfPlayNotifications;
	m_StreamUpdateBytes			= 0;
	m_dwFinalWritePosInBuffer	= 0;
	m_dwProgress				= 0;

	SetLooping( false );
	SetLocalize( false );
	SetFileChanged( false );
	SetDoneReading( false );
	SetFoundEnd( false );
	m_Flag.bAlmostDone = false;
}

