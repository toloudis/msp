/****************************************************************************\
**  gfPathsPACXbox.hpp
**
**      gfPathsPACXbox.hpp defines the game paths PAC for Xbox.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef GF_PATHSPACXBOX_HPP
#error gfPathsPACXbox.hpp multiply included
#endif
#define GF_PATHSPACXbox_HPP

#include <vector>

//============================================================================
//	Forward references
//============================================================================
class fsLocator;

namespace gfPathsPAC
{
	typedef std::vector<fsLocator> GamePathList;
	void InitPaths(std::vector<GamePathList>& o_Vec, const char* i_CDVolumeName);
	bool FindFile( const char* pPath, const char* pFilename );

	//============================================================================
	//	returns true if the a CD with the given name is present.  This function
	//	can be called without initializing the gf package.
	//============================================================================
	bool CDInDrive(const char* i_CDVolumeName);

	//========================================================================
	//	SetCDPath searched drives for i_CDVolumeName and sets the path if found
	//========================================================================
	void SetCDPath(std::vector<GamePathList>& o_Vec, const char* i_CDVolumeName);

}