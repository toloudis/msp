/*****************************************************************************
**  cmraAnimList.hpp
**
**      cmraAnimList enumerates a list of the animation files in the
**	project directory.
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/

#ifdef CMRA_ANIMLIST_HPP
#error cmraAnimList.hpp multiply included
#endif
#define CMRA_ANIMLIST_HPP

#ifndef IT_STRING_HPP
#include "Core/it/itString.hpp"
#endif

class fsysFileList;
class fsLocator;

//============================================================================
//============================================================================
namespace cmraAnimList
{
	//------------------------------------------------------------------------
	//	Init()
	//		i_SystemDirName		- the name of the directory for this system
	//		i_SceneSubDirName		- the sub-dir to look for extensions (data, model,...)
	//------------------------------------------------------------------------
	void Init( const itString& i_SystemDirName, 
			   const itString& i_SceneSubDirName );

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void CleanUp();

	//------------------------------------------------------------------------
	// Set app directory to look for geometry files within/
	//	the post-fix directory will be appended to this directory path.
	//------------------------------------------------------------------------
	void SetAppDirectory(const fsLocator &i_Dir);

	//------------------------------------------------------------------------
	// system directory name
	//------------------------------------------------------------------------
	itString GetSystemDirName();

	//------------------------------------------------------------------------
	// Get list of animation files, returning into given file list
	//------------------------------------------------------------------------
	 void BuildFileList(fsysFileList& o_FileList);
}
