/****************************************************************************\
**  envType.hpp
**
**      envType.hpp supplies machine independent data types for 1, 2, 4, and
**	8 byte length integers, and 4 and 8 byte floating point types.  These types
**	should only be used when it is absolutely necessary to guarantee a byte
**	size (for a file format, for instance).  For other applications you should
**	probably just use int, or float.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef ENV_TYPE_HPP
#error envType.hpp multiply included
#endif
#define ENV_TYPE_HPP

#ifndef ENV_PLATFORM_HPP
#include "Core/env/envPlatform.hpp"
#endif


//============================================================================
//============================================================================
namespace envType
{

//------------------------------------------------------------------------
//	Unfortunately I can think of no way to avoid this preprocessor stuff
//	here...if you can let me know.
//	Machine independent types must be defined in a platform dependent
//	way.
//------------------------------------------------------------------------
#if (ENV_MACHINE == ENV_PC) && (ENV_COMPILER == ENV_MSVCPP)

	typedef char Int8;
	typedef unsigned char UInt8;
	typedef short Int16;
	typedef unsigned short UInt16;
	typedef int Int32;
	typedef unsigned int UInt32;
	typedef __int64 Int64;
	typedef unsigned __int64 UInt64;
	typedef float Float32;
	typedef double Float64;

	// On Windows systems wchar_t is 16 bits (2 bytes), 
	// this define is used in binary file formats where the size of the character is important.
#define WCHART_SIZE 2
	// _WCHAR_T_DEFINED defines whether wchar_t is a type, set from VisualStudio compiler
#ifdef _WCHAR_T_DEFINED
	typedef wchar_t WChar;
#else
	typedef UInt16 WChar;
#endif

#else
	#if (ENV_MACHINE == ENV_PS2) && (ENV_COMPILER == ENV_CODEWARRIOR)
		typedef char Int8;
		typedef unsigned char UInt8;
		typedef short Int16;
		typedef unsigned short UInt16;
		typedef int Int32;
		typedef unsigned int UInt32;
		typedef long Int64;
		typedef unsigned long UInt64;
		typedef long long Int128;		//	128 bit types not necessarily defined on other platforms
		typedef unsigned long long UInt128;
		typedef float Float32;
		typedef double Float64;
	#else
		#if (ENV_MACHINE == ENV_PS2) && (ENV_COMPILER == ENV_PRODGPS2)
			typedef char Int8;
			typedef unsigned char UInt8;
			typedef short Int16;
			typedef unsigned short UInt16;
			typedef int Int32;
			typedef unsigned int UInt32;
			typedef long Int64;
			typedef unsigned long UInt64;
			typedef long long Int128;		//	128 bit types not necessarily defined on other platforms
			typedef unsigned long long UInt128;
			typedef float Float32;
			typedef double Float64;
		#else
			#if (ENV_MACHINE == ENV_XBOX) && (ENV_COMPILER == ENV_MSVCPP)
				typedef char Int8;
				typedef unsigned char UInt8;
				typedef short Int16;
				typedef unsigned short UInt16;
				typedef int Int32;
				typedef unsigned int UInt32;
				typedef __int64 Int64;
				typedef unsigned __int64 UInt64;
				typedef float Float32;
				typedef double Float64;
			#else
				#error Haven''t defined platform independent types for this compiler/OS/machine combination
			#endif
		#endif
	#endif
#endif
}
