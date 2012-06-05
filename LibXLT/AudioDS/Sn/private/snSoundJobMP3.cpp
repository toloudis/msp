/****************************************************************************\
**  snSoundJobMP3.cpp
**
**      snSoundJobMP3.hpp defines the snSoundJobMP3 class
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "AudioDS/sn/snSoundJobMP3.hpp"
#include "AudioDS/sn/private/snSoundJobMP3PAC.hpp"


//========================================================================
//	default constructors
//========================================================================
snSoundJobMP3::snSoundJobMP3()
	: snSoundJob()
{
	Init();
}


//========================================================================
//========================================================================
snSoundJobMP3::snSoundJobMP3( const snSoundJobMP3& i_CopyFrom )
	: snSoundJob()
{
	Init();

	*this = i_CopyFrom;
}


//========================================================================
//========================================================================
snSoundJobMP3::~snSoundJobMP3()
{
	Free();

	delete m_pPAC;
}


//========================================================================
//	Load()
//========================================================================
void	
snSoundJobMP3::Load()
{
	m_pPAC->Load();
}


//========================================================================
//	Free()
//========================================================================
void	
snSoundJobMP3::Free()
{
	m_pPAC->Free();
}


//========================================================================
//	Start()
//========================================================================
void	
snSoundJobMP3::Start( float i_SimulationTime )
{
	m_pPAC->Start( i_SimulationTime );
}


//========================================================================
//	Stop()
//========================================================================
void	
snSoundJobMP3::Stop()
{
	m_pPAC->Stop();
}


//========================================================================
//	Think()
//========================================================================
void	
snSoundJobMP3::Think( float i_SimulationTime )
{
	m_pPAC->Think( i_SimulationTime );
	return;
}


//========================================================================
//	Pause()
//========================================================================
void	
snSoundJobMP3::Pause()
{
	m_pPAC->Pause();
}


//========================================================================
//	Resume()
//========================================================================
void	
snSoundJobMP3::Resume( float i_SimulationTime )
{
	m_pPAC->Resume( i_SimulationTime );
}


//========================================================================
//	SetVolume()
//========================================================================
void		
snSoundJobMP3::SetVolume( const float i_NewVolume )
{
	m_pPAC->SetVolume( i_NewVolume );
}


//========================================================================
//	GetVolume()
//========================================================================
float		
snSoundJobMP3::GetVolume() const
{
	return m_pPAC->GetVolume();
}


//========================================================================
//	SetVolumeGlobalFactor()
//========================================================================
void		
snSoundJobMP3::SetVolumeGlobalFactor( const float i_Value )
{
	m_pPAC->SetVolumeGlobalFactor( i_Value );
}


//========================================================================
//	GetVolumeGlobalFactor()
//========================================================================
float		
snSoundJobMP3::GetVolumeGlobalFactor() const
{
	return m_pPAC->GetVolumeGlobalFactor();
}


//========================================================================
//	SetVolumeTypeFactor()
//========================================================================
void		
snSoundJobMP3::SetVolumeTypeFactor( const float i_Value )
{
	m_pPAC->SetVolumeTypeFactor( i_Value );
}


//========================================================================
//	GetVolumeTypeFactor()
//========================================================================
float		
snSoundJobMP3::GetVolumeTypeFactor() const
{
	return m_pPAC->GetVolumeTypeFactor();
}


//========================================================================
//	GetVolumeFinal()
//========================================================================
float		
snSoundJobMP3::GetVolumeFinal() const
{
	return m_pPAC->GetVolumeFinal();
}


//========================================================================
//	SetFadeFactor()
//========================================================================
void		
snSoundJobMP3::SetFadeFactor( float i_Value )
{
	m_pPAC->SetFadeFactor( i_Value );
}


//========================================================================
//	GetFadeFactor()
//========================================================================
float		
snSoundJobMP3::GetFadeFactor() const
{
	return m_pPAC->GetFadeFactor();
}


//========================================================================
//	SetFadeTo()
//
//		Fade the sound to a specific level.  This level is a percentage
//	from 0.0 to 1.0.
//========================================================================
void 
snSoundJobMP3::SetFadeTo( float i_TargetLevel, float i_Seconds, float i_SimulationTime )
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
snSoundJobMP3::AddToFade( float i_TargetLevel, float i_Seconds, float i_SimulationTime )
{
	m_pPAC->AddToFade( i_TargetLevel, i_Seconds, i_SimulationTime );
}


//========================================================================
//	SetLoops()
//========================================================================
void		
snSoundJobMP3::SetLoops( const int i_Value )
{
	m_pPAC->SetLoops( i_Value );
}


//========================================================================
//	GetLoops()
//========================================================================
int			
snSoundJobMP3::GetLoops() const
{
	return m_pPAC->GetLoops();
}


//========================================================================
//	SetFilename
//========================================================================
void
snSoundJobMP3::SetFilename( const fsLocator & Filename )
{
	m_pPAC->SetFilename( Filename );

	//	If there is no name for this job, give it the file name
	//
	//if ( this->GetName().GetLength() == 0 )
	//{
	//	SetName( Filename.GetLastName() );
	//}
}


//========================================================================
//	GetFilename
//========================================================================
const fsLocator &	
snSoundJobMP3::GetFilename() const
{
	return m_pPAC->GetFilename();
}


//========================================================================
//========================================================================
void	
snSoundJobMP3::SetLooping( const bool Value )
{ 
	m_pPAC->SetLooping( Value );
}


//========================================================================
//	SetDeleteWhenFinished()
//========================================================================
void		
snSoundJobMP3::SetDeleteWhenFinished( const bool i_bValue )
{
	m_pPAC->SetDeleteWhenFinished( i_bValue );
}


//========================================================================
//	IsDeleteWhenFinished()
//========================================================================
bool		
snSoundJobMP3::IsDeleteWhenFinished() const
{
	return m_pPAC->IsDeleteWhenFinished();
}


//========================================================================
//	SetFinished()
//========================================================================
void		
snSoundJobMP3::SetFinished( const bool i_bValue )
{
	m_pPAC->SetFinished( i_bValue );
}


//========================================================================
//	IsFinished()
//========================================================================
bool		
snSoundJobMP3::IsFinished() const
{
	return m_pPAC->IsFinished();
}


//========================================================================
//	InitData()
//========================================================================
void	
snSoundJobMP3::InitData()
{
}


//========================================================================
//	Init()
//========================================================================
void	
snSoundJobMP3::Init()
{
	m_pPAC	= new snSoundJobMP3PAC;
	m_pPAC->SetParentSoundJob(this);

	InitData();
}
