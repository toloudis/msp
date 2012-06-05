/****************************************************************************\
**  gfPathsPACPS2.hpp
**
**      gfPathsPACPS2.hpp defines the game paths PAC for windows.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef GF_PATHSPACPS2_HPP
#error gfPathsPACPS2.hpp multiply included
#endif
#define GF_PATHSPACPS2_HPP

#include <vector>

//============================================================================
//	Forward references
//============================================================================
class fsLocator;

namespace gfPathsPAC
{
	typedef std::vector<fsLocator> GamePathList;

	//========================================================================
	//========================================================================
	void InitPaths(std::vector<GamePathList>& o_Vec, const char* i_CDVolumeName);

	//========================================================================
	//	SetCDPath searched drives for i_CDVolumeName and sets the path if found
	//========================================================================
	void SetCDPath(std::vector<GamePathList>& o_Vec, const char* i_CDVolumeName);

}