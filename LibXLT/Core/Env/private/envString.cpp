//****************************************************************************
//  envString.cpp
//
//      see .hpp
//
//	StudioGPU
//	Copyright(C) 2009 - All Rights Reserved
//****************************************************************************
#include "Core/env/envString.hpp"
#include "Core/env/private/envStringPAC.hpp"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <iomanip>
#include <sstream>

//#include <vector>
//#include <stdarg.h>
//#include <stdio.h>
//#include <time.h>


//============================================================================
//============================================================================
namespace envString
{
	//------------------------------------------------------------------------
	// Convert from 16-bit wide unicode string to 8-bit multibyte
	// UTF8 unicode string.
	//------------------------------------------------------------------------
	std::string WideCharToUTF8(const envType::WChar *i_String, int i_Length)
	{
		return envStringPAC::WideCharToUTF8(i_String, i_Length);
	}

	//------------------------------------------------------------------------
	// Convert a wide wstring to UTF8 std::string
	//------------------------------------------------------------------------
	std::string WideCharToUTF8(const std::wstring &i_String)
	{
		return envStringPAC::WideCharToUTF8(i_String.c_str(), i_String.length());
	}

	//------------------------------------------------------------------------
	// Convert from 8-bit UTF8 multibyte unicode string to 16-bit wide
	// unicode string.
	//------------------------------------------------------------------------
	std::wstring UTF8ToWideChar(const char *i_String, int i_Length)
	{
		return envStringPAC::UTF8ToWideChar(i_String, i_Length);
	}

	//------------------------------------------------------------------------
	// Convert a UTF8 std::string to wide wstring
	//------------------------------------------------------------------------
	std::wstring UTF8ToWideChar(const std::string &i_String)
	{
		return envStringPAC::UTF8ToWideChar(i_String.c_str(), i_String.length());
	}

	//------------------------------------------------------------------------
	// Convert UTF-8 to UTF-16
	//------------------------------------------------------------------------
	std::u16string UTF8ToUTF16(const char *i_String, size_t i_Length)
	{
		const char16_t c_Replacement = 0xFFFD;
		const unsigned char* bytes = reinterpret_cast<const unsigned char*>(i_String);

		std::u16string res;
		res.reserve(i_Length);

		size_t i = 0;
		while( i < i_Length )
		{
			const unsigned char lead = bytes[i];
			if( lead < 0x80 )
			{
				res += char16_t(lead);
				i++;
				continue;
			}

			// The number of continuation bytes, and the range the first of
			// them must be in (narrower after E0, ED, F0 and F4, which rules
			// out overlong forms, surrogates and values above U+10FFFF);
			// later ones are always 80..BF.
			int num_cont;
			unsigned char first_lo = 0x80, first_hi = 0xBF;
			uint32_t code_point;
			if( lead >= 0xC2 && lead <= 0xDF )
			{
				num_cont = 1;
				code_point = lead & 0x1F;
			}
			else if( lead >= 0xE0 && lead <= 0xEF )
			{
				num_cont = 2;
				code_point = lead & 0x0F;
				if( lead == 0xE0 )
					first_lo = 0xA0;
				else if( lead == 0xED )
					first_hi = 0x9F;
			}
			else if( lead >= 0xF0 && lead <= 0xF4 )
			{
				num_cont = 3;
				code_point = lead & 0x07;
				if( lead == 0xF0 )
					first_lo = 0x90;
				else if( lead == 0xF4 )
					first_hi = 0x8F;
			}
			else
			{
				// a continuation byte, or C0, C1, F5..FF, which never occur
				res += c_Replacement;
				i++;
				continue;
			}

			size_t next = i + 1;
			bool valid = true;
			for( int k = 0 ; k < num_cont ; k++, next++ )
			{
				const unsigned char lo = (k == 0) ? first_lo : 0x80;
				const unsigned char hi = (k == 0) ? first_hi : 0xBF;
				if( next >= i_Length || bytes[next] < lo || bytes[next] > hi )
				{
					valid = false;
					break;
				}
				code_point = (code_point << 6) | (bytes[next] & 0x3F);
			}

			if( !valid )
			{
				// one replacement for the bytes so far; the byte that
				// didn't fit (if any) starts the next sequence
				res += c_Replacement;
				i = next;
				continue;
			}

			if( code_point >= 0x10000 )
			{
				code_point -= 0x10000;
				res += char16_t(0xD800 + (code_point >> 10));
				res += char16_t(0xDC00 + (code_point & 0x3FF));
			}
			else
			{
				res += char16_t(code_point);
			}
			i = next;
		}

		return res;
	}

	std::u16string UTF8ToUTF16(const std::string &i_String)
	{
		return UTF8ToUTF16(i_String.data(), i_String.size());
	}

	//------------------------------------------------------------------------
	// Convert UTF-16 to UTF-8
	//------------------------------------------------------------------------
	std::string UTF16ToUTF8(const char16_t *i_String, size_t i_Length)
	{
		std::string res;
		res.reserve(i_Length);

		size_t i = 0;
		while( i < i_Length )
		{
			uint32_t code_point = i_String[i++];
			if( code_point >= 0xD800 && code_point <= 0xDBFF
				&& i < i_Length && i_String[i] >= 0xDC00 && i_String[i] <= 0xDFFF )
			{
				code_point = 0x10000 + ((code_point - 0xD800) << 10) + (i_String[i++] - 0xDC00);
			}
			else if( code_point >= 0xD800 && code_point <= 0xDFFF )
			{
				code_point = 0xFFFD;	// unpaired surrogate
			}

			if( code_point < 0x80 )
			{
				res += char(code_point);
			}
			else if( code_point < 0x800 )
			{
				res += char(0xC0 | (code_point >> 6));
				res += char(0x80 | (code_point & 0x3F));
			}
			else if( code_point < 0x10000 )
			{
				res += char(0xE0 | (code_point >> 12));
				res += char(0x80 | ((code_point >> 6) & 0x3F));
				res += char(0x80 | (code_point & 0x3F));
			}
			else
			{
				res += char(0xF0 | (code_point >> 18));
				res += char(0x80 | ((code_point >> 12) & 0x3F));
				res += char(0x80 | ((code_point >> 6) & 0x3F));
				res += char(0x80 | (code_point & 0x3F));
			}
		}

		return res;
	}

	std::string UTF16ToUTF8(const std::u16string &i_String)
	{
		return UTF16ToUTF8(i_String.data(), i_String.size());
	}

	bool StartsWith(const std::string &i_String, const std::string &i_SubString)
	{
		return !i_SubString.empty()
			&& i_String.size() >= i_SubString.size()
			&& i_String.compare(0, i_SubString.size(), i_SubString) == 0;
	}

	bool EndsWith(const std::string &i_String, const std::string &i_SubString)
	{
		return !i_SubString.empty()
			&& i_String.size() >= i_SubString.size()
			&& i_String.compare(i_String.size() - i_SubString.size(), i_SubString.size(), i_SubString) == 0;
	}

	bool IsASCII(const std::string &i_String)
	{
		for( char c : i_String )
		{
			if( (unsigned char)c > 127 )
				return false;
		}
		return true;
	}

	//------------------------------------------------------------------------
	// Case-insensitive comparisons
	//------------------------------------------------------------------------
	int CompareNoCase(const std::string &i_String1, const std::string &i_String2)
	{
		const size_t count = std::min(i_String1.size(), i_String2.size());
		for( size_t i = 0; i < count; ++i )
		{
			const int c1 = std::tolower(static_cast<unsigned char>(i_String1[i]));
			const int c2 = std::tolower(static_cast<unsigned char>(i_String2[i]));
			if( c1 != c2 )
				return c1 - c2;
		}
		if( i_String1.size() == i_String2.size() )
			return 0;
		return (i_String1.size() < i_String2.size()) ? -1 : 1;
	}

	bool EqualNoCase(const std::string &i_String1, const std::string &i_String2)
	{
		return CompareNoCase(i_String1, i_String2) == 0;
	}

	bool StartsWithNoCase(const std::string &i_String, const std::string &i_Prefix)
	{
		if( i_String.size() < i_Prefix.size() )
			return false;
		return CompareNoCase(i_String.substr(0, i_Prefix.size()), i_Prefix) == 0;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	const std::string& ToLower(std::string &io_String)
	{
		for( char &c : io_String )
			c = char(std::tolower(static_cast<unsigned char>(c)));
		return io_String;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void TrimSpaces(std::string &io_String)
	{
		const std::string::size_type last = io_String.find_last_not_of(' ');
		if( last == std::string::npos )
		{
			io_String.clear();
			return;
		}
		io_String.erase(last + 1);
		io_String.erase(0, io_String.find_first_not_of(' '));
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	int GetInt(const std::string &i_String)
	{
		return atoi(i_String.c_str());
	}

	//------------------------------------------------------------------------
	// File-name helpers
	//------------------------------------------------------------------------
	void StripExtension(std::string &io_String)
	{
		const std::string::size_type period = io_String.rfind('.');
		if( period != std::string::npos )
			io_String.erase(period);
	}

	void ReplaceExtension(std::string &io_String, const std::string &i_NewExtension)
	{
		StripExtension(io_String);
		io_String += '.';
		io_String += i_NewExtension;
	}

	void GetExtension(const std::string &i_String, std::string &o_Extension)
	{
		const std::string::size_type period = i_String.rfind('.');
		if( period != std::string::npos )
			o_Extension = i_String.substr(period + 1);
	}

	void GetBase(const std::string &i_String, std::string &o_Base)
	{
		const std::string::size_type period = i_String.rfind('.');
		if( period != std::string::npos )
			o_Base = i_String.substr(0, period);
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	int Breakup_Filename(const std::string &i_Filename,
						 std::string &o_BaseFilename,
						 std::string &o_Ext,
						 int &o_InitialNumber)
	{
		if( i_Filename.empty() )
			return 0;

		GetExtension(i_Filename, o_Ext);
		GetBase(i_Filename, o_BaseFilename);

		//	the digits at the end of the base name are the number
		const std::string::size_type last = o_BaseFilename.find_last_not_of("0123456789");
		const std::string::size_type begin = (last == std::string::npos) ? 0 : last + 1;
		const int number_digits = int(o_BaseFilename.size() - begin);

		//	a base name that is all digits, or has none, has no number
		if( last == std::string::npos || number_digits == 0 )
		{
			o_InitialNumber = 0;
		}
		else
		{
			o_InitialNumber = GetInt(o_BaseFilename.substr(begin));
			o_BaseFilename.resize(begin);
		}

		return number_digits;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void Build_Filename(const std::string &i_Basename,
						const std::string &i_Ext,
						int i_Number,
						int i_NumDigits,
						std::string &o_FName)
	{
		std::ostringstream num_ss;
		num_ss << std::setw(i_NumDigits) << std::setfill('0') << i_Number;

		o_FName = i_Basename;
		o_FName += num_ss.str();
		o_FName += '.';
		o_FName += i_Ext;
	}
}
