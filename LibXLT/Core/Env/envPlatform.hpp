/****************************************************************************\
**  envPlatform.hpp
**
**      envPlatform.hpp manages platform definitions.  Some definitions, like
**	ENV_MACHINE and ENV_COMPILER, must be defined by the user in his
**	preprocessor defines sections.  Others, like ENV_ENDIAN and ENV_WINDOWS,
**	are deduced in this file based on other values.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef ENV_PLATFORM_HPP
#error envPlatform.hpp multiply included
#endif
#define ENV_PLATFORM_HPP


//========================================================================
//	define the standard windows variable
//========================================================================
#if defined(WIN32) || defined(WIN64)
	#if !defined(_WINDOWS)
		#define _WINDOWS
	#endif
#endif


//========================================================================
// Machine Type (ENV_MACHINE) possible values:
//========================================================================
#define		ENV_PC			1
#define		ENV_MAC			2

//========================================================================
//	Test to make sure user has picked one
//========================================================================
#if !defined(ENV_MACHINE)
	#if defined(WIN32) || defined(WIN64)
		#define ENV_MACHINE ENV_PC
	#else
		#error ENV_MACHINE not defined!
	#endif
#endif

#if	(ENV_MACHINE != ENV_PC)			&& \
	(ENV_MACHINE != ENV_MAC)
#error ENV_MACHINE not defined! (value not valid)
#endif

//========================================================================
// OS Type (ENV_OS) possible values.  Windows means Windows 10 or later.
// macOS and Linux are placeholders for future PAC implementations.
//========================================================================
#define		ENV_WINDOWSOS		10
#define		ENV_MACOS			20
#define		ENV_LINUXOS			30

//========================================================================
//	Test to make sure user has picked one
//========================================================================
#if !defined(ENV_OS)
	#if defined(WIN32) || defined(WIN64)
		#define ENV_OS ENV_WINDOWSOS
	#else
		#error ENV_OS not defined!
	#endif
#endif

#if	(ENV_OS != ENV_WINDOWSOS) &&	\
	(ENV_OS != ENV_MACOS	) &&	\
	(ENV_OS != ENV_LINUXOS	)
	#error ENV_OS not defined!
#endif

//========================================================================
//	Compiler (ENV_COMPILER) possible values:
//========================================================================
#define		ENV_MSVCPP			30

//========================================================================
//	Test to make sure user has picked one
//========================================================================
#if !defined(ENV_COMPILER)
	#if defined(WIN32) || defined(WIN64)
		#define ENV_COMPILER ENV_MSVCPP
	#else
		#error ENV_COMPILER not defined!
	#endif
#endif

#if	(ENV_COMPILER != ENV_MSVCPP)
	#error ENV_COMPILER not defined!
#endif

//========================================================================
//	Build (ENV_BUILD) possible values:
//
//========================================================================
#define		ENV_DEBUGBUILD		50
#define		ENV_RELEASEBUILD	51
#define		ENV_GOLDBUILD		52

//========================================================================
//	Test to make sure user has picked one
//========================================================================
#if !defined(ENV_BUILD)
	#ifdef _DEBUG
		#define ENV_BUILD ENV_DEBUGBUILD
	#else
		#ifdef NDEBUG
			#define ENV_BUILD ENV_RELEASEBUILD
		#else
			#error ENV_BUILD not defined!
		#endif
	#endif
#endif

#if	(ENV_BUILD != ENV_DEBUGBUILD) &&	\
	(ENV_BUILD != ENV_RELEASEBUILD) &&	\
	(ENV_BUILD != ENV_GOLDBUILD)
	#error ENV_BUILD not defined!
#endif

//========================================================================
//	ENV_ENDIAN possible values:
//========================================================================
#define		ENV_LITTLEENDIAN	61
#define		ENV_BIGENDIAN		62

//========================================================================
//	Deduce endian value (ENV_ENDIAN)
//========================================================================
#if	(ENV_MACHINE == ENV_PC)
	#define ENV_ENDIAN	ENV_LITTLEENDIAN
#endif

#if (ENV_MACHINE == ENV_MAC)
	#define ENV_ENDIAN	ENV_BIGENDIAN
#endif

#ifndef ENV_ENDIAN
	#error Couldn''t decide on ENV_ENDIAN
#endif

//========================================================================
//	ENV_WINDOWS can be either 1 or 0 (true or false)
//========================================================================
#if	(ENV_OS == ENV_WINDOWSOS)
	#define ENV_WINDOWS 1
#else
	#define ENV_WINDOWS 0
#endif

//========================================================================
//	ENV_DEBUG and ENV_NDEBUG can be either 1 or 0 (true or false)
//========================================================================
#if (ENV_BUILD == ENV_DEBUGBUILD )
	#define ENV_DEBUG 1
	#define ENV_NDEBUG 0
#else
	#define ENV_DEBUG 0
	#define ENV_NDEBUG 1
#endif

//========================================================================
//	Graphics Library (ENV_GRAPHICLIBRARY) possible values:
//========================================================================
#define		ENV_GLIB_DX9		70
#define		ENV_GLIB_DX10		71
#define		ENV_GLIB_DX11		72
#define		ENV_GLIB_OGL		77

//========================================================================
//	Graphics library type
//========================================================================
#if !defined(ENV_GRAPHICLIBRARY)
	#if (ENV_WINDOWS == 1)
		#define ENV_GRAPHICLIBRARY ENV_GLIB_DX9
	#else
		#define ENV_GRAPHICLIBRARY ENV_GLIB_OGL
	#endif
#endif

//========================================================================
//	Compiler specific configuration area
//	Place compiler configuration pragmas,etc in this area
//========================================================================

#if (ENV_COMPILER == ENV_MSVCPP)
#include <stddef.h>	 // for NULL

//	put any pragmas here.
//
#pragma warning( disable: 4514 )	//	warning C4514: unreferenced inline function has been removed
#pragma warning( disable: 4127 )	//	warning C4127: conditional expression is constant
#pragma warning( disable: 4786 )	//	warning C4786: identifier was truncated to 'number' characters in the debug information
#pragma warning( disable: 4100 )	//	warning C4100: '_P' : unreferenced formal parameter
#pragma warning( disable: 4663 )	//	warning C4663: C++ language change: to explicitly specialize class template 'vector' use the following syntax: template<> class vector<bool,class std::allocator<unsigned int> > ...
#pragma warning( disable: 4018 )	//	warning C4018: '<' : signed/unsigned mismatch
#pragma warning( disable: 4267 )	//	warning C4267: '=' : conversion from 'size_t' to 'int', possible loss of data
#pragma warning( error: 4150  )		//  warning C4150: deletion of pointer to incomplete type 'type'; no destructor called (report as error)

#endif  // visual studio
