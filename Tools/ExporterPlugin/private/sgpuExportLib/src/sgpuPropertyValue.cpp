/****************************************************************************\
**  sgpuPropertyValue.cpp
**
**      sgpuPropertyValue.hpp defines the sgpuPropertyValue class
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "sgpuPropertyValue.hpp"
#include "sgpuException.hpp"
#include "sgpuStringImpl.hpp"
#include "Core/It/itStringUtil.hpp"
#include "Core/Env/envString.hpp"
#include <string>
#include <algorithm>


sgpuPropertyValue::sgpuPropertyValue():
m_Type( eUnknown )
{}

sgpuPropertyValue::sgpuPropertyValue( const sgpuPropertyValue &i_Other):
m_Type( i_Other.m_Type),
m_Value(i_Other.m_Value)
{}

sgpuPropertyValue & sgpuPropertyValue::operator=( const sgpuPropertyValue & i_Other )
{
	m_Type = i_Other.m_Type;
	m_Value = i_Other.m_Value;
	return *this;
}

bool sgpuPropertyValue::GetBoolValue() const
{
	if( m_Type == eBool )
	{
		bool bVal = ( strcmp( m_Value.m_pImpl->m_Data.c_str(), "0" ) ) ? true : false;
		return bVal;
	}
	throw sgpuException( sgpuString( "not a boolean value") ) ;
}
bool sgpuPropertyValue::operator==( const sgpuPropertyValue & i_Other ) const
{
	return m_Type == i_Other.m_Type && m_Value == i_Other.m_Value;
}


#if defined(NEVER)

//=============================================================================
// Convert an ansi string to microsoft unicode, based on the
// current codepage settings for file apis.
// 
// For smaller strings, use a static cache
// For larger strings, use a heap
//=============================================================================

int MbcsToUnicode(const char *i_zMbcsString, std::wstring &o_WString )
{
	int nByte;
	static wchar_t zMbcsFilenameStatic[ 1024];
	wchar_t *zMbcsFilenameDynamic = NULL;
	wchar_t *zMbcsFilename = &zMbcsFilenameStatic[ 0 ];
	int codepage = AreFileApisANSI() ? CP_ACP : CP_OEMCP;

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
int UnicodeToMbcs(const wchar_t *i_zWString, char *o_pzBuffer,  )
{
	int nByte;
	static wchar_t zMbcsFilenameStatic[ 1024];
	wchar_t *zMbcsFilenameDynamic = NULL;
	wchar_t *zMbcsFilename = &zMbcsFilenameStatic[ 0 ];
	int codepage = AreFileApisANSI() ? CP_ACP : CP_OEMCP;

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
#endif