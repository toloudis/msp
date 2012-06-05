/*****************************************************************************
**  demCameraManipFree.hpp
**
**      demCameraManipFree allows user to freely move camera
**  by orbiting around target point and translating target.
**	It uses mouse and numpad controls.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef DEM_CAMERAMANIP_FREE_HPP
#error demCameraManipFree.hpp multiply included
#endif
#define DEM_CAMERAMANIP_FREE_HPP

#ifndef CAM_CAMERAMANIP_ORBIT_HPP
#include "camCameraManipOrbit.hpp"
#endif

class g2dWindow;

class demCameraManipFree
: public camCameraManipOrbit
{
	public:

		//====================================================================
		//====================================================================
		demCameraManipFree(g2dWindow& i_Window);

		//====================================================================
		//	Think should be called to update camera and push
		//	changes into g3d system.
		//====================================================================
		virtual void	Think();

	private:

		//====================================================================
		//	IncrementPitch
		//====================================================================
		void	IncrementPitch(float i_Radians);

		//====================================================================
		//	IncrementYaw
		//====================================================================
		void	IncrementYaw(float i_Radians);

		//====================================================================
		//	IncrementRadius
		//====================================================================
		void	IncrementRadius(float i_Radius);

		g2dWindow& m_Window;
};
