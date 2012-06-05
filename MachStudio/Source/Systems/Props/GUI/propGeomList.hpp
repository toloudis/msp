/*****************************************************************************
**  propGeomList.hpp
**
**      propGeomList holds a list of the prop templates.
**
**	StudioGPU
**	Copyright(C) 2002-5 - All Rights Reserved
\****************************************************************************/
#ifdef PROP_GEOMLIST_HPP
#error propGeomList.hpp multiply included
#endif
#define PROP_GEOMLIST_HPP

#ifndef CMM_FILELISTTEMPLATE_HPP
#include "Systems/Common/Templates/cmmFileListTemplate.hpp"
#endif

//============================================================================
// Sets file extensions for this file lister
//============================================================================
class propGeomListConfig
{
public:
	static itString GetFileTypeExtensions()
	{
		return itString("mhx;jnx;vtx;mx;chx;gxb");
	}
};

//============================================================================
// Enumerates geometry in Props directories
//============================================================================
class propGeomList : public cmmFileListTemplate<propGeomListConfig>
{
};
