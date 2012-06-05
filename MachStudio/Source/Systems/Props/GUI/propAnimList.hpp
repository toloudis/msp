/*****************************************************************************
**  propAnimList.hpp
**
**      propAnimList enumerates a list of the animation files in the
**	project directory.
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#ifdef PROP_ANIMLIST_HPP
#error propAnimList.hpp multiply included
#endif
#define PROP_ANIMLIST_HPP

#ifndef CMM_FILELISTTEMPLATE_HPP
#include "Systems/Common/Templates/cmmFileListTemplate.hpp"
#endif

//============================================================================
// Sets file extensions for this file lister
//============================================================================
class propAnimListConfig
{
public:
	static itString GetFileTypeExtensions()
	{
		return itString("mha;jna;vta;cha;gab");
	}
};

//============================================================================
// Enumerates animations in Props directories
//============================================================================
class propAnimList : public cmmFileListTemplate<propAnimListConfig>
{
};
