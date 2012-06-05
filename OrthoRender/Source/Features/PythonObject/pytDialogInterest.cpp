/****************************************************************************\
**	pytDialogInterest.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Features/PythonObject/pytDialogInterest.hpp"
#include "Systems/ptlt/GUI/ptltDialogDataUtil.hpp"
#include "Systems/ptlt/Object/ptltObjectMgr.hpp"
#include "Systems/ptlt/Undo/ptltOperations.hpp"

#include "Support/pyth/pythCommands.hpp"
#include "Support/pyth/pythModules.hpp"
#include "Support/pyth/pythUtil.hpp"
#include "Core/fs/fsFileUtil.hpp"


#include "Tool/gui/guiMainWindow.hpp"
#include "Tool/cma/cmaCommandMgr.hpp"

#ifndef ENV_TYPE_HPP
#include "Core/env/envType.hpp"
#endif

namespace
{
	const char* c_SystemName = "Python Objects";
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
pytDialogInterest::pytDialogInterest()
: cmmDialogInterest(c_SystemName)
{
}

//--------------------------------------------------------------------
//	Add Object to Placed
//--------------------------------------------------------------------
//virtual 
void pytDialogInterest::AddObject(const itString& i_FileName, const fsLocator& i_Path)
{

	try
	{
		std::string execObjectName;
		execObjectName = itStringUtil::GetStdString(i_FileName);
		ExecObject(execObjectName);
	}
	catch (const fsFileDoesntExistX& i_Ex)
	{
		std::string filename;
		fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);
		std::string msg = "Cannot find file\n" + filename;
		guiMessageBox::Show(msg.c_str(), "Error", guiMessageBox::e_OKOnly);
	}

	// change focus to the main app
	guiMainWindow::Focus();
}
//--------------------------------------------------------------------
//	Delete Object from Placed
//--------------------------------------------------------------------
//virtual 
void pytDialogInterest::DeleteObject(const nameString& i_Name)
{}

//--------------------------------------------------------------------
//	Duplicate Object from Placed
//--------------------------------------------------------------------
//virtual 
void pytDialogInterest::DuplicateObject(const nameString& i_Name)
{}

//--------------------------------------------------------------------
//	Reload Object from Placed
//--------------------------------------------------------------------
//virtual 
void pytDialogInterest::ReloadObject(const nameString& i_Name)
{}

//--------------------------------------------------------------------
//	Select Object from Placed
//--------------------------------------------------------------------
//virtual 
void pytDialogInterest::SelectObject(const nameString& i_Name, bool i_bAppend) const
{}

//--------------------------------------------------------------------
//	Remove Object from Selection
//--------------------------------------------------------------------
//virtual
void  pytDialogInterest::DeselectObject(const nameString& i_Name) const
{}

//--------------------------------------------------------------------
//	GetPlacedObjects - Get Placed objects
//--------------------------------------------------------------------
//virtual 
void pytDialogInterest::GetPlacedObjects(cmmDialogDataList& io_DataList) const
{}

//--------------------------------------------------------------------
//	GetAvailableObjects - Get available objects
//--------------------------------------------------------------------
//virtual 
void pytDialogInterest::GetAvailableObjects(fsysFileList& io_FileList) const
{
#if defined(PYTHON_ENABLED)

	fsLocator path;
	//path.Push("Data");
	path.Push(c_SystemName);
	std::vector<std::string> pObjectNames;
	pythCommands::GetPyObjectNames(pObjectNames);
	itString fname;
	const int num_objs = pObjectNames.size();
	for( int i = 0; i < num_objs; ++i )
	{
		fname = itString( pObjectNames[i].c_str() );
		//DBG_WARNING1("The object name is: %s", pObjectNames[i].c_str());
		io_FileList.AddFilename(path,fname);
	}
	
#endif
	
}

//--------------------------------------------------------------------
//	GetAllowedInteractions - returns a set of bits for which
//	interactions are allowed (edit, delete, etc.)
//--------------------------------------------------------------------
//virtual 
int pytDialogInterest::GetAllowedInteractions(const fsLocator& i_Path) const
{
	return (  e_DIAllow_Add );
}

//--------------------------------------------------------------------
//	ExecObject - On addObject, this function will execute the selected python script
//--------------------------------------------------------------------
void pytDialogInterest::ExecObject(std::string i_ObjectName)
{
	const std::vector< cmaCommand* >& cmd_list = cmaCommandMgr::GetCommandList();
	DBG_WARNING1("The object name is: %s", i_ObjectName.c_str());
	const int num_cmds = cmd_list.size();
	for (int i=0; i<num_cmds; ++i)
	{
		std::string obj_name =cmd_list[i]->GetTag();
		DBG_WARNING1("The object tag is: %s", obj_name.c_str());
		if (i_ObjectName == obj_name)
		{
			cmd_list[i]->Execute();
			return;
		}
	}

#if defined(PYTHON_ENABLED)
	PyErr_SetString(PyExc_NameError, "Could not find command by name.");
#endif
}
