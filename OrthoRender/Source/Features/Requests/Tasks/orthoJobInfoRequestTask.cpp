/********************************************************************************************\
**  orthoJobInfoRequestTask.cpp
**
**		Data for the JobInfoRequests
**
**  Extra Large Technology
**  Copyright(C) 2008 - All Rights Reserved
\********************************************************************************************/

#include "Features/Requests/Tasks/orthoJobInfoRequestTask.hpp"
#include "Features/Requests/orthoRemoteCommandMgr.hpp"
#include "Core/App/appTime.hpp"

#include "Core/Dbg/dbgLog.hpp"


//--------------------------------------------------------------------
//	orthoJobInfoRequestTask ExecuteTask - return true if task handled
//--------------------------------------------------------------------

void orthoJobInfoRequestTask::ExecuteTask()
{
	DBG_ASSERT0( GetTaskName() == "jobinfo", "Bad orthoJobInfoRequestTask name" );
	float start_time = appTime::GetTime();

	orthoRemoteCommandMgr::l_pSelectedObject = NULL;

	orthoRemoteCommandMgr::RemoteRenderData& rr = orthoRemoteCommandMgr::m_RenderParameters[0];
	rr.renderName = m_JobName;

	float end_time = appTime::GetTime();
	DBG_LOG2("JobInfo (%s) Execute took %6.3f seconds", GetTaskName().c_str(), (end_time - start_time));

	if (rr.GetRenderResponse() != NULL) {
		rr.GetRenderResponse()->setJobProcessingTime((end_time - start_time)) ;
	}
}