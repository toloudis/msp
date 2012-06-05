/*****************************************************************************
**	tmaRegistryUtil.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "ToolUIManaged/tma/tmaRegistryUtil.hpp"

#include "ToolUIManaged/tma/tmaManagedStringUtils.hpp"

#include "Core/dbg/dbgLog.hpp"
#include "Core/fs/fsFileUtil.hpp"


//============================================================================
//============================================================================
namespace tmaRegistryUtil
{
	namespace
	{
		std::string l_CompanyName;
		std::string l_AppName;


	}	// end of namespace

	//--------------------------------------------------------------------
	//  Initializes a registry entry for this application, all
	// reads and writes will be placed under this entry
	//--------------------------------------------------------------------
	void  Init(const std::string &i_CompanyName, const std::string &i_AppName)
	{
		l_CompanyName = i_CompanyName;
		l_AppName = i_AppName;
	}

	//--------------------------------------------------------------------
	//  SetValue to key in given folder (under application) to given value
	//
	//	i_Folder can be a series of Keys (folders) between the root and
	//	the keys.  For instance:  "\\AppSection\\SubSection" and the
	//	data keys will go under this.
	//--------------------------------------------------------------------
	void  SetValue(	Root i_Root, 
					const std::string &i_Folder,
					const std::string &i_Key, 
					const std::string &i_Value)
	{
	}

	//--------------------------------------------------------------------
	// GetValue()
	//
	//	i_Folder can be a series of Keys (folders) between the root and
	//	the keys.  For instance:  "\\AppSection\\SubSection" and the
	//	data keys will go under this.
	//
	//	An empty string is returned if the data can't be read
	//--------------------------------------------------------------------
	std::string  GetValue(	Root i_Root, 
							const std::string &i_Folder,
							const std::string &i_Key )
	{
		return "";
	}

	//--------------------------------------------------------------------
	//  SetKey create a key of the given folder (i.e. a folder w/no value)
	//
	//	i_Folder can be a series of Keys (folders) between the root and
	//	the keys.  For instance:  "\\AppSection\\SubSection" and the
	//	data keys will go under this.
	//--------------------------------------------------------------------
	void SetKey( Root i_Root, 
				const std::string &i_Folder )
	{
	}

	//--------------------------------------------------------------------
	// GetKeys()
	//
	//	i_Folder can be a series of Keys (folders) between the root and
	//	the keys.  For instance:  "\\AppSection\\SubSection" and the
	//	data keys will go under this.
	//
	//	An empty string is returned if the data can't be read, otherwise
	//	a list of the children keys below the folder path is returned.
	//--------------------------------------------------------------------
	void GetKeys(	Root i_Root, 
					const std::string &i_Folder,
					std::vector<std::string> &o_Keys)
	{
	}

	//--------------------------------------------------------------------
	//	KeyExists()
	//
	//	i_Folder can be a series of Keys (folders) between the root and
	//	the keys.  For instance:  "\\AppSection\\SubSection" and the
	//	data keys will go under this.
	//--------------------------------------------------------------------
	bool KeyExists(Root i_Root, const std::string &i_Folder)
	{
		return false;
	}

	//--------------------------------------------------------------------
	//	DeleteKey() - delete the key
	//--------------------------------------------------------------------
	void DeleteKey( Root i_Root, 
					const std::string &i_Folder )
	{
	}

}	// end of namespace
