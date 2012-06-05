/****************************************************************************\
**  snDSoundGlobalWin.hpp
**
**      snDSoundGlobalWin.hpp contains some DirectSound stuff that many components
**	in the Windows DirectSound PAC might need.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "AudioDS/sn/private/snDSoundGlobalWin.hpp"

#include "AudioDS/sn/snExceptionX.hpp"
#include "Core/app/private/appApplicationPACWin.hpp"

#include <sstream>


//============================================================================
//============================================================================
namespace snDSoundGlobal
{

//---------------------------------------------------------
//	The main DirectSound pointer
//---------------------------------------------------------
IDirectSound*			g_pDirectSound = NULL;		// direct sound

//---------------------------------------------------------
// The primary buffer all sound gets mixed into
//---------------------------------------------------------
IDirectSoundBuffer*		g_pPrimaryBuffer = NULL;	


//---------------------------------------------------------
//	Initialize DirectSound and create the primary buffer.
//---------------------------------------------------------
void 
Initialize()
{
	//	We are ready to initialize the DirectSound System
	//
	HRESULT			Error;
	DSBUFFERDESC	BufferDesc;
	WAVEFORMATEX	Format;

	//	If DirectSound already has been set-up don't do it again!
	//
	if ( !g_pDirectSound )
	{
		//
		//	Create DirectSound
		//
		Error = DirectSoundCreate( NULL, &g_pDirectSound, NULL );
		if ( Error )
		{
			throw snSoundSystemCreateFailedX();
		}

		//if ( Error = g_pDirectSound->SetCooperativeLevel( appApplicationPAC::GetHWND(), DSSCL_EXCLUSIVE ) )
		if ( Error = g_pDirectSound->SetCooperativeLevel( appApplicationPAC::GetHWND(), DSSCL_PRIORITY ) )
		//if ( Error = g_pDirectSound->SetCooperativeLevel( appApplicationPAC::GetHWND(), DSSCL_NORMAL ) )
		//if ( Error = g_pDirectSound->SetCooperativeLevel( NULL, DSSCL_PRIORITY ) )
		{
			if (Error == DSERR_ALLOCATED)
				DBG_ERROR("SetCooperativeLevel Error - Allocated");
			else if (Error == DSERR_INVALIDPARAM)
				DBG_ERROR("SetCooperativeLevel Error - Invalid Param");
			else if (Error == DSERR_UNINITIALIZED)
				DBG_ERROR("SetCooperativeLevel Error - Unitialized");
			else if (Error == DSERR_UNSUPPORTED)
				DBG_ERROR("SetCooperativeLevel Error - Unsupported");
			else
				DBG_ERROR("SetCooperativeLevel Error - Unknown");

			Deinitialize();
			throw snSoundSystemCreateFailedX();
		}
	}

	if ( !g_pPrimaryBuffer )
	{
		//
		//	Create the PRIMARY Sound Buffer
		//
		memset( &BufferDesc, 0, sizeof( BufferDesc ) );
		BufferDesc.dwSize		= sizeof( BufferDesc );
		BufferDesc.lpwfxFormat	= NULL;
		BufferDesc.dwFlags		= DSBCAPS_PRIMARYBUFFER;

		Error = g_pDirectSound->CreateSoundBuffer( &BufferDesc, &g_pPrimaryBuffer, NULL );
		if ( Error )
		{
			PrintDSError( Error );
			throw snSoundSystemBufferCreateFailedX();
		}

		DBG_ASSERT( g_pPrimaryBuffer, "Primary Sound Buffer is NULL" );

		//	format and start the PRIMARY Sound Buffer
		//
		WORD primary_channels = 2;
		DWORD primary_freq = 22050;
		WORD primary_bitrate = 16;
		memset( &Format, 0, sizeof( WAVEFORMATEX ) );
		Format.wFormatTag			= (WORD) WAVE_FORMAT_PCM;
		Format.nChannels			= primary_channels;
		Format.nSamplesPerSec		= primary_freq;
		Format.nBlockAlign			= sizeof( WORD );
		Format.wBitsPerSample		= primary_bitrate;
		Format.nBlockAlign			= (WORD) (Format.wBitsPerSample / 8 * Format.nChannels);
		Format.nAvgBytesPerSec		= (DWORD) (Format.nSamplesPerSec * Format.nBlockAlign);

		Error = g_pPrimaryBuffer->SetFormat( &Format );
		if ( Error )
		{
			PrintDSError( Error );
			return;
		}

		// FIX [rjk] setting the primary buffer playing -- needed?
		Error = g_pPrimaryBuffer->Play( 0, 0, DSBPLAY_LOOPING );
		if ( Error )
		{
			PrintDSError( Error );
			return;
		}
	}
}


//---------------------------------------------------------
//	Release DirectSound
//---------------------------------------------------------
void 
Deinitialize()
{
	g_pDirectSound->Release();
	g_pDirectSound		= NULL;

	// We don't need to release this buffer
	// as all buffers are automatically released
	// when the directsound interface is released
	//
	g_pPrimaryBuffer	= NULL;
}


//------------------------------------------------------
//	GetDirectSound()
//------------------------------------------------------
const IDirectSound* 
GetDirectSound()
{
	return g_pDirectSound;
}


//------------------------------------------------------
//	GetPrimaryBuffer()
//------------------------------------------------------
const IDirectSoundBuffer* 
GetPrimaryBuffer()
{
	return g_pPrimaryBuffer;
}


//------------------------------------------------------
//	CreateSoundBuffer()
//------------------------------------------------------
void	
CreateSoundBuffer( DSBUFFERDESC * BDesc, IDirectSoundBuffer ** pBuffer )
{
	HRESULT Error;
	if ( ( Error = 
			snDSoundGlobal::g_pDirectSound->CreateSoundBuffer( BDesc, pBuffer, NULL ) != DS_OK ) )
	{
		throw snSoundSystemBufferCreateFailedX();
	}
}


//---------------------------------------------------------
//	PrintDSError dumps an error message to the debug log
//---------------------------------------------------------
void 
PrintDSError( HRESULT hErr )
{       
    const char *dserr;
	std::ostringstream oss;
	switch (hErr)
    {
	case DSERR_ALLOCATED : oss << "DSERR_ALLOCATED"; dserr = oss.str().c_str();break;
	case DSERR_CONTROLUNAVAIL : oss <<  "DSERR_CONTROLUNAVAIL"; dserr = oss.str().c_str(); break;
	case DSERR_INVALIDPARAM : oss <<"DSERR_INVALIDPARAM";dserr = oss.str().c_str(); break;
	case DSERR_INVALIDCALL : oss << "DSERR_INVALIDCALL";dserr = oss.str().c_str(); break;
	case DSERR_GENERIC : oss << "DSERR_GENERIC";dserr = oss.str().c_str(); break;
	case DSERR_PRIOLEVELNEEDED : oss << "DSERR_PRIOLEVELNEEDED";dserr = oss.str().c_str(); break;
	case DSERR_OUTOFMEMORY : oss << "DSERR_OUTOFMEMORY";dserr = oss.str().c_str(); break;
	case DSERR_BADFORMAT : oss <<"DSERR_BADFORMAT";dserr = oss.str().c_str(); break;
	case DSERR_UNSUPPORTED : oss << "DSERR_UNSUPPORTED";dserr = oss.str().c_str(); break;
		case DSERR_NODRIVER : oss << "DSERR_NODRIVER"; dserr = oss.str().c_str();break;
		case DSERR_ALREADYINITIALIZED : oss <<"DSERR_ALREADYINITIALIZED";dserr = oss.str().c_str(); break;
		case DSERR_NOAGGREGATION : oss << "DSERR_NOAGGREGATION";dserr = oss.str().c_str(); break;
		case DSERR_BUFFERLOST : oss << "DSERR_BUFFERLOST";dserr = oss.str().c_str(); break;
		case DSERR_OTHERAPPHASPRIO : oss <<"DSERR_OTHERAPPHASPRIO";dserr = oss.str().c_str(); break;
		case DSERR_UNINITIALIZED : oss <<"DSERR_UNINITIALIZED";dserr = oss.str().c_str(); break;
		case DSERR_NOINTERFACE : oss <<"DSERR_NOINTERFACE";dserr = oss.str().c_str(); break;
		case DSERR_ACCESSDENIED : oss <<"DSERR_ACCESSDENIED";dserr = oss.str().c_str(); break;

		default: oss <<"Unknown Error";dserr = oss.str().c_str(); break;
	}

	DBG_LOG( "DirectSound Error: " << dserr );
}

}
