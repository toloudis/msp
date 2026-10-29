/*****************************************************************************
**  fsysFileUtil.hpp
**
**      fsysFileUtil is a set of functions for the fsys directory structure
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#ifdef FSYS_FILEUTIL_HPP
#error fsysFileUtil.hpp multiply included
#endif
#include <string>
#define FSYS_FILEUTIL_HPP


//============================================================================
//============================================================================
class fsLocator;


//============================================================================
//============================================================================
namespace fsysFileUtil
{
	//------------------------------------------------------------------------
	//	GetFilePath - return the first path found that contains the passed
	//	in file.  If the FoundDir size is 0 it wasn't found.
	//	Also returns false if the file wasn't found.
	//
	//	Note: this calls fsDirListUtil::BuildDirectoryList.
	//------------------------------------------------------------------------
	bool GetFilePath(	const std::string& i_SystemDirName, 
						const std::string& i_SceneSubDirName, 
						const std::string& i_FindFile, 
						fsLocator& o_FoundDirAndFile);
}
