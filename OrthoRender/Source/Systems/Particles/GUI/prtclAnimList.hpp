/*****************************************************************************
**  prtclAnimList.hpp
**
**      prtclAnimList holds a list of the particle templates.
**
**	Extra Large Technology
**	Copyright(C) 2002 - All Rights Reserved
\****************************************************************************/
#ifdef PRTCL_ANIMLIST_HPP
#error prtclAnimList.hpp multiply included
#endif
#define PRTCL_ANIMLIST_HPP

#ifndef CMM_FILELISTTEMPLATE_HPP
#include "Systems/Common/Templates/cmmFileListTemplate.hpp"
#endif

//============================================================================
// Sets file extensions for this file lister
//============================================================================
class prtclAnimListConfig
{
public:
	static itString GetFileTypeExtensions()
	{
		return itString("pta");
	}
};

//============================================================================
// Enumerates particle generators in directories
//============================================================================
class prtclAnimList : public cmmFileListTemplate<prtclAnimListConfig>
{
};
