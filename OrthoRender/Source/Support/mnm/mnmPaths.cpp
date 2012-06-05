/*****************************************************************************
**  mnmPaths.cpp
**
**      see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2002 - All Rights Reserved
\****************************************************************************/

#include "Support/mnm/mnmPaths.hpp"

#include "Core/dbg/dbgLog.hpp"
#include "Tool/doc/docSingleTypeMgr.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Graphics/mtr/mtrMaterialUtil.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
namespace
{
	static const char* c_SUBDIRNAME_STOCK	= "Stock";
	static const char* c_SUBDIRNAME_GENERAL = "General";

	//------------------------------------------------------------------------
	//	CreateSubDirectories() - create the sub directories for a dir
	//------------------------------------------------------------------------
	void CreateSubDirectories( fsLocator& i_RootPath )
	{
		i_RootPath.Push( gfPaths::GetSubPath(gfPaths::e_Data) );
		if ( !fsFileUtil::DirectoryExists( i_RootPath ) )
			fsFileUtil::CreateDirectory( i_RootPath );
		i_RootPath.Pop();

		i_RootPath.Push( gfPaths::GetSubPath(gfPaths::e_Textures) );
		if ( !fsFileUtil::DirectoryExists( i_RootPath ) )
			fsFileUtil::CreateDirectory( i_RootPath );
		i_RootPath.Pop();

		// Not créating Models or Sounds directories anymore
		//i_RootPath.Push( gfPaths::GetSubPath(gfPaths::e_Models) );
		//if ( !fsFileUtil::DirectoryExists( i_RootPath ) )
		//	fsFileUtil::CreateDirectory( i_RootPath );
		//i_RootPath.Pop();

		//i_RootPath.Push( gfPaths::GetSubPath(gfPaths::e_Sounds) );
		//if ( !fsFileUtil::DirectoryExists( i_RootPath ) )
		//	fsFileUtil::CreateDirectory( i_RootPath );
		//i_RootPath.Pop();
	}

	//------------------------------------------------------------------------
	//	CreateSystemDirectory() - create the system directories for a dir
	//------------------------------------------------------------------------
	void CreateSystemDirectory( fsLocator& i_RootDataPath, std::string i_SystemDirName )
	{
		i_RootDataPath.Push( i_SystemDirName.c_str() );
		if ( !fsFileUtil::DirectoryExists( i_RootDataPath ) )
			fsFileUtil::CreateDirectory( i_RootDataPath );

		i_RootDataPath.Push( c_SUBDIRNAME_GENERAL );
		if ( !fsFileUtil::DirectoryExists( i_RootDataPath ) )
			fsFileUtil::CreateDirectory( i_RootDataPath );

		CreateSubDirectories( i_RootDataPath );

		i_RootDataPath.Pop();
		i_RootDataPath.Pop();
	}
}

//----------------------------------------------------------------------------
//	SetupPaths build the paths in the gfPaths
//----------------------------------------------------------------------------
void mnmPaths::SetupPaths( const fsLocator& i_AppPathLocator )
{
	itString scene_name( c_SUBDIRNAME_STOCK );
	SetupPaths( i_AppPathLocator, scene_name );
}

//----------------------------------------------------------------------------
//	SetupPaths build the paths in the gfPaths and the Scene Directory Name
//----------------------------------------------------------------------------
void mnmPaths::SetupPaths( const fsLocator& i_AppPathLocator, const itString& i_SceneDirName )
{
	DBG_ASSERT0( i_AppPathLocator.GetNumNames() > 0, "App Path has nothing in it" );

	itString scene_dir_name(i_SceneDirName);

	//	if the scene name is zero then just use the stock folders
	//
	if ( i_SceneDirName.GetLength() == 0 )
	{
		scene_dir_name = itString(c_SUBDIRNAME_STOCK);
	}

	fsLocator locator;
	//std::string dir;
	//fsFileUtil::LocatorToANSIFilename( gfPaths::GetPath(gfPaths::e_ExePath), dir );
	//DBG_LOG1( "EXE ROOT directory = [%s]", dir.c_str() );

	//
	//	based on executable path
	//
	locator = gfPaths::GetPath(gfPaths::e_ExePath);
	locator.Push("Data");
	locator.Push("Icons");
	gfPaths::SetPath(mnmPaths::e_ExeArt, locator);

	// project files	($EXE_PATH/Projects)
	locator.Clear();
	locator.Push(gfPaths::GetPath(gfPaths::e_ExePath));
	locator.Push("Projects");
	gfPaths::SetPath(mnmPaths::e_SaveProjectFiles, locator);

	// config files	($EXE_PATH/Configs)
	locator.Clear();
	locator.Push(gfPaths::GetPath(gfPaths::e_ExePath));
	locator.Push("Configs");
	gfPaths::SetPath(mnmPaths::e_Configs, locator);

	// DEFAULT config files	($EXE_PATH/Data/Configs)
	locator.Clear();
	locator.Push(gfPaths::GetPath(gfPaths::e_ExePath));
	locator.Push("Data");
	locator.Push("Configs");
	gfPaths::SetPath(mnmPaths::e_DefaultConfigs, locator);

	//
	//	Set the app path
	//
	gfPaths::SetAppPath( i_AppPathLocator );

	//fsFileUtil::LocatorToANSIFilename( i_AppPathLocator, dir );
	//DBG_LOG1( "APP ROOT directory = [%s]", dir.c_str() );

	//
	// set up the paths
	//

	//	footage
	locator.Clear();
	locator.Push(gfPaths::GetAppPath());
	locator.Push("Footage");
	locator.Push(scene_dir_name);
	gfPaths::SetPath(mnmPaths::e_SaveFootage, locator);

	//fsFileUtil::LocatorToANSIFilename( locator, dir );
	//DBG_LOG1( "FOOTAGE  directory = [%s]", dir.c_str() );

	//	shot save files
	locator.Clear();
	locator.Push( gfPaths::GetAppPath() );
	locator.Push("Shots");
	locator.Push(scene_dir_name);
	gfPaths::SetPath(mnmPaths::e_SaveShots, locator);

	// auto save backups go in here.
	locator.Push("BackUps");
	gfPaths::SetPath(mnmPaths::e_AutoSave, locator);

	//	set the data "root"
	locator.Clear();
	locator.Push( gfPaths::GetAppPath() );
	locator.Push("Data");
	gfPaths::SetPath(mnmPaths::e_DataRoot, locator);

	//fsFileUtil::LocatorToANSIFilename( locator, dir );
	//DBG_LOG1( "DATA ROOT directory = [%s]", dir.c_str() );

	//	set the data paths
	//
	SetupDataPaths( scene_dir_name );

	//
	// All non-props (we do all props in one swoop below)
	//

	//	Effects
	locator.Clear();
	locator.Push( gfPaths::GetPath(mnmPaths::e_DataRoot) );
	locator.Push(c_SUBDIRNAME_STOCK);	// FIX: - effects needs to be handled like any other system
	locator.Push( "Effects" );
	locator.Push(c_SUBDIRNAME_GENERAL);
	gfPaths::SetPath(mnmPaths::e_Effects, locator);

	//	Sky
	locator.Clear();
	locator.Push( gfPaths::GetPath(mnmPaths::e_DataRoot) );
	locator.Push(scene_dir_name);
	locator.Push("Sky");
	gfPaths::SetPath(mnmPaths::e_Sky, locator);

	//	Storyboards
	locator.Clear();
	locator.Push( gfPaths::GetPath(mnmPaths::e_DataRoot) );
	locator.Push(scene_dir_name);
	locator.Push("Storyboards");
	gfPaths::SetPath(mnmPaths::e_Storyboards, locator);
	locator.Pop();

	// Material library
	locator.Clear();
	locator.Push( gfPaths::GetPath(mnmPaths::e_DataStock) );
	locator.Push("Materials");
	gfPaths::SetMaterialLibraryPath(locator);

	docSingleTypeMgr::SetAppDirectory( i_AppPathLocator );
}

//----------------------------------------------------------------------------
//	SetupDataPaths build the paths in the gfPaths
//----------------------------------------------------------------------------
void mnmPaths::SetupDataPaths( const itString& i_SceneDirName )
{
	fsLocator locator;

	locator.Clear();
	locator.Push( gfPaths::GetPath(mnmPaths::e_DataRoot) );
	locator.Push(c_SUBDIRNAME_STOCK);
	gfPaths::SetPath(mnmPaths::e_DataStock, locator);			// initially set all of the paths to stock
	gfPaths::SetPath(mnmPaths::e_DataScene, locator);
	gfPaths::SetPath(mnmPaths::e_DataSceneAndStock, locator);

	if ( i_SceneDirName != itString(c_SUBDIRNAME_STOCK) )
	{
		locator.Clear();
		locator.Push( gfPaths::GetPath(mnmPaths::e_DataRoot) );
		locator.Push( i_SceneDirName );
		gfPaths::SetPath(mnmPaths::e_DataScene, locator);

		gfPaths::SetPath(mnmPaths::e_DataSceneAndStock, locator);
		locator.Pop();
		locator.Push(c_SUBDIRNAME_STOCK);
		gfPaths::AddPath(mnmPaths::e_DataSceneAndStock, locator);	// add it to this one AFTER the scene one
	}
}

//----------------------------------------------------------------------------
//	AppendSubPath appends the subpath string to the locator
//----------------------------------------------------------------------------
void mnmPaths::AppendSubPath( gfPaths::SubPaths i_SubPathIndex, fsLocator& io_Locator )
{
	io_Locator.Push( gfPaths::GetSubPath(i_SubPathIndex) );
}

//------------------------------------------------------------------------
//	CreateProjectDirectories() - create the directories for a new project
//------------------------------------------------------------------------
void mnmPaths::CreateProjectDirectories( const fsLocator& i_Root )
{
	fsLocator rootpath( i_Root );

	//	root
	if ( !fsFileUtil::DirectoryExists( rootpath ) )
		fsFileUtil::CreateDirectory( rootpath );

	CreateStockDirectories( rootpath, itString(c_SUBDIRNAME_STOCK) );
}


//------------------------------------------------------------------------
//	CreateStockDirectories() - create the Stock directories for a new project
//------------------------------------------------------------------------
void mnmPaths::CreateStockDirectories( const fsLocator& i_Root, const itString& i_SceneDirName )
{
	fsLocator rootpath( i_Root );

	//	Footage
	rootpath.Push( "Footage" );
	if ( !fsFileUtil::DirectoryExists( rootpath ) )
		fsFileUtil::CreateDirectory( rootpath );

	//	for footage, don't create all the directories since the rendering of
	//	scenes will create the directories.  Having all the directories isn't
	//	necessary (and is confusing).
	//
	//rootpath.Push( i_SceneDirName );
	//if ( !fsFileUtil::DirectoryExists( rootpath ) )
	//	fsFileUtil::CreateDirectory( rootpath );
	//rootpath.Pop();
	rootpath.Pop();

	//	Shots
	rootpath.Push( "Shots" );
	if ( !fsFileUtil::DirectoryExists( rootpath ) )
		fsFileUtil::CreateDirectory( rootpath );
	rootpath.Push( i_SceneDirName );
	if ( !fsFileUtil::DirectoryExists( rootpath ) )
		fsFileUtil::CreateDirectory( rootpath );
	rootpath.Push( "BackUps" );
	if ( !fsFileUtil::DirectoryExists( rootpath ) )
		fsFileUtil::CreateDirectory( rootpath );
	rootpath.Pop();
	rootpath.Pop();
	rootpath.Pop();

	// Data
	rootpath.Push( "Data" );
	if ( !fsFileUtil::DirectoryExists( rootpath ) )
		fsFileUtil::CreateDirectory( rootpath );

	// Create Stock directory
	rootpath.Push( i_SceneDirName );
	if ( !fsFileUtil::DirectoryExists( rootpath ) )
		fsFileUtil::CreateDirectory( rootpath );

	//
	// TODO: - have each system generically polled to create their directories
	//	maybe expose the function CreateSystemDirectory()?
	//

	// Sounds 
	rootpath.Push( "Sounds" );
	if ( !fsFileUtil::DirectoryExists( rootpath ) )
		fsFileUtil::CreateDirectory( rootpath );
	rootpath.Pop();

	//	Materials
	rootpath.Push("Materials");
	if ( !fsFileUtil::DirectoryExists( rootpath ) )
		fsFileUtil::CreateDirectory( rootpath );
	rootpath.Pop();

	//	Effects
	//CreateSystemDirectory( rootpath, "Effects" );
	rootpath.Push("Effects");
	if ( !fsFileUtil::DirectoryExists( rootpath ) )
		fsFileUtil::CreateDirectory( rootpath );
	rootpath.Pop();

	//	Props
	CreateSystemDirectory( rootpath, "Props" );

	//	Characters
	CreateSystemDirectory( rootpath, "Characters" );

	//	Sets
	//CreateSystemDirectory( rootpath, "Sets" );

	//	Storyboards + Sounds sub-folder
	rootpath.Push("Storyboards");
	if ( !fsFileUtil::DirectoryExists( rootpath ) )
		fsFileUtil::CreateDirectory( rootpath );
	//rootpath.Push("Sounds");
	//if ( !fsFileUtil::DirectoryExists( rootpath ) )
	//	fsFileUtil::CreateDirectory( rootpath );
}

//------------------------------------------------------------------------
//	CreateSceneDirectories() - create the directories for a new scene
//------------------------------------------------------------------------
void mnmPaths::CreateSceneDirectories( const fsLocator& i_Root, const itString& i_SceneDirName )
{
	fsLocator rootpath( i_Root );

	//	Shots
	rootpath.Push( "Shots" );
	if ( !fsFileUtil::DirectoryExists( rootpath ) )
		fsFileUtil::CreateDirectory( rootpath );
	rootpath.Push( i_SceneDirName );
	if ( !fsFileUtil::DirectoryExists( rootpath ) )
		fsFileUtil::CreateDirectory( rootpath );
	rootpath.Push( "BackUps" );
	if ( !fsFileUtil::DirectoryExists( rootpath ) )
		fsFileUtil::CreateDirectory( rootpath );
	rootpath.Pop();
	rootpath.Pop();
	rootpath.Pop();

	// Create scene directory under Data
	rootpath.Push( "Data" );
	if ( !fsFileUtil::DirectoryExists( rootpath ) )
		fsFileUtil::CreateDirectory( rootpath );
	rootpath.Push( i_SceneDirName );
	if ( !fsFileUtil::DirectoryExists( rootpath ) )
		fsFileUtil::CreateDirectory( rootpath );

	// Add just Data and Textures to the Scene directory
	CreateSubDirectories(rootpath);

}

//------------------------------------------------------------------------
//	return the name of the object directory from the passed in directory.
//------------------------------------------------------------------------
void mnmPaths::ExtractObjectDirectoryName( const fsLocator i_FullDir, itString& o_ObjDirName )
{
	fsLocator dir( i_FullDir );

	dir.RemoveBefore( itString("Data") );

	//Note: this 2 is alright to hard-code since it fits in with the path
	//	methodology.  After "Data" there is the "scene name" directory,
	//	then the "system" directory, then the "object directory" (0,1,2).
	//
	o_ObjDirName = dir.GetName(2);
}

//------------------------------------------------------------------------
//	Convert the filename to have the correct project path
//------------------------------------------------------------------------
void mnmPaths::ConvertFilename( fsLocator& io_FileName )
{
	std::string dir;
	
	fsFileUtil::LocatorToANSIFilename( io_FileName, dir );
	DBG_LOG1( "original directory = [%s]", dir.c_str() );

	fsFileUtil::LocatorToANSIFilename( gfPaths::GetPath(gfPaths::e_AppPath), dir );
	DBG_LOG1( "     app directory = [%s]", dir.c_str() );

	fsFileUtil::LocatorToANSIFilename( io_FileName, dir );
	DBG_LOG1( "     new directory = [%s]", dir.c_str() );
}

