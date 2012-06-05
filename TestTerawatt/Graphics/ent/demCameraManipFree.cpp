/*****************************************************************************
**  demCameraManipFree.cpp
**
**      demCameraManipFree allows user to freely move camera
**  by orbiting around target point and translating target.
**	It uses mouse and numpad controls.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "demCameraManipFree.hpp"

#include "inDeviceMgr.hpp"
#include "inVirtualJoystick.hpp"
#include "maConstants.hpp"
#include "camCamera.hpp"
#include "g2dWindow.hpp"

namespace
{

const float c_MaxPitch = 80.0f * maConstants::c_fAngleToRad;
const float c_MinPitch = -80.0f * maConstants::c_fAngleToRad;
const float c_MaxRadius = 2500.0f;
const float c_MinRadius = 3.0f;
const float c_MinRadiusMod = 2.0f;

const float c_MinAboveGround = 0.0f;


}


//====================================================================
//====================================================================
demCameraManipFree::demCameraManipFree(g2dWindow& i_Window)
: m_Window(i_Window)
{
	SetTarget(maPoint3d(0, 90, 0));
	SetPitch(20.0f * maConstants::c_fAngleToRad);
	SetYaw(180.0f * maConstants::c_fAngleToRad);
	SetRadius(70.0f);
}


//====================================================================
//	Think should be called to update camera and push
//	changes into g3d system.
//====================================================================
void	
demCameraManipFree::Think()
{
	if (!GetCamera()) return;
	
	camCamera& camera = (*GetCamera());
	inKeyboard *pKey = inDeviceMgr::GetKeyboard(0);
	inMouse *pMouse = inDeviceMgr::GetMouse(0);
	//float delta = coSimTime::GetDelta();
	float delta = 0.05f;	// 20 fps
	bool speed = pKey->IsPressed(inKeys::e_LSHIFT) ||
					pKey->IsPressed(inKeys::e_RSHIFT);
	const float pan_multiplier = (speed) ? 80.0f : 30.0f;	// units/sec
	const float orbit_multiplier = (speed) ? 80.0f : 20.0f; // degs/sec

	// Handle user input

	if( pKey->IsPressed(inKeys::e_NUMPAD4) )
	{
		maVector3d shift = camera.GetLeft();
		shift *= pan_multiplier * delta;
		SetTarget(GetTarget() + shift);
	}

	if( pKey->IsPressed(inKeys::e_NUMPAD6) )
	{
		maVector3d shift = camera.GetLeft();
		shift *= -pan_multiplier * delta;
		SetTarget(GetTarget() + shift);
	}

	if( pKey->IsPressed(inKeys::e_NUMPAD8) )
	{
		maVector3d shift = camera.GetUp();
		shift *= pan_multiplier * delta;
		SetTarget(GetTarget() + shift);
	}

	if( pKey->IsPressed(inKeys::e_NUMPAD2) )
	{
		maVector3d shift = camera.GetUp();
		shift *= -pan_multiplier * delta;
		SetTarget(GetTarget() + shift);
	}

	if( pKey->IsPressed(inKeys::e_NUMPADMINUS) )
	{
		maVector3d shift = camera.GetDirection();
		shift *= pan_multiplier * delta;
		SetTarget(GetTarget() + shift);
	}

	if( pKey->IsPressed(inKeys::e_NUMPADPLUS) )
	{
		maVector3d shift = camera.GetDirection();
		shift *= -pan_multiplier * delta;
		SetTarget(GetTarget() + shift);
	}

	// Handle mouse input
	if (inDeviceMgr::GetMouse())
	{
		if( pMouse->IsHeld(0) )
		{
			int dX, dY;
			pMouse->GetAnalogDir(0, dX, dY);//stick #0
			if( inDeviceMgr::GetKeyboard()->IsHeld(inKeys::e_LSHIFT) || inDeviceMgr::GetKeyboard()->IsHeld(inKeys::e_RSHIFT) )
			{
				camCamera& camera = (*GetCamera());
				maVector3d x_shift = camera.GetLeft();
				maVector3d y_shift = camera.GetUp();
				const float multiplier = 0.4f;
				const maPoint3d& old_pos = this->GetTarget();
				this->SetTarget( old_pos + (x_shift * multiplier * dX) + (y_shift * multiplier * dY) );
			}
			else
			{
				// base modifiers on screen size
				float p_modifier = 0.010f;
				float y_modifier = 0.010f;
				IncrementPitch( p_modifier * dY);
				IncrementYaw( -y_modifier * dX);
			}
		}
		else if( pMouse->IsHeld(1) )	// Right
		{
			int dX, dY;
			pMouse->GetAnalogDir(0, dX, dY);//stick #0

			int width, height;
			m_Window.GetDimensions(width, height);
			float multiplier = GetRadius() / float(height);

			maVector3d shift = camera.GetUp() * multiplier * dY +
					camera.GetLeft() * multiplier * dX;
			SetTarget(GetTarget() + shift);
		}
		else if( pMouse->IsHeld(2) )	 // Middle
		{
			int dX, dY;
			pMouse->GetAnalogDir(0, dX, dY);//stick #0

			float r_modifier = 0.5f;
			IncrementRadius( r_modifier * (dY-dX));
		}
	
	}

	camCameraManipOrbit::Think();
}

void	
demCameraManipFree::IncrementPitch(float i_Radians)
{
	float pitch = GetPitch() + i_Radians;
	if (pitch > c_MaxPitch) pitch = c_MaxPitch;
	else if (pitch < c_MinPitch) pitch = c_MinPitch;
	SetPitch( pitch );
}

void	
demCameraManipFree::IncrementYaw(float i_Radians)
{
	SetYaw( GetYaw() + i_Radians);
}

void	
demCameraManipFree::IncrementRadius(float i_Radius)
{
	float radius = GetRadius() + i_Radius;	
	if (radius > c_MaxRadius) radius = c_MaxRadius;
	else if (radius < c_MinRadius) radius = c_MinRadius;
	SetRadius( radius );
}
