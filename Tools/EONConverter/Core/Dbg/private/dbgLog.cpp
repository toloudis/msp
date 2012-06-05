//****************************************************************************
//  dbgLog.cpp
//
//      see .hpp
//
//	Extra Large Technology
//	Copyright(C) 2003 - All Rights Reserved
//****************************************************************************

#include <stdarg.h>
#include <stdio.h>

#include "Core/dbg/dbgLog.hpp"
#include "Core/dbg/private/dbgLogPAC.hpp"

namespace dbgLog
{
	//------------------------------------------------------------------------
	///	This write function is not used by any macros; it just dumps stuff
	///	to the debug log without writing any file or line information.
	///
	/// \param i_Format standard sprintf style format string
	/// \param etc format string values
	///
	/// \return void
	//------------------------------------------------------------------------
	void Write(const char* i_Format, ...)
	{
		// write log uses single byte characters
		va_list		args;
		char		format_text[1024];

		va_start( args, i_Format );
		vsprintf( format_text, i_Format, args );
		dbgLogPAC::WriteLog(format_text);		
	}

	//------------------------------------------------------------------------
	///	"Write" functions are used by the debug logging macros below.
	///	They always use single-byte ANSI characters.
	///
	/// \param i_Format standard sprintf style format string
	/// \param etc format string values
	///
	/// \return void
	//------------------------------------------------------------------------
	void WriteLog(const char* i_File, int i_Line, const char* i_Format, ...)
	{
		// write log uses single byte characters
		va_list		args;
		char		format_text[1024];
		char		debug_line[1024];

		const char* last_slash = strrchr(i_File, '/');
		if( last_slash == NULL )
			last_slash = strrchr(i_File, '\\');

		if( last_slash )
			i_File = last_slash + 1;

		va_start( args, i_Format );
		vsprintf( format_text, i_Format, args );
		sprintf( debug_line, "%s(%u): %s", i_File, i_Line, format_text );
		dbgLogPAC::WriteLog(debug_line);
	}

	//------------------------------------------------------------------------
	///	"Write" functions are used by the debug logging macros below.
	///	They always use single-byte ANSI characters.
	///
	/// \param i_Format standard sprintf style format string
	/// \param etc format string values
	///
	/// \return void
	//------------------------------------------------------------------------
	void WriteWarning(const char* i_File, int i_Line, const char* i_Format, ...)
	{
		va_list		args;
		char		format_text[1024];
		char		debug_line[1024];

		const char* last_slash = strrchr(i_File, '/');
		if( last_slash == NULL )
			last_slash = strrchr(i_File, '\\');

		if( last_slash )
			i_File = last_slash + 1;

		va_start( args, i_Format );
		vsprintf( format_text, i_Format, args );
		sprintf( debug_line, "%s(%u): **Warning** %s", i_File, i_Line, format_text );
		dbgLogPAC::WriteLog(debug_line);
	}

	//------------------------------------------------------------------------
	///	"Write" functions are used by the debug logging macros below.
	///	They always use single-byte ANSI characters.
	///
	/// \param i_Format standard sprintf style format string
	/// \param etc format string values
	///
	/// \return void
	//------------------------------------------------------------------------
	void WriteError(const char* i_File, int i_Line, const char* i_Format, ...)
	{
		va_list		args;
		char		format_text[1024];
		char		debug_line[1024];

		va_start( args, i_Format );
		vsprintf( format_text, i_Format, args );
		sprintf( debug_line, "%s(%u): *******ERROR******* %s", i_File, i_Line, format_text );
		dbgLogPAC::WriteLog(debug_line);
	}

	//------------------------------------------------------------------------
	///	UnicodetoANSI is a convenience function for debug logging that will turn an
	///	itString into a std::string that is then returned
	///	this function is windows only
	///
	/// \param i_String original itString
	/// \param i_Length length of the string
	///
	/// \return the converted string
	//------------------------------------------------------------------------
	//std::string UnicodetoANSI(const envType::UInt16 *i_String, int i_Length)
	//{
	//	return dbgLogPAC::UnicodetoANSI(i_String, i_Length);
	//}

	//------------------------------------------------------------------------
	///	Don't call Init() and CleanUp() yourself; they are called 
	///	by the package Init and Cleanup.
	///
	/// \param none
	///
	/// \return void
	//------------------------------------------------------------------------
	void Init()
	{
		dbgLogPAC::Init();
	}

	//------------------------------------------------------------------------
	///	Don't call Init() and CleanUp() yourself; they are called 
	///	by the package Init and Cleanup.
	///
	/// \param none
	///
	/// \return void
	//------------------------------------------------------------------------
	void CleanUp() throw()
	{
		dbgLogPAC::CleanUp();
	}

}

