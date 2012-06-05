/*****************************************************************************
**  prjltTextureList.hpp
**
**      prjltTextureList handles enumerating the texture files.
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef PRJLT_TEXTURELIST_HPP
#error prjltTextureList.hpp multiply included
#endif
#define PRJLT_TEXTURELIST_HPP

#ifndef CMM_FILELISTTEMPLATE_HPP
#include "Systems/Common/Templates/cmmFileListTemplate.hpp"
#endif

//============================================================================
// Sets file extensions for this file lister
//============================================================================
class prjltTextureListConfig
{
public:
	static itString GetFileTypeExtensions()
	{
		return itString("dds;bmp;png;tga");
	}
};

//============================================================================
// Enumerates textures in directories
//============================================================================
class prjltTextureList : public cmmFileListTemplate<prjltTextureListConfig>
{
};

