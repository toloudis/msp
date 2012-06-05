/****************************************************************************\
**  snSoundSystemPACDSound.cpp
**
**      snSoundSystemPACDSound.cpp implements the DirectShow portion of the
**	snSoundSystemPAC.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "AudioDS/sn/private/snSoundSystemPACDSound.hpp"

#include "AudioDS/sn/private/snDSoundGlobalWin.hpp"
#include "Core/dbg/dbgMsg.hpp"

#include <dsound.h>


//============================================================================
//============================================================================
namespace
{
/* Macro to release an object. */
#define RELEASE(x) if (x != NULL) {x->Release(); x = NULL;} 
}


//============================================================================
//============================================================================
namespace snSoundSystemPAC
{

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

	//------------------------------------------------------------------------
	//	Flags
	//------------------------------------------------------------------------
	struct
	{
		bool	bInitialized	:1;
	}
	m_Flag;


//------------------------------------------------------------------------
//	Initialized
//------------------------------------------------------------------------
bool	IsInitialized()							{ return m_Flag.bInitialized; }
void	SetInitialized( const bool Value )		{ m_Flag.bInitialized = Value; }


//------------------------------------------------------------------------
//	Initialize()
//------------------------------------------------------------------------
void 
Initialize()
{
	if ( !IsInitialized() )
	{
		snDSoundGlobal::Initialize();

		SetInitialized( true );

		//DBG_LOG( "Sound System Initialized" );
	}
}


//------------------------------------------------------------------------
//	DeInitialize()
//------------------------------------------------------------------------
void 
DeInitialize()
{
	if ( IsInitialized() )
	{
		snDSoundGlobal::Deinitialize();

		SetInitialized( false );

		//DBG_LOG( "Sound System DeInitialized" );
	}
}


//------------------------------------------------------------------------
//	Init()
//
//	Initialize this classes data
//------------------------------------------------------------------------
void	
Init()
{
	SetInitialized( false );

	snDSoundGlobal::g_pDirectSound		= NULL;
	snDSoundGlobal::g_pPrimaryBuffer	= NULL;
}


//------------------------------------------------------------------------
//	CleanUp()
//------------------------------------------------------------------------
void 
CleanUp() throw ()
{
	if ( IsInitialized() )
	{
		DBG_WARNING( "Sound System NOT DeInitialized -- automatic deinitialization taking place" );

		// You must deinitialize the sound system!
		//
		DeInitialize();
	}

	SetInitialized( false );
}

}	// namespace
