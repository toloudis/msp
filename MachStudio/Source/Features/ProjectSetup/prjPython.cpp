/****************************************************************************\
**	prjPython.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Features/ProjectSetup/prjPython.hpp"

#include "Features/ProjectSetup/ProjectSetupMgr.hpp"
#include "Support/pyth/pythFunctionUtil.hpp"
#include "Support/pyth/pythModules.hpp"

#include "Core/Fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/Fs/fsResourceTracker.hpp"
#include "Tool/doc/docSingleDocumentMgr.hpp"
#include "ToolUIWx/twx/twxSystem.hpp"


//============================================================================
//============================================================================
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

	//--------------------------------------------------------------------
	// Save a scene with the given file name
	//--------------------------------------------------------------------
	PyObject *
	save_scene(PyObject *self, PyObject *args)
	{
		const char *scene_location;
		if (!PyArg_ParseTuple(args, "s", &scene_location))
			return NULL;

		//create the fsLocator using the given path
		fsLocator sceneLocator;
		//sceneLocator = fsLocator( itString(scene_location) );
		fsFileUtil::ANSIFilenameToLocator(scene_location, sceneLocator);
		try
		{
			docSingleDocumentMgr::SaveDocument(sceneLocator);
		}
		catch ( const envExceptionX& i_Ex )
		{
			std::string msg = "Problem saving file, " + i_Ex.GetErrorMessage();
			DBG_ERROR(msg);
		}
		catch( ... )
		{
			std::string msg = "General exception error saving file.";
			DBG_ERROR(msg);
		}

		return Py_None;
	}

	//--------------------------------------------------------------------
	// Open a scene with the given file name
	//--------------------------------------------------------------------
	PyObject *
	open_scene(PyObject *self, PyObject *args)
	{
		const char *scene_location;
		if (!PyArg_ParseTuple(args, "s", &scene_location))
			return NULL;

		fsLocator sceneLocator;
		//sceneLocator = fsLocator( itString(scene_location) );
		fsFileUtil::ANSIFilenameToLocator(scene_location, sceneLocator);

		fsResourceTracker::Init();

		bool bNewerVersion = false;		// track if loaded file is newer than supported version

		//	Load the document
		//
		try
		{
			bNewerVersion = docSingleDocumentMgr::LoadDocument(sceneLocator);
		}
		catch( const fsNewerVersionAbortX& )
		{
			//	don't report the error.
		}
		catch ( const envExceptionX& i_Ex )
		{
			std::string msg = "Problem loading file, " + i_Ex.GetErrorMessage();
			DBG_ERROR(msg);
		}
		catch (const std::bad_alloc&)
		{
			std::string msg = "Out of system memory, could not load file.";
			DBG_ERROR(msg);
		}
		catch( ... )
		{
			std::string msg = "General exception error loading file.";
			DBG_ERROR(msg);
		}

		//return Py_None;
		// If the document has a filename, then the load suceeded, return True or False
		return pythFunctionUtil::ConvertBoolean(docSingleDocumentMgr::GetFilename().GetNumNames() > 0);
	}

	//--------------------------------------------------------------------
	// Close a scene minus asking to save
	//--------------------------------------------------------------------
	PyObject *
	close_scene(PyObject *self, PyObject *args)
	{

#ifdef USE_WXWIDGETS
		//force the frame to close, deleting the window
		twxSystem::g_pMainForm->Destroy();
#endif

		return Py_None;
	}
	
	//--------------------------------------------------------------------
	// Open a new scene without asking to save
	//--------------------------------------------------------------------
	PyObject *
	open_new_scene(PyObject *self, PyObject *args)
	{
		docSingleDocumentMgr::NewDocument();
		return Py_None;
	}

#endif
}	// end of namespace

//--------------------------------------------------------------------
// Add commands related to the selection list
//--------------------------------------------------------------------
void prjPython::AddCommands(const std::string &i_ModuleName)
{
#if defined(PYTHON_ENABLED)
	//pythModules::AddCommand(i_ModuleName, "getSceneName", 
	//	"Return name of current scene (not the filename)", 
	//	get_scene_name);
	//pythModules::AddCommand(i_ModuleName, "getProjectName", 
	//	"Return name of project", 
	//	get_project_name);
	pythModules::AddCommand(i_ModuleName, "getProjectDirectory", 
		"Return directory of project", 
		get_project_dir);
	pythModules::AddCommand(i_ModuleName, "saveScene", 
		"Save the scene at the given location", 
		save_scene);
	pythModules::AddCommand(i_ModuleName, "openScene", 
		"Open the scene with the given filename", 
		open_scene);
	pythModules::AddCommand(i_ModuleName, "newScene", 
		"Open a new scene", 
		open_new_scene);
	pythModules::AddCommand(i_ModuleName, "exit", 
		"Exit Application", 
		close_scene);
#endif

}