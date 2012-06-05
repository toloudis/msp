/*****************************************************************************
**  demPrtTestMode.hpp
**
**		This mode is a convenient base class for some of the demo/test modes
**	for the Sc package.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef DEM_PRTTESTMODE_HPP
#error demPrtTestMode.hpp multiply included
#endif
#define DEM_PRTTESTMODE_HPP

#ifndef APP_CHAREVENTHANDLER_HPP
#include "appCharEventHandler.hpp"
#endif

#ifndef DEM_MODE_HPP
#include "demMode.hpp"
#endif

#ifndef G2D_FONTHANDLE_HPP
#include "g2dFontHandle.hpp"
#endif

#ifndef CAM_CAMERA_HPP
#include "camCamera.hpp"
#endif

#ifndef CAM_CAMERAMANIP_ORBIT_HPP
#include "camCameraManipOrbit.hpp"
#endif


class demPrtTestMode 
:	public demMode,
	public appCharEventHandler	
{
	public:

		//====================================================================
		//====================================================================
		demPrtTestMode();

		//====================================================================
		//====================================================================
		virtual ~demPrtTestMode();

		//====================================================================
		//	Think
		//====================================================================
		virtual void Think();

		//====================================================================
		//====================================================================
		virtual void Initialize();

		//====================================================================
		//====================================================================
		virtual void DeInitialize();

		//====================================================================
		//	Override this function to get appCharEvents.
		//====================================================================
		virtual void ReceiveCharEvent(appCharEvent& i_Event);

	protected:

		//====================================================================
		//	GetQuitSignaled returns true if the user is done with this mode 
		//	(pressed the space bar).
		//====================================================================
		bool GetQuitSignaled() const;

		//====================================================================
		//	SetYaw changes the camera's yaw
		//====================================================================
		void SetYaw(float i_Radians);

		//====================================================================
		//	SetPitch changes the camera's pitch
		//====================================================================
		void SetPitch(float i_Radians);

		//====================================================================
		//	SetRadius changes the camera's distance from it's orbit point
		//====================================================================
		void SetRadius(float i_Radius);

		//====================================================================
		//====================================================================
		camCamera& Camera();
		const camCamera& GetCamera() const;

		//====================================================================
		//====================================================================
		const camCameraManipOrbit& GetCameraManip() const;

	private:

		bool m_QuitSignaled;

		camCamera m_Camera;
		camCameraManipOrbit m_CameraManip;
};
