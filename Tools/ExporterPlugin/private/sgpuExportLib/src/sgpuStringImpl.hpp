/****************************************************************************\
**  sgpuStringImpl.hpp
**
**      sgpuStringImpl.hpp defines private implementation of common utilities
**		used in the exporter SDK
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifndef SGPU_STRINGIMPL_HPP
#define SGPU_STRINGIMPL_HPP

#include "sgpuExportLib.hpp"
#include <string>
struct sgpuStringImpl
{
	sgpuStringImpl(){}
	sgpuStringImpl( const char *i_pzName );
	sgpuStringImpl( const wchar_t *i_pzName );
	std::string m_Data;
};

int UTF8ToUnicode_NumBytes( const char *i_zMbcsString );
int UTF8ToUnicode(const char *i_zMbcsString, std::wstring &o_WString );
int UnicodeToUTF8_NumBytes(const wchar_t *i_zWString );
int UnicodeToUTF8(const wchar_t *i_zWString, std::string & o_String );

#endif // #ifndef SGPU_STRINGIMPL_HPP
