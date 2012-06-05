/*****************************************************************************
**  chtrAnimList.hpp
**
**      chtrAnimList enumerates a list of the animation files in the
**	project directory.
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/

#ifdef CHTR_ANIMLIST_HPP
#error chtrAnimList.hpp multiply included
#endif
#define CHTR_ANIMLIST_HPP

#ifndef IT_STRING_HPP
#include "Core/it/itString.hpp"
#endif

class fsysFileList;
class fsLocator;


//============================================================================
//============================================================================
namespace chtrAnimList
{
	//------------------------------------------------------------------------
	//	Init()
	//		i_SystemDirName		- the name of the directory for this system
	//		i_SceneSubDirName		- the sub-dir to look for extensions (data, model,...)
	//------------------------------------------------------------------------
	void Init( const itString& i_SystemDirName, const itString& i_SceneSubDirName );

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
	// Get list of animation files, returning into given file list.
	// Pass in directory of the character file (.chd or .chx) in order
	// to locate the animations for this character only. 
	// The directory should be the result of chtrScriptObject::GetDirectory().
	//------------------------------------------------------------------------
	 void BuildFileList(fsysFileList& o_FileList, 
						const fsLocator& i_CharacterDirectory);

	//------------------------------------------------------------------------
	// GetAnimationLocator - get locator to full path to animation
	//	file with given filename for the character that was loaded
	//	from the given directory.
	//------------------------------------------------------------------------
	fsLocator GetAnimationLocator(const itString& i_Filename, 
								  const fsLocator& i_CharacterDirectory);
}
