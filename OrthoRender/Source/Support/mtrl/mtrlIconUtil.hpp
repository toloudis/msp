/*****************************************************************************
**  mtrlIconUtil.hpp
**
**      Sets up operations to create an image to act as an icon based on the properties of a material file
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#ifdef MTRL_ICON_UTIL_HPP
#error mtrlIconUtil.hpp multiply included
#endif
#define MTRL_ICON_UTIL_HPP

//============================================================================
//============================================================================
class fsLocator;
class mdlMaterialInfo;

//============================================================================
//============================================================================
namespace mtrlIconUtil
{
	//--------------------------------------------------------------------
	// createMtrlIcon -- Creates a material Icon from a specified material (mtl) file
	//--------------------------------------------------------------------
	void createMtrlIcon(const mdlMaterialInfo i_matInfo);

};