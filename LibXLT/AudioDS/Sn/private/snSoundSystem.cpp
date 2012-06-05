/****************************************************************************\
**  snSoundSystem.cpp
**
**      snSoundSystem.hpp defines the snSoundSystem class
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "AudioDS/sn/snSoundSystem.hpp"
#include "AudioDS/sn/private/snSoundSystemPAC.hpp"



//============================================================================
//============================================================================
namespace snSoundSystem
{

//========================================================================
//	Initialized
//========================================================================
bool	IsInitialized()							{ return snSoundSystemPAC::IsInitialized(); };
void	SetInitialized( const bool Value )		{ snSoundSystemPAC::SetInitialized( Value ); };

//========================================================================
//	Initialize()
//========================================================================
void 
Initialize()
{
	snSoundSystemPAC::Initialize();
}


//========================================================================
//	DeInitialize()
//========================================================================
void 
DeInitialize()
{
	snSoundSystemPAC::DeInitialize();
}


//========================================================================
//	Init()
//
//	Initialize this classes data
//========================================================================
void	
Init()
{
	snSoundSystemPAC::Init();
}


//========================================================================
//	CleanUp()
//========================================================================
void 
CleanUp() throw ()
{
	snSoundSystemPAC::CleanUp();
}

}	// namespace

