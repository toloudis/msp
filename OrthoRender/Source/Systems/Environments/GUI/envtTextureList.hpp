/*****************************************************************************
**  envtTextureList.hpp
**
**      envtTextureList handles enumerating the texture files.
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef ENVT_TEXTURELIST_HPP
#error envtTextureList.hpp multiply included
#endif
#define ENVT_TEXTURELIST_HPP

#ifndef CMM_FILELISTTEMPLATE_HPP
#include "Systems/Common/Templates/cmmFileListTemplate.hpp"
#endif

//============================================================================
// Sets file extensions for this file lister
//============================================================================
class envtTextureListConfig
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
class envtTextureList : public cmmFileListTemplate<envtTextureListConfig>
{
};

