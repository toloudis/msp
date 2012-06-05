/****************************************************************************\
**  itString.cpp
**
**      itString.cpp defines the windows version of the itString class, which is the basic string
**	class for localized text in Terawatt.  Please notice that this string is
**	not necessarily NULL terminated.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Core/it/itString.hpp"

#include <algorithm>
#include <string.h>


//------------------------------------------------------------------------
//	default and copy constructors
//------------------------------------------------------------------------
itString::itString()
{
}

itString::itString(const itString& i_CopyFrom)
:	m_String(i_CopyFrom.m_String)
{
}

//------------------------------------------------------------------------
//	This constructor's argument is a single-byte, NULL terminated "C"
//	string.  It will be converted into a Unicode string.
//------------------------------------------------------------------------
itString::itString(const char* i_CopyFrom)
{
	*this = i_CopyFrom;
}

//------------------------------------------------------------------------
//	This constructor's argument is a unicode, NULL terminated "C"
//	string.  It will be converted into a Unicode string.
//------------------------------------------------------------------------
itString::itString(const CharType* i_CopyFrom)
{
	int size = 0;
	while( i_CopyFrom[size] )
		size++;
	
	m_String.resize(size);
	if (size > 0)
		::memcpy(&(m_String[0]), i_CopyFrom, sizeof(CharType) * size);
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
itString::~itString() throw()
{
}

//------------------------------------------------------------------------
//	operator = copies the input string
//------------------------------------------------------------------------
itString& itString::operator = (const itString& i_CopyFrom)
{
	m_String = i_CopyFrom.m_String;
	return *this;
}

//------------------------------------------------------------------------
//	this operator = takes a single-byte NULL terminated "C" string, and
//	converts it to a Unicode string
//------------------------------------------------------------------------
itString& itString::operator = (const char* i_CopyFrom)
{
	size_t len = ::strlen(i_CopyFrom);
	size_t i;

	m_String.resize(len);

	for( i = 0 ; i < len ; i++ )
		m_String[i] = static_cast<unsigned char>( i_CopyFrom[i] );

	return *this;
}

//------------------------------------------------------------------------
//	this operator = takes a NULL terminated Unicode string.
//------------------------------------------------------------------------
itString& itString::operator = (const CharType* i_CopyFrom)
{
	int size = 0;
	while( i_CopyFrom[size] )
		size++;
	
	m_String.resize(size);

	if (size > 0)
		::memcpy(&(m_String[0]), i_CopyFrom, sizeof(CharType) * size);
	return *this;
}


//------------------------------------------------------------------------
//	This constructor creates the string from a range of characters in
//	the string parameter.  The substring begins at character # i_Begin and
//	lasts for i_Num characters.
//------------------------------------------------------------------------
itString::itString(int i_Begin, int i_Num, const itString& i_String)
{
	int i;
	m_String.resize(i_Num);

	DBG_ASSERT0(i_Num + i_Begin <= i_String.m_String.size(), "Substring out of range");

	for( i = 0 ; i < i_Num ; i++ )
		m_String[i] = i_String.m_String[i+i_Begin];
}

//------------------------------------------------------------------------
//	this operator += concatenates a string
//------------------------------------------------------------------------
void itString::operator += (const itString& i_String)
{
	std::copy(i_String.m_String.begin(), i_String.m_String.end(), std::back_inserter(m_String));	
}

//------------------------------------------------------------------------
//	this operator += concatenates a single character
//------------------------------------------------------------------------
void itString::operator += (CharType i_Char)
{
	m_String.push_back(i_Char);
}

//------------------------------------------------------------------------
//	operator == and != test for equality of strings
//------------------------------------------------------------------------
bool itString::operator == (const itString& i_String) const
{
	return m_String == i_String.m_String;
}
bool itString::operator != (const itString& i_String) const 
{ 
	bool result = (*this == i_String);
	return !result; 
}

//------------------------------------------------------------------------
//	operator < does a lexicographical comparison.
//------------------------------------------------------------------------
bool itString::operator < (const itString& i_String) const
{
	return m_String < i_String.m_String;
}

//------------------------------------------------------------------------
//	operator <= does a lexicographical comparison.
//------------------------------------------------------------------------
bool itString::operator <= (const itString& i_String) const
{
	return m_String <= i_String.m_String;
}

//------------------------------------------------------------------------
//	operator >= does a lexicographical comparison.
//------------------------------------------------------------------------
bool itString::operator >= (const itString& i_String) const
{
	return m_String >= i_String.m_String;
}

//------------------------------------------------------------------------
//	operator > does a lexicographical comparison.
//------------------------------------------------------------------------
bool itString::operator > (const itString& i_String) const
{
	return m_String > i_String.m_String;
}

//------------------------------------------------------------------------
//	GetString returns a pointer to the beginning of the string
//------------------------------------------------------------------------
const itString::CharType* itString::GetString() const
{
	if (m_String.size() > 0)
		return &(m_String[0]);
	else
		return 0;
}

//------------------------------------------------------------------------
//	GetString returns a pointer to the beginning of the string
//------------------------------------------------------------------------
itString::CharType* itString::GetString()
{
	if (m_String.size() > 0)
		return &(m_String[0]);
	else
		return 0;
}

//------------------------------------------------------------------------
//	SetLength sets the length of the string in characters (not bytes)
//------------------------------------------------------------------------
void itString::SetLength(int i_Length)
{
	DBG_ASSERT1(0 <= i_Length, "illegal string length #%d", i_Length);

	m_String.resize(i_Length);
}

//------------------------------------------------------------------------
//	GetLength returns the length of the string in characters (not bytes)
//------------------------------------------------------------------------
int itString::GetLength() const
{
	return int(m_String.size());
}

//------------------------------------------------------------------------
//	Clear empties the string
//------------------------------------------------------------------------
void itString::Clear()
{
	m_String.clear();
}

//------------------------------------------------------------------------
//	HasSubstring returns true if the string contains the given substring
//	at some point, exactly.
//------------------------------------------------------------------------
bool itString::HasSubString(const itString& i_SubString) const
{
	std::vector<CharType>::const_iterator it;
	it = std::search(	m_String.begin(), 
						m_String.end(), 
						i_SubString.m_String.begin(), 
						i_SubString.m_String.end());
	return it != m_String.end();
}

//------------------------------------------------------------------------
//	InsertCharAt inserts i_Char at i_Index
//------------------------------------------------------------------------
void itString::InsertCharAt(int i_Index, CharType i_Char)
{
	DBG_ASSERT1(0 <= i_Index && i_Index <= m_String.size(), "Invalid Index #%d", i_Index);

	m_String.insert(m_String.begin() + i_Index, i_Char);
}

//------------------------------------------------------------------------
//	RemoveCharAt removes the character at i_Index
//------------------------------------------------------------------------
void itString::RemoveCharAt(int i_Index)
{
	DBG_ASSERT1(0 <= i_Index && i_Index < m_String.size(), "Invalid Index #%d", i_Index);

	m_String.erase(m_String.begin() + i_Index);
}

//------------------------------------------------------------------------
//	ResizeAtNULL resizes the string so that it's length is one less than
//	the first zero in it, if any.
//------------------------------------------------------------------------
void itString::ResizeAtNULL()
{
	std::vector<CharType>::iterator it = std::find(m_String.begin(), m_String.end(), 0);
	if( it != m_String.end() )
		m_String.resize(it - m_String.begin());
}

//------------------------------------------------------------------------
//	StripExtension removes all characters including and after any period
//	found in the string.
//------------------------------------------------------------------------
void itString::StripExtension()
{
	// See Effective STL, by Scott Meyers, 
	//   Item 28: "Understand how to use a reverse_iterator's base iterator."
	std::vector<CharType>::reverse_iterator ri = std::find(m_String.rbegin(), m_String.rend(), itString::CharType('.'));
	if ( m_String.rend() != ri )
		m_String.erase((++ri).base(), m_String.end());
}

//------------------------------------------------------------------------
//	return the extension of the string after the LAST period
//------------------------------------------------------------------------
void itString::GetExtension( itString& o_Extension ) const
{
	int i;
	int index = m_String.size();
	for( i = 0 ; i < m_String.size(); i++ )
	{
		if (m_String[i] == '.')
		{
			index = i;
			break;
		}
	}

	if (index != m_String.size())
	{
		o_Extension.Clear();
		for ( i = index+1; i < m_String.size(); ++i )
		{
			o_Extension += m_String[i];
		}
	}

	//std::vector<CharType>::reverse_iterator ri = std::find(m_String.rbegin(), m_String.rend(), itString::CharType('.'));
	//if ( m_String.rend() != ri )
	//{
	//	++ri;	// skip the '.'
	//
	//	o_Extension.Clear();
	//
	//	int i = 0;
	//	while (++ri != m_String.rend())
	//	{
	//		o_Extension[i++] = *ri;
	//	}
	//}
}


//------------------------------------------------------------------------
//	return the base of the string before the LAST period
//------------------------------------------------------------------------
void itString::GetBase( itString& o_Base ) const
{
	int i;
	int index = m_String.size();
	for( i = 0 ; i < m_String.size(); i++ )
	{
		if (m_String[i] == '.')
		{
			index = i;
			break;
		}
	}

	if (index != m_String.size())
	{
		o_Base.Clear();
		for ( i = 0; i < index; ++i )
		{
			o_Base += m_String[i];
		}
	}

	//std::vector<CharType>::reverse_iterator ri = std::find(m_String.rbegin(), m_String.rend(), itString::CharType('.'));
	//std::vector<CharType>::reverse_iterator it = m_String.rbegin();
	//
	//if ( m_String.rend() != ri )
	//{
	//	o_Base.Clear();
	//
	//	int i = 0;
	//	while (it != ri)
	//	{
	//		o_Base[i++] = *it;
	//		++it;
	//	}
	//}
}
