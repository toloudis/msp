/****************************************************************************\
**	mainPython.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "stdafx.h"
#include "MainApp/mainPython.hpp"
#include "MainApp/MainForm.h"
#include "MainApp/mainConstants.hpp"

#include "Support/pyth/pythModules.hpp"
#include "Support/pyth/pythFunctionUtil.hpp"

#include "Core/dbg/dbgLog.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Tool/doc/docSingleDocumentMgr.hpp"
#include "ToolUIManaged/tma/tmaManagedStringUtils.hpp"

#ifdef _MANAGED
using namespace StudioFramework;
#endif

namespace
{
#if defined(PYTHON_ENABLED)
	//============================================================================
	// Commands made available to python
	//============================================================================

	//--------------------------------------------------------------------
	// Return version of application
	//--------------------------------------------------------------------
	PyObject *
	get_version(PyObject *self, PyObject *args)
	{
		std::string exestr;

#ifdef _MANAGED
		tmaManagedStringUtils::ManagedStringToStdString(MainForm::FormInstance->GetExecutableVersion(), exestr);
#else
		exestr = mainConstants::mc_ExecutableVersion;
#endif //end managed

		return pythFunctionUtil::ConvertString(exestr);
	}
	//--------------------------------------------------------------------
	// Return filepath of current scene
	//--------------------------------------------------------------------
	PyObject *
	get_scene_filepath(PyObject *self, PyObject *args)
	{
		std::string filepath;
		fsLocator loc = docSingleDocumentMgr::GetFilename();
		fsFileUtil::LocatorToANSIFilename(loc, filepath);
		return pythFunctionUtil::ConvertString(filepath);
	}
	//--------------------------------------------------------------------
	// Return filename of current scene
	//--------------------------------------------------------------------
	PyObject *
	get_scene_filename(PyObject *self, PyObject *args)
	{
		std::string filename;
		docSingleDocumentMgr::GetFilenameOnly(filename);
		return pythFunctionUtil::ConvertString(filename);
	}

#endif
}	// end of namespace

//--------------------------------------------------------------------
// Add commands related to the selection list
//--------------------------------------------------------------------
void mainPython::AddCommands(const std::string &i_ModuleName)
{
#if defined(PYTHON_ENABLED)
	pythModules::AddCommand(i_ModuleName, "getVersion", 
		"Return version of application", 
		get_version);
	pythModules::AddCommand(i_ModuleName, "getSceneFilepath", 
		"Return full directory and filename path of current scene", 
		get_scene_filepath);
	pythModules::AddCommand(i_ModuleName, "getSceneFilename", 
		"Return filename only of current scene", 
		get_scene_filename);
#endif

}