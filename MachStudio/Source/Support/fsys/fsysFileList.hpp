/*****************************************************************************
**  fsysFileList.hpp
**
**      fsysFileList holds a list of files that have been gathered based on
**	the app (project) directory, the scene, the system dir, and the file dir.
**
**		the full directory path is made up of:
**	
**		1) one directory in mnmPaths::e_DataSceneAndStock ($DataPath/Stock or $DataPath/SceneName)
**		2) the system directory name ("Props", "Sets", ...)
**		3) the object directory name ("General", "Bob", ...)
**		4) the data directory name ("data", "model", ...)
**		5) the filename itself
**
**		When building the file list the scene path will take precedence over the stock version.
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#ifdef FSYS_FILELIST_HPP
#error fsysFileList.hpp multiply included
#endif
#define FSYS_FILELIST_HPP

#ifndef FSYS_FILEINFO_HPP
#include "Support/fsys/fsysFileInfo.hpp"
#endif

#ifndef FS_FILEENUM_HPP
#include "Core/fs/fsFileEnum.hpp"
#endif

#include <vector>
#include <string>


//============================================================================
//============================================================================
class fsLocator;


//============================================================================
//============================================================================
class fsysFileList : public fsFileEnum::EnumTarget
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	fsysFileList();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	~fsysFileList();

	//------------------------------------------------------------------------
	//	BuildFileList - build the file list using all the folders under
	//	stock and scene folders for the specific system dir.  this will
	//	also get all of the sub-dirs under the system dir.
	//
	//	Note: it is assumed that the system directory name, data sub-dir name
	//	and file extensions has been set already (like during the systems
	//	initialization.
	//------------------------------------------------------------------------
	void BuildFileList();

	//------------------------------------------------------------------------
	// A variation of BuildFileList that does not clear out the
	//	previous file list
	//------------------------------------------------------------------------
	void AppendFileList();

	//------------------------------------------------------------------------
	//	Used by gfFileEnum::EnumerateFiles()
	//------------------------------------------------------------------------
	virtual bool Notify( const fsLocator& i_Directory, const fsLocator& i_File );

	//------------------------------------------------------------------------
	// Clear - clear out the list
	//------------------------------------------------------------------------
	void Clear();

	//------------------------------------------------------------------------
	// AppendFromFileList - append the passed in filelist to this one.
	//------------------------------------------------------------------------
	void AppendFromFileList( const fsysFileList& i_FileList );

	//------------------------------------------------------------------------
	// Set the sub-directory name (generally, Data, Model, ....)
	//------------------------------------------------------------------------
	void SetSubDirName(const std::string& i_SubDirName);

	//------------------------------------------------------------------------
	// Set the sub-directory name (Data or Textures)
	// to use only in the Scene folder. 
	//------------------------------------------------------------------------
	void SetSceneSubDirName(const std::string& i_SceneSubDirName);

	//------------------------------------------------------------------------
	// Set the object directory name 
	// (the name of the character: CLOE_outfit01, etc.)
	//------------------------------------------------------------------------
	void SetObjectDirName(const std::string& i_ObjectDirName);

	//------------------------------------------------------------------------
	// system directory name
	//------------------------------------------------------------------------
	void SetSystemDirName(const std::string& i_SysDirName);
	void GetSystemDirName(std::string& o_SysDirName);

	//------------------------------------------------------------------------
	// Set the file type extensions to build the list for
	//------------------------------------------------------------------------
	void SetFileTypeExtensions(const std::string& i_FileTypeExtensions);

	//------------------------------------------------------------------------
	// Size() - number of file entres
	//------------------------------------------------------------------------
	int Size() const;

	//------------------------------------------------------------------------
	// GetIndex - return the index of the file.
	//	The function returns -1 if filename not found.
	//
	//	Note: this function will return the first occurrence of the filename.
	//	Note: this function is case INSENSITIVE
	//------------------------------------------------------------------------
	int GetIndex(const std::string& i_Filename);

	//------------------------------------------------------------------------
	// GetFilename
	//------------------------------------------------------------------------
	const std::string& GetFilename(int i_Index) const;

	//------------------------------------------------------------------------
	// FindFilePath - get the fullpath for the filename (w/out the filename).
	// Returns false if it cannot be found. Safer version compared to
	// GetFilePath as long as the caller is prepared for the return value.
	//------------------------------------------------------------------------
	bool FindFilePath(const std::string& i_Filename, fsLocator& o_FilePath);

	//------------------------------------------------------------------------
	// GetFilePath - get the path for the filename (w/out the filename).
	// Note: This throws file not found exception if not found.
	//------------------------------------------------------------------------
	void GetFilePath(int i_Index, fsLocator& o_FilePath) const;

	//------------------------------------------------------------------------
	// GetFilePath - get the path for the filename (w/out the filename)
	//------------------------------------------------------------------------
	void GetFilePath(const std::string& i_Filename, fsLocator& o_FilePath);

	//------------------------------------------------------------------------
	// GetDataPath
	//------------------------------------------------------------------------
	const fsLocator& GetDataPath(int i_Index);

	//------------------------------------------------------------------------
	// GetDataPath
	//------------------------------------------------------------------------
	const fsLocator& GetDataPath(const std::string& i_Filename);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void SetAppDir( const fsLocator& i_AppDir );

	//------------------------------------------------------------------------
	//	Set the sort flag, but don't actually sort it.  This is done when
	//	the file list is built.
	//------------------------------------------------------------------------
	void SetSortListFlag(bool i_bSort);

	//------------------------------------------------------------------------
	// Set this flag to true in order to recursively search through 
	//	sub directories. Default is true.
	//------------------------------------------------------------------------
	void SetSearchSubDirectories(bool i_bRecursive);

	//------------------------------------------------------------------------
	//	AddFilename - add a filename to the list
	//------------------------------------------------------------------------
	void AddFilename(const fsLocator& i_FullDirectory, const std::string& i_Filename);

	//------------------------------------------------------------------------
	//	Strip the paths before the directory name passed in.  If the 
	//	i_bIncludePassDir is true then the passed in dir name is also removed.
	//------------------------------------------------------------------------
	void StripPathsBefore(const std::string& i_DirName, bool i_bIncludePassedDir);

	//------------------------------------------------------------------------
	//	Prepend the string before all the paths
	//------------------------------------------------------------------------
	void PrependOnPaths(const std::string& i_DirName);

	//------------------------------------------------------------------------
	//	Sort the filenames within each directory
	//------------------------------------------------------------------------
	void Sort();

private:
	//------------------------------------------------------------------------
	//	BuildFileListFromDirectory - build file list from the directory
	//------------------------------------------------------------------------
	void BuildFileListFromDirectory(const fsLocator& i_FullDirectory);

	//------------------------------------------------------------------------
	//	FilenameExists - returns true if the file exists regardless if the 
	//	path is the same.
	//------------------------------------------------------------------------
	bool FilenameExists(const std::string& i_Filename);

	//------------------------------------------------------------------------
	//	sort criteria
	//------------------------------------------------------------------------
	//bool MyDataSortPredicate(const fsysFileInfo& lhs, const fsysFileInfo& rhs);

private:
	//------------------------------------------------------------------------
	//	typedefs
	//------------------------------------------------------------------------
	typedef std::vector<fsysFileInfo> FileListType;

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	bool		m_bSortList;
	bool		m_bAllowDuplicateFilenames;
	bool		m_bSearchSubDirectories;
	std::string	m_SystemDirName;
	std::string	m_FileExtensions;
	std::string	m_SubDirName;
	std::string	m_SceneSubDirName;
	std::string	m_ObjectDirName;
	fsLocator	m_AppDir;			// TODO: - can we get rid of this?

	FileListType	m_FileList;
};

