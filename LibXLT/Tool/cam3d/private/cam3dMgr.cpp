/*****************************************************************************
**	cam3dMgr.hpp
**
**		The cam3dMgr keeps track of cameras for the level and
**	the editor camera for the modes.
**
**	StudioGPU
**	Copyright(C) 2000-2002 - All Rights Reserved
\****************************************************************************/
#include "Tool/cam3d/cam3dMgr.hpp"

#include "Core/app/appSimTime.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Core/ma/maConstants.hpp"
#include "Core/ma/maFunctions.hpp"
#include "Graphics/Cam/camCameraManipTarget.hpp"
#include "Tool/cam3d/cam3dManipMaya.hpp"
#include "Tool/cam3d/cam3dManipOrthoPan.hpp"
#include "Tool/gpx/gpxCamera.hpp"

#include <algorithm>
#include <vector>


//============================================================================
//============================================================================
namespace cam3dMgr
{
	namespace
	{
		// Switched editor camera to shared_ptr so that the application
		// can share access to it.
		shared_ptr<camCamera> l_EditorCamera(new camCamera);

		camCamera l_TopCamera, l_FrontCamera, l_SideCamera; // 3 Ortho cameras
		camCamera* l_pScriptedCamera = NULL;
		camCameraManip* l_pCameraManip = NULL; // points to current manip, not owned
		std::vector<camCameraManip*> l_CameraManips;

		// Camera proxies so that camera manip classes can operate in multi-threaded mode
		shared_ptr<gpxCamera> l_EditorCameraProxy;
		shared_ptr<gpxCamera> l_TopCameraProxy;
		shared_ptr<gpxCamera> l_FrontCameraProxy;
		shared_ptr<gpxCamera> l_SideCameraProxy;

		// Need this wrapper to apply camera manipulators to a camCamera or gpxCamera
		camCameraManipTargetWrapper<camCamera> l_CameraWrapper;
		camCameraManipTargetWrapper<gpxCamera> l_CameraProxyWrapper;

		bool l_bEnabled = true;

		const float l_fNearClip  = 0.3f;

		//--------------------------------------------------------------------
		// Proper camera cleanup
		//--------------------------------------------------------------------
		void manip_cleanup( camCameraManip* i_pManip )
		{
			i_pManip->Detach();
			delete i_pManip;
		}

		//--------------------------------------------------------------------
		//	compute_camera_space_min_max determines the min and max points
		//	of the bounding box in camera space
		//--------------------------------------------------------------------
		void compute_camera_space_min_max( const maMatrix4x4& i_Matrix,
										   const maAxisBox& i_BBox,
										   maPoint3d& o_MinPoint,
										   maPoint3d& o_MaxPoint)
		{
			maPoint3d point = i_Matrix * i_BBox.GetBoxPoint(0);

			// initialize the min and max points to the first point
			o_MinPoint = point;
			o_MaxPoint = point;

			int i = 0;
			for ( i = 1; i < 8; ++i )
			{
				point = i_Matrix * i_BBox.GetBoxPoint(i);

				if ( point.GetX() < o_MinPoint.GetX() )
					o_MinPoint.SetX( point.GetX() );
				if ( point.GetX() > o_MaxPoint.GetX() )
					o_MaxPoint.SetX( point.GetX() );
				if ( point.GetY() < o_MinPoint.GetY() )
					o_MinPoint.SetY( point.GetY() );
				if ( point.GetY() > o_MaxPoint.GetY() )
					o_MaxPoint.SetY( point.GetY() );
			}
		}

	} // local namespace

	//--------------------------------------------------------------------
	//	Initialize will be called when the level is new and default fog
	//	should be added
	//--------------------------------------------------------------------
	void Initialize()
	{
		// All cameras should begin by matching the window's aspect ratio.
		l_EditorCamera->SetMatchAspectToWindow(true);
		l_TopCamera.SetMatchAspectToWindow(true);
		l_FrontCamera.SetMatchAspectToWindow(true);
		l_SideCamera.SetMatchAspectToWindow(true);

		// initialize the camera
		//int width, height;
		//g2dScreen::GetDimensions( width, height );
		//l_Camera.SetAspect( float( width ) / height );
		l_EditorCamera->SetAspect( 4.0f / 3.0f );
		l_EditorCamera->SetFOV( 90.0f );
		l_EditorCamera->SetClip(l_fNearClip, 10000.0f);

		//l_ScriptedCamera.SetAspect( 4.0f / 3.0f );
		//l_ScriptedCamera.SetFOV( 90.0f );
		//l_ScriptedCamera.SetClip(0.1f, 1000.0f);

		// Orthographic cameras
		l_TopCamera.LookAt(maPoint3d(0,100,0), maPoint3d(0,0,0), maVector3d(0,0,-1));
		l_TopCamera.SetOrthographic(true);
		l_TopCamera.SetOrthoWidth(100.0f);
		l_FrontCamera.LookAt(maPoint3d(0,0,100), maPoint3d(0,0,0), maVector3d(0,1,0));
		l_FrontCamera.SetOrthographic(true);
		l_FrontCamera.SetOrthoWidth(100.0f);
		l_SideCamera.LookAt(maPoint3d(100,0,0), maPoint3d(0,0,0), maVector3d(0,1,0));
		l_SideCamera.SetOrthographic(true);
		l_SideCamera.SetOrthoWidth(100.0f);

		// Create the camera manips
		l_CameraManips.resize(e_NumManipModes);
		cam3dManipMaya *pDefaultManip = new cam3dManipMaya();
		l_CameraManips[e_MayaOrbit] = pDefaultManip;
		l_CameraManips[e_OrthoPan] = new cam3dManipOrthoPan();

		l_pCameraManip = pDefaultManip;
		// Use wrapper class to attach manip to camera
		l_CameraWrapper.ChangeManipTarget(l_EditorCamera.get());
		pDefaultManip->Attach(&l_CameraWrapper);
		pDefaultManip->SetRadius( 40.0f );
		pDefaultManip->SetZoomFactor( 40.0f );
	}

	//--------------------------------------------------------------------
	//	removes fog from the level
	//
	//--------------------------------------------------------------------
	void DeInitialize()
	{
		l_pCameraManip = NULL;
		envSTLHelpers::ForAll(l_CameraManips, manip_cleanup);
		l_CameraManips.clear();

		// clean up proxies
		l_EditorCameraProxy.reset();
		l_TopCameraProxy.reset();
		l_FrontCameraProxy.reset();
		l_SideCameraProxy.reset();
	}

	//---------------------------------------------------------------------------=
	//	Think
	//---------------------------------------------------------------------------=
	void Think()
	{
		//	move camera based on input, if enabled
		//
		if (l_bEnabled)
		{
			l_pCameraManip->Think();
		}

		// Either editor camera or scripted camera
		GetCamera().Set();

		//maPoint3d position = GetCamera().GetPosition();
		//char CursorPos[64];
		//sprintf( CursorPos, "Camera: %.2f %.2f %.2f", position.GetX(),position.GetY(),position.GetZ() );
		//g2dScreen::SetDebugInfo( 14, CursorPos );
	}

	//---------------------------------------------------------------------------=
	//	GetCamera returns the current camera
	//---------------------------------------------------------------------------=
	camCamera& GetCamera( )
	{
		return (IsUsingScriptedCamera()) ? (*l_pScriptedCamera) : (*l_EditorCamera);
	}
	camCamera& GetEditorCamera( )
	{
		return (*l_EditorCamera);
	}
	camCamera& GetTopCamera( )
	{
		return l_TopCamera;
	}
	camCamera& GetFrontCamera( )
	{
		return l_FrontCamera;
	}
	camCamera& GetSideCamera( )
	{
		return l_SideCamera;
	}

	//----------------------------------------------------------------------------
	//	Get shared_ptr to the editor camera 
	//----------------------------------------------------------------------------
	shared_ptr<camCamera> GetEditorCameraPtr()
	{
		return l_EditorCamera;
	}

	//----------------------------------------------------------------------------
	// Set camera proxy for editor camera.
	// This triggers a proxied mode for all editor cameras such that all
	// camera manipulators after this call will be connected to the proxies,
	// not directly to the cameras.
	//----------------------------------------------------------------------------
	void SetEditorCameraProxy(const shared_ptr<gpxCamera> &i_EditorProxy)
	{
		l_EditorCameraProxy = i_EditorProxy;

		// now create the other editor camera proxies so that all
		// editor cameras have buffered thread-safe changes.
		l_TopCameraProxy.reset(new gpxCamera(l_TopCamera));
		l_FrontCameraProxy.reset(new gpxCamera(l_FrontCamera));
		l_SideCameraProxy.reset(new gpxCamera(l_SideCamera));
	}

	//----------------------------------------------------------------------------
	//	Get pointers to the editor camera's proxies
	//----------------------------------------------------------------------------
	gpxCamera* GetEditorCameraProxy()
	{
		return l_EditorCameraProxy.get();
	}
	gpxCamera* GetTopCameraProxy()
	{
		return l_TopCameraProxy.get();
	}
	gpxCamera* GetFrontCameraProxy()
	{
		return l_FrontCameraProxy.get();
	}
	gpxCamera* GetSideCameraProxy()
	{
		return l_SideCameraProxy.get();
	}


	//----------------------------------------------------------------------------
	//	Set application's camera as the one to use instead of editor camera.
	//	If the camera is NULL, then it uses the editor camera again.
	//----------------------------------------------------------------------------
	//void UseScriptedCamera(camCamera* i_pCamera, 
	//					   CameraManipMode i_ManipType)
	//{
	//	DBG_ASSERT(i_ManipType >= 0 && i_ManipType < l_CameraManips.size(), "Manip mode %d out of range, num modes %d", i_ManipType, l_CameraManips.size());
	//	UseScriptedCamera(i_pCamera, l_CameraManips[i_ManipType]);
	//}
	void UseScriptedCamera(gpxCamera* i_pCameraProxy, 
						   CameraManipMode i_ManipType)
	{
		DBG_ASSERT(i_ManipType >= 0 && i_ManipType < l_CameraManips.size(), "Manip mode out of range, " << i_ManipType << " num modes " << l_CameraManips.size());
		UseScriptedCamera(i_pCameraProxy ? (&i_pCameraProxy->GetCamera()) : NULL, 
						  i_pCameraProxy, l_CameraManips[i_ManipType]);
	}

	//----------------------------------------------------------------------------
	// This function gives you the opportunity to set your own camera manip
	//	class into the manager. The caller maintains ownership of the 
	//	manipulator.
	//----------------------------------------------------------------------------
	void UseScriptedCamera(camCamera* i_pCamera, 
						   gpxCamera* i_pCameraProxy, 
						   camCameraManip* i_pCameraManip)
	{
		DBG_ASSERT(i_pCameraManip, "Camera Manip pointer is NULL:");

		// Should always detach and reattach manip, because then the
		// manip can update its values to the position of the
		// camera that might be different than last time.
		// (Since scripted cameras can be animating)

		//if (l_pScriptedCamera != i_pCamera)
		{
			l_pCameraManip->Detach();
			l_pCameraManip = NULL;

			l_pScriptedCamera = i_pCamera;

			// Attach camera to given camera manip
			l_pCameraManip = i_pCameraManip;

			if (i_pCameraManip)
			{
				// By passing in true for preserve camera, the manip
				// updates its radius, yaw, pitch, etc. variables to
				// match the current position of the camera.
				const bool bPreserveCamera = true;
				//l_pCameraManip->Attach(&GetCamera(), bPreserveCamera);
				if (i_pCameraProxy)
				{
					// Use wrapper class to attach manip to camera proxy
					l_CameraProxyWrapper.ChangeManipTarget(i_pCameraProxy);
					l_pCameraManip->Attach(&l_CameraProxyWrapper, bPreserveCamera);
				}
				else
				{
					// Use wrapper class to attach manip to camera
					l_CameraWrapper.ChangeManipTarget(&GetCamera());
					l_pCameraManip->Attach(&l_CameraWrapper, bPreserveCamera);
				}
			}
		}
	}

	//----------------------------------------------------------------------------
	// Return to use of the built-in editor camera
	//----------------------------------------------------------------------------
	void EndScriptedCamera()
	{
		UseScriptedCamera(NULL, l_EditorCameraProxy.get(), l_CameraManips[e_MayaOrbit]);
	}

	//----------------------------------------------------------------------------
	// Returns true if application camera is being used instead of editor camera.
	//----------------------------------------------------------------------------
	bool IsUsingScriptedCamera()
	{
		return (l_pScriptedCamera != NULL);
	}

	//---------------------------------------------------------------------------=
	//	GetCameraManip returns the current camera manipulator
	//---------------------------------------------------------------------------=
	//camCameraManip* GetCameraManip( )
	//{
	//	return l_pCameraManip;
	//}

	//----------------------------------------------------------------------------
	//	Camera Manipulator is normally enabled, but it can be disabled by
	//	with this function to make the camera static or to have it driven
	//	by scripting.
	//----------------------------------------------------------------------------
	void EnableManip( bool i_bEnabled)
	{
		l_bEnabled = i_bEnabled;
	}
	bool IsManipEnabled()
	{
		return l_bEnabled;
	}

	//----------------------------------------------------------------------------
	//	set the position of the manip
	//----------------------------------------------------------------------------
	void SetManipPosition( const maPoint3d& i_Pos )
	{
		if (camCameraManipOrbit* pOrbitManip = dynamic_cast<camCameraManipOrbit*>(l_pCameraManip))
		{
			pOrbitManip->SetPositionTarget( i_Pos , pOrbitManip->GetTarget() );
		}
	}

	//----------------------------------------------------------------------------
	//	set the position and target of the manip
	//----------------------------------------------------------------------------
	void SetManipPositionAndTarget( const maPoint3d& i_Pos, const maPoint3d& i_Target )
	{
		if (camCameraManipOrbit* pOrbitManip = dynamic_cast<camCameraManipOrbit*>(l_pCameraManip))
		{
			pOrbitManip->SetPositionTarget( i_Pos , i_Target );
		}
	}

	//----------------------------------------------------------------------------
	//	set the FOV of the camera
	//----------------------------------------------------------------------------
	//void SetCameraFOV( camCamera& i_Cam, float i_fFOV )
	//{
	//	i_Cam.SetFOV( i_fFOV );
	//}

	//--------------------------------------------------------------------
	// Focus_Camera centers camera with respect to the point
	//--------------------------------------------------------------------
	void FocusCamera(const maPoint3d& i_Focus, float i_Radius, maAxisBox i_Box)
	{
		l_pCameraManip->FocusCamera(i_Focus, i_Radius, i_Box);
	}

	//--------------------------------------------------------------------
	// FocusCamera centers camera with respect to the point
	//--------------------------------------------------------------------
	void FocusCamera(const maAxisBox& i_Box)
	{
		l_pCameraManip->FocusCamera(i_Box.GetCenter(), 1.5f * i_Box.GetRadius(), i_Box );
	}

	//--------------------------------------------------------------------
	// SetFarClip sets the far clipping planes
	//--------------------------------------------------------------------
//	void SetFarClip( float i_fFar)
//	{
//		i_fFar = ( i_fFar < l_fNearClip)? l_fNearClip : i_fFar;
//		l_EditorCamera->SetClip(l_fNearClip, i_fFar);
////		daySkyMgr::SetFarClip( i_fFar );
//	}

	//--------------------------------------------------------------------
	// GetFarClip gets the far clipping plane
	//--------------------------------------------------------------------
	//float GetFarClip()
	//{
	//	return l_EditorCamera->GetFarClip();
	//}

	//--------------------------------------------------------------------
	// SetAspect sets aspect ratio for window with given dimensions
	//--------------------------------------------------------------------
	//void SetAspect( int i_Width, int i_Height )
	//{
	//	float aspect = i_Width / (float)i_Height;
	//	l_EditorCamera->SetAspect(aspect);
	//}

	//--------------------------------------------------------------------
	// SetPanRate sets the speed of camera panning (tracking)
	//--------------------------------------------------------------------
	void SetPanRate( float i_Rate )
	{
		// should this be a virtual function on camCameraManip instead? 
		cam3dManipMaya::SetPanRate(i_Rate);
		cam3dManipOrthoPan::SetPanRate(i_Rate);
	}

	//--------------------------------------------------------------------
	// SetManipFastRender makes things render faster when manipulating
	//	the camera.
	//--------------------------------------------------------------------
	void SetManipFastRender( bool i_bFastRender )
	{
		if (l_pCameraManip)
			l_pCameraManip->SetManipFastRender(i_bFastRender);
	}

	//--------------------------------------------------------------------
	// SetManipDepth tells the camera manipulator at what depth the latest 
	//	mouse click happened.
	//--------------------------------------------------------------------
	void SetManipDepth( float i_Depth )
	{
		if (l_pCameraManip)
			l_pCameraManip->SetManipDepth(i_Depth);
	}

	//--------------------------------------------------------------------
	// SetManipDiagonal tells the camera manipulator what the length of
	// the current bounding box is
	//--------------------------------------------------------------------
	void SetManipBox( maAxisBox i_Box )
	{
		if (l_pCameraManip)
			l_pCameraManip->SetManipBox(i_Box);
	}
}
