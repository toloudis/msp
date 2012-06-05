/****************************************************************************\
**	rmanUtil.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "ImportExport/rman/export/private/rmanUtil.hpp"

#include "ImportExport/rman/export/rmanExportData.hpp"

#include "Core/env/envSTLHelpers.hpp"
//#include "Core/fs/fsLocator.hpp"


//--------------------------------------------------------------------
//	LookupTexturePath()
//--------------------------------------------------------------------
fsLocator rmanUtil::LookupTexturePath(std::string i_Tex , rmanGlobalData & io_GlobalData)
{
	for(std::map<fsLocator , std::vector< std::string >>::const_iterator it = io_GlobalData.m_FullTextureMap.begin(); it != io_GlobalData.m_FullTextureMap.end(); ++it)
	{
		std::vector<std::string> objects = it->second;
		
		if ( envSTLHelpers::Contains(objects,i_Tex) )
		{
			return it->first;
		}
	}
	return fsLocator();
}

//--------------------------------------------------------------------
//	LookupGeneratedTexName()
//--------------------------------------------------------------------
std::string rmanUtil::LookupGeneratedTexName(fsLocator i_Tex, rmanGlobalData & io_GlobalData)
{
	for(std::map<fsLocator , std::string >::const_iterator it = io_GlobalData.m_TextureIDs.begin(); it != io_GlobalData.m_TextureIDs.end(); ++it)
	{		
		if ( i_Tex == it->first )
		{
			return it->second;
		}
	}
	return "";
}

//--------------------------------------------------------------------
//	LookupGeneratedTexName()
//--------------------------------------------------------------------
std::string rmanUtil::LookupGeneratedTexName(std::string i_Tex, rmanGlobalData & io_GlobalData)
{
	if ( i_Tex == "" ) return i_Tex;
	fsLocator texturePath = LookupTexturePath(i_Tex,io_GlobalData);
	std::string texName = LookupGeneratedTexName(texturePath,io_GlobalData);
	if ( texName != "" ) texName += ".tex";
	return texName;
}