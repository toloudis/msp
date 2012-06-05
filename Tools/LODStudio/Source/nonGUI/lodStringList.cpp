/*****************************************************************************
**  lodStringList.cpp
**
**      Encapsulates std::vector of std::string for use
**	by MFC classes.
**
**	Extra Large Technology
**	Copyright(C) 2002 - All Rights Reserved
\****************************************************************************/

#include "lodStringList.hpp"


namespace
{

}	// end of namespace

//========================================================================
//========================================================================
lodStringList::lodStringList()
{

}

//========================================================================
//========================================================================
lodStringList::~lodStringList()
{

}

//========================================================================
//	AddString - add given string to list
//========================================================================
void lodStringList::AddString(const std::string& i_String)
{
	m_StringList.push_back(i_String);
}

//========================================================================
//	GetNumStrings - return number of strings in list
//========================================================================
int lodStringList::GetNumStrings() const
{
	return m_StringList.size();
}

//========================================================================
//	GetString - return indexed string as const char*
//		The pointer should not be deleted and will be valid
//		until next call to this function.
//========================================================================
const char* lodStringList::GetString(int i_Index) const
{
	static std::string static_string;
	static_string = m_StringList[i_Index];
	return static_string.c_str();
}
