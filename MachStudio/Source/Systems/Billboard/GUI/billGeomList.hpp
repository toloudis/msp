/*****************************************************************************
**  billGeomList.hpp
**
**      billGeomList manages directories of textures for use 
**	in the billboards.
**
**	StudioGPU
**	Copyright(C) 2002-5 - All Rights Reserved
\****************************************************************************/

#ifdef BILL_GEOMLIST_HPP
#error billGeomList.hpp multiply included
#endif
#define BILL_GEOMLIST_HPP

#ifndef CMM_FILELISTTEMPLATE_HPP
#include "Systems/Common/Templates/cmmFileListTemplate.hpp"
#endif

//============================================================================
// Sets file extensions for this file lister
//============================================================================
class billGeomListConfig
{
public:
	static itString GetFileTypeExtensions()
	{
		return itString("dds;bmp;png;tga"); // scm,nmp,rmp,tuv
	}
};

//============================================================================
// Enumerates textures in directories
//============================================================================
class billGeomList : public cmmFileListTemplate<billGeomListConfig>
{
public:
	//------------------------------------------------------------------------
	// Expand from filename into full locator searching in directory tree.
	// Returns true if found.
	//------------------------------------------------------------------------
	 static bool FindFile(const itString& i_Filename, fsLocator& o_Locator);
};

