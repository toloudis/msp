/*****************************************************************************
**	camsFollowUtil.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Support/cams/camsFollowUtil.hpp"

#include "Support/cams/camsCameraMgr.hpp"
#include "Support/cams/camsDirectorsCutMgr.hpp"

#include "Tool/cam3d/cam3dMgr.hpp"
#include "Core/dbg/dbgLog.hpp"


//============================================================================
//============================================================================
namespace camsFollowUtil
{
	namespace
	{
		const int c_EditorCameraIndex = -1;
		int l_FollowIndex = c_EditorCameraIndex;
	}	// end of namespace

	//--------------------------------------------------------------------
	// Return total number of cameras and directors cuts
	//--------------------------------------------------------------------
	int GetTotalCount()
	{
		return camsCameraMgr::GetNumCameras() + camsDirectorsCutMgr::GetNumDirectorsCuts();
	}

	//--------------------------------------------------------------------
	// Index of scripted camera to follow,
	// use -1 for editor camera
	//--------------------------------------------------------------------
	int GetFollowIndex()
	{
		return l_FollowIndex;
	}
	void SetFollowIndex(int i_Index)
	{
		l_FollowIndex = i_Index;
	}

	//--------------------------------------------------------------------
	//	move to the next index in the follow list (and cycle around)
	//--------------------------------------------------------------------
	void SetFollowIndexNext()
	{
		l_FollowIndex += 1;
		if (l_FollowIndex >= GetTotalCount())
		{
			l_FollowIndex = c_EditorCameraIndex;
		}
	}
	void SetFollowIndexPrev()
	{
		l_FollowIndex -= 1;
		if (l_FollowIndex < c_EditorCameraIndex)
		{
			l_FollowIndex = GetTotalCount() - 1;
		}
	}

	//--------------------------------------------------------------------
	// Set FollowIndex to appropriate negative values without
	//	"magic numbers"
	//--------------------------------------------------------------------
	void SetEditorCamera()
	{
		SetFollowIndex(c_EditorCameraIndex);
	}
	//void SetDirectorsCut()
	//{
	//	SetFollowIndex(c_DirectorsCutIndex);
	//}

	//--------------------------------------------------------------------
	// Check Follow Index without use of magic numbers
	//--------------------------------------------------------------------
	bool IsEditorCamera()
	{
		return (l_FollowIndex == c_EditorCameraIndex);
	}
	bool IsDirectorsCut()
	{
		// This is a director's cut if the follow index is greater 
		// than the number of cameras.
		return (l_FollowIndex >= camsCameraMgr::GetNumCameras());
	}

	//--------------------------------------------------------------------
	// Get name of currently selected camera
	//--------------------------------------------------------------------
	void GetCurrentCameraName( nameString &o_Name )
	{
		//	return the appropriate name
		switch (l_FollowIndex)
		{
		case c_EditorCameraIndex:	// editor
			o_Name.SetString( std::string("EC") );
			break;
		//case c_DirectorsCutIndex:	// director's
		//	o_Name.SetString( std::string("DC") );
		//	break;
		default:
			//DBG_LOG2("Follow index: %d Num Names %d", l_FollowIndex, l_Names.size());
			if (l_FollowIndex < camsCameraMgr::GetNumCameras())
				camsCameraMgr::GetCameraName(l_FollowIndex, o_Name);
			else
			{
				int dcut_index = l_FollowIndex - camsCameraMgr::GetNumCameras();
				if (dcut_index < camsDirectorsCutMgr::GetNumDirectorsCuts())
					camsDirectorsCutMgr::GetDirectorsCutName(dcut_index, o_Name);
				else
					o_Name.SetString( std::string("NC") ); // no camera, an error condition?
			}
			break;
		}
	}
	
	//--------------------------------------------------------------------
	// Get description of camera that is curently being used
	//--------------------------------------------------------------------
	void GetCurrentCameraDescription( std::string& o_Description )
	{
		o_Description.clear();

		//	return the appropriate name
		switch (l_FollowIndex)
		{
			//case c_DirectorsCutIndex:	// director's
			case c_EditorCameraIndex:	// editor
				break;
			default:
				//DBG_LOG2("Follow index: %d Num Names %d", l_FollowIndex, l_Names.size());
				if (l_FollowIndex < camsCameraMgr::GetNumCameras())
					camsCameraMgr::GetCameraDescription(l_FollowIndex, o_Description);
				else
				{
					int dcut_index = l_FollowIndex - camsCameraMgr::GetNumCameras();
					if (dcut_index < camsDirectorsCutMgr::GetNumDirectorsCuts())
						camsDirectorsCutMgr::GetDirectorsCutDescription(dcut_index, o_Description);
				}
				break;
		}
	}

	//--------------------------------------------------------------------
	// Update camera if following one of the scripted cameras
	//--------------------------------------------------------------------
	camCamera* GetFollowCamera()
	{
		if (camsFollowUtil::IsEditorCamera())
		{
			// Editor camera
			return &cam3dMgr::GetEditorCamera();
		}
		else if (l_FollowIndex < camsCameraMgr::GetNumCameras())
		{
			// Scripted camera
			return ConfigureCamera(camsFollowUtil::GetFollowIndex());
		}
		else 
		{
			// Use camsDirectorsCutMgr to get index for camera
			int num_cams = camsCameraMgr::GetNumCameras();
			int dcut_index = l_FollowIndex - num_cams;
			int cam_index = camsDirectorsCutMgr::GetCameraIndex(dcut_index);
			if (cam_index >= 0 && cam_index < num_cams)
				return ConfigureCamera(cam_index);
			else
				return &cam3dMgr::GetEditorCamera(); // director's cut returned an index out of range
		}

		// Should set other camera values?, restoring them to default in editor camera
	}

	//--------------------------------------------------------------------
	// Set up current camera in position of camera with given
	//	index.  This is used in multiple camera views immediately
	//	before a render.
	//--------------------------------------------------------------------
	camCamera* ConfigureCamera(int i_CameraIndex)
	{
		if (i_CameraIndex >= 0 && i_CameraIndex < camsCameraMgr::GetNumCameras())
		{
			camCamera *pCamera = camsCameraMgr::GetCamera(i_CameraIndex);
			DBG_ASSERT1( pCamera != 0, "No scripted camera at this index (%d)", i_CameraIndex );

			// Set cam3dMgr to use the scripted camera
			//cam3dMgr::UseScriptedCamera(pCamera);
			pCamera->Set();

			return pCamera;
		}
		//else if ( i_CameraIndex == c_EditorCameraIndex )
		//{
			// Should editor camera have fixed cipping plane and FOV?
			//camCamera &camera = cam3dMgr::GetEditorCamera();
			//camera.SetClip(0.1f, 1000.0f);
			//camera.SetFOV(90.0f);
		//}

		// Editor camera
		return &cam3dMgr::GetEditorCamera();
		//return NULL;
	}

}	// end of namespace
