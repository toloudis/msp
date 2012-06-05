/*****************************************************************************
**  prtclTextureList.hpp
**
**      prtclTextureList handles enumerating the texture files.
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PRTCL_TEXTURELIST_HPP
#error prtclTextureList.hpp multiply included
#endif
#define PRTCL_TEXTURELIST_HPP

#ifndef CMM_FILELISTTEMPLATE_HPP
#include "Systems/Common/Templates/cmmFileListTemplate.hpp"
#endif


//============================================================================
// Sets file extensions for this file lister
//============================================================================
class prtclTextureListConfig
{
public:
	static itString GetFileTypeExtensions()
	{
		return itString("dds;bmp;png;tga;tuv;uva");
	}
};

//============================================================================
// Enumerates textures in directories
//============================================================================
class prtclTextureList : public cmmFileListTemplate<prtclTextureListConfig>
{
};

