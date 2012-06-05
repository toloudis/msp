//****************************************************************************
///	\file dbgLog.hpp
///
/// writes out strings to a log file for debugging.
/// dbgLog contains debug logging (file and debugger) functions and 
///	macros.  These write a message, possibly with sprintf style arguments,
///	to a debug log.  On Windows the message is written as a line in a file
///	"debug.log" in the same directory as the executable, and also to the 
///	Visual Studio debugger.
///
///	Extra Large Technology
///	Copyright(C) 2003 - All Rights Reserved
//****************************************************************************
#ifdef DBG_LOG_HPP
#error dbgLog.hpp multiply included
#endif
#define DBG_LOG_HPP

#include <string>

#ifndef ENV_TYPE_HPP
#include "Core/env/envType.hpp"
#endif


//============================================================================
/// Debug Log interface
//============================================================================
namespace dbgLog
{
	//------------------------------------------------------------------------
	///	This write function is not used by any macros; it just dumps stuff
	///	to the debug log without writing any file or line information.
	///
	/// \param i_Format standard sprintf style formatting string
	///
	/// \return void
	//------------------------------------------------------------------------
	void Write(const char* i_Format, ...);

	//------------------------------------------------------------------------
	///	"Write" functions are used by the debug logging macros below.
	///	They always use single-byte ANSI characters.
	///
	/// \param i_File file name the message came FROM
	/// \param i_Line line number
	/// \param i_Format standard sprintf style formatting string
	///
	/// \return void
	//------------------------------------------------------------------------
	void WriteLog(const char* i_File, int i_Line, const char* i_Format, ...);

	//------------------------------------------------------------------------
	///	"Write" functions are used by the debug logging macros below.
	///	They always use single-byte ANSI characters.
	///
	/// \param i_File file name the message came FROM
	/// \param i_Line line number
	/// \param i_Format standard sprintf style formatting string
	///
	/// \return void
	//------------------------------------------------------------------------
	void WriteWarning(const char* i_File, int i_Line, const char* i_Format, ...);

	//------------------------------------------------------------------------
	///	"Write" functions are used by the debug logging macros below.
	///	They always use single-byte ANSI characters.
	///
	/// \param i_File file name the message came FROM
	/// \param i_Line line number
	/// \param i_Format standard sprintf style formatting string
	///
	/// \return void
	//------------------------------------------------------------------------
	void WriteError(const char* i_File, int i_Line, const char* i_Format, ...);

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
	std::string UnicodetoANSI(const envType::UInt16 *i_String, int i_Length);

	//------------------------------------------------------------------------
	///	Don't call Init() and CleanUp() yourself; they are called 
	///	by the package Init and Cleanup.
	///
	/// \return void
	//------------------------------------------------------------------------
	void Init();

	//------------------------------------------------------------------------
	///	Don't call Init() and CleanUp() yourself; they are called 
	///	by the package Init and Cleanup.
	///
	/// \return void
	//------------------------------------------------------------------------
	void CleanUp() throw();
}

//------------------------------------------------------------------------
///	Debug logging macros exist only in debug builds.
///	Errors and Warnings exist in debug, release, and gold.
///	usage example: DBG_LOG0("Inner Loop");
///	usage example: DBG_LOG1("Character Name %s", pCharacter->GetName());
//------------------------------------------------------------------------

#define	DBG_WARNING0( a )							dbgLog::WriteWarning( __FILE__, __LINE__, a )
#define	DBG_WARNING1( a,b )							dbgLog::WriteWarning( __FILE__, __LINE__, a,b )
#define	DBG_WARNING2( a,b,c )						dbgLog::WriteWarning( __FILE__, __LINE__, a,b,c )
#define	DBG_WARNING3( a,b,c,d )						dbgLog::WriteWarning( __FILE__, __LINE__, a,b,c,d )
#define	DBG_WARNING4( a,b,c,d,e )					dbgLog::WriteWarning( __FILE__, __LINE__, a,b,c,d,e )
#define	DBG_WARNING5( a,b,c,d,e,f )					dbgLog::WriteWarning( __FILE__, __LINE__, a,b,c,d,e,f )
#define	DBG_WARNING6( a,b,c,d,e,f,g )				dbgLog::WriteWarning( __FILE__, __LINE__, a,b,c,d,e,f,g )
#define	DBG_WARNING7( a,b,c,d,e,f,g,h )				dbgLog::WriteWarning( __FILE__, __LINE__, a,b,c,d,e,f,g,h )
#define	DBG_WARNING8( a,b,c,d,e,f,g,h,i )			dbgLog::WriteWarning( __FILE__, __LINE__, a,b,c,d,e,f,g,h,i )
#define	DBG_WARNING9( a,b,c,d,e,f,g,h,i,j )			dbgLog::WriteWarning( __FILE__, __LINE__, a,b,c,d,e,f,g,h,i,j )

#define	DBG_ERROR0( a )								dbgLog::WriteError( __FILE__, __LINE__, a )
#define	DBG_ERROR1( a,b )							dbgLog::WriteError( __FILE__, __LINE__, a,b )
#define	DBG_ERROR2( a,b,c )							dbgLog::WriteError( __FILE__, __LINE__, a,b,c )
#define	DBG_ERROR3( a,b,c,d )						dbgLog::WriteError( __FILE__, __LINE__, a,b,c,d )
#define	DBG_ERROR4( a,b,c,d,e )						dbgLog::WriteError( __FILE__, __LINE__, a,b,c,d,e )
#define	DBG_ERROR5( a,b,c,d,e,f )					dbgLog::WriteError( __FILE__, __LINE__, a,b,c,d,e,f )
#define	DBG_ERROR6( a,b,c,d,e,f,g )					dbgLog::WriteError( __FILE__, __LINE__, a,b,c,d,e,f,g )
#define	DBG_ERROR7( a,b,c,d,e,f,g,h )				dbgLog::WriteError( __FILE__, __LINE__, a,b,c,d,e,f,g,h )
#define	DBG_ERROR8( a,b,c,d,e,f,g,h,i )				dbgLog::WriteError( __FILE__, __LINE__, a,b,c,d,e,f,g,h,i )
#define	DBG_ERROR9( a,b,c,d,e,f,g,h,i,j )			dbgLog::WriteError( __FILE__, __LINE__, a,b,c,d,e,f,g,h,i,j )

#if ENV_BUILD == ENV_DEBUGBUILD

#define	DBG_LOG0( a )							dbgLog::WriteLog( __FILE__, __LINE__, a )
#define	DBG_LOG1( a,b )							dbgLog::WriteLog( __FILE__, __LINE__, a,b )
#define	DBG_LOG2( a,b,c )						dbgLog::WriteLog( __FILE__, __LINE__, a,b,c )
#define	DBG_LOG3( a,b,c,d )						dbgLog::WriteLog( __FILE__, __LINE__, a,b,c,d )
#define	DBG_LOG4( a,b,c,d,e )					dbgLog::WriteLog( __FILE__, __LINE__, a,b,c,d,e )
#define	DBG_LOG5( a,b,c,d,e,f )					dbgLog::WriteLog( __FILE__, __LINE__, a,b,c,d,e,f )
#define	DBG_LOG6( a,b,c,d,e,f,g )				dbgLog::WriteLog( __FILE__, __LINE__, a,b,c,d,e,f,g )
#define	DBG_LOG7( a,b,c,d,e,f,g,h )				dbgLog::WriteLog( __FILE__, __LINE__, a,b,c,d,e,f,g,h )
#define	DBG_LOG8( a,b,c,d,e,f,g,h,i )			dbgLog::WriteLog( __FILE__, __LINE__, a,b,c,d,e,f,g,h,i )
#define	DBG_LOG9( a,b,c,d,e,f,g,h,i,j )			dbgLog::WriteLog( __FILE__, __LINE__, a,b,c,d,e,f,g,h,i,j )
#define	DBG_LOG10( a,b,c,d,e,f,g,h,i,j,k )		dbgLog::WriteLog( __FILE__, __LINE__, a,b,c,d,e,f,g,h,i,j,k )

#endif

#if ENV_BUILD == ENV_RELEASEBUILD

#define	DBG_LOG0( a )
#define	DBG_LOG1( a,b )
#define	DBG_LOG2( a,b,c )
#define	DBG_LOG3( a,b,c,d )
#define	DBG_LOG4( a,b,c,d,e )
#define	DBG_LOG5( a,b,c,d,e,f )
#define	DBG_LOG6( a,b,c,d,e,f,g )
#define	DBG_LOG7( a,b,c,d,e,f,g,h )
#define	DBG_LOG8( a,b,c,d,e,f,g,h,i )
#define	DBG_LOG9( a,b,c,d,e,f,g,h,i,j )
#define	DBG_LOG10( a,b,c,d,e,f,g,h,i,j,k )

#endif

#if ENV_BUILD == ENV_GOLDBUILD

#define	DBG_LOG0( a )
#define	DBG_LOG1( a,b )
#define	DBG_LOG2( a,b,c )
#define	DBG_LOG3( a,b,c,d )
#define	DBG_LOG4( a,b,c,d,e )
#define	DBG_LOG5( a,b,c,d,e,f )
#define	DBG_LOG6( a,b,c,d,e,f,g )
#define	DBG_LOG7( a,b,c,d,e,f,g,h )
#define	DBG_LOG8( a,b,c,d,e,f,g,h,i )
#define	DBG_LOG9( a,b,c,d,e,f,g,h,i,j )
#define	DBG_LOG10( a,b,c,d,e,f,g,h,i,j,k )

#endif
