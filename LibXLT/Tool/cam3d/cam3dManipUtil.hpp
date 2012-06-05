/*****************************************************************************
**	cam3dManipUtil.hpp
**
**		Based on the user's current preferences for mouse configuration, 
**	cam3dManipUtil, will tell the camera which actions to perform. 
**
**	StudioGPU
**	Copyright(C) 2009
\****************************************************************************/
#ifdef CAM3D_MANIP_UTIL_HPP
#error cam3dManipUtil.hpp multiply included
#endif
#define CAM3D_MANIP_UTIL_HPP


//============================================================================
//============================================================================
class inMouse;
class inKeyboard;


//============================================================================
//============================================================================
namespace cam3dManipUtil
{
	//------------------------------------------------------------------------
	//enum for different camera configurations
	//------------------------------------------------------------------------
	enum 
	{
		e_MSPRO = 0,
		e_MAYA,
		e_MAX,
		e_NUM_CONFIGS
	};

	//------------------------------------------------------------------------
	//enum for camera motions
	//------------------------------------------------------------------------
	enum 
	{
		e_ZOOM,
		e_PAN,
		e_ROTATE,
		e_PIVOT,
		e_NONE
	};

	//------------------------------------------------------------------------
	//struct that will pass data to the cam3d object to tell it
	//which operations to perform
	//------------------------------------------------------------------------
	struct CameraControl
	{
	public:
		bool m_bValidControls;
		bool m_bScrollWheel;
		bool m_bXYZoom;
		bool m_bInverseZoom;
		int m_CamManipMode;
	};

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void SetCameraControlMode( int i_MouseConfigID );
	
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------	
	void GetCameraControls( inMouse* i_pMouse, inKeyboard* i_pKeyboard, CameraControl& o_CamManip );

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------	
	void DoMSProControls( inMouse* i_pMouse, inKeyboard* i_pKeyboard, CameraControl& o_CamManip );

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------	
	void DoMayaControls( inMouse* i_pMouse, inKeyboard* i_pKeyboard, CameraControl& o_CamManip );

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------	
	void DoMaxControls( inMouse* i_pMouse, inKeyboard* i_pKeyboard, CameraControl& o_CamManip );

} // end namespace cam3dManipUtil
