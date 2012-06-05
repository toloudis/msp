/*****************************************************************************
**  camCameraManipOrbit.hpp
**
**      camCameraManipOrbit is a camera manipulator class which can be
**  used to control a camera by orbiting the camera around a target point.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef CAM_CAMERAMANIP_ORBIT_HPP
#error camCameraManipOrbit.hpp multiply included
#endif
#define CAM_CAMERAMANIP_ORBIT_HPP

#ifndef CAM_CAMERAMANIP_HPP
#include "Graphics/cam/camCameraManip.hpp"
#endif

#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif

//============================================================================
//============================================================================
class camCameraManipOrbit
:	public camCameraManip
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		camCameraManipOrbit();

		//--------------------------------------------------------------------
		//	Attach is called to connect this manipulator to control
		//  the given camera. If i_bPreserveCamera is true, the manipulator
		//  should set its data to try to preserve the camera's position
		//  and orientation.
		//--------------------------------------------------------------------
		virtual void Attach(camCameraManipTarget* i_pCamera, bool i_bPreserveCamera = false);

		//--------------------------------------------------------------------
		//	Sets target, pitch, yaw, radius from given position and target
		//	positions.
		//--------------------------------------------------------------------
		void SetPositionTarget(const maPoint3d& i_Position, const maPoint3d& i_Target);

		//--------------------------------------------------------------------
		//	SetTarget changes the point in space that the camera looks at.
		//	This will shift the position, keeping yaw, pitch, radius the same
		//--------------------------------------------------------------------
		void SetTarget(const maPoint3d& i_Target);

		//--------------------------------------------------------------------
		//	SetPitch sets the pitch of the camera.  This is the pitch as
		//	measured from the target point to the camera.
		//--------------------------------------------------------------------
		void SetPitch(float i_Radians);

		//--------------------------------------------------------------------
		//	SetYaw sets the yaw of the camera.  Again, this is the yaw
		//	as measured from the target point to the camera.
		//--------------------------------------------------------------------
		void SetYaw(float i_Radians);

		//--------------------------------------------------------------------
		//	SetDiagonal sets the bounding boxes diagonal length
		//--------------------------------------------------------------------
		void SetBox(maAxisBox i_Box);

		//--------------------------------------------------------------------
		//	SetRadius allows the user to set the distance from the camera
		//	to the target point.  When this value changes the camera position
		//	(not the target position) will move.
		//--------------------------------------------------------------------
		void SetRadius(float i_Radius);

		//--------------------------------------------------------------------
		//	SetZoomFactor sets the zooming factor
		//--------------------------------------------------------------------
		void SetZoomFactor(float i_ZoomFactor);

		//--------------------------------------------------------------------
		//	GetTarget returns the target of the camCamera
		//--------------------------------------------------------------------
		const maPoint3d& GetTarget() const;

		//--------------------------------------------------------------------
		//	GetPitch returns the pitch of the camera (in radians)
		//--------------------------------------------------------------------
		float GetPitch() const;

		//--------------------------------------------------------------------
		//	GetYaw returns the yaw of the camera (in radians)
		//--------------------------------------------------------------------
		float GetYaw() const;

		//--------------------------------------------------------------------
		//	GetRadius returns the distance from the camera to the target
		//	point
		//--------------------------------------------------------------------
		float GetRadius() const;

		//--------------------------------------------------------------------
		//	GetZoomFactor returns the zooming factor
		//--------------------------------------------------------------------
		float GetZoomFactor() const;

		//--------------------------------------------------------------------
		//	ResetCamera() - reset the camera based on the manips values.
		//--------------------------------------------------------------------
		virtual void ResetCamera();

		//--------------------------------------------------------------------
		// Focus_Camera centers camera with respect to the point
		//--------------------------------------------------------------------
		virtual void FocusCamera(const maPoint3d& i_Focus, float i_Radius, maAxisBox i_Box);

	private:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void update();

	private:
		maPoint3d m_Target;
		float m_Yaw;
		float m_Pitch;
		float m_Radius;
		float m_ZoomFactor;
		maAxisBox m_Box;
};
