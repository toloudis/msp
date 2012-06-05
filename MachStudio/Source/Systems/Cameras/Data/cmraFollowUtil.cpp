/*****************************************************************************
**	cmraFollowUtil.cpp
**
**	Namespace with routines for matching editor and scripted cameras
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Systems/Cameras/Data/cmraFollowUtil.hpp"

#include "Systems/Cameras/Object/cmraScriptObject.hpp"
//#include "Systems/Cameras/Cue/cmraCueDataUtil.hpp"
#include "Systems/Cameras/Object/cmraObjectMgr.hpp"

#include "Support/cams/camsCameraMgr.hpp"
#include "Support/cams/camsFollowUtil.hpp"
#include "Support/tmln/tmlnTimeLine.hpp"

#include "Graphics/cam/camCamera.hpp"
#include "Tool/cam3d/cam3dMgr.hpp"
#include "Core/ma/maConstants.hpp"


//============================================================================
//============================================================================
namespace cmraFollowUtil
{
	namespace
	{

	}	// end of namespace

	//--------------------------------------------------------------------
	// Get data from the current camera view
	//--------------------------------------------------------------------
	void GetCurrentCamera(cmraCameraData& o_CamData)
	{
		camCamera& cam = cam3dMgr::GetCamera();

		o_CamData.m_Far			= cam.GetFarClip();
		o_CamData.m_FOV			= cam.GetFOV();
		o_CamData.m_Near		= cam.GetNearClip();
		o_CamData.m_Position	= cam.GetPosition();
		o_CamData.m_Target		= cam.GetTarget();
	}

	//--------------------------------------------------------------------
	// Get position and target of current camera view
	//--------------------------------------------------------------------
	void GetCurrentCamera(maPoint3d &o_Position, maPoint3d &o_Target)
	{
		o_Position = cam3dMgr::GetCamera().GetPosition();
		o_Target = cam3dMgr::GetCamera().GetTarget();

		//DBG_LOG3("Cam pos: %f %f %f", o_Position.m_X, o_Position.m_Y, o_Position.m_Z);
		//DBG_LOG3("Cam tgt: %f %f %f", o_Target.m_X, o_Target.m_Y, o_Target.m_Z);
	}


}	// end of namespace
