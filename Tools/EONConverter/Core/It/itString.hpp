/****************************************************************************\
**  itString.hpp
**
**      itString.hpp defines the itString class, which is the basic string
**	class for localized text in Terawatt.  Please notice that this string is
**	not necessarily NULL terminated.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef IT_STRING_HPP
#error itString.hpp multiply included
#endif
#define IT_STRING_HPP

#include <vector>

#ifndef ENV_TYPE_HPP
#include "Core/env/envType.hpp"
#endif

#ifndef DBG_ASSERT_HPP
#include "Core/dbg/dbgAssert.hpp"
#endif


//============================================================================
//============================================================================
class itString
{
	public:

		//------------------------------------------------------------------------
		//	Here is the Unicode character type.  It is a 16-bit wide character
		//	on all platforms.  There are some advantages to defining it as a
		//	TCHAR on Microsoft OSs but this is more convenient, and there is
		//	little chance that Microsoft will suddenly develop a non-16 bit 
		//	variant of Unicode
		//------------------------------------------------------------------------
		//typedef envType::UInt16 CharType;
		typedef wchar_t CharType;

		//------------------------------------------------------------------------
		//	default and copy constructors
		//------------------------------------------------------------------------
		itString();
		itString(const itString& i_CopyFrom);

		//------------------------------------------------------------------------
		//	This constructor's argument is a single-byte, NULL terminated "C"
		//	string.  It will be converted into a Unicode string.
		//------------------------------------------------------------------------
		explicit itString(const char* i_CopyFrom);

		//------------------------------------------------------------------------
		//	This constructor's argument is a unicode, NULL terminated "C"
		//	string.  It will be converted into a Unicode string.
		//------------------------------------------------------------------------
		explicit itString(const CharType* i_CopyFrom);

		//------------------------------------------------------------------------
		//	This constructor creates the string from a range of characters in
		//	the string parameter.  The substring begins at character # i_Begin and
		//	lasts for i_Num characters.
		//------------------------------------------------------------------------
		itString(int i_Begin, int i_Num, const itString& i_String);

		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		~itString() throw();

		//------------------------------------------------------------------------
		//	operator = copies the input string
		//------------------------------------------------------------------------
		itString& operator = (const itString& i_CopyFrom);

		//------------------------------------------------------------------------
		//	this operator = takes a NULL terminated Unicode string.
		//------------------------------------------------------------------------
		itString& operator = (const CharType* i_CopyFrom);

		//------------------------------------------------------------------------
		//	this operator = takes a single-byte NULL terminated "C" string, and
		//	converts it to a Unicode string
		//------------------------------------------------------------------------
		itString& operator = (const char* i_CopyFrom);

		//------------------------------------------------------------------------
		//	this operator += concatenates a string
		//------------------------------------------------------------------------
		void operator += (const itString& i_String);

		//------------------------------------------------------------------------
		//	this operator += concatenates a single character
		//------------------------------------------------------------------------
		void operator += (CharType i_Char);

		//------------------------------------------------------------------------
		//	operator == and != test for equality of strings
		//------------------------------------------------------------------------
		bool operator == (const itString& i_String) const;
		bool operator != (const itString& i_String) const;

		//------------------------------------------------------------------------
		//	operator < does a lexicographical comparison.
		//------------------------------------------------------------------------
		bool operator < (const itString& i_String) const;

		//------------------------------------------------------------------------
		//	operator <= does a lexicographical comparison.
		//------------------------------------------------------------------------
		bool operator <= (const itString& i_String) const;

		//------------------------------------------------------------------------
		//	operator > does a lexicographical comparison.
		//------------------------------------------------------------------------
		bool operator > (const itString& i_String) const;

		//------------------------------------------------------------------------
		//	operator >= does a lexicographical comparison.
		//------------------------------------------------------------------------
		bool operator >= (const itString& i_String) const;

		//------------------------------------------------------------------------
		//	operator [] returns the CharType at i_Index.
		//------------------------------------------------------------------------
		CharType& operator [] (int i_Index);
		CharType operator [] (int i_Index) const;

		//------------------------------------------------------------------------
		//	GetString returns a pointer to the beginning of the string.
		//------------------------------------------------------------------------
		const CharType* GetString() const;
		CharType* GetString();

		//------------------------------------------------------------------------
		//	SetLength sets the length of the string in characters (not bytes)
		//------------------------------------------------------------------------
		void SetLength(int i_Length);

		//------------------------------------------------------------------------
		//	GetLength returns the length of the string in characters (not bytes)
		//------------------------------------------------------------------------
		int GetLength() const;

		//------------------------------------------------------------------------
		//	Clear empties the string
		//------------------------------------------------------------------------
		void Clear();

		//------------------------------------------------------------------------
		//	HasSubstring returns true if the string contains the given substring
		//	at some point, exactly.
		//------------------------------------------------------------------------
		bool HasSubString(const itString& i_SubString) const;

		//------------------------------------------------------------------------
		//	InsertCharAt inserts i_Char at i_Index
		//------------------------------------------------------------------------
		void InsertCharAt(int i_Index, CharType i_Char);

		//------------------------------------------------------------------------
		//	RemoveCharAt removes the character at i_Index
		//------------------------------------------------------------------------
		void RemoveCharAt(int i_Index);

		//------------------------------------------------------------------------
		//	ResizeAtNULL resizes the string so that it's length is one less than
		//	the first zero in it, if any.
		//------------------------------------------------------------------------
		void ResizeAtNULL();

		//------------------------------------------------------------------------
		//	StripExtension removes all characters including and after any period
		//	found in the string.
		//------------------------------------------------------------------------
		void StripExtension();

		//------------------------------------------------------------------------
		//	return the extension of the string after the LAST period
		//------------------------------------------------------------------------
		void GetExtension( itString& o_Extension ) const;

		//------------------------------------------------------------------------
		//	return the base of the string before the LAST period
		//------------------------------------------------------------------------
		void GetBase( itString& o_Base ) const;

	private:

		std::vector<CharType> m_String;
};

//------------------------------------------------------------------------
//	operator [] returns the CharType at i_Index.
//------------------------------------------------------------------------
inline itString::CharType& itString::operator [] (int i_Index)
{
	DBG_ASSERT1(0 <= i_Index && i_Index < (int)m_String.size(), "Invalid index #%d", i_Index);

	return m_String[i_Index];
}

inline itString::CharType itString::operator [] (int i_Index) const
{
	DBG_ASSERT1(0 <= i_Index && i_Index < (int)m_String.size(), "Invalid index #%d", i_Index);

	return m_String[i_Index];
}