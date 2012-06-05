/********************************************************************************************\
**  orthoSceneRequestTask.cpp
**
**		Data for the SceneRequests
**
**  Extra Large Technology
**  Copyright(C) 2008 - All Rights Reserved
\********************************************************************************************/

#include "Features/Requests/Tasks/orthoObjectsRequestTask.hpp"
#include "Features/Requests/orthoRemoteCommandMgr.hpp"
#include "Features/Capture/orthoAvatarDataUtil.hpp"

#include "Core/App/appTime.hpp"
#include "Core/Dbg/dbgLog.hpp"
#include "Tool/doc/docSingleDocumentMgr.hpp"

void orthoObjectsRequestModelTask::ExecuteTask()
{
	DBG_ASSERT0( GetTaskName() == "model", "Bad orthoObjectsRequestModelTask name" );
	DBG_ERROR0( "Cannot execute an individual task, use orthoObjectsRequestModelsTask" );
}

//--------------------------------------------------------------------
//	orthoObjectsRequestModelsTask ExecuteTask - return true if task handled
//--------------------------------------------------------------------
void orthoObjectsRequestModelsTask::ExecuteTask()
{
	DBG_ASSERT0( GetTaskName() == "models", "Bad orthoObjectsRequestModelsTask name" );

	float start_time = appTime::GetTime();

	fsLocator current_scene = docSingleDocumentMgr::GetFilename();
	if( current_scene.GetNumNames() > 0 )	//make sure we have a scene first
	{
		try
		{
			orthoRemoteCommandMgr::l_pSelectedObject = NULL;
			std::vector<std::string> names;
			std::vector<std::string> filenames;
			std::vector<bool> is_base_models;
			for (int i=0; i < m_Tasks.size(); ++i)
			{
				names.push_back( m_Tasks[i].m_Name );
				filenames.push_back( m_Tasks[i].m_FileName );
				is_base_models.push_back( m_Tasks[i].m_bIsBaseModel );
			}
			orthoAvatarDataUtil::LoadCharacters( names, filenames, is_base_models );
		}
		catch( ... )
		{
			//DBG_ERROR1("Trying to load a character but the file name (%s) does not exist", pTask->m_FileName.c_str() ); //orthoRemoteCommandMgr::m_RenderCharacterFileName.c_str() );
		}
	}
	else
	{
		DBG_ERROR0( "Cannot load models without a valid scene first." );
	}

	float end_time = appTime::GetTime();
	DBG_LOG2("Objects (%s) Execute took %6.3f seconds", GetTaskName().c_str(), (end_time - start_time));

	orthoRemoteCommandMgr::RemoteRenderData& rr = orthoRemoteCommandMgr::m_RenderParameters[0];
	if (rr.GetRenderResponse() != NULL) {
		rr.GetRenderResponse()->setObjectsProcessingTime((end_time - start_time)) ;
	}
}