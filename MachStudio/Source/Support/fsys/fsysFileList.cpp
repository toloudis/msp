/*****************************************************************************
**  fsysFileList.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Support/fsys/fsysFileList.hpp"

#include "Support/fsys/fsysDirListUtil.hpp"
#include "Support/mnm/mnmPaths.hpp"

#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/gf/gfFileEnum.hpp"
#include "Core/it/itStringUtil.hpp"

#include <algorithm>


//============================================================================
//============================================================================
namespace
{
	struct filename_search
	{
		inline filename_search(const itString& i_Name) : m_Name(i_Name) {};
		inline bool operator () ( fsysFileInfo& i_Info )
		{
			return (m_Name == i_Info.GetFilename());
		}
		itString m_Name;
	};
	struct filename_sort
	{
		inline bool operator()(fsysFileInfo& lhs, fsysFileInfo& rhs) 
		{ 
			return itStringUtil::LessThan(lhs.GetFilename(),rhs.GetFilename()); 
		} 
	};
}


//------------------------------------------------------------------------
//------------------------------------------------------------------------
fsysFileList::fsysFileList()
:	m_bAllowDuplicateFilenames(false),
	m_bSortList(true),
	m_bSearchSubDirectories(true)
{
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
fsysFileList::~fsysFileList()
{
	m_FileList.clear();
}

//------------------------------------------------------------------------
//	BuildFileList - build the file list using all the folders under
//	stock and scene folders for the specific system dir.  this will
//	also get all of the sub-dirs under the system dir.
//
//	Note: it is assumed that the system directory name has been set
//	already (like on the initialization of the system.)
//------------------------------------------------------------------------
void fsysFileList::BuildFileList()
{
	m_FileList.clear();

	this->AppendFileList();
}

//------------------------------------------------------------------------
// A variation of BuildFileList that does not clear out the
//	previous file list
//------------------------------------------------------------------------
void fsysFileList::AppendFileList()
{
	//DBG_LOG( "------------------file list start-------------------" );

	//	find all the paths for this path index and use each one of these as the
	//	base for the full path.
	//
	fsLocator full_dir;
	int numpaths = gfPaths::GetNumPathsInList( mnmPaths::e_DataSceneAndStock );
	for ( int i = 0 ; i < numpaths ; ++i )
	{
		//	generate the list of child directories for this path
		//
		full_dir = gfPaths::GetPath(mnmPaths::e_DataSceneAndStock, i);
		full_dir.Push( m_SystemDirName );

		// This extra level of directories represents the object name or
		// "General". In the new style, this directory level is rarely used.
		// We can read all the old formats by just using the SystemDirName
		// and recursing on all subdirectories. So, unless the object directory
		// name is specified, keep it simple and recursive.
		if (m_ObjectDirName.GetLength() == 0)
		{
			// Just do the system name directory
			BuildFileListFromDirectory( full_dir );
		}
		else
		{
			// This test is redundant now, but I leave it as reference in case
			// we need to roll back the changes for some reason.
			std::vector<itString> dirs = (m_ObjectDirName.GetLength() > 0) ?
				fsysDirListUtil::BuildDirectoryList( full_dir, m_ObjectDirName ) :
				fsysDirListUtil::BuildDirectoryList( full_dir );

			//std::string fd;											// Debug only
			//fsFileUtil::LocatorToANSIFilename( full_dir, fd );
			//DBG_LOG2("%02d %s", i, fd.c_str() );

			//	for each child directory append the data directory name, 
			//	get all the files of the appropriate extension, and add them
			//	to the list.
			//
			int numdirs = dirs.size();
			bool use_subdirname = (m_SubDirName.GetLength() > 0);
			for ( int j = 0 ; j < numdirs ; ++j )
			{
				full_dir.Push( dirs[j] );
				if (use_subdirname)
					full_dir.Push( m_SubDirName );

				//fsFileUtil::LocatorToANSIFilename( full_dir, fd );
				//DBG_LOG2("   %02d %s", j, fd.c_str() );

				//	get the files for this directory
				//
				BuildFileListFromDirectory( full_dir );

				if (use_subdirname)
					full_dir.Pop();
				full_dir.Pop();
			}
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
		if (m_SceneSubDirName.GetLength() > 0)
			full_dir.Push( m_SceneSubDirName );

		//	get the files for this directory
		//
		BuildFileListFromDirectory( full_dir );
	}

	//-------------------------------------------------------------------------------
	//	sort the list
	if (m_bSortList)
	{
		Sort();
	}
	//DBG_LOG( "------------------file list end-------------------" );
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
//virtual 
bool fsysFileList::Notify( const fsLocator& i_Directory, const fsLocator& i_File )
{
	itString fname;
	fsFileUtil::LocatorToUnicodeString(i_File, fname);

	// the function puts an extra "\" at end, so need to remove it
	if (fname[fname.GetLength()-1] == '\\')
		fname.RemoveCharAt(fname.GetLength()-1);

	//	DEBUG only
	//DBG_LOG( "   file(" << dbgLog::UnicodetoANSI(fname.GetString(),fname.GetLength()).c_str() << ")" );

	//	DEBUG only
	//std::string file_dir;
	//fsFileUtil::LocatorToANSIFilename( i_Directory, file_dir );
	//DBG_LOG( "FileList directory (" << file_dir.c_str() << ")"  );

	//	add the directory and filename
	AddFilename( i_Directory, fname );
	return true;
}

//------------------------------------------------------------------------
// Clear - clear out the list
//------------------------------------------------------------------------
void fsysFileList::Clear()
{
	m_FileList.resize(0);
}

//------------------------------------------------------------------------
// AppendFromFileList - append the passed in filelist to this one.
//------------------------------------------------------------------------
void fsysFileList::AppendFromFileList( const fsysFileList& i_FileList )
{
	int size = m_FileList.size();
	int otherlist_size = i_FileList.Size();
	m_FileList.resize(size + otherlist_size);

	//DBG_LOG("AppendFromFileList list");
	for (int i = 0; i < otherlist_size; ++i)
	{
		m_FileList[size+i].SetFilename( i_FileList.GetFilename(i) );
		fsLocator path;
		i_FileList.GetFilePath(i, path);
		m_FileList[size+i].SetFilePath( path );

		//std::string pathstring;
		//fsFileUtil::LocatorToANSIFilename(path, pathstring);
		//DBG_LOG2("  %02d  adding path = (%s)", i, pathstring.c_str());
	}
}

//------------------------------------------------------------------------
// system directory name
//------------------------------------------------------------------------
void fsysFileList::SetSystemDirName(const itString& i_SysDirName)
{
	m_SystemDirName = i_SysDirName;
}
void fsysFileList::GetSystemDirName(itString& o_SysDirName)
{
	o_SysDirName = m_SystemDirName;
}

//------------------------------------------------------------------------
// Set the sub-directory name (generally, Data, Model, ....)
//------------------------------------------------------------------------
void fsysFileList::SetSubDirName(const itString& i_SubDirName)
{
	m_SubDirName = i_SubDirName;
}

//------------------------------------------------------------------------
// Set the sub-directory name (Data or Textures)
// to use only in the Scene folder. 
//------------------------------------------------------------------------
void fsysFileList::SetSceneSubDirName(const itString& i_SceneSubDirName)
{
	m_SceneSubDirName = i_SceneSubDirName;
}

//------------------------------------------------------------------------
// Set the object directory name 
// (the name of the character: CLOE_outfit01, etc.)
//------------------------------------------------------------------------
void fsysFileList::SetObjectDirName(const itString& i_ObjectDirName)
{
	m_ObjectDirName = i_ObjectDirName;
}

//------------------------------------------------------------------------
// Set the file type extensions to build the list for
//------------------------------------------------------------------------
void fsysFileList::SetFileTypeExtensions(const itString& i_FileTypeExtensions)
{
	m_FileExtensions = i_FileTypeExtensions;
}

//------------------------------------------------------------------------
// Size() - number of file entres
//------------------------------------------------------------------------
int fsysFileList::Size() const
{
	return m_FileList.size();
}

//------------------------------------------------------------------------
// GetIndex - return the index of the file.
//	The function returns -1 if filename not found.
//
//	Note: this function will return the first occurrence of the filename.
//	Note: this function is case INSENSITIVE
//------------------------------------------------------------------------
int fsysFileList::GetIndex(const itString& i_Filename)
{
	FileListType::iterator it = m_FileList.begin();

	itString s1, s2;
	s1 = i_Filename;
	itStringUtil::ToLower(s1);

	//DBG_LOG2( "GetIndex [%s][%s]", itStringUtil::GetStdString( i_Filename ).c_str(), itStringUtil::GetStdString( s1 ).c_str() );

	int count = 0;
	while (it != m_FileList.end())
	{
		s2 = it->GetFilename();
		itStringUtil::ToLower(s2);
		//DBG_LOG2( "         comparing[%s][%s]", itStringUtil::GetStdString( s2 ).c_str(), itStringUtil::GetStdString( s1 ).c_str() );
		if (s1 == s2)
		{
			return count;
		}
		it++;
		count++;
	}
	return -1;
}

//------------------------------------------------------------------------
// GetFilename
//------------------------------------------------------------------------
const itString& fsysFileList::GetFilename(int i_Index) const
{
	DBG_ASSERT( i_Index >= 0 && i_Index < m_FileList.size(), "Index out of range" );

	return m_FileList[i_Index].GetFilename();
}

//------------------------------------------------------------------------
// GetFilePath - get the path for the filename (w/out the filename)
//------------------------------------------------------------------------
void fsysFileList::GetFilePath(int i_Index, fsLocator& o_FilePath) const
{
	DBG_ASSERT( i_Index >= 0 && i_Index < m_FileList.size(), "Index out of range" );

	o_FilePath = m_FileList[i_Index].GetFilePath();
}

//------------------------------------------------------------------------
// FindFilePath - get the fullpath for the filename (w/out the filename).
// Returns false if it cannot be found. Safer version compared to
// GetFilePath as long as the caller is prepared for the return value.
//------------------------------------------------------------------------
bool fsysFileList::FindFilePath(const itString& i_Filename, fsLocator& o_FilePath)
{
	if (i_Filename.GetLength() > 0)
	{
		//	try to find the file
		int index = GetIndex( i_Filename );
		if (index >= 0)
		{
			o_FilePath = m_FileList[index].GetFilePath();
			return true;
		}
	}
	return false;
}

//------------------------------------------------------------------------
// GetFilePath - get the path for the filename (w/out the filename).
// Note: This throws file not found exception if not found.
//------------------------------------------------------------------------
void fsysFileList::GetFilePath(const itString& i_Filename, fsLocator& o_FilePath)
{
	if (i_Filename.GetLength() == 0)
	{
		// Empty filenames cannot be found, return empty locator
		o_FilePath.Clear();
		return;
	}

	//	try to find the file
	if (!FindFilePath(i_Filename, o_FilePath))
	{
		// file not found...do something
		o_FilePath.Clear();
		fsLocator filename;
		filename.Push(i_Filename);
		std::string fname;
		fsFileUtil::LocatorToANSIFilename( filename, fname );
		DBG_ERROR( "GetFilePath Error " << fname.c_str() );
		throw fsFileDoesntExistX( filename );
	}
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void fsysFileList::SetAppDir( const fsLocator& i_AppDir )
{
	m_AppDir = i_AppDir;
}

//------------------------------------------------------------------------
//	Set the sort flag, but don't actually sort it.  This is done when
//	the file list is built.
//------------------------------------------------------------------------
void fsysFileList::SetSortListFlag(bool i_bSort)
{
	this->m_bSortList = i_bSort;
}

//------------------------------------------------------------------------
// Set this flag to true in order to recursively search through 
//	sub directories.
//------------------------------------------------------------------------
void fsysFileList::SetSearchSubDirectories(bool i_bRecursive)
{
	this->m_bSearchSubDirectories = i_bRecursive;
}

//------------------------------------------------------------------------
//	BuildFileListFromDirectory - build file list from the directory
//------------------------------------------------------------------------
void fsysFileList::BuildFileListFromDirectory(const fsLocator& i_FullDirectory)
{
	//	DEBUG only
	//std::string data_dir;
	//fsFileUtil::LocatorToANSIFilename( i_FullDirectory, data_dir );
	//DBG_LOG2( "Searching (%s) in directory = (%s)", dbgLog::UnicodetoANSI(m_FileExtensions.GetString(),m_FileExtensions.GetLength()).c_str(), 
	//												data_dir.c_str() );

	// Parse through the list of extensions and convert them into
	// an array to give to gfFileEnum
	//
	std::vector<itString> file_exts;
	const int num_chars = m_FileExtensions.GetLength();
	itString ext;
	for (int i=0; i<num_chars; ++i)
	{
		if (m_FileExtensions[i] == ';')
		{
			if (ext.GetLength() > 0)
			{
				file_exts.push_back(ext);
				ext.Clear();
			}
		}
		else
		{
			ext += m_FileExtensions[i];
		}
	}
	// Add whatever is left to the list
	if (ext.GetLength() > 0)
		file_exts.push_back(ext);

	gfFileEnum::EnumerateFiles( i_FullDirectory, *(this), file_exts, m_bSearchSubDirectories );
	
}

//------------------------------------------------------------------------
//	AddFilename - add a filename to the list
//------------------------------------------------------------------------
void fsysFileList::AddFilename(const fsLocator& i_FullDirectory, const itString& i_Filename)
{
	bool bOKToAdd = true;
	if ( !m_bAllowDuplicateFilenames )
	{
		bOKToAdd = !FilenameExists( i_Filename );
	}

	if (bOKToAdd)
	{
		int index = m_FileList.size();
		m_FileList.resize( m_FileList.size() + 1 );

		m_FileList[index].SetFilename( i_Filename );
		m_FileList[index].SetFilePath( i_FullDirectory );
	}
}

//------------------------------------------------------------------------
//	FilenameExists - returns true if the file exists regardless if the 
//	path is the same.
//------------------------------------------------------------------------
bool fsysFileList::FilenameExists(const itString& i_Filename)
{
	FileListType::iterator it = std::find_if(m_FileList.begin(), m_FileList.end(), filename_search(i_Filename));
	return (it != m_FileList.end());
}

//------------------------------------------------------------------------
//	Strip the paths before the directory name passed in.  If the 
//	i_bIncludePassDir is true then the passed in dir name is also removed.
//------------------------------------------------------------------------
void fsysFileList::StripPathsBefore(const itString& i_DirName, bool i_bIncludePassedDir)
{
	FileListType::iterator it = m_FileList.begin();

	while (it != m_FileList.end())
	{
		fsLocator path = it->GetFilePath();
		path.RemoveBefore(i_DirName);
		if (i_bIncludePassedDir)
			path.Remove(i_DirName);
		it->SetFilePath(path);

		it++;
	}
}

//------------------------------------------------------------------------
//	Prepend the string before all the paths
//------------------------------------------------------------------------
void fsysFileList::PrependOnPaths(const itString& i_DirName)
{
	//DBG_LOG("PrependPathList");

	FileListType::iterator it = m_FileList.begin();

	while (it != m_FileList.end())
	{
		fsLocator path;
		path.Push(i_DirName);
		path.Push(it->GetFilePath());
		it->SetFilePath(path);

		//std::string pathstring;
		//fsFileUtil::LocatorToANSIFilename(it->GetFilePath(), pathstring);
		//DBG_LOG("  New path = (" << pathstring.c_str() << ")" );

		it++;
	}
}

//------------------------------------------------------------------------
//	Sort the filenames within each directory
//------------------------------------------------------------------------
void fsysFileList::Sort()
{
	// Sort the vector using predicate and std::sort  
	std::sort(m_FileList.begin(), m_FileList.end(), filename_sort()); 
}

