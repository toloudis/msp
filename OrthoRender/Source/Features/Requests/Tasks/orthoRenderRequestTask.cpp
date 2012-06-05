/********************************************************************************************\
**  orthoRenderRequestTask.hpp
**
**		Data for the RenderRequests
**
**  Extra Large Technology
**  Copyright(C) 2008 - All Rights Reserved
\********************************************************************************************/

#include "Features/Requests/Tasks/orthoRequestTaskUtil.hpp"

#include "Features/Requests/orthoRemoteCommandMgr.hpp"
#include "Features/Capture/cptrRenderOutputDataUtil.hpp"
#include "Features/Capture/orthoPackage.hpp"
#include "Support/tmln/tmlnTimeLine.hpp"
#include "Core/App/appTime.hpp"
#include "Core/Dbg/dbgLog.hpp"


void orthoRenderRequestTask::ExecuteTask()
{
	DBG_ERROR0( "Bad orthoRenderRequestTask, must be a Frame or Sequence." );
}

//--------------------------------------------------------------------
//	orthoRenderRequestSequenceTask ExecuteTask - return true if task handled
//--------------------------------------------------------------------
void orthoRenderRequestSequenceTask::ExecuteTask()
{
	DBG_ASSERT0( GetTaskName() == "sequence", "Bad orthoRenderRequestSequenceTask name" );

	float start_time = appTime::GetTime();

	orthoRemoteCommandMgr::m_RenderParameters[0].renderName.clear();
	orthoRemoteCommandMgr::m_RenderParameters[0].responseFormat.clear();
	orthoRemoteCommandMgr::m_RenderParameters[0].responseFormat.append("XML");

	CreateRenderResponse( orthoRemoteCommandMgr::m_RenderModelName, orthoRemoteCommandMgr::m_RenderParameters[0].renderName );
	orthoRemoteCommandMgr::m_RenderParameters[0].SetRenderResponse( GetRenderResponse() );
	orthoRemoteCommandMgr::m_RenderParameters[0].renderWidth		= m_Width;
	orthoRemoteCommandMgr::m_RenderParameters[0].renderHeight		= m_Height;
	orthoRemoteCommandMgr::m_RenderParameters[0].renderFPS			= m_fFrameRate;
	orthoRemoteCommandMgr::m_RenderParameters[0].renderCamera		= m_CameraName;
	orthoRemoteCommandMgr::m_RenderParameters[0].renderTimePoint	= tmlnTimeLine::GetValue();
	orthoRemoteCommandMgr::m_RenderParameters[0].imageFormat		= m_ImageFormat;
	orthoRemoteCommandMgr::m_RenderParameters[0].maskFormat			= m_MaskFormat;

	//	trigger render mode
	if (orthoRemoteCommandMgr::m_RenderParameters[0].renderCamera.compare("ALL") == 0) 
	{
		cptrPackage::LaunchRender(	orthoRemoteCommandMgr::m_RenderParameters[0].renderWidth,
			orthoRemoteCommandMgr::m_RenderParameters[0].renderHeight );
		orthoRemoteCommandMgr::l_CameraToRender = "FRONTAL";
	}
	else 
	{
		if (orthoRemoteCommandMgr::m_RenderParameters[0].renderCamera.compare("ROTATION") == 0) 
		{
			cptrPackage::LaunchRender(	orthoRemoteCommandMgr::m_RenderParameters[0].renderWidth,
				orthoRemoteCommandMgr::m_RenderParameters[0].renderHeight );
			orthoRemoteCommandMgr::l_CameraToRender = "FRONTAL";
		}
		else 
		{
			//cptrRenderOutputData& data = cptrRenderOutputDataUtil::Data();
			//data.

			cptrPackage::LaunchRender(	orthoRemoteCommandMgr::m_RenderParameters[0].renderWidth,
				orthoRemoteCommandMgr::m_RenderParameters[0].renderHeight );
			orthoRemoteCommandMgr::l_CameraToRender = m_CameraName;
		}
		cptrRenderOutputDataUtil::SetOneCameraToRender(orthoRemoteCommandMgr::l_CameraToRender);
	}
	float animLength = tmlnTimeLine::GetMaximum() - tmlnTimeLine::GetMinimum();
	DBG_WARNING3( "Rendering %d frame(s) of camera %s at %2.1f FPS", (int)(animLength * m_fFrameRate)+1, orthoRemoteCommandMgr::l_CameraToRender.c_str(), m_fFrameRate);

	float end_time = appTime::GetTime();
	DBG_LOG2("RenderRequest (%s) Execute took %6.3f seconds", GetTaskName().c_str(), (end_time - start_time));

	orthoRemoteCommandMgr::RemoteRenderData& rr = orthoRemoteCommandMgr::m_RenderParameters[0];
	if (rr.GetRenderResponse() != NULL) {
		rr.GetRenderResponse()->setRenderRequestProcessingTime((end_time - start_time)) ;
	}
}

//--------------------------------------------------------------------
//	orthoRenderRequestFrameTask ExecuteTask - return true if task handled
//--------------------------------------------------------------------
void orthoRenderRequestFrameTask::ExecuteTask()
{
	DBG_ASSERT0( GetTaskName() == "frame", "Bad orthoRenderRequestFrameTask name" );

	float start_time = appTime::GetTime();

	orthoRemoteCommandMgr::m_RenderParameters[0].renderName.clear();
	orthoRemoteCommandMgr::m_RenderParameters[0].responseFormat.clear();
	orthoRemoteCommandMgr::m_RenderParameters[0].responseFormat.append("XML");

	CreateRenderResponse( orthoRemoteCommandMgr::m_RenderModelName, orthoRemoteCommandMgr::m_RenderParameters[0].renderName );

	orthoRemoteCommandMgr::m_RenderParameters[0].SetRenderResponse( GetRenderResponse() );
	orthoRemoteCommandMgr::m_RenderParameters[0].renderWidth		= m_Width;
	orthoRemoteCommandMgr::m_RenderParameters[0].renderHeight		= m_Height;
	orthoRemoteCommandMgr::m_RenderParameters[0].renderStartWidth	= m_StartWidth;
	orthoRemoteCommandMgr::m_RenderParameters[0].renderStartHeight	= m_StartHeight;
	orthoRemoteCommandMgr::m_RenderParameters[0].renderCamera		= m_CameraName;
	orthoRemoteCommandMgr::m_RenderParameters[0].renderTimePoint	= m_fTime;
	orthoRemoteCommandMgr::m_RenderParameters[0].imageFormat		= m_ImageFormat;
	orthoRemoteCommandMgr::m_RenderParameters[0].maskFormat			= m_MaskFormat;

	//	trigger render mode
	if ((orthoRemoteCommandMgr::m_RenderParameters[0].renderWidth != orthoRemoteCommandMgr::m_RenderParameters[0].renderStartWidth) || 
		(orthoRemoteCommandMgr::m_RenderParameters[0].renderHeight != orthoRemoteCommandMgr::m_RenderParameters[0].renderStartHeight)) 
	{
		int minWidth = min(orthoRemoteCommandMgr::m_RenderParameters[0].renderWidth, orthoRemoteCommandMgr::m_RenderParameters[0].renderStartWidth);
		int maxWidth = max(orthoRemoteCommandMgr::m_RenderParameters[0].renderWidth, orthoRemoteCommandMgr::m_RenderParameters[0].renderStartWidth);
		int minHeight = min(orthoRemoteCommandMgr::m_RenderParameters[0].renderHeight, orthoRemoteCommandMgr::m_RenderParameters[0].renderStartHeight);
		int maxHeight = max(orthoRemoteCommandMgr::m_RenderParameters[0].renderHeight, orthoRemoteCommandMgr::m_RenderParameters[0].renderStartHeight);
		cptrPackage::LaunchProgressiveFrameRender(
			minWidth,
			minHeight,
			maxWidth,
			maxHeight);
	}
	else 
	{
		cptrPackage::LaunchFrameRender( orthoRemoteCommandMgr::m_RenderParameters[0].renderWidth,
			orthoRemoteCommandMgr::m_RenderParameters[0].renderHeight );
	}

	orthoRemoteCommandMgr::l_CameraToRender = m_CameraName;
	cptrRenderOutputDataUtil::SetOneCameraToRender(orthoRemoteCommandMgr::l_CameraToRender);

	DBG_WARNING2( "Rendering a frame of camera %s at time %6.3f", m_CameraName.c_str(), m_fTime );

	float end_time = appTime::GetTime();
	DBG_LOG2("RenderRequest (%s) Execute took %6.3f seconds", GetTaskName().c_str(), (end_time - start_time));

	orthoRemoteCommandMgr::RemoteRenderData& rr = orthoRemoteCommandMgr::m_RenderParameters[0];
	if (rr.GetRenderResponse() != NULL) {
		rr.GetRenderResponse()->setRenderRequestProcessingTime((end_time - start_time)) ;
	}
}
