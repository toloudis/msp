/*****************************************************************************
**  camCameraManipOrbit.hpp
**
**      camCameraManipOrbit allows user to freely move camera
**  by orbiting around target point and translating target.
**	Uses a maya style config
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef CAM_CAMERAMANIPRORBIT_HPP
#error camCameraManipOrbit.hpp multiply included
#endif
#define CAM_CAMERAMANIPRORBIT_HPP

#ifndef SC_CAMERAMANIP_ORBIT_HPP
#include "scCameraManipOrbit.hpp"
#endif

class camCameraManipOrbit : public scCameraManipOrbit
{
	public:

		//====================================================================
		//====================================================================
		camCameraManipOrbit();

		//====================================================================
		//	Think should be called to update camera and push
		//	changes into g3d system.
		//====================================================================
		virtual void	Think();

		//====================================================================
		//	Returns true if the camera is eating the left mouse button
		//====================================================================
		inline bool	IsHoldingLeftInput();

		//====================================================================
		//	Returns true if the camera is eating the right mouse button
		//====================================================================
		inline bool	IsHoldingRightInput();

	private:

		//====================================================================
		//	SafeSetTarget
		//====================================================================
		void	SafeSetTarget(const maPoint3d &i_Target);

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

		bool m_bHoldingLeftInput;
		bool m_bHoldingRightInput;
};

//====================================================================
//	Returns true if the camera is eating the left mouse button
//====================================================================
bool	camCameraManipOrbit::IsHoldingLeftInput()
{
	return m_bHoldingLeftInput;
}

//====================================================================
//	Returns true if the camera is eating the right mouse button
//====================================================================
bool	camCameraManipOrbit::IsHoldingRightInput()
{
	return m_bHoldingRightInput;
}

