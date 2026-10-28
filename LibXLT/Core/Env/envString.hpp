//****************************************************************************
//	\file envString.hpp
//
// Conversion routines for converting between wide UTF-16 strings and
// char based UTF-8 strings.
//
//	StudioGPU
//	Copyright(C) 2009 - All Rights Reserved
//****************************************************************************
#ifdef ENV_STRING_HPP
#error envString.hpp multiply included
#endif
#define ENV_STRING_HPP


#ifndef ENV_TYPE_HPP
#include "Core/env/envType.hpp"
#endif

#include <string>


//============================================================================
// envString interface
//============================================================================
namespace envString
{
	//------------------------------------------------------------------------
	// Convert from 16-bit wide unicode string to 8-bit multibyte
	// UTF8 unicode string.
	//------------------------------------------------------------------------
	std::string WideCharToUTF8(const envType::WChar *i_String, int i_Length);

	//------------------------------------------------------------------------
	// Convert a wide wstring to UTF8 std::string
	//------------------------------------------------------------------------
	std::string WideCharToUTF8(const std::wstring &i_String);

	//------------------------------------------------------------------------
	// Convert from 8-bit UTF8 multibyte unicode string to 16-bit wide
	// unicode string.
	//------------------------------------------------------------------------
	std::wstring UTF8ToWideChar(const char *i_String, int i_Length);

	//------------------------------------------------------------------------
	// Convert a UTF8 std::string to wide wstring
	//------------------------------------------------------------------------
	std::wstring UTF8ToWideChar(const std::string &i_String);

	//------------------------------------------------------------------------
	// Convert UTF-8 to UTF-16 (the string format of binary chunk files).
	// Portable: no platform calls, the same on every platform. Embedded
	// NULs are kept. Invalid UTF-8 (a stray continuation byte, a lead
	// byte without all its continuation bytes, an overlong form, an
	// encoded surrogate, a value above U+10FFFF) becomes U+FFFD, one for
	// each maximal invalid subpart, as the Unicode standard recommends.
	//------------------------------------------------------------------------
	std::u16string UTF8ToUTF16(const char *i_String, size_t i_Length);
	std::u16string UTF8ToUTF16(const std::string &i_String);

	//------------------------------------------------------------------------
	// Convert UTF-16 to UTF-8. Portable, as UTF8ToUTF16. Embedded NULs are
	// kept; an unpaired surrogate becomes U+FFFD.
	//------------------------------------------------------------------------
	std::string UTF16ToUTF8(const char16_t *i_String, size_t i_Length);
	std::string UTF16ToUTF8(const std::u16string &i_String);

}
