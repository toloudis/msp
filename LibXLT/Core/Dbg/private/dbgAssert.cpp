//****************************************************************************
//  dbgAssert.cpp
//
//      see .hpp
//
//	StudioGPU
//	Copyright(C) 2003 - All Rights Reserved
//****************************************************************************

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "Core/dbg/dbgAssert.hpp"
#include "Core/dbg/private/dbgAssertPAC.hpp"
#include "Core/dbg/private/dbgLogPAC.hpp"

#include <sstream>
#include <string>

// Platform specific define
//
#if ENV_COMPILER == ENV_MSVCPP
	#define print_function _vsnprintf
#else
	#if ENV_COMPILER == ENV_PRODGPS2
		#define print_function vsnprintf
	#else
		#define print_function vsnprintf
	#endif
#endif


//============================================================================
//============================================================================
namespace dbgAssert
{

//------------------------------------------------------------------------
//	This function is used by the debug assert and message macros below.
//	It always uses single-byte ANSI characters
//
// \param i_File file name the message came FROM
// \param i_Line line number
// \param i_Format standard sprintf style formatting string
// \param etc the values for the format string
//
// \return void
//------------------------------------------------------------------------
void OKMessage(const char* i_File, int i_Line, const char* i_Format, ...)
{
	va_list		args;
	char		format_text[1024];

	// format the string using sprintf
	va_start( args, i_Format );
	vsprintf( format_text, i_Format, args );   //***is there a way to convert vsprintf to a more stable format?

	std::ostringstream assert_ss(std::ostringstream::out);
	assert_ss << "message from " << i_File << "(" << i_Line << "):\n" << format_text;

	// hang and wait for user to hit OK
	dbgAssertPAC::OKMessage(assert_ss.str().c_str());
}

//------------------------------------------------------------------------
//	This function is used by the debug assert and message macros below.
//	It always uses single-byte ANSI characters
//
// \param i_File file name the message came FROM
// \param i_Line line number
// \param i_Format standard sprintf style formatting string
// \param etc the values for the format string
//
// \return void
//------------------------------------------------------------------------
void AssertMessage(const char* i_File, int i_Line, const char* i_Format, ...)
{
	va_list		args;
	char		format_text[1024];

	va_start( args, i_Format );
	vsprintf( format_text, i_Format, args ); //***is there a way to convert vsprintf to a more stable format?

	std::ostringstream assert_ss(std::ostringstream::out);
	assert_ss << "assertion failure in " << i_File << "(" << i_Line << "):\n" << format_text;

	dbgLogPAC::WriteLog(assert_ss.str().c_str());
		
	if ( dbgAssertPAC::AbortContinueMessage( assert_ss.str().c_str()) )
	{
		// true means abort
		exit(1);
	}
}

//------------------------------------------------------------------------
//	Don't call Init() and CleanUp() yourself; they are called 
//	by the package Init and Cleanup.
//
// \param none
//
// \return void
//------------------------------------------------------------------------
void Init()
{
	dbgAssertPAC::Init();
}

//------------------------------------------------------------------------
//	Don't call Init() and CleanUp() yourself; they are called 
//	by the package Init and Cleanup.
//
// \param none
//
// \return void
//------------------------------------------------------------------------
void CleanUp() throw()
{
	dbgAssertPAC::CleanUp();
}

}

