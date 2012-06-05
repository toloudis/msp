#ifndef _LOG_H_
#define _LOG_H_

//#include <windows.h>


class Log
{
public:
	Log();
	~Log();

	bool StartLog( const char * filename, const char * title, bool append = false );
	bool StartLog( const wchar_t * filename, const wchar_t * title, bool append = false );
	void StopLog();
	
	void WriteLog( const char * format, ... );
	void WriteLog( long sessionID, const char * format, ... );
	void WriteLog( const wchar_t * format, ... );
	void WriteLog( long sessionID, const wchar_t * format, ... );

	bool IsLogging();

private:
	FILE * log;
};

#endif