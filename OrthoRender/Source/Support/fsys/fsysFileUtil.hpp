/*****************************************************************************
**  fsysFileUtil.hpp
**
**      fsysFileUtil is a set of functions for the fsys directory structure
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#ifdef FSYS_FILEUTIL_HPP
#error fsysFileUtil.hpp multiply included
#endif
#define FSYS_FILEUTIL_HPP


//============================================================================
//============================================================================
class fsLocator;
class itString;


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
	bool GetFilePath(	const itString& i_SystemDirName, 
						const itString& i_SceneSubDirName, 
						const itString& i_FindFile, 
						fsLocator& o_FoundDirAndFile);
}
