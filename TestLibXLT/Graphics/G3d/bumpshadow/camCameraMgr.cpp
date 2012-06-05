/*****************************************************************************
**  camCameraMgr.cpp
**
**      See camCameraMgr.hpp
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "camCameraMgr.hpp"

//#include "mrsCameraManipFollow.hpp"
#include "camCameraManipOrbit.hpp"

#include "envSTLHelpers.hpp"
#include "maConstants.hpp"
#include "scCamera.hpp"

#include <vector>

namespace
{
	typedef std::vector< scCameraManip* > Manips;
	Manips l_CameraManips;

	scCamera* l_Camera;

	int l_eCurrentManip = camCameraMgr::e_Unknown;


	void create_manip_orbit( int i_nIndex )
	{
		camCameraManipOrbit *pOrbitManip = new camCameraManipOrbit();
		l_CameraManips[ i_nIndex ] = pOrbitManip;
		
	}
//	void create_manip_follow( int i_nIndex )
//	{
//		mrsCameraManipFollow *pManip = new mrsCameraManipFollow;
//		l_CameraManips[i_nIndex] = pManip;
//	}
}

//====================================================================
// Initialize will create all of the cameras used in Mars
//====================================================================
void camCameraMgr::Initialize(scCamera* i_Camera)
{
	DBG_ASSERT0(NULL != i_Camera, "Invalid NULL camera!");
	l_Camera = i_Camera;

	// create each of the cameras, these are specific
	l_CameraManips.resize( e_UserManipsBegin );

	create_manip_orbit(e_Orbit);
//	create_manip_follow(e_Follow);

//	mrsCameraManipUtil::Initialize(l_Camera);
}

//====================================================================
// DeInitialize will clean all of the cameras
//====================================================================
void camCameraMgr::DeInitialize()
{
	envSTLHelpers::DeleteContainer( l_CameraManips );
	l_eCurrentManip = camCameraMgr::e_Unknown;

//	mrsCameraManipUtil::DeInitialize();
}

//====================================================================
// Think will think on the current camera, should this be necessary.
//====================================================================
void camCameraMgr::Think()
{
	DBG_ASSERT0( l_CameraManips.size() > 0, "You didn't initialize.");

 	if ( e_Unknown != l_eCurrentManip )
	{
		DBG_ASSERT0(l_CameraManips[l_eCurrentManip], "Invalid camera, did you initialize?");
		
		l_CameraManips[l_eCurrentManip]->Think();
	}
	// else, nothing to do
}

//====================================================================
// GetCamera returns the camCamera, there is only one.
//====================================================================
scCamera& camCameraMgr::GetCamera()
{
	return *l_Camera;
}

//====================================================================
// AddCameraManip will add a user defined camera manip to the camera Mgr
// Note  The camCameraMgr takes ownership of the manip and will delete
// it on DeInitialize.
// The return is the new index (CameraType) of the given manip.
//====================================================================
int camCameraMgr::AddCameraManip( scCameraManip& i_Manip )
{
	int ret = l_CameraManips.size();
	l_CameraManips.push_back( &i_Manip );  // we keep forever
	return ret;
}

//====================================================================
// GetCameraManip will return the requested manip, this is useful 
// for setting those parameters that are manip and mode state specific
//====================================================================
scCameraManip& camCameraMgr::GetCameraManip( int i_Type )
{
	DBG_ASSERT0( ((int)i_Type) >= 0 && ((int)i_Type) < l_CameraManips.size(),
		"Invalid Camera Type sought!");
	DBG_ASSERT0( ((int)i_Type) < l_CameraManips.size(),
		"Invalid camera type index, perhaps you didn't Initialize?");

	return *l_CameraManips[i_Type];
}

//====================================================================
// SetCurrentManip will set the current camera to the given manip type
//====================================================================
void camCameraMgr::SetCurrentManip( int i_Type, bool i_bPreserveCamera/*=true*/ )
{
	DBG_ASSERT0( ((int)i_Type) >= 0 && ((int)i_Type) < l_CameraManips.size(),
		"Invalid Camera Type sought!");
	DBG_ASSERT0( ((int)i_Type) < l_CameraManips.size(),
		"Invalid camera type index, perhaps you didn't Initialize?");

	// re-attach when called, no matter what
	l_eCurrentManip = i_Type;

	l_CameraManips[l_eCurrentManip]->Attach( l_Camera, i_bPreserveCamera );
}
