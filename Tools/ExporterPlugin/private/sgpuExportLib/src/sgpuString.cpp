/****************************************************************************\
**  sgpuString.cpp
**
**      sgpuString.hpp defines the sgpuString class
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "sgpuString.hpp"
#include "sgpuException.hpp"
#include "sgpuStringImpl.hpp"
#include "Core/It/itStringUtil.hpp"
#include "Core/Env/envString.hpp"
#include <string>
#include <algorithm>


//=============================================================================
// Convert an ansi string to microsoft unicode, based on the
// current codepage settings for file apis.
// 
// For smaller strings, use a static cache
// For larger strings, use a heap
//=============================================================================

int UTF8ToUnicode_NumBytes( const char *i_zMbcsString )
{
	int codepage = CP_UTF8;
	int nByte = MultiByteToWideChar(codepage, 0, i_zMbcsString, -1, NULL,0)*sizeof(wchar_t);
	return nByte;
}

int UTF8ToUnicode(const char *i_zMbcsString, std::wstring &o_WString )
{
	int nByte;
	static wchar_t zMbcsFilenameStatic[ 1024];
	wchar_t *zMbcsFilenameDynamic = NULL;
	wchar_t *zMbcsFilename = &zMbcsFilenameStatic[ 0 ];
	int codepage = CP_UTF8;
	nByte = MultiByteToWideChar(codepage, 0, i_zMbcsString, -1, NULL,0)*sizeof(wchar_t);
	if( nByte > 1024)
	{
		zMbcsFilenameDynamic = new wchar_t [ nByte  ];
		if( zMbcsFilenameDynamic==0 ){
			return 0;
		}
		zMbcsFilename = zMbcsFilenameDynamic ;
	}
	nByte = MultiByteToWideChar(codepage, 0, i_zMbcsString, -1, zMbcsFilename, nByte);
	if( nByte > 0 )
	{
		o_WString = std::wstring( zMbcsFilename );
	}
	delete [] zMbcsFilenameDynamic;
	return nByte;
}

int UnicodeToUTF8_NumBytes(const wchar_t *i_zWString )
{
	int nByte;
	int codepage = CP_UTF8;
	nByte = WideCharToMultiByte(codepage, 0, i_zWString, -1, NULL,0,NULL, NULL)*sizeof(char);
	return nByte;
}

int UnicodeToUTF8(const wchar_t *i_zWString, std::string & o_String )
{
	int nByte;
	static char zUTF8FilenameStatic[ 1024];
	char *zUTF8FilenameDynamic = NULL;
	char *zUTF8Filename = &zUTF8FilenameStatic[ 0 ];
	int codepage = CP_UTF8;

	nByte = WideCharToMultiByte(codepage, 0, i_zWString, -1, NULL,0,NULL, NULL)*sizeof(char);
	if( nByte > 1024)
	{
		zUTF8FilenameDynamic = new char [ nByte  ];
		if( zUTF8FilenameDynamic==0 ){
			return 0;
		}
		zUTF8Filename = zUTF8FilenameDynamic ;
	}
	nByte = WideCharToMultiByte(codepage, 0, i_zWString, -1, zUTF8Filename, nByte, NULL, NULL );
	if( nByte > 0 )
	{
		o_String = std::string( zUTF8Filename );
	}
	delete [] zUTF8FilenameDynamic;
	return nByte;
}

sgpuStringImpl::sgpuStringImpl( const char * i_pzString )
{
	m_Data = std::string( i_pzString );
}

sgpuStringImpl::sgpuStringImpl( const wchar_t * i_pzWString )
{
	UnicodeToUTF8( i_pzWString, m_Data);
}



sgpuString::sgpuString()
:m_pImpl( new sgpuStringImpl )
{}

sgpuString::sgpuString( const sgpuString &i_Other ):
m_pImpl(new sgpuStringImpl(*i_Other.m_pImpl))
{}

sgpuString::~sgpuString()
{
	delete m_pImpl;
}

sgpuString& sgpuString::operator=(const sgpuString& i_CopyFrom)
{
	if (&i_CopyFrom != this)
	{ 
		delete m_pImpl; 
		m_pImpl = new sgpuStringImpl(*i_CopyFrom.m_pImpl); 
	} 
	return *this; 
}
bool sgpuString::operator==( const sgpuString& i_Other ) const
{
	return m_pImpl->m_Data == i_Other.m_pImpl->m_Data;
}

/*
Number of bytes required to represent this string
as a zero delimited utf-8 string
*/
int sgpuString::GetNumBytes_UTF8() const
{
	int nByte = m_pImpl->m_Data.length() + 1;
	return nByte;
}
/*
Number of bytes required to represent this string
as a zero delimited wchar_t string
*/
int sgpuString::GetNumBytes_WChart() const
{
	int nBytes = UTF8ToUnicode_NumBytes( m_pImpl->m_Data.c_str() );
	return nBytes;
}
/*
copies the string as a MByte string to the buffer o_pzValue.
o_pzValue should b as big as i_nBufferSize.
If i_nBufferSize >= GetNumBytes_MByte(),
then the string is converted and returned through the buffer
Also returns, the number of bytes actually taken for representing
the NULL terminated buffer string.
else, return 0.
*/
int sgpuString::GetData_UTF8( char * o_pzValue, int i_nBufferSize )const
{
	int nRequired = GetNumBytes_UTF8();
	if( i_nBufferSize < nRequired )
	{
		return -1;
	}
	DBG_ASSERT( i_nBufferSize >= nRequired, "Logical Error, i_nBufferSize:" << i_nBufferSize << " should be greater than or equal to: " << nRequired );
	memcpy( o_pzValue, m_pImpl->m_Data.c_str(), nRequired );
	return nRequired;
}

/*
copies the string as a wchar_t string to the buffer o_pzValue.
o_pzValue should b as big as i_nBufferSize.
If i_nBufferSize >= GetNumBytes_WChart(),
then the string is converted and returned through the buffer
Also returns, the number of bytes actually taken for representing
the NULL terminated buffer string.
else, return 0.
*/
int sgpuString::GetData_WChart( wchar_t * o_pzValue, int i_nBufferSize )const
{
	std::wstring wdata;
	UTF8ToUnicode( m_pImpl->m_Data.c_str(), wdata );
	int nBytes = (wdata.length() + 1)* sizeof( wchar_t ) ;
	if( i_nBufferSize < nBytes )
	{
		return -1;
	}
	memcpy( o_pzValue, wdata.c_str(), nBytes );
	return nBytes;
}


sgpuString::sgpuString( const char *i_Name ):
m_pImpl( new sgpuStringImpl(i_Name) )
{
}

sgpuString::sgpuString( const wchar_t * i_Name ):
m_pImpl( new sgpuStringImpl(i_Name) )
{

}

