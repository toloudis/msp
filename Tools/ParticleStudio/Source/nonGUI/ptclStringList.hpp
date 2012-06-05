/*****************************************************************************
**  ptclStringList.hpp
**
**      Encapsulates std::vector of std::string for use
**	by MFC classes.
**
**	Extra Large Technology
**	Copyright(C) 2002 - All Rights Reserved
\****************************************************************************/

#ifdef PTCL_STRINGLIST_HPP
#error ptclStringList.hpp multiply included
#endif
#define PTCL_STRINGLIST_HPP

#ifndef ENV_PLATFORM_HPP
#include "Core/env/envPlatform.hpp"
#endif

#include <string>
#include <vector>

class ptclStringList
{
public:
	//========================================================================
	//========================================================================
	ptclStringList();

	//========================================================================
	//========================================================================
	~ptclStringList();

	//========================================================================
	//	AddString - add given string to list
	//========================================================================
	void AddString(const std::string& i_String);

	//========================================================================
	//	GetNumStrings - return number of strings in list
	//========================================================================
	int GetNumStrings() const;

	//========================================================================
	//	GetString - return indexed string as const char*
	//		The pointer should not be deleted and will be valid
	//		until next call to this function.
	//========================================================================
	const char* GetString(int i_Index) const;

private:
	std::vector<std::string> m_StringList;
};

