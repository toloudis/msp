/*****************************************************************************
**  camCmeraMgr.hpp
**
**      camCameraMgr holds the list of cameras available in the user's application
**	they are created on Initialize and destroyed on DeInitialize.
**		Anyone can access them, but generally this is reserved for the mode states.
**		This receives thinks from mrsModeState.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef CAM_CAMERAMGR_HPP
#error camCameraMgr.hpp multiply included
#endif
#define CAM_CAMERAMGR_HPP

class scCamera;
class scCameraManip;

namespace camCameraMgr
{
	enum CameraTypes
	{
		e_Unknown = -1,
		e_FirstManip = 0,

		e_Orbit		 = e_FirstManip,  
		
		e_UserManipsBegin
	};

	//====================================================================
	// Initialize will create all of the camera manips used, all will be
	// attached to the given camera when needed.
	// Note:  The given camera should already be setup with its
	//		Aspec ratio, FOV, and Clipping.
	//====================================================================
	void Initialize(scCamera* i_Camera);

	//====================================================================
	// DeInitialize will clean all of the cameras
	//====================================================================
	void DeInitialize();

	//====================================================================
	// Think will think on the current camera, should this be necessary.
	//====================================================================
	void Think();

	//====================================================================
	// GetCamera returns the scCamera, there is only one.
	//====================================================================
	scCamera& GetCamera();

	//====================================================================
	// AddCameraManip will add a user defined camera manip to the camera Mgr
	// Note  The camCameraMgr takes ownership of the manip and will delete
	// it on DeInitialize.
	// The return is the new index (CameraType) of the given manip.
	//====================================================================
	int AddCameraManip( scCameraManip& i_Manip );

	//====================================================================
	// GetCameraManip will return the requested manip, this is useful 
	// for setting those parameters that are manip and mode state specific
	//====================================================================
	scCameraManip& GetCameraManip( int i_nType );

	//====================================================================
	// SetCurrentManip will set the current camera to the given manip type
	//====================================================================
	void SetCurrentManip( int i_Type, bool i_bPreserveCamera=true );

	//====================================================================
	// StartCameraShake will initiate a camera shake for the given 
	// duration at the given radius, this will interrupt and replace any
	// currently executing camera shake
	//====================================================================
	void StartCameraShake( float i_fDuration, float i_fRadius );

	//====================================================================
	// StopCameraShake will stop the current camera shake (if any)
	//====================================================================
	void StopCameraShake();
}
