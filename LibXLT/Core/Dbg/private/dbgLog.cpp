#error dbgLog.cpp is obsolete

//****************************************************************************
//  dbgLog.cpp
//
//      see .hpp
//
//	StudioGPU
//	Copyright(C) 2003 - All Rights Reserved
//****************************************************************************
#include <stdarg.h>
#include <stdio.h>
#include <time.h>

#include "Core/Dbg/dbgLog.hpp"
#include "Core/Dbg/private/dbgLogPAC.hpp"
#include "Core/Dbg/DbgMsg.hpp"
#include "Core/Env/envString.hpp"

#include <vector>


//============================================================================
//============================================================================
namespace dbgLog
{
//	const int DBG_LOG_STRING_LENGTH = 8192;

	//------------------------------------------------------------------------
	//	This write function is not used by any macros; it just dumps stuff
	//	to the debug log without writing any file or line information.
	//
	// \param i_Format standard sprintf style format string
	// \param etc format string values
	//
	// \return void
	//------------------------------------------------------------------------
	void Write(const char* i_Format, ...)
	{
		// write log uses single byte characters
		va_list		args;
		char*		format_text;

		va_start( args, i_Format );
		int len = _vscprintf( i_Format, args )+1; // _vscprintf doesn't count terminating '\0'
		format_text = new char[ len ];
		vsprintf_s( format_text, len, i_Format, args );

		//dbgLogPAC::WriteLog(format_text);
		delete [] format_text;
	}

	//------------------------------------------------------------------------
	//	"Write" functions are used by the debug logging macros below.
	//	They always use single-byte ANSI characters.
	//
	// \param i_Format standard sprintf style format string
	// \param etc format string values
	//
	// \return void
	//------------------------------------------------------------------------
	void WriteLog(std::wstring& i_File, int i_Line, const char* i_Format, ...)
	{
		// write log uses single byte characters
		va_list		args;
		char*		format_text;

		/*
		const char* last_slash = strrchr(i_File, '/');
		if( last_slash == NULL )
			last_slash = strrchr(i_File, '\\');

		if( last_slash )
			i_File = last_slash + 1;
		*/

		va_start( args, i_Format );
		int len = _vscprintf( i_Format, args )+1; // _vscprintf doesn't count terminating '\0'
		format_text = new char[ len ];
		vsprintf_s( format_text, len, i_Format, args );

		//Write to all open debug streams
		std::vector<dbgStream*> _osList = dbgMsg::GetStreamList(); 	
		for(int i = 0; i < _osList.size(); ++i)
		{
			if(_osList[i]->isStreamActive())
				if(_osList[i]->logEnabled())
					*(dbgMsg::WriteLog( i_File, i_Line , _osList[i])) << format_text << std::endl;
		} 
	
		delete [] format_text;
	}

	//------------------------------------------------------------------------
	//	"Write" functions are used by the debug logging macros below.
	//	They always use single-byte ANSI characters.
	//
	// \param i_Format standard sprintf style format string
	// \param etc format string values
	//
	// \return void
	//------------------------------------------------------------------------
	void WriteWarning(std::wstring& i_File, int i_Line, const char* i_Format, ...)
	{
		va_list		args;
		char*		format_text;

		/*
		const char* last_slash = strrchr(i_File, '/');
		if( last_slash == NULL )
			last_slash = strrchr(i_File, '\\');

		if( last_slash )
			i_File = last_slash + 1;
		*/

		va_start( args, i_Format );
		int len = _vscprintf( i_Format, args )+1; // _vscprintf doesn't count terminating '\0'
		format_text = new char[ len ];
		vsprintf_s( format_text, len, i_Format, args );
		
		//Write to all open debug streams
		std::vector<dbgStream*> _osList = dbgMsg::GetStreamList(); 	
		for(int i = 0; i < _osList.size(); ++i)
		{
			if(_osList[i]->isStreamActive())
				if(_osList[i]->warningEnabled())
					*(dbgMsg::WriteWarning( i_File, i_Line , _osList[i])) << format_text << std::endl;
		} 

		delete [] format_text;
	}

	//------------------------------------------------------------------------
	//	"Write" functions are used by the debug logging macros below.
	//	They always use single-byte ANSI characters.
	//
	// \param i_Format standard sprintf style format string
	// \param etc format string values
	//
	// \return void
	//------------------------------------------------------------------------
	void WriteError(std::wstring& i_File, int i_Line, const char* i_Format, ...)
	{
		va_list	args;
		char*	format_text;

		va_start( args, i_Format );
		int len = _vscprintf( i_Format, args )+1; // _vscprintf doesn't count terminating '\0'
		format_text = new char[ len ];
		vsprintf_s( format_text, len, i_Format, args );

		//Write to all open debug streams
		std::vector<dbgStream*> _osList = dbgMsg::GetStreamList(); 	
		for(int i = 0; i < _osList.size(); ++i)
		{
			if(_osList[i]->isStreamActive())
				if(_osList[i]->errorEnabled())
					*(dbgMsg::WriteError( i_File, i_Line , _osList[i])) << format_text << std::endl;
		} 
	
		delete [] format_text;
	}

	//------------------------------------------------------------------------
	//	UnicodetoANSI is a convenience function for debug logging that will turn an
	//	itString into a std::string that is then returned
	//	this function is windows only
	//
	// \param i_String original itString
	// \param i_Length length of the string
	//
	// \return the converted string
	//------------------------------------------------------------------------
	std::string UnicodetoANSI(const envType::UInt16 *i_String, int i_Length)
	{
		return dbgLogPAC::UnicodetoANSI(i_String, i_Length);
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
		dbgLogPAC::Init();
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
		dbgLogPAC::CleanUp();
	}
}
