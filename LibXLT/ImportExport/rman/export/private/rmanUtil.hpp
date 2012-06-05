/*****************************************************************************\
**	rmanUtil.hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef RMAN_UTIL_HPP
#error rmanUtil.hpp multiply included
#endif
#define RMAN_UTIL_HPP

//============================================================================
//	Forward References
//============================================================================
struct rmanGlobalData;
class fsLocator;

#include <fstream>
#include <map>

//============================================================================
//============================================================================
namespace rmanUtil
{
	//--------------------------------------------------------------------
	//	LookupTexturePath()
	//--------------------------------------------------------------------
	fsLocator LookupTexturePath(std::string i_Tex , rmanGlobalData & io_GlobalData);

	//--------------------------------------------------------------------
	//	LookupGeneratedTexName()
	//--------------------------------------------------------------------
	std::string LookupGeneratedTexName(fsLocator i_Tex, rmanGlobalData & io_GlobalData);

	//--------------------------------------------------------------------
	//	LookupGeneratedTexName()
	//--------------------------------------------------------------------
	std::string LookupGeneratedTexName(std::string i_Tex, rmanGlobalData & io_GlobalData);

};
