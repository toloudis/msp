/*****************************************************************************
**  demMatTestMode.hpp
**
**		This mode displays a demonstration/test of the texture/material
**	concepts used in the Terawatt mat engine.
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#ifdef DEM_MATTESTMODE_HPP
#error demMatTestMode.hpp multiply included
#endif
#define DEM_MATTESTMODE_HPP

#ifndef APP_CHAREVENTHANDLER_HPP
#include "appCharEventHandler.hpp"
#endif

#ifndef DEM_MODE_HPP
#include "demMode.hpp"
#endif

#ifndef CAM_CAMERA_HPP
#include "camCamera.hpp"
#endif

#ifndef G2D_FONTHANDLE_HPP
#include "g2dFontHandle.hpp"
#endif

#ifndef MA_POINT3D_HPP
#include "maPoint3d.hpp"
#endif

class demMatTestMode 
:	public demMode,
	public appCharEventHandler	
{
	public:

		//====================================================================
		//====================================================================
		demMatTestMode();

		//====================================================================
		//====================================================================
		virtual ~demMatTestMode();

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

		camCamera m_Camera;
};

//====================================================================
//====================================================================
inline camCamera& demMatTestMode::Camera()
{
	return m_Camera;
}
inline const camCamera& demMatTestMode::GetCamera() const
{
	return m_Camera;
}
