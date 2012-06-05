/*****************************************************************************
**  tasData.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "tasData.hpp"

#include <vector>


namespace
{
	std::vector<std::string> m_List;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void tasData::Clear()
{
	m_List.clear();
}

//------------------------------------------------------------------------
//	AddString - add given string to list
//------------------------------------------------------------------------
void tasData::AddString(const std::string& i_String)
{
	m_List.push_back(i_String);
}

//------------------------------------------------------------------------
//	GetNumStrings - return number of strings in list
//------------------------------------------------------------------------
int tasData::GetNumStrings()
{
	return m_List.size();
}

//------------------------------------------------------------------------
//	GetString - return indexed string as const char*
//------------------------------------------------------------------------
const char* tasData::GetString(int i_Index)
{
	static std::string static_string;

	if (i_Index >= m_List.size())
	{
		static_string.clear();
		return 0;
	}

	static_string = m_List[i_Index];
	return static_string.c_str();
}
