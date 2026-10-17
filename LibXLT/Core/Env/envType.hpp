/****************************************************************************\
**  envType.hpp
**
**      envType.hpp supplies the machine independent wide character type used
**	in binary file formats, and checks that float and double are 4 and 8
**	bytes.  For fixed-size integers use the standard int32_t, uint8_t, etc.
**	(<cstdint>, included by envPlatform.hpp).
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

	// On Windows systems wchar_t is 16 bits (2 bytes),
	// this define is used in binary file formats where the size of the character is important.
#define WCHART_SIZE 2
	// _WCHAR_T_DEFINED defines whether wchar_t is a type, set from VisualStudio compiler
#ifdef _WCHAR_T_DEFINED
	typedef wchar_t WChar;
#else
	typedef uint16_t WChar;
#endif

#else
	#error Haven''t defined platform independent types for this compiler/OS/machine combination
#endif

	static_assert(sizeof(float) * 8 == 32, "float must be 32 bits");
	static_assert(sizeof(double) * 8 == 64, "double must be 64 bits");
}
