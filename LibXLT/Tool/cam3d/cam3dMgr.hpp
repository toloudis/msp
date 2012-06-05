/*****************************************************************************
**	cam3dMgr.hpp
**
**		The cam3dMgr keeps track of cameras for the level and
**	the editor camera for the modes.
**
**	StudioGPU
**	Copyright(C) 2000-2002 - All Rights Reserved
\****************************************************************************/
#ifdef CAM3D_MGR_HPP
#error cam3dMgr.hpp multiply included
#endif
#define CAM3D_MGR_HPP

#ifndef CAM_CAMERA_HPP
#include "Graphics/cam/camCamera.hpp"
#endif
#ifndef CAM_CAMERAMANIP_ORBIT_HPP
#include "Graphics/cam/camCameraManipOrbit.hpp"
#endif
#ifndef ENV_BOOST_HPP
#include "Core/Env/envBoost.hpp"
#endif


//============================================================================
//	Forward References
//============================================================================
class maAxisBox;
class gpxCamera;


//============================================================================
//============================================================================
namespace cam3dMgr
{
	enum CameraManipMode
	{
		e_MayaOrbit = 0,
		e_OrthoPan,
		e_NumManipModes
	};

	//--------------------------------------------------------------------
	//	Initialize
	//--------------------------------------------------------------------
	void Initialize();

	//--------------------------------------------------------------------
	//	removes
	//--------------------------------------------------------------------
	void DeInitialize();

	//----------------------------------------------------------------------------
	//	Think
	//----------------------------------------------------------------------------
	void Think();

	//----------------------------------------------------------------------------
	//	GetCamera returns the current camera
	//----------------------------------------------------------------------------
	camCamera& GetCamera();
	camCamera& GetEditorCamera();
	camCamera& GetTopCamera();
	camCamera& GetFrontCamera();
	camCamera& GetSideCamera();

	//----------------------------------------------------------------------------
	//	Get shared_ptr to the editor camera 
	//----------------------------------------------------------------------------
	shared_ptr<camCamera> GetEditorCameraPtr();

	//----------------------------------------------------------------------------
	// Set camera proxy for editor camera.
	// This triggers a proxied mode for all editor cameras such that all
	// camera manipulators after this call will be connected to the proxies,
	// not directly to the cameras.
	//----------------------------------------------------------------------------
	void SetEditorCameraProxy(const shared_ptr<gpxCamera> &i_EditorProxy);

	//----------------------------------------------------------------------------
	//	Get pointers to the editor camera's proxies.
	//	Will be NULL if SetEditorCameraProxy() has not been called.
	//----------------------------------------------------------------------------
	gpxCamera* GetEditorCameraProxy();
	gpxCamera* GetTopCameraProxy();
	gpxCamera* GetFrontCameraProxy();
	gpxCamera* GetSideCameraProxy();

	//----------------------------------------------------------------------------
	//	Set application's camera as the one to use instead of editor camera.
	//	If the camera is NULL, then it uses the editor camera again.
	//	Pointer to camera is not owned by cam3dMgr.
	//----------------------------------------------------------------------------
	//void UseScriptedCamera(camCamera* i_pCamera, 
	//					   CameraManipMode i_ManipType = e_MayaOrbit);
	void UseScriptedCamera(gpxCamera* i_pCameraProxy, 
						   CameraManipMode i_ManipType = e_MayaOrbit);
	
	//----------------------------------------------------------------------------
	// This function gives you the opportunity to set your own camera manip
	//	class into the manager. The caller maintains ownership of the 
	//	manipulator.
	//----------------------------------------------------------------------------
	//void UseScriptedCamera(camCamera* i_pCamera, 
	//					   camCameraManip* i_pCameraManip);
	void UseScriptedCamera(camCamera* i_pCamera, 
						   gpxCamera* i_pCameraProxy, 
						   camCameraManip* i_pCameraManip);

	//----------------------------------------------------------------------------
	// Return to use of the built-in editor camera
	//----------------------------------------------------------------------------
	void EndScriptedCamera();

	//----------------------------------------------------------------------------
	// Returns true if application camera is being used instead of editor camera.
	//----------------------------------------------------------------------------
	bool IsUsingScriptedCamera();

	//----------------------------------------------------------------------------
	//	GetCameraManip returns the current camera manipulator
	//----------------------------------------------------------------------------
	//camCameraManip* GetCameraManip();

	//----------------------------------------------------------------------------
	//	Camera Manipulator is normally enabled, but it can be disabled by
	//	with this function to make the camera static or to have it driven
	//	by scripting.
	//----------------------------------------------------------------------------
	void EnableManip( bool i_bEnabled);
	bool IsManipEnabled();

	//----------------------------------------------------------------------------
	//	set the position of the manip
	//----------------------------------------------------------------------------
	void SetManipPosition( const maPoint3d& i_Pos );

	//----------------------------------------------------------------------------
	//	set the position and target of the manip
	//----------------------------------------------------------------------------
	void SetManipPositionAndTarget( const maPoint3d& i_Pos, const maPoint3d& i_Target );

	//----------------------------------------------------------------------------
	//	set the FOV of the camera
	//----------------------------------------------------------------------------
	//void SetCameraFOV( camCamera& i_Cam, float i_fFOV );

	//--------------------------------------------------------------------
	// Focus_Camera centers camera with respect to the point
	//--------------------------------------------------------------------
	void FocusCamera(const maPoint3d& i_Focus, float i_Radius, maAxisBox i_Box);

	//--------------------------------------------------------------------
	// Focus_Camera centers camera with respect to the bounding box
	//--------------------------------------------------------------------
	void FocusCamera(const maAxisBox& i_Box);

	//--------------------------------------------------------------------
	// SetFarClip sets the far clipping planes
	//--------------------------------------------------------------------
	//void SetFarClip( float i_fFar);

	//--------------------------------------------------------------------
	// GetFarClip gets the far clipping plane
	//--------------------------------------------------------------------
	//float GetFarClip();

	//--------------------------------------------------------------------
	// SetAspect sets aspect ratio for window with given dimensions
	//--------------------------------------------------------------------
	//void SetAspect( int i_Width, int i_Height );

	//--------------------------------------------------------------------
	// SetPanRate sets the speed of camera panning (tracking)
	//--------------------------------------------------------------------
	void SetPanRate( float i_Rate );

	//--------------------------------------------------------------------
	// SetManipFastRender makes things render faster when manipulating
	//	the camera.
	//--------------------------------------------------------------------
	void SetManipFastRender( bool i_bFastRender );

	//--------------------------------------------------------------------
	// SetManipDepth tells the camera manipulator at what depth the latest 
	//	mouse click happened.
	//--------------------------------------------------------------------
	void SetManipDepth( float i_Depth );

	//--------------------------------------------------------------------
	// SetManipDiagonal tells the camera manipulator what the length of
	// the current bounding box is
	//--------------------------------------------------------------------
	void SetManipBox( maAxisBox i_Box );
}
