#include "Log.h"

#pragma warning(disable:4996)
#include <time.h>
#include "stdarg.h"
#include <string>

#include <windows.h>


using namespace std;

Log::Log():
	log(0),
	indent(0)
{
}

Log::~Log()
{
	StopLog();
}

bool
Log::StartLog( const char * filename, const char * title, bool append )
{
	char sTime[256], sDate[256];

	if (log)
		StopLog();
	log = fopen( filename, append?"at":"wt" );
	if ( !log  )
	{
		return false;
	}

  _strdate( sDate );
	_strtime( sTime );
	
	fprintf( log, "\n-----------------------------------------------------------------\n" );
	fprintf( log, "%s log at %s %s\n", title, sDate, sTime );
	fprintf( log, "-----------------------------------------------------------------\n\n" );

	fflush( log );

	return true;
}

bool
Log::StartLog( const wchar_t * filename, const wchar_t * title, bool append )
{
	wchar_t sTime[256], sDate[256];

	if (log)
		StopLog();
	log = _wfopen( filename, append?L"at":L"wt" );
	if ( !log )
	{
		return false;
	}

 	_wstrdate( sDate );
	_wstrtime( sTime );

	fprintf( log, "\n-----------------------------------------------------------------\n" );
	fwprintf( log, L"%s log at %s %s\n", title, sDate, sTime );
	fprintf( log, "-----------------------------------------------------------------\n\n" );

	fflush( log );

	return true;
}

void
Log::StopLog()
{
	if (log)
	{
		fprintf( log, "\n\n--------------------------- End of log --------------------------\n" );
		fclose( log );
		log = NULL;
	}
}

bool
Log::IsLogging()
{
	return log!=NULL;
}

//Write info into log file
void
Log::WriteLog( const char * format, ... )
{
	va_list va;

	if (log)
	{
		va_start( va, format );
		fprintf(log, "%s", sindent.c_str() );
		vfprintf( log, format, va );
		//vprintf( format, va );

#ifdef _DEBUG
		//char s[8192];
		//vsprintf( s, format, va );
		//OutputDebugStringA( s );
#endif

		va_end( va );
		fflush( log );
	}
}

void
Log::WriteLog( long sessionID, const char * format, ... )
{
	va_list va;

	if (log)
	{
		va_start( va, format );
		fprintf( log, "%4d:%s", sessionID, sindent.c_str() );
		vfprintf( log, format, va );
		va_end( va );
		fflush( log );
	}
}

void
Log::WriteLog( const wchar_t * format, ... )
{
	va_list va;

	if (log)
	{
		va_start( va, format );
		fwprintf( log, L"%s",  wsindent.c_str() );
		vfwprintf( log, format, va );
		//vprintf( format, va );

#ifdef _DEBUG
		wchar_t s[8192];
		vswprintf( s, format, va );
		OutputDebugString( s );
#endif

		va_end( va );
		fflush( log );
	}
}

void
Log::WriteLog( long sessionID, const wchar_t * format, ... )
{
	va_list va;

	if (log)
	{
		va_start( va, format );
		fwprintf( log, L"%4d:%s", sessionID,  wsindent.c_str() );
		vfwprintf( log, format, va );
		va_end( va );
		fflush( log );
	}
}