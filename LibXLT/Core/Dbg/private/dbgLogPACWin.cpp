//#error dbgLogPACWin.cpp is obsolete

//****************************************************************************
//  dbgLogPACWin.cpp
//
//      see .hpp
//
//	StudioGPU
//	Copyright(C) 2003 - All Rights Reserved
//****************************************************************************
#include <string.h>
#include <tchar.h>
#include <time.h>
#include <windows.h>

#include "dbgLogPACWin.hpp"
#include "Core/Env/envInitX.hpp"
#include "Core/Env/envString.hpp"
#include "Core/Dbg/dbgMsg.hpp"

#include <sstream>

//============================================================================
//============================================================================
namespace dbgLogPAC
{

namespace
{
	const int DBG_LOG_STRING_LENGTH = 8192; //= MAX_PATH;
	HANDLE l_DebugFile = INVALID_HANDLE_VALUE;
}


//------------------------------------------------------------------------
//	WriteLog writes the given text to the platform debug log.	
//
// \param i_Text string to write to the log file
//
// \return void
//------------------------------------------------------------------------	
void WriteLog(const char* i_Text)
{
	// debug log text is assumed to be single byte english
	std::string line(i_Text);

	line += '\r';
	line += '\n';

//	DWORD bytes_written;
//	BOOL ret_val;

	/*
	if ( l_DebugFile != INVALID_HANDLE_VALUE )
	{
		ret_val = ::WriteFile(	l_DebugFile,
								line.c_str(),
								line.size(),
								&bytes_written,
								NULL);

	}*/



	::OutputDebugStringA(line.c_str());
}

//------------------------------------------------------------------------
//	WriteDebugOutput sends UTF-8 text to the debugger's output window.
//
// \param i_UTF8Text text to send, without any line ending added
//
// \return void
//------------------------------------------------------------------------
void WriteDebugOutput(const std::string& i_UTF8Text)
{
	// The stream text is UTF-8, so promote it to a wide string and use the
	// "W" variation, even in ANSI builds.
	std::wstring wide_str = envString::UTF8ToWideChar( i_UTF8Text );
	::OutputDebugStringW( wide_str.c_str() );
}

//------------------------------------------------------------------------
//	Don't call Init() yourself; it is called by the package Init().
//
// \param none
//
// \return void
//------------------------------------------------------------------------
void Init()
{
	char filename[DBG_LOG_STRING_LENGTH];
	
	// find filename of executable
	::GetModuleFileNameA(NULL, filename, DBG_LOG_STRING_LENGTH);
		
	// find last slash
	char* last_slash = ::strrchr(filename, _T('\\'));

	// how can there be no slash in the filename?  should be full path
	// handle by just writing to the filename directly?
	if( last_slash == NULL )
	{
		::strcpy(filename, "debug.log");
	}
	else
	{
		// advance to character after next
		last_slash++;

		// terminate string
		*last_slash = 0;

		// add our debug log filename
		::strcat(filename, "debug.log");
	}

/*
#if ENV_BUILD != ENV_GOLDBUILD
	l_DebugFile = ::CreateFileA(filename,
								GENERIC_WRITE,
								FILE_SHARE_READ,
								NULL,
								CREATE_ALWAYS,
								FILE_FLAG_WRITE_THROUGH,
								NULL);
#endif
*/
}

//------------------------------------------------------------------------
//	Don't call CleanUp() yourself; it is called by the package CleanUp().
//
// \param none
//
// \return void
//------------------------------------------------------------------------
void CleanUp() throw()
{
	char date_string[64];
	char time_string[64];
//	char total_string[256];
	_strdate(date_string);
	_strtime(time_string);
	std::ostringstream log_ss(std::ostringstream::out);
	log_ss << "base debug log terminating at " << time_string << ", " << date_string;

	WriteLog(log_ss.str().c_str());

	if ( l_DebugFile != INVALID_HANDLE_VALUE )
		::CloseHandle( l_DebugFile );
}

}
