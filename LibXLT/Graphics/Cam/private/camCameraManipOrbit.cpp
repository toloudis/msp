/*****************************************************************************
**  camCameraManipOrbit.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "Graphics/cam/camCameraManipOrbit.hpp"
#include "Graphics/cam/camCameraManipTarget.hpp"

#include "Core/ma/maConstants.hpp"

#include "Tool/cam3d/cam3dMgr.hpp"

//--------------------------------------------------------------------
//--------------------------------------------------------------------
camCameraManipOrbit::camCameraManipOrbit()
:	m_Yaw(0.0f),
	m_Pitch(0.0f),
	m_Radius(1.0f),
	m_Target(0, 0, 1),
	m_ZoomFactor(1.0f)
{
}

//--------------------------------------------------------------------
//	Attach is called to connect this manipulator to control
//  the given camera. If i_bPreserveCamera is true, the manipulator
//  should set its data to try to preserve the camera's position
//  and orientation.
//--------------------------------------------------------------------
void camCameraManipOrbit::Attach(camCameraManipTarget* i_pCamera, bool i_bPreserveCamera)
{
	camCameraManip::Attach(i_pCamera, i_bPreserveCamera);

	if (i_bPreserveCamera)
	{
		// Use camera's position and target point to set
		// our radius, yaw, pitch values
		maPoint3d cam_pos = i_pCamera->GetPosition();
		maVector3d cam_dir = i_pCamera->GetDirection();

		m_Target = i_pCamera->GetTarget();
		m_Radius = ( cam_pos - m_Target ).Length();

		maPoint3d target_dir = -cam_dir;

		m_Yaw = float(::atan2f(target_dir.m_X, target_dir.m_Z));
		float zx_len = sqrtf( target_dir.m_Z * target_dir.m_Z + target_dir.m_X * target_dir.m_X );
		m_Pitch = float(::atan2f(target_dir.m_Y, zx_len));
	}
	else
	{
		// set our current values into camera
		update();
	}
}

//--------------------------------------------------------------------
//	Sets target, pitch, yaw, radius from given position and target
//	positions.
//--------------------------------------------------------------------
void camCameraManipOrbit::SetPositionTarget(const maPoint3d& i_Position, const maPoint3d& i_Target)
{
	m_Target = i_Target;

	maPoint3d target_dir = i_Position - m_Target;
	m_Radius = target_dir.Length();

	m_Yaw = float(::atan2f(target_dir.m_X, target_dir.m_Z));
	float zx_len = sqrtf( target_dir.m_Z * target_dir.m_Z + target_dir.m_X * target_dir.m_X );
	m_Pitch = float(::atan2f(target_dir.m_Y, zx_len));
	update();
}

//--------------------------------------------------------------------
//	SetTarget changes the point in space that the camera looks at.
//	This will shift the position, keeping yaw, pitch, radius the same
//--------------------------------------------------------------------
void camCameraManipOrbit::SetTarget(const maPoint3d& i_Target)
{
	m_Target = i_Target;

	update();
}

//--------------------------------------------------------------------
//	SetPitch sets the pitch of the camera
//--------------------------------------------------------------------
void camCameraManipOrbit::SetPitch(float i_Radians)
{
	//	we use this value to modify the position of the camera
	//	keeping the same target point
	m_Pitch = i_Radians;

	update();
}

//--------------------------------------------------------------------
//	SetYaw sets the yaw of the camera
//--------------------------------------------------------------------
void camCameraManipOrbit::SetYaw(float i_Radians)
{
	//	we use this value to modify the position of the camera
	//	keeping the same target point
	m_Yaw = i_Radians;

	update();
}

//--------------------------------------------------------------------
//	SetRadius allows the user to set the distance from the camera
//	to the target point.  When this value changes the camera position
//	(not the target position) will move.
//--------------------------------------------------------------------
void camCameraManipOrbit::SetRadius(float i_Radius)
{
	m_Radius = i_Radius;
	update();
}

//--------------------------------------------------------------------
//	SetZoomFactor sets the zooming factor
//--------------------------------------------------------------------
void camCameraManipOrbit::SetZoomFactor(float i_ZoomFactor)
{
	m_ZoomFactor = i_ZoomFactor;
	update();
}

//--------------------------------------------------------------------
//	SetDiagonal sets the bounding boxes diagonal length
//--------------------------------------------------------------------
void camCameraManipOrbit::SetBox(maAxisBox i_Box)
{
	m_Box = i_Box;
	update();
}

//--------------------------------------------------------------------
//	GetTarget returns the target of the camCameraManipOrbit
//--------------------------------------------------------------------
const maPoint3d& camCameraManipOrbit::GetTarget() const
{
	return m_Target;
}

//--------------------------------------------------------------------
//	GetPitch returns the pitch of the camera (in radians)
//--------------------------------------------------------------------
float camCameraManipOrbit::GetPitch() const
{
	return m_Pitch;
}

//--------------------------------------------------------------------
//	GetYaw returns the yaw of the camera (in radians)
//--------------------------------------------------------------------
float camCameraManipOrbit::GetYaw() const
{
	return m_Yaw;
}

//--------------------------------------------------------------------
//	GetRadius returns the distance from the camera to the target
//	point
//--------------------------------------------------------------------
float camCameraManipOrbit::GetRadius() const
{
	return m_Radius;
}

//--------------------------------------------------------------------
//	GetZoomFactor returns the zooming factor
//--------------------------------------------------------------------
float camCameraManipOrbit::GetZoomFactor() const
{
	return m_ZoomFactor;
}

//--------------------------------------------------------------------
//	ResetCamera() - reset the camera based on the manips values.
//--------------------------------------------------------------------
//virtual 
void camCameraManipOrbit::ResetCamera()
{
	update();
}

//--------------------------------------------------------------------
// Focus_Camera centers camera with respect to the point
//--------------------------------------------------------------------
void camCameraManipOrbit::FocusCamera(const maPoint3d& i_Focus, float i_Radius, maAxisBox i_Box)
{
	this->SetTarget(i_Focus);
	this->SetRadius(i_Radius);
	this->SetBox(i_Box);
	this->SetZoomFactor(i_Radius);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void camCameraManipOrbit::update()
{
	if (!GetCamera()) return;

	float x = float(sin(m_Yaw) * cos(m_Pitch));
	float y = float(sin(m_Pitch));
	float z = float(cos(m_Yaw) * cos(m_Pitch));

	maVector3d dir(-x, -y, -z);

	maPoint3d pos = m_Target - dir * m_Radius;

	GetCamera()->LookAt(pos, m_Target, maVector3d(0,1,0));

	cam3dMgr::SetManipDepth( (pos - m_Target).Length() );
	cam3dMgr::SetManipBox( m_Box );
}

