/****************************************************************************\
**	prjPython.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Features/ProjectSetup/prjPython.hpp"

#include "Core/dbg/dbgLog.hpp"
#include "Features/ProjectSetup/ProjectSetupMgr.hpp"
#include "Support/pyth/pythModules.hpp"
#include "Support/pyth/pythFunctionUtil.hpp"

namespace
{
#if defined(PYTHON_ENABLED)
	//============================================================================
	// Commands made available to python
	//============================================================================

	//--------------------------------------------------------------------
	// Return name of current scene
	//--------------------------------------------------------------------
	PyObject *
	get_scene_name(PyObject *self, PyObject *args)
	{
		return pythFunctionUtil::ConvertString(ProjectSetupMgr::GetData().m_CurrentSceneName);
	}
	//--------------------------------------------------------------------
	// Return name of project
	//--------------------------------------------------------------------
	PyObject *
	get_project_name(PyObject *self, PyObject *args)
	{
		return pythFunctionUtil::ConvertString(ProjectSetupMgr::GetData().m_ProjectName);
	}
	//--------------------------------------------------------------------
	// Return directory of project
	//--------------------------------------------------------------------
	PyObject *
	get_project_dir(PyObject *self, PyObject *args)
	{
		return pythFunctionUtil::ConvertString(ProjectSetupMgr::GetData().m_ProjectDirectory);
	}

#endif
}	// end of namespace

//--------------------------------------------------------------------
// Add commands related to the selection list
//--------------------------------------------------------------------
void prjPython::AddCommands(const std::string &i_ModuleName)
{
#if defined(PYTHON_ENABLED)
	pythModules::AddCommand(i_ModuleName, "getSceneName", 
		"Return name of current scene (not the filename)", 
		get_scene_name);
	pythModules::AddCommand(i_ModuleName, "getProjectName", 
		"Return name of project", 
		get_project_name);
	pythModules::AddCommand(i_ModuleName, "getProjectDirectory", 
		"Return directory of project", 
		get_project_dir);
#endif

}