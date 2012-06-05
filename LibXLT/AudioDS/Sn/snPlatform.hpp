/****************************************************************************\
**  snPlatform.hpp
**
**      snPlatform.hpp manages audio platform definitions.  Some definitions, like
**	ENV_SOUNDSYSTEM, must be defined by the user in his
**	preprocessor defines sections.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef SN_PLATFORM_HPP
#error snPlatform.hpp multiply included
#endif
#define SN_PLATFORM_HPP


//========================================================================
// Machine Type (SN_SOUNDSYSTEM) possible values:
//========================================================================
#define		SN_DIRECTSOUND		1
#define		SN_MILES			2

//========================================================================
//	Test to make sure user has picked one
//========================================================================
#if !defined(SN_SOUNDSYSTEM)
#error SN_SOUNDSYSTEM not defined!
#endif

#if	(SN_SOUNDSYSTEM != SN_DIRECTSOUND) &&		\
	(SN_SOUNDSYSTEM != SN_MILES) 		
#error SN_SOUNDSYSTEM not defined!
#endif

//========================================================================
//	Make sure the proper sound system is set up based on the OS.
//========================================================================
#ifdef ENV_WINDOWS
#if	(SN_SOUNDSYSTEM != SN_DIRECTSOUND) &&		\
	(SN_SOUNDSYSTEM != SN_MILES) 		
#error SN_SOUNDSYSTEM defined improperly
#endif
#else
#if	(SN_SOUNDSYSTEM == SN_DIRECTSOUND) ||		\
	(SN_SOUNDSYSTEM == SN_MILES) 		
#error SN_SOUNDSYSTEM defined improperly
#endif
#endif
