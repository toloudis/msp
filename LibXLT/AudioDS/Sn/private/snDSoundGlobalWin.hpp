/****************************************************************************\
**  snDSoundGlobalWin.hpp
**
**      snDSoundGlobalWin.hpp contains some DirectSound stuff that many components
**	in the Windows DirectSound PAC might need.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef SN_DSOUNDGLOBALWIN_HPP
#error snDSoundGlobalWin.hpp multiply included
#endif
#define SN_DSOUNDGLOBALWIN_HPP

#include <dsound.h>


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
namespace snDSoundGlobal
{
//============================================================================
//	The main DirectSound pointer
//============================================================================
extern	IDirectSound*			g_pDirectSound;		// direct sound

//============================================================================
// The primary buffer all sound gets mixed into
//============================================================================
extern	IDirectSoundBuffer*		g_pPrimaryBuffer;	

//============================================================================
//	Initialize DirectSound and create the primary buffer.
//============================================================================
void Initialize();

//============================================================================
//	Release DirectSound
//============================================================================
void Deinitialize();

//========================================================================
//	GetDirectSound()
//========================================================================
const IDirectSound* GetDirectSound();

//========================================================================
//	GetPrimaryBuffer()
//========================================================================
const IDirectSoundBuffer* GetPrimaryBuffer();

//========================================================================
//	CreateSoundBuffer()
//========================================================================
void CreateSoundBuffer( DSBUFFERDESC * BDesc, IDirectSoundBuffer ** pBuffer );

//============================================================================
//	PrintDSError dumps an error message to the debug log
//============================================================================
void PrintDSError( HRESULT hErr );
}

