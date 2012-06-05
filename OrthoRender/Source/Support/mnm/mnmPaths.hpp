/*****************************************************************************
**  mnmCommonPaths.hpp
**
**      mnmPaths has the indices for the paths (gfPaths)
**
**	Extra Large Technology
**	Copyright(C) 2002 - All Rights Reserved
\****************************************************************************/
#ifdef MNM_PATHS_HPP
#error mnmPaths.hpp multiply included
#endif
#define MNM_PATHS_HPP

#ifndef GF_PATHS_HPP
#include "Core/gf/gfPaths.hpp"
#endif


//============================================================================
//	Forward References
//============================================================================
class fsLocator;
class itString;

// TODO: - clean up the path index for machstudio

//============================================================================
//============================================================================
namespace mnmPaths
{
	//----------------------------------------------------------------------------
	//	The paths in the gfPaths can be accessed with these indices.
	//----------------------------------------------------------------------------
	enum PathIndex
	{
		e_ExeArt = gfPaths::e_FirstUserDefPath,
		e_SaveProjectFiles,
		e_SaveShots,
		e_AutoSave,
		e_SaveFootage,
		e_Effects,
		e_Sky,
		e_DataRoot,
		e_DataStock,
		e_DataScene,
		e_DataSceneAndStock,		//
		e_Storyboards,
		e_Configs,
		e_DefaultConfigs,
		e_NumPaths
	};

	//----------------------------------------------------------------------------
	//	SetupPaths build the paths in the gfPaths and the Scene Directory Name
	//----------------------------------------------------------------------------
	void SetupPaths( const fsLocator& i_AppPathLocator, const itString& i_SceneDirName );

	//----------------------------------------------------------------------------
	//	SetupPaths build the paths in the gfPaths
	//----------------------------------------------------------------------------
	void SetupPaths( const fsLocator& i_AppPathLocator );

	//----------------------------------------------------------------------------
	//	SetupDataPaths build the paths in the gfPaths
	//----------------------------------------------------------------------------
	void SetupDataPaths( const itString& i_SceneDirName );

	//----------------------------------------------------------------------------
	//	AppendSubPath appends the subpath string to the locator
	//----------------------------------------------------------------------------
	void AppendSubPath( gfPaths::SubPaths i_SubPathIndex, fsLocator& io_Locator );

	//------------------------------------------------------------------------
	//	CreateProjectDirectories() - create the directories for a new project
	//------------------------------------------------------------------------
	void CreateProjectDirectories( const fsLocator& i_Root );

	//------------------------------------------------------------------------
	//	CreateStockDirectories() - create the Stock directories for a new project
	//------------------------------------------------------------------------
	void CreateStockDirectories( const fsLocator& i_Root, const itString& i_SceneDirName );

	//------------------------------------------------------------------------
	//	CreateSceneDirectories() - create the directories for a new scene
	//------------------------------------------------------------------------
	void CreateSceneDirectories( const fsLocator& i_Root, const itString& i_SceneDirName );

	//------------------------------------------------------------------------
	//	return the name of the object directory from the passed in directory.
	//------------------------------------------------------------------------
	void ExtractObjectDirectoryName( const fsLocator i_FullDir, itString& o_ObjDirName );

	//------------------------------------------------------------------------
	//	Convert the filename to have the correct project path
	//------------------------------------------------------------------------
	void ConvertFilename( fsLocator& io_FileName );
}

