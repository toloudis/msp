/****************************************************************************\
**  snSoundJobRedbookPACWin.cpp
**
**      snSoundJobRedbookPACWin.cpp implements the DirectSound portion of the
**	snSoundJobRedbookPAC.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "AudioDS/sn/private/snSoundJobRedbookPACWin.hpp"


//========================================================================
//	default and copy constructors
//========================================================================
snSoundJobRedbookPAC::snSoundJobRedbookPAC()
{
	Init();
}


//========================================================================
//========================================================================
snSoundJobRedbookPAC::snSoundJobRedbookPAC( const snSoundJobRedbookPAC& i_CopyFrom )
{
	*this = i_CopyFrom;
}


//========================================================================
//========================================================================
snSoundJobRedbookPAC::~snSoundJobRedbookPAC()
{
	if ( m_MCIOpen.wDeviceID )
	{
		m_MCIClose.dwCallback = 0; 
		mciSendCommand( m_MCIOpen.wDeviceID, MCI_CLOSE, MCI_OPEN_TYPE,(DWORD_PTR) &m_MCIClose);
	}

	Free();
}


//========================================================================
//	Load()
//========================================================================
void	
snSoundJobRedbookPAC::Load()
{
	if ( !IsLoaded() )
	{
		m_MCIOpen.dwCallback = 0; 
		m_MCIOpen.lpstrDeviceType = TEXT( "cdaudio" );
		mciSendCommand(NULL, MCI_OPEN, MCI_OPEN_TYPE,(DWORD_PTR) &m_MCIOpen);

		m_MCIStatus.dwItem = MCI_STATUS_NUMBER_OF_TRACKS;
		if (mciSendCommand(m_MCIOpen.wDeviceID, MCI_STATUS, MCI_STATUS_ITEM|MCI_WAIT, (DWORD_PTR)&m_MCIStatus))
		{
			mciSendCommand(m_MCIOpen.wDeviceID, MCI_CLOSE, NULL, NULL);
			return;
		}
		m_NumberOfTracks = (short)m_MCIStatus.dwReturn;

		SetLoaded( true );
	}
}


//========================================================================
//	Free()
//========================================================================
void	
snSoundJobRedbookPAC::Free()
{
	if ( IsLoaded() )
	{
		SetLoaded( false );
	}
}


//========================================================================
//	Start()
//========================================================================
void	
snSoundJobRedbookPAC::Start()
{
	Start( m_Track );
}


//========================================================================
//	Start()
//========================================================================
void	
snSoundJobRedbookPAC::Start( const int i_TrackNum )
{
	if ( IsLoaded() )
	{
		//	m_MCICuepoint.ulCuepoint = m_record.record[Track].ulEndAddr;
		//	mciSendCommand( m_MCIOpen.wDeviceID, MCI_SET_CUEPOINT, MCI_WAIT | MCI_SET_CUEPOINT_ON, &m_MCICuepoint, 0 );

		//m_MCISeek.dwCallback = 0;
		//m_MCISeek.dwTo = m_Track;
		//mciSendCommand( m_MCIOpen.wDeviceID, MCI_SEEK, NULL, (DWORD)&m_MCISeek);

		m_MCIPlay.dwCallback	= 0;
		m_MCIPlay.dwFrom		= i_TrackNum;
		//m_MCIPlay.dwTo		= i_TrackNum;
		mciSendCommand( m_MCIOpen.wDeviceID, MCI_PLAY, MCI_NOTIFY | MCI_FROM, (DWORD_PTR)&m_MCIPlay);
	}
}


//========================================================================
//	Stop()
//========================================================================
void	
snSoundJobRedbookPAC::Stop()
{
	if ( IsLoaded() )
	{
		m_MCIStop.dwCallback = 0;
		mciSendCommand( m_MCIOpen.wDeviceID, MCI_STOP, NULL, (DWORD_PTR)&m_MCIStop);
	}
}


//========================================================================
//	Think()
//========================================================================
void	
snSoundJobRedbookPAC::Think( float i_SimulationTime )
{
	if ( !IsLoaded() )
	{
		return;
	}

	//
}


//========================================================================
//	Pause()
//========================================================================
void	
snSoundJobRedbookPAC::Pause()
{
	if ( IsLoaded() )
	{
		m_MCIStop.dwCallback = 0;
		mciSendCommand( m_MCIOpen.wDeviceID, MCI_PAUSE, NULL, (DWORD_PTR)&m_MCIStop);
		this->SetPaused( true );
	}
}


//========================================================================
//	Resume()
//========================================================================
void	
snSoundJobRedbookPAC::Resume()
{
	if ( IsLoaded() )
	{
		m_MCIStop.dwCallback = 0;
		mciSendCommand( m_MCIOpen.wDeviceID, MCI_RESUME, NULL, (DWORD_PTR)&m_MCIStop);
		this->SetPaused( false );
	}
}


//========================================================================
//	GetTrack()
//========================================================================
int 
snSoundJobRedbookPAC::GetTrack()
{
	if ( IsLoaded() )
	{
		return m_Track;
	}

	return 0;
}


//========================================================================
//	SetTrack()
//========================================================================
void 
snSoundJobRedbookPAC::SetTrack( const int i_TrackNum )
{
	if ( IsLoaded() )
	{
		m_Track	= i_TrackNum;
	}
}


//========================================================================
//	GetNumberOfTracks()
//========================================================================
int 
snSoundJobRedbookPAC::GetNumberOfTracks()
{
	if ( IsLoaded() )
	{
		return 0;
	}

	return 0;
}


//========================================================================
//	NextTrack()
//========================================================================
void 
snSoundJobRedbookPAC::NextTrack()
{
	if ( IsLoaded() )
	{
		//if ( m_Track < AIL_redbook_tracks( m_RedbookHandle ) )
		//{
			m_Track++;
		//}
		//else
		//{
		//	m_Track = 1;
		//}

		Start( m_Track );
	}
}


//========================================================================
//	PreviousTrack()
//========================================================================
void 
snSoundJobRedbookPAC::PreviousTrack()
{
	if ( IsLoaded() )
	{
		if ( m_Track > 1 )
		{
			m_Track--;
		}
		else
		{
			//
		}

		Start( m_Track );
	}
}


//========================================================================
//	Init()
//
//	Initialize this classes data
//========================================================================
void	
snSoundJobRedbookPAC::Init()
{
	SetLoaded( false );
	SetPaused( false );
	SetLooping( false );

	m_Track				= 1;
	m_NumberOfTracks	= 1;

	memset ( &m_MCIOpen, 0, sizeof ( MCI_OPEN_PARMS ) );
	memset ( &m_MCIStop, 0, sizeof ( MCI_GENERIC_PARMS ) );
	memset ( &m_MCIPlay, 0 ,sizeof ( MCI_PLAY_PARMS ) );
	memset ( &m_MCIClose,0 ,sizeof ( MCI_GENERIC_PARMS ) );
	memset ( &m_MCISeek ,0 ,sizeof ( MCI_SEEK_PARMS ) );
	memset ( &m_MCIStatus ,0 ,sizeof ( MCI_STATUS_PARMS ) );
}

