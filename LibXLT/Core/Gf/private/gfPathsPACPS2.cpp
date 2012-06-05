/****************************************************************************\
**  gfPathsPACPS2.cpp
**
**      gfPathsPACPS2.cpp defines the game paths PAC for windows.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "gfPathsPACPS2.hpp"

#include "fsLocator.hpp"
#include "gfPaths.hpp"

namespace gfPathsPAC
{

//============================================================================
//	InitPaths handles initializing all the predefined game paths
//	
//============================================================================
void InitPaths(std::vector<GamePathList>& o_Vec, const char* i_CDVolumeName)
{
	o_Vec[gfPaths::e_ExePath].resize(1);
	o_Vec[gfPaths::e_ExePath][0].Push("host0:~");	//	ProDG fileserver "home" directory

	o_Vec[gfPaths::e_CDROMPath].resize(1);
	o_Vec[gfPaths::e_CDROMPath][0].Push("cdrom0:");

	//	we skip system temp path and system data path
}

//========================================================================
//	SetCDPath searched drives for i_CDVolumeName and sets the path if found
//========================================================================
void SetCDPath(std::vector<GamePathList>& o_Vec, const char* i_CDVolumeName)
{
}

}
