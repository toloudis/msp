/*****************************************************************************
**  demG3dTestMode.hpp
**
**		This mode displays a demonstration/test of the different model space
**	concepts used in the Terawatt 3d engine.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef DEM_G3DTESTMODE_HPP
#error demG3dTestMode.hpp multiply included
#endif
#define DEM_G3DTESTMODE_HPP

#ifndef APP_CHAREVENTHANDLER_HPP
#include "appCharEventHandler.hpp"
#endif

#ifndef DEM_MODE_HPP
#include "demMode.hpp"
#endif

#ifndef G2D_FONTHANDLE_HPP
#include "g2dFontHandle.hpp"
#endif
#ifndef MA_POINT3D_HPP
#include "maPoint3d.hpp"
#endif
#ifndef SC_CAMERA_HPP
#include "scCamera.hpp"
#endif
#ifndef SC_CAMERAMANIPORBIT_HPP
#include "scCameraManipOrbit.hpp"
#endif

class demG3dTestMode 
:	public demMode,
	public appCharEventHandler	
{
	public:

		//====================================================================
		//====================================================================
		demG3dTestMode();

		//====================================================================
		//====================================================================
		virtual ~demG3dTestMode();

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
		//====================================================================
		inline scCamera& GetCamera();

		//====================================================================
		//====================================================================
		inline scCameraManipOrbit& GetCameraManip();

	private:

		g2dFontHandle m_Font;
		bool m_QuitSignaled;

		scCamera m_Camera;
		scCameraManipOrbit m_CameraManip;
};

//====================================================================
//====================================================================
inline scCamera& demG3dTestMode::GetCamera()
{
	return m_Camera;
}

//====================================================================
//====================================================================
inline scCameraManipOrbit& demG3dTestMode::GetCameraManip()
{
	return m_CameraManip;
}

