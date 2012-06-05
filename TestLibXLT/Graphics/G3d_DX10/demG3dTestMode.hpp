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
#include "Core/app/appCharEventHandler.hpp"
#endif

#ifndef DEM_MODE_HPP
#include "demMode.hpp"
#endif

#ifndef CAM_CAMERA_HPP
#include "Graphics/cam/camCamera.hpp"
#endif

#ifndef G2D_FONTHANDLE_HPP
#include "Graphics/g2d/g2dFontHandle.hpp"
#endif

#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
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
		inline camCamera& Camera();
		inline const camCamera& GetCamera() const;

	private:

		bool m_QuitSignaled;

		float m_Yaw;
		float m_Pitch;
		float m_Radius;
		float m_PanX;
		float m_PanY;
		camCamera m_Camera;
};

//====================================================================
//====================================================================
inline camCamera& demG3dTestMode::Camera()
{
	return m_Camera;
}
inline const camCamera& demG3dTestMode::GetCamera() const
{
	return m_Camera;
}
