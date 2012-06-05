/********************************************************************************************\
**  orthoSceneRequestTask.cpp
**
**		Data for the SceneRequests
**
**  Extra Large Technology
**  Copyright(C) 2008 - All Rights Reserved
\********************************************************************************************/

#include "Features/Requests/Tasks/orthoSceneRequestTask.hpp"
#include "Features/Requests/orthoRemoteCommandMgr.hpp"
#include "Core/Fs/fsFileUtil.hpp"
#include "Core/App/appTime.hpp"
#include "Core/Dbg/dbgLog.hpp"
#include "Tool/doc/docSingleDocumentMgr.hpp"
#include "Tool/gui/guiSingleDocHandler.hpp"

//--------------------------------------------------------------------
//	data ExecuteTask - return true if task handled
//--------------------------------------------------------------------
void orthoSceneRequestLoadTask::ExecuteTask()
{
	float start_time = appTime::GetTime();

	DBG_ASSERT0( GetTaskName() == "scene", "Bad SceneRequestLoadTask name" );

	orthoRemoteCommandMgr::l_pSelectedObject = NULL;
	fsLocator scenefile;
	fsFileUtil::ANSIFilenameToLocator(m_FileName.c_str(), scenefile );
	std::string sf;
	fsFileUtil::LocatorToANSIFilename(scenefile, sf);

	//	only load the scene if the scene filenames are different
	//
	fsLocator current_scene = docSingleDocumentMgr::GetFilename();
	if (current_scene != scenefile)
	{
		DBG_LOG1("Loading %s", sf.c_str());
		try 
		{
			const bool bSKIP_SAVE_CHECK = true;
			guiSingleDocHandler::Open(scenefile, bSKIP_SAVE_CHECK);
			docSingleDocumentMgr::SetFilename(scenefile);
		}
		catch ( ... ) 
		{
			DBG_ERROR1("Error loading scene (%s)", sf.c_str() );
		}
	}

	float end_time = appTime::GetTime();
	DBG_LOG2("Scene (%s) Execute took %6.3f seconds", GetTaskName().c_str(), (end_time - start_time));

	orthoRemoteCommandMgr::RemoteRenderData& rr = orthoRemoteCommandMgr::m_RenderParameters[0];
	if (rr.GetRenderResponse() != NULL)
	{
		rr.GetRenderResponse()->setSceneProcessingTime((end_time - start_time)) ;
	}
}
