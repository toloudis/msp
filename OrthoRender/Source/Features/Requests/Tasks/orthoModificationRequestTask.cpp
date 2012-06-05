/********************************************************************************************\
**  orthoModificationRequestTask.hpp
**
**		Data for the ModificationRequests
**
**  Extra Large Technology
**  Copyright(C) 2008 - All Rights Reserved
\********************************************************************************************/

#include "Features/Requests/Tasks/orthoModificationRequestTask.hpp"

#include "Features/Capture/orthoAvatarDataUtil.hpp"
#include "Features/Requests/orthoRemoteCommandMgr.hpp"

#include "Core/App/appTime.hpp"
#include "Core/Dbg/dbgLog.hpp"
#include "Core/Ma/maConstants.hpp"


//--------------------------------------------------------------------
//	orthoModificationRequestExpressionTask ExecuteTask - return true if task handled
//--------------------------------------------------------------------
void orthoModificationRequestExpressionTask::ExecuteTask()
{
	DBG_ASSERT0( GetTaskName() == "expression", "Bad orthoModificationRequestExpressionTask name" );
	float start_time = appTime::GetTime();

	const char* c_Expression_Name = "Expression";
	orthoAvatarDataUtil::AddExpression( m_Name, c_Expression_Name, itString(m_FileName.c_str()), m_Weight );

	float end_time = appTime::GetTime();
	DBG_LOG2("Modification (%s) Execute took %6.3f seconds", GetTaskName().c_str(), (end_time - start_time));

	orthoRemoteCommandMgr::RemoteRenderData& rr = orthoRemoteCommandMgr::m_RenderParameters[0];
	if (rr.GetRenderResponse() != NULL)
	{
		rr.GetRenderResponse()->setModificationsProcessingTime((end_time - start_time)) ;
	}
}

//--------------------------------------------------------------------
//	orthoModificationRequestShowCharacterTask ExecuteTask - return true if task handled
//--------------------------------------------------------------------
void orthoModificationRequestShowCharacterTask::ExecuteTask()
{
	DBG_ASSERT0( GetTaskName() == "character", "Bad orthoModificationRequestShowCharacterTask name" );
	float start_time = appTime::GetTime();

	orthoAvatarDataUtil::ShowCharacter( m_FileName, true );

	float end_time = appTime::GetTime();
	DBG_LOG2("Modification (%s) Execute took %6.3f seconds", GetTaskName().c_str(), (end_time - start_time));

	orthoRemoteCommandMgr::RemoteRenderData& rr = orthoRemoteCommandMgr::m_RenderParameters[0];
	if (rr.GetRenderResponse() != NULL)
	{
		rr.GetRenderResponse()->setModificationsProcessingTime((end_time - start_time)) ;
	}
}

//--------------------------------------------------------------------
//	orthoModificationRequestHideCharacterTask ExecuteTask - return true if task handled
//--------------------------------------------------------------------
void orthoModificationRequestHideCharacterTask::ExecuteTask()
{
	DBG_ASSERT0( GetTaskName() == "character", "Bad orthoModificationRequestHideCharacterTask name" );
	float start_time = appTime::GetTime();

	orthoAvatarDataUtil::ShowCharacter( m_FileName, false );

	float end_time = appTime::GetTime();
	DBG_LOG2("Modification (%s) Execute took %6.3f seconds", GetTaskName().c_str(), (end_time - start_time));

	orthoRemoteCommandMgr::RemoteRenderData& rr = orthoRemoteCommandMgr::m_RenderParameters[0];
	if (rr.GetRenderResponse() != NULL)
	{
		rr.GetRenderResponse()->setModificationsProcessingTime((end_time - start_time)) ;
	}
}

//--------------------------------------------------------------------
//	orthoModificationRequestDeleteCharacterTask ExecuteTask - return true if task handled
//--------------------------------------------------------------------
void orthoModificationRequestDeleteCharacterTask::ExecuteTask()
{
	DBG_ASSERT0( GetTaskName() == "character", "Bad orthoModificationRequestDeleteCharacterTask name" );
	float start_time = appTime::GetTime();

	orthoRemoteCommandMgr::l_pSelectedObject = NULL;
	orthoAvatarDataUtil::DeleteCharacter( m_FileName );

	float end_time = appTime::GetTime();
	DBG_LOG2("Modification (%s) Execute took %6.3f seconds", GetTaskName().c_str(), (end_time - start_time));

	orthoRemoteCommandMgr::RemoteRenderData& rr = orthoRemoteCommandMgr::m_RenderParameters[0];
	if (rr.GetRenderResponse() != NULL)
	{
		rr.GetRenderResponse()->setModificationsProcessingTime((end_time - start_time)) ;
	}
}

//--------------------------------------------------------------------
//	orthoModificationRequestAttachmentTask ExecuteTask - return true if task handled
//--------------------------------------------------------------------
void orthoModificationRequestAttachmentTask::ExecuteTask()
{
	DBG_ASSERT0( GetTaskName() == "attachment", "Bad orthoModificationRequestAttachmentTask name" );
	float start_time = appTime::GetTime();

	orthoAvatarDataUtil::AttachPart( m_Name, m_JointName, m_FileName, true );

	float end_time = appTime::GetTime();
	DBG_LOG2("Modification (%s) Execute took %6.3f seconds", GetTaskName().c_str(), (end_time - start_time));

	orthoRemoteCommandMgr::RemoteRenderData& rr = orthoRemoteCommandMgr::m_RenderParameters[0];
	if (rr.GetRenderResponse() != NULL)
	{
		rr.GetRenderResponse()->setModificationsProcessingTime((end_time - start_time)) ;
	}
}

//--------------------------------------------------------------------
//	orthoModificationRequestDetachmentTask ExecuteTask - return true if task handled
//--------------------------------------------------------------------
void orthoModificationRequestDetachmentTask::ExecuteTask()
{
	DBG_ASSERT0( GetTaskName() == "detachment", "Bad orthoModificationRequestDetachmentTask name" );
	float start_time = appTime::GetTime();

	orthoAvatarDataUtil::AttachPart( m_Name, m_JointName, m_FileName, false );

	float end_time = appTime::GetTime();
	DBG_LOG2("Modification (%s) Execute took %6.3f seconds", GetTaskName().c_str(), (end_time - start_time));

	orthoRemoteCommandMgr::RemoteRenderData& rr = orthoRemoteCommandMgr::m_RenderParameters[0];
	if (rr.GetRenderResponse() != NULL)
	{
		rr.GetRenderResponse()->setModificationsProcessingTime((end_time - start_time)) ;
	}
}

//--------------------------------------------------------------------
//	orthoModificationRequestTextureTask ExecuteTask - return true if task handled
//--------------------------------------------------------------------
void orthoModificationRequestTextureTask::ExecuteTask()
{
	DBG_ASSERT0( GetTaskName() == "texture", "Bad orthoModificationRequestTextureTask name" );
	float start_time = appTime::GetTime();

	orthoAvatarDataUtil::ChangeTexture( m_ShaderName, m_LayerName, m_FileName);

	float end_time = appTime::GetTime();
	DBG_LOG2("Modification (%s) Execute took %6.3f seconds", GetTaskName().c_str(), (end_time - start_time));

	orthoRemoteCommandMgr::RemoteRenderData& rr = orthoRemoteCommandMgr::m_RenderParameters[0];
	if (rr.GetRenderResponse() != NULL)
	{
		rr.GetRenderResponse()->setModificationsProcessingTime((end_time - start_time)) ;
	}
}

//--------------------------------------------------------------------
//	orthoModificationRequestMaterialTask ExecuteTask - return true if task handled
//--------------------------------------------------------------------
void orthoModificationRequestMaterialTask::ExecuteTask()
{
	DBG_ASSERT0( GetTaskName() == "material", "Bad orthoModificationRequestMaterialTask name" );
	float start_time = appTime::GetTime();

	orthoAvatarDataUtil::ChangeMaterial( m_SurfaceName, m_FileName);

	float end_time = appTime::GetTime();
	DBG_LOG2("Modification (%s) Execute took %6.3f seconds", GetTaskName().c_str(), (end_time - start_time));

	orthoRemoteCommandMgr::RemoteRenderData& rr = orthoRemoteCommandMgr::m_RenderParameters[0];
	if (rr.GetRenderResponse() != NULL)
	{
		rr.GetRenderResponse()->setModificationsProcessingTime((end_time - start_time)) ;
	}
}

//--------------------------------------------------------------------
//	orthoModificationRequestColorTask ExecuteTask - return true if task handled
//--------------------------------------------------------------------
void orthoModificationRequestColorTask::ExecuteTask()
{
	DBG_ASSERT0( GetTaskName() == "color", "Bad orthoModificationRequestColorTask name" );
	float start_time = appTime::GetTime();

	orthoAvatarDataUtil::MaterialChanged( m_SurfaceName, m_Color );

	float end_time = appTime::GetTime();
	DBG_LOG2("Modification (%s) Execute took %6.3f seconds", GetTaskName().c_str(), (end_time - start_time));

	orthoRemoteCommandMgr::RemoteRenderData& rr = orthoRemoteCommandMgr::m_RenderParameters[0];
	if (rr.GetRenderResponse() != NULL)
	{
		rr.GetRenderResponse()->setModificationsProcessingTime((end_time - start_time)) ;
	}
}

//--------------------------------------------------------------------
//	orthoModificationRequestAnimationAddTask ExecuteTask - return true if task handled
//--------------------------------------------------------------------
void orthoModificationRequestAnimationAddTask::ExecuteTask()
{
	DBG_ASSERT0( GetTaskName() == "animation", "Bad orthoModificationRequestAnimationAddTask name" );
	float start_time = appTime::GetTime();

	std::vector<std::string> cameras;
	cameras.push_back( m_CameraName );
	orthoAvatarDataUtil::AddAnimation( m_FileName, m_Directions, cameras );

	float end_time = appTime::GetTime();
	DBG_LOG2("Modification (%s) Execute took %6.3f seconds", GetTaskName().c_str(), (end_time - start_time));

	orthoRemoteCommandMgr::RemoteRenderData& rr = orthoRemoteCommandMgr::m_RenderParameters[0];
	if (rr.GetRenderResponse() != NULL)
	{
		rr.GetRenderResponse()->setModificationsProcessingTime((end_time - start_time)) ;
	}
}

//--------------------------------------------------------------------
//	orthoModificationRequestAnimationDeleteAllTask ExecuteTask - return true if task handled
//--------------------------------------------------------------------
void orthoModificationRequestAnimationDeleteAllTask::ExecuteTask()
{
	DBG_ASSERT0( GetTaskName() == "deleteallanimation", "Bad orthoModificationRequestAnimationDeleteAllTask name" );
	float start_time = appTime::GetTime();

	orthoAvatarDataUtil::DeleteSceneDrivers();

	float end_time = appTime::GetTime();
	DBG_LOG2("Modification (%s) Execute took %6.3f seconds", GetTaskName().c_str(), (end_time - start_time));

	orthoRemoteCommandMgr::RemoteRenderData& rr = orthoRemoteCommandMgr::m_RenderParameters[0];
	if (rr.GetRenderResponse() != NULL)
	{
		rr.GetRenderResponse()->setModificationsProcessingTime((end_time - start_time)) ;
	}
}

//--------------------------------------------------------------------
//	orthoModificationRequestScaleCharacterTask ExecuteTask - return true if task handled
//--------------------------------------------------------------------
void orthoModificationRequestScaleCharacterTask::ExecuteTask()
{
	DBG_ASSERT0( GetTaskName() == "scale", "Bad orthoModificationRequestScaleCharacterTask name" );
	float start_time = appTime::GetTime();

	orthoAvatarDataUtil::ScaleCharacter( m_Scale );

	float end_time = appTime::GetTime();
	DBG_LOG2("Modification (%s) Execute took %6.3f seconds", GetTaskName().c_str(), (end_time - start_time));

	orthoRemoteCommandMgr::RemoteRenderData& rr = orthoRemoteCommandMgr::m_RenderParameters[0];
	if (rr.GetRenderResponse() != NULL)
	{
		rr.GetRenderResponse()->setModificationsProcessingTime((end_time - start_time)) ;
	}
}

//--------------------------------------------------------------------
//	orthoModificationRequestRotateCharacterTask ExecuteTask - return true if task handled
//--------------------------------------------------------------------
void orthoModificationRequestRotateCharacterTask::ExecuteTask()
{
	DBG_ASSERT0( GetTaskName() == "rotate", "Bad orthoModificationRequestRotateCharacterTask name" );
	float start_time = appTime::GetTime();

	orthoAvatarDataUtil::RotateCharacter( m_Angle );

	float end_time = appTime::GetTime();
	DBG_LOG2("Modification (%s) Execute took %6.3f seconds", GetTaskName().c_str(), (end_time - start_time));

	orthoRemoteCommandMgr::RemoteRenderData& rr = orthoRemoteCommandMgr::m_RenderParameters[0];
	if (rr.GetRenderResponse() != NULL)
	{
		rr.GetRenderResponse()->setModificationsProcessingTime((end_time - start_time)) ;
	}
}

//--------------------------------------------------------------------
//	orthoModificationRequestChangeCameraTask ExecuteTask - return true if task handled
//--------------------------------------------------------------------
void orthoModificationRequestChangeCameraTask::ExecuteTask()
{
	DBG_ASSERT0( GetTaskName() == "camera", "Bad orthoModificationRequestRotateCharacterTask name" );
	float start_time = appTime::GetTime();

	orthoAvatarDataUtil::CameraChange( m_CameraName, m_Data );

	float end_time = appTime::GetTime();
	DBG_LOG2("Modification (%s) Execute took %6.3f seconds", GetTaskName().c_str(), (end_time - start_time));

	orthoRemoteCommandMgr::RemoteRenderData& rr = orthoRemoteCommandMgr::m_RenderParameters[0];
	if (rr.GetRenderResponse() != NULL)
	{
		rr.GetRenderResponse()->setModificationsProcessingTime((end_time - start_time)) ;
	}
}