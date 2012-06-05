#include <stdafx.h>
#include "Log.h"

#pragma warning(disable:4996)

#include <stdio.h>

Log::Log():log(0)
{
}

Log::~Log()
{
	StopLog();
}

bool
Log::StartLog( const char * filename, const char * title, bool append )
{
	SYSTEMTIME time;
	char sTime[256], sDate[256];

	if (log)
		StopLog();

	if ( !(log = fopen( filename, append?"at":"wt" )) )
	{
		return false;
	}

	GetLocalTime( &time );
	if (!GetDateFormatA( LOCALE_USER_DEFAULT, DATE_LONGDATE, &time, NULL, sDate, 250 ) )
	{
		fprintf( log, "\nDate format error!\n" );
		StopLog();
		return false;
	}

	if (!GetTimeFormatA( LOCALE_USER_DEFAULT, 0, &time, NULL, sTime, 250 ) )
	{
		fprintf( log, "\nTime format error!\n" );
		StopLog();
		return false;
	}
	
	fprintf( log, "\n-----------------------------------------------------------------\n" );
	fprintf( log, "%s log at %s %s\n", title, sDate, sTime );
	fprintf( log, "-----------------------------------------------------------------\n\n" );

	fflush( log );

	return true;
}

bool
Log::StartLog( const wchar_t * filename, const wchar_t * title, bool append )
{
	SYSTEMTIME time;
	wchar_t sTime[256], sDate[256];

	if (log)
		StopLog();

	if ( !(log = _wfopen( filename, append?L"at":L"wt" )) )
	{
		return false;
	}

	GetLocalTime( &time );
	if (!GetDateFormatW( LOCALE_USER_DEFAULT, DATE_SHORTDATE, &time, NULL, sDate, 250 ) )
	{
		fprintf( log, "\nDate format error!\n" );
		StopLog();
		return false;
	}

	if (!GetTimeFormatW( LOCALE_USER_DEFAULT, 0, &time, NULL, sTime, 250 ) )
	{
		fprintf( log, "\nTime format error!\n" );
		StopLog();
		return false;
	}
	
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
		vfprintf( log, format, va );
		//vprintf( format, va );

#ifdef _DEBUG
		char s[8192];
		vsprintf( s, format, va );
		OutputDebugStringA( s );
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
		fprintf( log, "%4d : ", sessionID );
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
		vfwprintf( log, format, va );
		//vprintf( format, va );

#ifdef _DEBUG
		wchar_t s[8192];
		vswprintf( s, format, va );
		OutputDebugStringW( s );
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
		fwprintf( log, L"%4d : ", sessionID );
		vfwprintf( log, format, va );
		va_end( va );
		fflush( log );
	}
}