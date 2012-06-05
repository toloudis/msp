#ifndef _LOG_H_
#define _LOG_H_

#include <stdio.h>
#include <string>

class LogUser;


class Log
{
public:
	


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
	int	indent;
	std::wstring wsindent;
	std::string  sindent;
public:
struct Indent
	{
		Indent( Log & log ):
		m_log( log )
		{
			m_log.sindent.append("  ");
			m_log.wsindent.append(L"  ");
			size_t slen = m_log.sindent.length();
		}
		~Indent( )
		{
			size_t slen = m_log.sindent.length();
			std::string::iterator last =m_log.sindent.end();
			--last;
			--last;
			m_log.sindent.erase( last, m_log.sindent.end() );
			slen = m_log.sindent.length();
			std::wstring::iterator wlast = m_log.wsindent.end();
			--wlast;
			--wlast;
			m_log.wsindent.erase( wlast, m_log.wsindent.end() );
		}
		Indent &operator=( const Indent &other )
		{
			m_log = other.m_log;
			return *this;
		}
		Log &m_log;
	};
};



class LogUser
{
public:
	virtual ~LogUser();
	LogUser();
	bool StartLog( const std::string &logFilePath, const std::string &header );
	bool StartLog( const std::wstring &logFilePath, const std::string &header );
	Log m_log;
};

#endif