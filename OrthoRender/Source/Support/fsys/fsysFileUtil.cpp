/*****************************************************************************
**  fsysFileUtil.cpp
**
**      see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Core/env/envPlatform.hpp"
#include "Support/fsys/fsysFileUtil.hpp"

#include "Support/fsys/fsysDirListUtil.hpp"
#include "Support/mnm/mnmPaths.hpp"

#include "Core/dbg/dbgLog.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsLocator.hpp"
#include "Core/gf/gfFileEnum.hpp"
#include "Core/it/itStringUtil.hpp"


namespace
{

	//----------------------------------------------------------------------------
	//	FindFileEnum is used to find a file within subdirectories
	//----------------------------------------------------------------------------
	class FindFileEnum : public fsFileEnum::EnumTarget
	{
		itString m_Filename;
	public:
		fsLocator m_FullPath;
		bool m_bFound;

		// constructor takes vector to fill
		FindFileEnum(const itString& i_Filename)
			: m_Filename(i_Filename), m_bFound(false)
		{

		}

		virtual bool Notify( const fsLocator& i_Directory, const fsLocator& i_File )
		{
			// Case-insensitive comparison
			if (itStringUtil::Equal(i_File.GetLastName(), m_Filename))
			{
				m_FullPath = i_Directory;
				m_FullPath.Push( i_File );
				m_bFound = true;
				return false; // can stop the enumeration now
			}

			return true;
		}
	};
}


//------------------------------------------------------------------------
//	GetFilePath - return the first path found that contains the passed
//	in file.  If the FoundDir size is 0 it wasn't found.
//	Also returns false if the file wasn't found.
//
//	Note: this calls fsDirListUtil::BuildDirectoryList.
//------------------------------------------------------------------------
bool fsysFileUtil::GetFilePath( const itString& i_SystemDirName, 
								const itString& i_SceneSubDirName, 
								const itString& i_FindFile, 
								fsLocator& o_FoundDirAndFile)
{
	o_FoundDirAndFile.Clear();

	//	find all the paths for this path index and use each one of these as the
	//	base for the full path.
	//
	fsLocator full_dir;
	int numpaths = gfPaths::GetNumPathsInList( mnmPaths::e_DataSceneAndStock );
	for ( int i = 0 ; i < numpaths ; ++i )
	{
		//std::string fdir;
		//fsFileUtil::LocatorToANSIFilename( full_dir, fdir );
		//DBG_LOG2("GetFilePath() %02d - (%s)", i, fdir.c_str() );

		//	generate the list of child directories for this path
		//
		full_dir = gfPaths::GetPath(mnmPaths::e_DataSceneAndStock, i);
		if (i_SystemDirName.GetLength() > 0)
			full_dir.Push( i_SystemDirName );


		FindFileEnum find_target(i_FindFile);
		fsFileEnum::EnumerateFiles(full_dir, find_target, true);

		if (find_target.m_bFound)
		{
			o_FoundDirAndFile = find_target.m_FullPath;
			return true;
		}
	}

	//-------------------------------------------------------------------------------
	// Now do the new Scene style: search a shallow directory under the scene name,
	// either Data or Textures with no use of system or object name.
	numpaths = gfPaths::GetNumPathsInList( mnmPaths::e_DataScene );
	for ( int i = 0 ; i < numpaths ; ++i )
	{
		//	generate the list of child directories for this path
		//
		full_dir = gfPaths::GetPath(mnmPaths::e_DataScene, i);

		// Different sub directory name under Scene (either "Data" or "Textures")
		if (i_SceneSubDirName.GetLength() > 0)
			full_dir.Push( i_SceneSubDirName );

		FindFileEnum find_target(i_FindFile);
		fsFileEnum::EnumerateFiles(full_dir, find_target, true);

		if (find_target.m_bFound)
		{
			o_FoundDirAndFile = find_target.m_FullPath;
			return true;
		}
	}
	return false;
}

