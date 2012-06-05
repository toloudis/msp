/*****************************************************************************
**  setsGeomList.hpp
**
**      setsGeomList holds a list of the set item templates.
**
**	Extra Large Technology
**	Copyright(C) 2002-5 - All Rights Reserved
\****************************************************************************/
#ifdef SETS_GEOMLIST_HPP
#error setsGeomList.hpp multiply included
#endif
#define SETS_GEOMLIST_HPP

#ifndef CMM_FILELISTTEMPLATE_HPP
#include "Systems/Common/Templates/cmmFileListTemplate.hpp"
#endif

//============================================================================
// Sets file extensions for this file lister
//============================================================================
class setsGeomListConfig
{
public:
	static itString GetFileTypeExtensions()
	{
		return itString("mx");
	}
};

//============================================================================
// Enumerates geometry in Sets directories
//============================================================================
class setsGeomList : public cmmFileListTemplate<setsGeomListConfig>
{
};
