/*****************************************************************************
**  mtrlIconUtil.hpp
**
**      Creates a scene to extract the image data from a material, and
**		exports it as a bmp data into the .mtl file when a material is exported
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#ifdef MTRL_ICON_UTIL_HPP
#error mtrlIconUtil.hpp multiply included
#endif
#define MTRL_ICON_UTIL_HPP


#ifndef ENV_BOOST_HPP
#include "Core/Env/envBoost.hpp"
#endif 
//============================================================================
//============================================================================
class mdlMaterialInfo;
class mtrlScriptObject;
class mtrThumbnailData;

//============================================================================
//============================================================================
namespace mtrlIconUtil
{
	//--------------------------------------------------------------------
	// createMtrlIcon -- Creates a material icon 
	// from a specified material data sturcture, returning image data
	// in the thumbnail data structure.
	//--------------------------------------------------------------------
	void createMtrlIcon(const mdlMaterialInfo& i_MatInfo, 
						mtrThumbnailData &o_Thumbnail);

	//--------------------------------------------------------------------
	// cleanUpTemplate -- cleanup the model template used to load the gxb sphere
	//--------------------------------------------------------------------
	void cleanUpTemplate();

};
