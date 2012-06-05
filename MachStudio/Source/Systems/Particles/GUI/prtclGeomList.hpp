/*****************************************************************************
**  prtclGeomList.hpp
**
**      prtclGeomList holds a list of the particle templates.
**
**	StudioGPU
**	Copyright(C) 2002 - All Rights Reserved
\****************************************************************************/
#ifdef PRTCL_GEOMLIST_HPP
#error prtclGeomList.hpp multiply included
#endif
#define PRTCL_GEOMLIST_HPP

#ifndef CMM_FILELISTTEMPLATE_HPP
#include "Systems/Common/Templates/cmmFileListTemplate.hpp"
#endif

//============================================================================
// Sets file extensions for this file lister
//============================================================================
class prtclGeomListConfig
{
public:
	static itString GetFileTypeExtensions()
	{
		return itString("tpr");
	}
};

//============================================================================
// Enumerates particle generators in directories
//============================================================================
class prtclGeomList : public cmmFileListTemplate<prtclGeomListConfig>
{
};
