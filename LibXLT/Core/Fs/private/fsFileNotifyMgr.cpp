
/*****************************************************************************
**  fsFileNotifyMgr.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/


#include "Core/fs/private/fsFileNotifyMgr.hpp"
#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif
#ifndef FS_FILEUTIL_HPP
#include "Core/fs/fsFileUtil.hpp"
#endif


#ifndef ENV_STRING_HPP
#include "Core/env/envString.hpp"
#endif
#include "Core/fs/fsXMLReader.hpp"
#include "Core/fs/fsXMLWriter.hpp"
#ifndef FS_PATHUTIL_HPP
#include "Core/fs/fsPathUtil.hpp"
#endif
#include "Core/gf/gfPaths.hpp"
#include "Core/name/nameMgr.hpp"
#include "Core/name/nameObject.hpp"

//#include "Tool/gui/guiMessageBox.hpp"

#include <algorithm>
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <system_error>

namespace FileUpdated
{
	bool l_PromptBeforeUpdate = false;
	bool l_Revision = false;
	//--------------------------------------------------------------------
	// Set autoupdate flag on/off
	//--------------------------------------------------------------------
	void SetAutoUpdate(const bool& i_PromptBeforeUpdate)
	{
		l_PromptBeforeUpdate = i_PromptBeforeUpdate; 
	}

	//--------------------------------------------------------------------
	// Set revision flag on/off
	//--------------------------------------------------------------------
	void SetEnableRevisionFlag(const bool& i_Revision)
	{
		l_Revision = i_Revision;
	}

	bool GetEnableRevisionFlag()
	{
		return l_Revision;
	}
}

namespace 
{
	// One entry per watched file in each, in step (same index, same file)
	std::vector<fsLocator> fileNames;
	std::vector<std::filesystem::file_time_type> initTimes;
	std::vector<bool> need_update;

	// Check: when it last looked at files, and the first file of the next
	// batch it looks at
	unsigned short m_prevTime = 0;
	size_t l_NextToCheck = 0;

	const unsigned short c_CheckIntervalSeconds = 5;
	const size_t c_CheckBatchSize = 100;
	void (*l_ReloadObjectsFunction)(const fsLocator& i_Path, const std::string i_SystemName);
	void (*l_ReloadTextureFunction)(const fsLocator& i_TextureName, bool i_bIsMipMap);
	void (*l_AddObjectsFunction) (const std::string& i_FileName, const fsLocator& i_Path);
 

	//--------------------------------------------------------------------
	//	last_modified - when the file was last written, or c_Unreadable
	//		if that can't be read (the file is missing, say)
	//--------------------------------------------------------------------
	const std::filesystem::file_time_type c_Unreadable = std::filesystem::file_time_type::min();

	std::filesystem::file_time_type last_modified(const fsLocator& i_File)
	{
		std::error_code error;
		const std::filesystem::file_time_type time = std::filesystem::last_write_time(fsPathUtil::LocatorToPath(i_File), error);
		return error ? c_Unreadable : time;
	}

	//--------------------------------------------------------------------
	//	FileAdded - a file has been 
	//		added to the system
	//--------------------------------------------------------------------
	void file_added( const fsLocator& i_FileToCheck)
	{
		std::string fileNameStr;
		for(int i = 0; i < fileNames.size() ; i++)
		{
			if(i_FileToCheck == fileNames[i])
				return;
		}
		fsFileUtil::LocatorToANSIFilename(i_FileToCheck, fileNameStr);
		fileNames.push_back(i_FileToCheck);
		DBG_TRACE(" Object "<<fileNameStr<<" has been added");
		initTimes.push_back(last_modified(i_FileToCheck));
		need_update.push_back(true);
	}

	//--------------------------------------------------------------------
	//	FileRemoved - a file has been  
	//		removed from the system
	//--------------------------------------------------------------------
	void file_removed(const fsLocator& i_FileToCheck)
	{
		for(int i = 0; i < fileNames.size() ; i++)
		{
			if(i_FileToCheck == fileNames[i])
			{
 				std::string fileNameStr;
				fsFileUtil::LocatorToANSIFilename(fileNames[i], fileNameStr);
				DBG_TRACE(" Object "<<fileNameStr<<" has been deleted");
				fileNames.erase(fileNames.begin() + i);
				initTimes.erase(initTimes.begin() + i);
				need_update.erase(need_update.begin() + i);
				return;
			}
		}
	}

	//--------------------------------------------------------------------
	//	FileRenamed - a file has been renamed
	//--------------------------------------------------------------------
	void file_renamed(const fsLocator& i_FileToRename, const fsLocator& i_RenamedFile)
	{
		file_removed(i_FileToRename);
		file_added(i_RenamedFile);
	}

	//--------------------------------------------------------------------
	// check if a file already exists in the scene
	//--------------------------------------------------------------------
	bool file_exists(const fsLocator& i_FileToCheck)
	{
		for(int i = 0; i < fileNames.size() ; i++)
		{
			if(i_FileToCheck == fileNames[i])
			{
				return true;
			}
		}
		return false;
	}

	

	//--------------------------------------------------------------------
	// Checks if a file needs to be updated. 
	// Reloads Objects and textures if needed
	//--------------------------------------------------------------------
	void file_check(const size_t start, const size_t end)
	{
		std::string fileName, i_fileName; 
		std::string t_FileName, o_Extension;
		std::filesystem::file_time_type newTime;
		bool UpdateObject = false;
		
		for(size_t i = start; i < end; i++)
		{
			fsFileUtil::LocatorToUnicodeString(fileNames[i],t_FileName);
			fsFileUtil::LocatorToANSIFilename(fileNames[i], fileName);
			fsFileUtil::LocatorToANSIFilename(fileNames[i].GetLastName(), i_fileName);

			newTime = last_modified(fileNames[i]);
			// Check whether the object has been updated 
			if(newTime != initTimes[i] && newTime != c_Unreadable && need_update[i])
			{
				int length = int(t_FileName.size());
				// Break out extension
				o_Extension = "";
				int pos = length-1;
				if (t_FileName.find('.') != std::string::npos)
				{
					envString::GetExtension(t_FileName, o_Extension);
					envString::ToLower(o_Extension);

				}
				DBG_TRACE(" Object "<<fileName<<" has been updated");
				
				// If the dialog needs to be shown 
				std::string message = " Do you want to update the file " + fileName + "?"; 
				if(FileUpdated::l_PromptBeforeUpdate)
				{
					int retVal = 0;//guiMessageBox::Show(message.c_str(),"File Updated", guiMessageBox::e_YesNo);
			
					if ( retVal == 1 )
					{
						UpdateObject = false;
						need_update[i] = false;

						return;
					}
					else
						UpdateObject = true;
				}
				// If the object needs to be updated

				if(UpdateObject || !FileUpdated::l_PromptBeforeUpdate)
				{
					// Check if the file that needs to be updated is an object
					if(l_ReloadObjectsFunction !=NULL && (o_Extension.find("gxb") != std::string::npos))
					{
						if(fsFileUtil::FileExists(fileNames[i]))
						{
							l_ReloadObjectsFunction(fileNames[i], "Objects");
							file_added(fileNames[i]);
						}
					
						else
							DBG_TRACE("Object " << fileNames[i] <<" has been renamed or deleted");
					}

				
				// Reload Textures
					else if(l_ReloadTextureFunction !=NULL && ( (o_Extension.find("bmp") != std::string::npos) || (o_Extension.find("png") != std::string::npos)
															   ||(o_Extension.find("dds") != std::string::npos) || (o_Extension.find("tga") != std::string::npos)
																		   ||(o_Extension.find("jpg") != std::string::npos) || (o_Extension.find("hdr") != std::string::npos)
																		   ||(o_Extension.find("tif") != std::string::npos)))
					{
						if(fsFileUtil::FileExists(fileNames[i]))
						{
							l_ReloadTextureFunction(fileNames[i], true);
							file_added(fileNames[i]);
						}
					}
				}// end of checking if object needs update
					initTimes[i] = newTime;
			}// end of checking if the object has updated
		}// end of for loop
	}// end of Check function
}// end of namespace

 


//------------------------------------------------------------------------
// Initialize
//------------------------------------------------------------------------
	
void fsFileNotifyMgr::Intialize()
{
	fileNames.clear();
	initTimes.clear();
	need_update.clear();
	m_prevTime = 0;
	l_NextToCheck = 0;
}

//------------------------------------------------------------------------
// DeInitialize
//------------------------------------------------------------------------
void fsFileNotifyMgr::DeInitialize()
{
	Intialize();
}

//--------------------------------------------------------------------
// Check if a file has been modified. Called every frame; when more
// than c_CheckIntervalSeconds have passed since it last looked, it
// looks at the next c_CheckBatchSize files, starting again from the
// first after the last.
//
// i_Time is the clock in seconds cut to 16 bits, so it wraps (about
// every 18 hours); the difference is taken in 16 bits too, so the
// interval is right across the wrap.
//--------------------------------------------------------------------
void fsFileNotifyMgr::Check(unsigned short time)
{
	const unsigned short elapsed = static_cast<unsigned short>(time - m_prevTime);
	if (elapsed <= c_CheckIntervalSeconds)
		return;
	m_prevTime = time;

	if (l_NextToCheck >= fileNames.size())
		l_NextToCheck = 0;

	const size_t first = l_NextToCheck;
	const size_t last = std::min(first + c_CheckBatchSize, fileNames.size());
	l_NextToCheck = last;
	file_check(first, last);
}

//--------------------------------------------------------------------
// Check if a new file has been exported 
//--------------------------------------------------------------------
void fsFileNotifyMgr::CheckForNewObjects()
{
	fsLocator path_loc;
	fsLocator full_path;
	fsLocator textfilePath = gfPaths::GetPath(gfPaths::e_LocalSystemDataPath);
	
	textfilePath.Push("Temp");
	textfilePath.Push("sgpuExport_Rhino.txt");
	
	std::string cfgpath;
	fsFileUtil::LocatorToANSIFilename(textfilePath,cfgpath);
	if(fsFileUtil::FileExists(textfilePath))
	{
		fsXMLReader pXMLReader(cfgpath);
		pXMLReader.Open();
		fsXMLData::fs_Node_Type node_type;
		std::string keyname, strvalue;
		while ((node_type = pXMLReader.ReadNode(keyname,strvalue)) != fsXMLData::e_EOF) 
			
		{
			if((node_type == fsXMLData::e_EndElement) && (strcmp(keyname.c_str(), "export_files") == 0))
			{
				continue;
			}
			if(strcmp(keyname.c_str(),"version") == 0)
				continue;
			else if ((node_type == fsXMLData::e_Text) && (keyname.length() > 0))
			{
				fsFileUtil::ANSIFilenameToLocator(strvalue.c_str(), path_loc);
				full_path.Push("Objects");
				full_path.Push(path_loc);

				if(!file_exists(path_loc))
				{
					file_added(path_loc);
					l_AddObjectsFunction(full_path.GetLastName(), full_path);
				}
				full_path.Clear();
				DBG_TRACE(" KeyName " <<keyname <<"StrValue :"<<strvalue);
			}
		}
		pXMLReader.Close();
		fsFileUtil::DeleteFile(textfilePath);
	}
	
}
			

//--------------------------------------------------------------------
// Function pointer for ObjectReload function 
//--------------------------------------------------------------------
void fsFileNotifyMgr::ObjectReloadFunction(void (*i_ObjectReloadFunction)(const fsLocator& i_Path, const std::string i_SystemName))
{
	l_ReloadObjectsFunction = i_ObjectReloadFunction;
}

void fsFileNotifyMgr::ObjectAddFunction(void (*i_ObjectAddFunction)(const std::string& i_Name, const fsLocator& i_Path))
{
	l_AddObjectsFunction = i_ObjectAddFunction;
}

void fsFileNotifyMgr::TextureReloadFunction(void (*i_TextureReloadFunction)(const fsLocator& i_TextureName, bool i_bIsMipMap))
{
	l_ReloadTextureFunction = i_TextureReloadFunction;
}

void fsFileNotifyMgr::FileAdded(const fsLocator& i_FileToCheck)
{
	file_added(i_FileToCheck);
}

void fsFileNotifyMgr::FileRenamed(const fsLocator& i_FileToRename, const fsLocator& i_RenamedFile)
{
	file_renamed(i_FileToRename, i_RenamedFile);
}

void fsFileNotifyMgr::FileRemoved(const fsLocator& i_FileToCheck)
{
	file_removed(i_FileToCheck);
}

