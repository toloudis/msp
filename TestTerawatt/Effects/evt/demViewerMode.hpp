/*****************************************************************************
**  demViewerMode.hpp
**
**		This mode is a convenient base class for some of the demo/test modes.
**	It uses a mouse-controlled camera.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef DEM_VIEWERMODE_HPP
#error demViewerMode.hpp multiply included
#endif
#define DEM_VIEWERMODE_HPP

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

#ifndef DEM_CAMERAMANIP_FREE_HPP
#include "demCameraManipFree.hpp"
#endif

class g3dViewer;

class demViewerMode 
:	public demMode,
	public appCharEventHandler	
{
	public:

		//====================================================================
		//====================================================================
		demViewerMode(g3dViewer &i_Viewer);

		//====================================================================
		//====================================================================
		virtual ~demViewerMode();

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
		//	GetFont returns the font that the mode should use
		//====================================================================
		g2dFontHandle GetFont() const;

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
		//	SetTarget sets target point for camera
		//====================================================================
		void SetTarget(const maPoint3d &i_Target);

		//====================================================================
		//====================================================================
		const camCamera& GetCamera() const;
		camCamera& Camera();

		//====================================================================
		//====================================================================
		const camCameraManipOrbit& GetCameraManip() const;

	private:

		g2dFontHandle m_Font;
		bool m_QuitSignaled;

		camCamera m_Camera;
		demCameraManipFree m_CameraManip;
};
