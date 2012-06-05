/****************************************************************************\
**  snSoundJob2D.cpp
**
**      snSoundJob2D.hpp defines the snSoundJob2D class
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "AudioDS/sn/snSoundJob2D.hpp"
#include "AudioDS/sn/private/snSoundJob2DPAC.hpp"
	

//========================================================================
//	constructors
//========================================================================
snSoundJob2D::snSoundJob2D(snSoundJob2DPAC * i_pSoundData)
:	snSoundJob(),
	m_pPAC( i_pSoundData )
{
	Init();
}


//========================================================================
//	default constructors
//========================================================================
snSoundJob2D::snSoundJob2D()
: snSoundJob(),
	m_pPAC( NULL )
{
	Init();
}


//========================================================================
//========================================================================
snSoundJob2D::snSoundJob2D( const snSoundJob2D& i_CopyFrom )
: snSoundJob(),
	m_pPAC( NULL )
{
	Init();

	*this = i_CopyFrom;
}


//========================================================================
//========================================================================
snSoundJob2D::~snSoundJob2D()
{
	delete m_pPAC;
}


//========================================================================
//	Load()
//========================================================================
void	
snSoundJob2D::Load()
{
	// Load the data
	//
	m_pPAC->Load();
}


//========================================================================
//	Free()
//========================================================================
void	
snSoundJob2D::Free()
{
	m_pPAC->Free();
}


//========================================================================
//	Reload()
//========================================================================
void	
snSoundJob2D::Reload()
{
	// Reload the data
	//
	m_pPAC->Reload();
}


//========================================================================
//	Unload()
//========================================================================
void	
snSoundJob2D::Unload(bool i_bReloadOnStart /*= false*/)
{
	// Unload the data
	//
	m_pPAC->Unload(i_bReloadOnStart);
}


//========================================================================
//	Start()
//========================================================================
void	
snSoundJob2D::Start( float i_SimulationTime )
{
	m_pPAC->Start( i_SimulationTime );
}


//========================================================================
//	Restart()
//========================================================================
void	
snSoundJob2D::Restart( float i_SimulationTime )
{
	m_pPAC->Restart( i_SimulationTime );
}


//========================================================================
//	Stop()
//========================================================================
void	
snSoundJob2D::Stop()
{
	m_pPAC->Stop();
}


//========================================================================
//	Think()
//========================================================================
void	
snSoundJob2D::Think( float i_SimulationTime )
{
	m_pPAC->Think( i_SimulationTime );
}


//========================================================================
//	Pause()
//========================================================================
void	
snSoundJob2D::Pause()
{
	m_pPAC->Pause();
}


//========================================================================
//	Resume()
//========================================================================
void	
snSoundJob2D::Resume( float i_SimulationTime )
{
	m_pPAC->Resume( i_SimulationTime );
}


//========================================================================
//	SetFadeTo()
//
//		Fade the sound to a specific level.  This level is a percentage
//	from 0.0 to 1.0.
//========================================================================
void 
snSoundJob2D::SetFadeTo( float i_TargetLevel, float i_Seconds, float i_SimulationTime )
{
	m_pPAC->SetFadeTo( i_TargetLevel, i_Seconds, i_SimulationTime );
}


//========================================================================
//	AddToFade()
//
//		Add a value to the Fade.  This value cannot exceed the Fade
//	percentage from 0.0 to 1.0.
//========================================================================
void 
snSoundJob2D::AddToFade( float i_TargetLevel, float i_Seconds, float i_SimulationTime )
{
	m_pPAC->AddToFade( i_TargetLevel, i_Seconds, i_SimulationTime );
}


//========================================================================
//	SetPanTo()
//
//		Pan the sound to a specific channel over time.  This channel is a
//	percentage from -1.0 (all left) to 0.0 (center) to 1.0 (all right).
//========================================================================
void 
snSoundJob2D::SetPanTo( float i_TargetLevel, float i_Seconds, float i_SimulationTime  )
{
	m_pPAC->SetPanTo( i_TargetLevel, i_Seconds, i_SimulationTime );
}


//========================================================================
//	AddToPan()
//
//		Add a value to the Pan.  This value cannot exceed the pan
//	percentage from -1.0 to 1.0.
//========================================================================
void 
snSoundJob2D::AddToPan( float i_TargetLevel, float i_Seconds, float i_SimulationTime  )
{
	m_pPAC->AddToPan( i_TargetLevel, i_Seconds, i_SimulationTime );
}


//========================================================================
//	SetFrequencyTo()
//
//	The sound system provides a way to manipulate frequencies based on a
//	percentage rather than dealing with the frequency values themselves.
//	Given a percent, where 1.0 is 100% (normal playback), 0.5 is 50% 
//	(half speed), and 2.0 is 200% (double speed) we can calculate the 
//	frequency.  the valid percent is 0.0 to 10.0.
//========================================================================
void 
snSoundJob2D::SetFrequencyTo( float i_TargetLevel, float i_Seconds, float i_SimulationTime  )
{
	m_pPAC->SetFrequencyTo( i_TargetLevel, i_Seconds, i_SimulationTime );
}


//========================================================================
//	AddToFrequency()
//
//		Add a value to the frequency factor.  This level is a factor 
//	from -10.0 to 10.0.
//========================================================================
void 
snSoundJob2D::AddToFrequency( float i_TargetLevel, float i_Seconds, float i_SimulationTime  )
{
	m_pPAC->AddToFrequency( i_TargetLevel, i_Seconds, i_SimulationTime );
}


//========================================================================
//	IsPlaying()
//
//	return true if the sound job is currently playing.
//========================================================================
bool	
snSoundJob2D::IsPlaying() const
{
	return m_pPAC->IsPlaying();
}


//========================================================================
//========================================================================
snSoundJob2D&
snSoundJob2D::operator = ( const snSoundJob2D& i_SoundJob )
{
	if ( (m_pPAC) != NULL )
	{
		InitData();
	}
	
	*m_pPAC = *(i_SoundJob.m_pPAC);

	SetTypeMask(i_SoundJob.GetTypeMask());

	return(*this);
}


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
snSoundJob2DPAC *	
snSoundJob2D::GetPAC() const
{
	return m_pPAC;
}


//========================================================================
//	SetPAC()
//
//	SetPAC is for Terawatt PAC components which needs to set the PAC.
//========================================================================
void 
snSoundJob2D::SetPAC( snSoundJob2DPAC * pData )
{
	m_pPAC	= pData;
}


//========================================================================
//	GetDataSize()
//
//	Returns the size of sound data.
//========================================================================
int
snSoundJob2D::GetDataSize() const
{
	return m_pPAC->GetDataSize();
}

//========================================================================
//	SetFilename
//========================================================================
void
snSoundJob2D::SetFilename( const fsLocator & Filename )
{
	m_pPAC->SetFilename( Filename );

	//	If there is no name for this job, give it the file name
	//
	if ( this->GetName().GetLength() == 0 )
	{
		if ( Filename.GetNumNames() != 0 )
		{
			SetName( Filename.GetLastName() );
		}
	}
}


//========================================================================
//	GetFilename
//========================================================================
const fsLocator &	
snSoundJob2D::GetFilename() const
{
	return m_pPAC->GetFilename();
}


//========================================================================
//	SetVolume()
//========================================================================
void		
snSoundJob2D::SetVolume( const float i_NewVolume )
{
	m_pPAC->SetVolume( i_NewVolume );
}


//========================================================================
//	GetVolume()
//========================================================================
float		
snSoundJob2D::GetVolume() const
{
	return m_pPAC->GetVolume();
}


//========================================================================
//	SetVolumeGlobalFactor()
//========================================================================
void		
snSoundJob2D::SetVolumeGlobalFactor( const float i_Value )
{
	m_pPAC->SetVolumeGlobalFactor( i_Value );
}


//========================================================================
//	GetVolumeGlobalFactor()
//========================================================================
float		
snSoundJob2D::GetVolumeGlobalFactor() const
{
	return m_pPAC->GetVolumeGlobalFactor();
}


//========================================================================
//	SetVolumeTypeFactor()
//========================================================================
void		
snSoundJob2D::SetVolumeTypeFactor( const float i_Value )
{
	m_pPAC->SetVolumeTypeFactor( i_Value );
}


//========================================================================
//	GetVolumeTypeFactor()
//========================================================================
float		
snSoundJob2D::GetVolumeTypeFactor() const
{
	return m_pPAC->GetVolumeTypeFactor();
}


//========================================================================
//	GetVolumeFinal()
//========================================================================
float		
snSoundJob2D::GetVolumeFinal() const
{
	return m_pPAC->GetVolumeFinal();
}


//========================================================================
//	SetFrequency()
//========================================================================
void		
snSoundJob2D::SetFrequency( float i_Value )
{
	m_pPAC->SetFrequency( i_Value );
}


//========================================================================
//	GetFrequency()
//========================================================================
float		
snSoundJob2D::GetFrequency() const
{
	return m_pPAC->GetFrequency();
}


//========================================================================
//	GetFrequencyFactor()
//========================================================================
float		
snSoundJob2D::GetFrequencyFactor() const
{
	return m_pPAC->GetFrequencyFactor();
}


//========================================================================
//	SetPan()
//========================================================================
void		
snSoundJob2D::SetPan( float i_Value )
{
	m_pPAC->SetPan( i_Value );
}


//========================================================================
//	GetPan()
//========================================================================
float		
snSoundJob2D::GetPan() const
{
	return m_pPAC->GetPan();
}


//========================================================================
//	GetPanPosition()
//========================================================================
float		
snSoundJob2D::GetPanPosition() const
{
	return m_pPAC->GetPanPosition();
}


//========================================================================
//	SetFadeFactor()
//========================================================================
void		
snSoundJob2D::SetFadeFactor( float i_Value )
{
	m_pPAC->SetFadeFactor( i_Value );
}


//========================================================================
//	GetFadeFactor()
//========================================================================
float		
snSoundJob2D::GetFadeFactor() const
{
	return m_pPAC->GetFadeFactor();
}


//========================================================================
//	SetLoops()
//========================================================================
void		
snSoundJob2D::SetLoops( const int i_Value )
{
	m_pPAC->SetLoops( i_Value );
}


//========================================================================
//	GetLoops()
//========================================================================
int			
snSoundJob2D::GetLoops() const
{
	return m_pPAC->GetLoops();
}


//========================================================================
//	SetLoopCurrent()
//========================================================================
void		
snSoundJob2D::SetLoopCurrent( const int i_Value )
{
	m_pPAC->SetLoopCurrent( i_Value );
}


//========================================================================
//	GetLoopCurrent()
//========================================================================
int			
snSoundJob2D::GetLoopCurrent() const
{
	return m_pPAC->GetLoopCurrent();
}


//========================================================================
//	SetPosition()
//========================================================================
void		
snSoundJob2D::SetPosition( const maVector3d& i_Value )
{
	m_pPAC->SetPosition( i_Value );
}


//========================================================================
//	GetPosition()
//========================================================================
maVector3d			
snSoundJob2D::GetPosition() const
{
	return m_pPAC->GetPosition();
}


//========================================================================
//	SetListenerPosition()
//
//	physical location of listener in world (used by localize)
//========================================================================
void
snSoundJob2D::SetListenerPosition( const maVector3d& i_Position )
{
	m_pPAC->SetListenerPosition( i_Position );
}


//========================================================================
//	GetListenerPosition()
//
//	physical location of listener in world (used by localize)
//========================================================================
maVector3d		
snSoundJob2D::GetListenerPosition() const
{
	return m_pPAC->GetListenerPosition();
}


//========================================================================
//	SetListenerOrientation()
//
//	physical orientation of listener in world (used by localize)
//========================================================================
void
snSoundJob2D::SetListenerOrientation( const maRotation& i_Value )
{
	m_pPAC->SetListenerOrientation( i_Value );
}


//========================================================================
//	GetListenerOrientation()
//
//	physical location of listener in world (used by localize)
//========================================================================
maRotation
snSoundJob2D::GetListenerOrientation() const
{
	return m_pPAC->GetListenerOrientation();
}


//========================================================================
//	SetSoundDistance()
//
//	Distance that the sound falls off at.
//========================================================================
void
snSoundJob2D::SetSoundDistance( float i_Distance )
{
	m_pPAC->SetSoundDistance( i_Distance );
}


//========================================================================
//	GetSoundDistance()
//
//	Distance that the sound falls off at.
//========================================================================
float
snSoundJob2D::GetSoundDistance() const
{
	return m_pPAC->GetSoundDistance();
}


//========================================================================
//	SetFalloffDistance()
//
//	Distance that the sound falls off at.  The sound starts falling off
//	after it goes past the SoundDistance.
//========================================================================
void
snSoundJob2D::SetFalloffDistance( float i_Distance )
{
	m_pPAC->SetFalloffDistance( i_Distance );
}


//========================================================================
//	GetFalloffDistance()
//
//	Distance that the sound falls off at.  The sound starts falling off
//	after it goes past the SoundDistance.
//========================================================================
float
snSoundJob2D::GetFalloffDistance() const
{
	return m_pPAC->GetFalloffDistance();
}


//========================================================================
//	SetPositionInSoundByPercent()
//
//	i_Percent - the percent of the sound to jump to
//
//	return the current percentage
//========================================================================
float snSoundJob2D::SetPositionInSoundByPercent( float i_fPercent )
{
	return m_pPAC->SetPositionInSoundByPercent( i_fPercent );
}

//========================================================================
//	GetPositionInSoundByPercent()
//
//	Get the current percent of the playing sound.
//========================================================================
float snSoundJob2D::GetPositionInSoundByPercent()
{
	return m_pPAC->GetPositionInSoundByPercent();
}

//========================================================================
//	GetTimeLength()
//
//	Get the length of the sound in seconds.
//========================================================================
float snSoundJob2D::GetTimeLength()
{
	return m_pPAC->GetTimeLength();
}

//========================================================================
//	SetDeleteWhenFinished()
//========================================================================
void		
snSoundJob2D::SetDeleteWhenFinished( const bool i_bValue )
{
	m_pPAC->SetDeleteWhenFinished( i_bValue );
}


//========================================================================
//	IsDeleteWhenFinished()
//========================================================================
bool		
snSoundJob2D::IsDeleteWhenFinished() const
{
	return m_pPAC->IsDeleteWhenFinished();
}


//========================================================================
//	SetFinished()
//========================================================================
void		
snSoundJob2D::SetFinished( const bool i_bValue )
{
	m_pPAC->SetFinished( i_bValue );
}


//========================================================================
//	IsFinished()
//========================================================================
bool		
snSoundJob2D::IsFinished() const
{
	return m_pPAC->IsFinished();
}


//========================================================================
//	SetLocalize()
//========================================================================
void		
snSoundJob2D::SetLocalize( const bool i_bValue )
{
	m_pPAC->SetLocalize( i_bValue );
}


//========================================================================
//	IsLocalize()
//========================================================================
bool		
snSoundJob2D::IsLocalize() const
{
	return m_pPAC->IsLocalize();
}

//========================================================================
// Loacalize2D
//========================================================================
void
snSoundJob2D::SetLocalize2D( const bool i_bValue )
{
	m_pPAC->SetLocalize( i_bValue );
}
bool
snSoundJob2D::IsLocalize2D() const
{
	return m_pPAC->IsLocalize2D();
}

//========================================================================
//	StopOnFadeFinish
//========================================================================
bool	
snSoundJob2D::IsStopOnFadeFinish() const
{
	return m_pPAC->IsStopOnFadeFinish();
}

void	
snSoundJob2D::SetStopOnFadeFinish( const bool i_bValue )
{
	m_pPAC->SetStopOnFadeFinish( i_bValue );
}

//========================================================================
//	IsPaused()
//========================================================================
bool
snSoundJob2D::IsPaused() const
{
	return m_pPAC->IsPaused();
}


//========================================================================
//	InitData()
//========================================================================
void	
snSoundJob2D::InitData()
{
	if ( m_pPAC == NULL )
	{
		m_pPAC	= new snSoundJob2DPAC;
	}

	m_pPAC->SetParentSoundJob(this);
}


//========================================================================
//	Init()
//========================================================================
void	
snSoundJob2D::Init()
{
	InitData();
}
