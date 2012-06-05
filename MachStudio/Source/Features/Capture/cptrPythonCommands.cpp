/*****************************************************************************
**	cptrPythonCommands.cpp
**
**		See .hpp
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
*****************************************************************************/
#include "Features/Capture/cptrPythonCommands.hpp"

#include "Features/Capture/cptrModeRender.hpp"
#include "Features/Capture/cptrModeRenderBatch.hpp"
#include "Features/Capture/cptrPackage.hpp"
#include "Features/Capture/cptrRenderBatchDataUtil.hpp"
#include "Features/Capture/cptrRenderUtil.hpp"

#include "Support/capt/captRenderOutputDataUtil.hpp"
#include "Support/mnm/mnmAppUtil.hpp"
#include "Support/mnm/mnmAutoSaveMgr.hpp"
#include "Support/mnm/mnmPaths.hpp"
#include "Support/mode/modeModeMgr.hpp"
#include "Support/pyth/pythFunctionUtil.hpp"
#include "Support/pyth/pythModules.hpp"
#include "Support/rlyr/rlyrRenderLayerMgr.hpp"

#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Tool/doc/docSingleDocumentMgr.hpp"


// When python is disabled, the whole namespace is removed
#if defined(PYTHON_ENABLED)
//============================================================================
//============================================================================
namespace cptrPythonCommands
{
	namespace
	{
		modeModeID l_CaptureModeID;
		
		//--------------------------------------------------------------------
		// Render the scene without a gui
		//--------------------------------------------------------------------
		PyObject* render_scene(PyObject *self, PyObject *args)
		{	
			// now launch the correct mode
			//
			modeMode* pMode = modeModeMgr::GetMode( l_CaptureModeID );
			DBG_ASSERT( pMode != 0, "No capture mode" );
			cptrModeRender *pCapMode = dynamic_cast<cptrModeRender*>(pMode);
			DBG_ASSERT( pCapMode != 0, "capture mode ID is not the capture mode" );

			//
			//guiSingleDocHandler::Open(i_SceneFile);
			mnmAutoSaveMgr::Initialize(false,10,1);	// turn off auto-save

			rlyrRenderLayerMgr::Update();
			captRenderOutputData& data = captRenderOutputDataUtil::Data();
			captRenderOutputDataUtil::BuildCameraList( data );
			int num_cameras = data.m_Cameras.GetNumberOfItems();
					
			//older mab scenes may have incorrect camera data after the scene is initially opened
			//so we need ot verify that the sizes of the 2 lists are the same
			if(data.m_CameraList.size() == num_cameras)
			{
				for (int i = 0; i < num_cameras; ++i)
				{
					data.m_CameraList[i].m_bCapture.SetValue( data.m_Cameras.GetValueFlag(i) );
				}
			}

			//	set the prefix to be the scene name
			itString current_scenename;
			current_scenename = docSingleDocumentMgr::GetFilename().GetLastName();
			current_scenename.StripExtension();
			captRenderOutputDataUtil::SetCurrentScene( current_scenename );

			//	set-up the render mode to launch directly
			//
			pCapMode->SetSkipCaptureOptions( true );

			modeModeMgr::Push(l_CaptureModeID);
			return Py_None;
		}

		//--------------------------------------------------------------------
		// Add scenes to a batch render
		//--------------------------------------------------------------------
		PyObject* add_to_batch(PyObject *self, PyObject *args)
		{
			const char *scene_location;
			if (!PyArg_ParseTuple(args, "s", &scene_location))
				return NULL;

			std::string scene_path(scene_location);
			
			//add the scene to the batch data
			cptrRenderBatchDataItem new_item;
			new_item.m_bChecked.SetValue(true);
			new_item.m_Filename.SetString(scene_path);

			cptrRenderBatchData& batchData = cptrRenderBatchDataUtil::Data();
			batchData.m_Scenes.push_back(new_item);
			
			return Py_None;
		}

		//--------------------------------------------------------------------
		// delete a scene from the batch render
		//--------------------------------------------------------------------
		PyObject* delete_from_batch(PyObject *self, PyObject *args)
		{
			const char *scene_location;
			if (!PyArg_ParseTuple(args, "s", &scene_location))
				return NULL;

			std::string scene_path(scene_location);
			
			//add the scene to the batch data
			cptrRenderBatchDataItem del_item;
			del_item.m_bChecked.SetValue(true);
			del_item.m_Filename.SetString(scene_path);

			cptrRenderBatchData& batchData = cptrRenderBatchDataUtil::Data();
			for(int i = 0; i < batchData.m_Scenes.size(); ++i)
			{
				if( batchData.m_Scenes[i].m_Filename.GetString() == scene_path )
					batchData.m_Scenes.erase( batchData.m_Scenes.begin(), batchData.m_Scenes.begin() + i );
			}
			
			return Py_None;
		}

		//--------------------------------------------------------------------
		// Save the batch render to a file
		//--------------------------------------------------------------------
		PyObject* save_batch_file(PyObject *self, PyObject *args)
		{
			const char *scene_location;
			if (!PyArg_ParseTuple(args, "s", &scene_location))
				return NULL;

			fsLocator sceneLocator;
			sceneLocator = fsLocator( itString(scene_location) );

			cptrRenderBatchData& batchData = cptrRenderBatchDataUtil::Data();
			cptrRenderBatchDataUtil::WriteData(sceneLocator, batchData);

			return Py_None;
		}

		//--------------------------------------------------------------------
		// Save the batch render to a file
		//--------------------------------------------------------------------
		PyObject* load_batch_file(PyObject *self, PyObject *args)
		{
			const char *scene_location;
			if (!PyArg_ParseTuple(args, "s", &scene_location))
				return NULL;

			fsLocator sceneLocator;
			sceneLocator = fsLocator( itString(scene_location) );

			cptrRenderBatchData& batchData = cptrRenderBatchDataUtil::Data();
			cptrRenderBatchDataUtil::ReadData(sceneLocator, batchData);

			return Py_None;
		}

		//--------------------------------------------------------------------
		// Render the scene without a gui
		//--------------------------------------------------------------------
		PyObject* batch_render_scene(PyObject *self, PyObject *args)
		{
			const char *batch_file;
			//if a file is specified use the batch file, if not, just use whatever is in 
			//the batch render object
			if (!PyArg_ParseTuple(args, "s", &batch_file))
				return Py_None;

			fsLocator batchLocator;
			batchLocator = fsLocator( itString(batch_file) );

			//	open the batch file and loop through the entries
			//  We'll check the file here, so cptrPackage doesn't try to open a message box if there's an error
			try
			{
				cptrRenderBatchData& batchData = cptrRenderBatchDataUtil::Data();
				cptrRenderBatchDataUtil::ReadData( batchLocator, batchData );
			}
			catch ( const envExceptionX& i_Ex )
			{
				std::string msg = "Error reading batch data, " + i_Ex.GetErrorMessage();
				DBG_ERROR(msg);
				return Py_None;
			}
			
			
			cptrPackage::LaunchBatchRender(batchLocator, false);
			return Py_None;
		}

		//--------------------------------------------------------------------
		// Render the scene without a gui
		//--------------------------------------------------------------------
		PyObject* skip_scene_with_errors(PyObject *self, PyObject *args)
		{
			bool flag;
			//if a file is specified use the batch file, if not, just use whatever is in 
			//the batch render object
			if (!PyArg_ParseTuple(args, "b", &flag))
				return Py_None;

			cptrRenderBatchDataUtil::SetSkipSceneWithErrors(flag);
			
			return Py_None;
		}
	}

	//--------------------------------------------------------------------
	// Add commands related to the selection list
	//--------------------------------------------------------------------
	void AddCommands(const std::string &i_ModuleName)
	{
		pythModules::AddCommand(i_ModuleName, 
			"render", "Render the current scene", render_scene);
		pythModules::AddCommand(i_ModuleName, 
			"addToBatch", "Add a scene to the batch data", add_to_batch);
		pythModules::AddCommand(i_ModuleName, 
			"deleteFromBatch", "Remove a scene from the batch data", delete_from_batch);
		pythModules::AddCommand(i_ModuleName, 
			"saveBatchFile", "Save the batch data to a file", save_batch_file);
		pythModules::AddCommand(i_ModuleName, 
			"loadBatchFile", "load a batch file", load_batch_file);
		pythModules::AddCommand(i_ModuleName, 
			"batchRender", "Perform a batch render on a batch file", batch_render_scene);
		pythModules::AddCommand(i_ModuleName, 
			"setSkipSceneWithErrors", "Skip current scene when there are errors occurred", skip_scene_with_errors);
	}
};

#endif