/*****************************************************************************
**  billGeomList.cpp
**
**      billGeomList manages directories of textures for use 
**	in the billboards.
**
**	StudioGPU
**	Copyright(C) 2002-5 - All Rights Reserved
\****************************************************************************/

#include "Systems/Billboard/GUI/billGeomList.hpp"

#include "Core/fs/fsFileUtil.hpp"
#include "Support/fsys/fsysDirListUtil.hpp"
#include "Support/mnm/mnmPaths.hpp"

//------------------------------------------------------------------------
// Expand from filename into full locator searching in directory tree.
// Returns true if found.
//------------------------------------------------------------------------
bool billGeomList::FindFile(const itString& i_Filename, fsLocator& o_Locator)
{
	fsysFileList file_list;
	billGeomList::BuildFileList(file_list);
	file_list.GetFilePath(i_Filename, o_Locator);
	o_Locator.Push(i_Filename);
	return true;

	//fsLocator tex_dir;
	//int numpaths = gfPaths::GetNumPathsInList( mnmPaths::e_DataSceneAndStock );
	//for ( int i = 0 ; i < numpaths ; ++i )
	//{
	//	//	generate the list of child directories for this path
	//	//
	//	tex_dir = gfPaths::GetPath(mnmPaths::e_DataSceneAndStock, i);
	//	tex_dir.Push( billGeomList::GetSystemDirName() );

	//	//	for each child directory append the data directory name, 
	//	//	get all the files of the appropriate extension, and add them
	//	//	to the list.
	//	//
	//	std::vector<itString> dirs = fsysDirListUtil::BuildDirectoryList( tex_dir );
	//	int numdirs = dirs.size();
	//	for ( int j = 0 ; j < numdirs ; ++j )
	//	{
	//		tex_dir.Push( dirs[j] );
	//		tex_dir.Push( gfPaths::GetSubPath(gfPaths::e_Textures) );
	//		tex_dir.Push( i_Filename );

	//		//	debug only
	//		//std::string tempstr;
	//		//fsFileUtil::LocatorToANSIFilename(tex_dir, tempstr);
	//		//DBG_LOG3( "%d-%d - %s", i, j, tempstr.c_str() );

	//		if ( fsFileUtil::FileExists( tex_dir ) )
	//		{
	//			o_Locator = tex_dir;
	//			return true;
	//		}

	//		tex_dir.Pop();
	//		tex_dir.Pop();
	//		tex_dir.Pop();
	//	}
	//}
	//return false;
}

