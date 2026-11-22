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

	//------------------------------------------------------------------------
	// True if i_String starts or ends with i_SubString. An empty
	// i_SubString gives false (as itString::StartsWith and EndsWith did).
	// C++17's std::string has no starts_with or ends_with.
	//------------------------------------------------------------------------
	bool StartsWith(const std::string &i_String, const std::string &i_SubString);
	bool EndsWith(const std::string &i_String, const std::string &i_SubString);

	//------------------------------------------------------------------------
	// True if every byte of i_String is ASCII (below 0x80).
	//------------------------------------------------------------------------
	bool IsASCII(const std::string &i_String);

	//------------------------------------------------------------------------
	// Case-insensitive comparisons. Only ASCII letters are folded; other
	// bytes (UTF-8 included) compare as they are. CompareNoCase returns
	// less than, equal to or greater than zero, like strcmp.
	//------------------------------------------------------------------------
	int CompareNoCase(const std::string &i_String1, const std::string &i_String2);
	bool EqualNoCase(const std::string &i_String1, const std::string &i_String2);
	bool StartsWithNoCase(const std::string &i_String, const std::string &i_Prefix);

	//------------------------------------------------------------------------
	// Lowercase the ASCII letters of io_String in place, and return it.
	//------------------------------------------------------------------------
	const std::string& ToLower(std::string &io_String);

	//------------------------------------------------------------------------
	// Remove the leading and trailing spaces.
	//------------------------------------------------------------------------
	void TrimSpaces(std::string &io_String);

	//------------------------------------------------------------------------
	// i_String as an int (atoi: 0 if it doesn't start with a number).
	//------------------------------------------------------------------------
	int GetInt(const std::string &i_String);

	//------------------------------------------------------------------------
	// File-name helpers. StripExtension removes everything from the last
	// period on (nothing if there is none); ReplaceExtension strips the
	// extension and appends a period and i_NewExtension; GetExtension and
	// GetBase give what follows and what precedes the last period, and
	// leave o_Extension and o_Base unchanged if there is none.
	//------------------------------------------------------------------------
	void StripExtension(std::string &io_String);
	void ReplaceExtension(std::string &io_String, const std::string &i_NewExtension);
	void GetExtension(const std::string &i_String, std::string &o_Extension);
	void GetBase(const std::string &i_String, std::string &o_Base);

	//------------------------------------------------------------------------
	// Break a numbered file name, TextXXX.ext, into its base name (without
	// the number), extension and number. Returns the number of digits (0,
	// and a number of 0, if the base name doesn't end in digits).
	//------------------------------------------------------------------------
	int Breakup_Filename(const std::string &i_Filename,
						 std::string &o_BaseFilename,
						 std::string &o_Ext,
						 int &o_InitialNumber);

	//------------------------------------------------------------------------
	// Build a file name from the parts Breakup_Filename gives, the number
	// zero-padded to i_NumDigits.
	//------------------------------------------------------------------------
	void Build_Filename(const std::string &i_Basename,
						const std::string &i_Ext,
						int i_Number,
						int i_NumDigits,
						std::string &o_FName);

}
