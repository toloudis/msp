/**********************************************************
**  dbgLogPACXbox.cpp
**
**      dbgLogPACXbox is the definition for the Xbox
**	version of the debug log PAC.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\*********************************************************/

#include <string.h>
#include <tchar.h>
#include <time.h>
#include <xtl.h>

#include "dbgLogPACXbox.hpp"
#include "envInitX.hpp"

namespace dbgLogPAC
{

namespace
{

HANDLE l_DebugFile = INVALID_HANDLE_VALUE;

}

//========================================================================
//	WriteLog writes the given text to the platform debug log.	
//========================================================================	
void WriteLog(const char* i_Text)
{
	// debug log text is assumed to be single byte english
	std::string line(i_Text);

	line += '\r';
	line += '\n';

	DWORD bytes_written;
	BOOL ret_val;

	if ( l_DebugFile != INVALID_HANDLE_VALUE )
	{
		ret_val = ::WriteFile(	l_DebugFile,
								line.c_str(),
								line.size(),
								&bytes_written,
								NULL);

	}

	::OutputDebugStringA(line.c_str());
}

//========================================================================
//	UnicodetoANSI is a convenience function for debug logging that will turn an
//	itString into a std::string that is then returned.  This function,
//	in the Xbox implementation, will only work with "ANSI Unicode" - the
//	lower 256 values of Unicode.
//========================================================================
std::string UnicodetoANSI(const envType::UInt16 *i_String, int i_Length)
{
	std::string ret_val;
	int i;
	for( i = 0 ; i < i_Length ; ++i )
	{
		ret_val += char(i_String[i]);
	}
	
	return ret_val;
}

//========================================================================
//	Don't call Init() yourself; it is called by the package Init().
//========================================================================
void Init()
{
	char filename[MAX_PATH];
	
	// find filename of executable
//	::GetModuleFileNameA(NULL, filename, MAX_PATH);
	// set it to empty string now, will fill it once we know how to
	// get the module name on Xbox.
	filename[0] = 0;
		
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

//#if ENV_BUILD != ENV_GOLDBUILD
#if ENV_BUILD == ENV_DEBUGBUILD
	l_DebugFile = ::CreateFileA(filename,
								GENERIC_WRITE,
								0,
								NULL,
								CREATE_ALWAYS,
								FILE_FLAG_WRITE_THROUGH,
								NULL);
#endif

	char date_string[64];
	char time_string[64];
	char total_string[256];
	_strdate(date_string);
	_strtime(time_string);
	sprintf(total_string, "Terawatt base debug log initialized at %s, %s", time_string, date_string);

	WriteLog(total_string);
}

//========================================================================
//	Don't call CleanUp() yourself; it is called by the package CleanUp().
//========================================================================
void CleanUp() throw()
{
	char date_string[64];
	char time_string[64];
	char total_string[256];
	_strdate(date_string);
	_strtime(time_string);
	sprintf(total_string, "Terawatt base debug log terminating at %s, %s", time_string, date_string);

	WriteLog(total_string);

	if ( l_DebugFile != INVALID_HANDLE_VALUE )
		::CloseHandle( l_DebugFile );
}

}
