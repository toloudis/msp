/*****************************************************************************
**  camCameraManipOrbit.cpp
**
**      See camCameraManipOrbit.hpp
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "camCameraManipOrbit.hpp"

#include "appSimTime.hpp"
#include "inDeviceMgr.hpp"
#include "inVirtualJoystick.hpp"
#include "g2dScreen.hpp"
#include "maConstants.hpp"
#include "scCamera.hpp"

namespace
{
	const float c_MaxPitch = 89.0f * maConstants::c_fAngleToRad;
	const float c_MinPitch = -89.0f * maConstants::c_fAngleToRad;
	const float c_MaxRadius = 1000.0f;
	const float c_MinRadius = 1.0f;
	const float c_MinRadiusMod = 2.0f;

	const float lc_ZoomRadiusModifier = 0.5f;
	// base modifiers on screen size
	const float lc_RotatePitchModifier = 0.01f;
	const float lc_RotateYawModifier = 0.01f;
	const float lc_fPanMultiplier = 0.002f;

	enum eMouseButton
	{
		e_MouseLeft = 0,
		e_MouseRight,
		e_MouseMiddle
	};
}


//====================================================================
//====================================================================
camCameraManipOrbit::camCameraManipOrbit()
:	m_bHoldingLeftInput(false), m_bHoldingRightInput(false)
{
	SetTarget(maPoint3d(0, 0, 0));
	SetPitch(45.0f * maConstants::c_fAngleToRad);
	SetYaw(0.2f);//180.0f * maConstants::c_fAngleToRad);
	SetRadius(30.0f);
}


//====================================================================
//	Think should be called to update camera and push
//	changes into g3d system.
//====================================================================
void
camCameraManipOrbit::Think()
{
	if (!GetCamera()) return;

	// dont do anything if the window does not have focus
	scCamera& camera = (*GetCamera());
	const float frame_time_delta = appSimTime::GetDelta();
	const float pan_multiplier = 50.0f;	// units/sec
	const float orbit_multiplier = 20.0f; // degs/sec

	// Handle user input
	inKeyboard* pKeyboard = inDeviceMgr::GetKeyboard();
	inMouse* pMouse = inDeviceMgr::GetMouse();

	DBG_ASSERT0(pKeyboard, "A Keyboard is required for this camera!");

	if( pKeyboard->IsHeld(inKeys::e_NUMPAD4) )  // pan left
	{
		maVector3d shift = camera.GetLeft();
		shift *= pan_multiplier * frame_time_delta;
		SafeSetTarget(GetTarget() + shift);
	}

	if( pKeyboard->IsHeld(inKeys::e_NUMPAD6) )	// pan right
	{
		maVector3d shift = camera.GetLeft();
		shift *= -pan_multiplier * frame_time_delta;
		SafeSetTarget(GetTarget() + shift);
	}

	if( pKeyboard->IsHeld(inKeys::e_NUMPAD9) )	// pan up
	{
		maVector3d shift = camera.GetUp();
		shift *= pan_multiplier * frame_time_delta;
		SafeSetTarget(GetTarget() + shift);
	}

	if( pKeyboard->IsHeld(inKeys::e_NUMPAD3) )	// pan down
	{
		maVector3d shift = camera.GetUp();
		shift *= -pan_multiplier * frame_time_delta;
		SafeSetTarget(GetTarget() + shift);
	}

	if( pKeyboard->IsHeld(inKeys::e_NUMPAD8) )	// Dolly Forward
	{
		maVector3d shift = camera.GetDirection();
		shift *= pan_multiplier * frame_time_delta;
		SafeSetTarget(GetTarget() + shift);
	}

	if( pKeyboard->IsHeld(inKeys::e_NUMPAD5) || 
		pKeyboard->IsHeld(inKeys::e_NUMPAD2)    )	// Dolly Back
	{
		maVector3d shift = camera.GetDirection();
		shift *= -pan_multiplier * frame_time_delta;
		SafeSetTarget(GetTarget() + shift);
	}

	if( pKeyboard->IsHeld(inKeys::e_LEFT) )	// orbit left
	{
		float angle = GetYaw();
		angle += -orbit_multiplier * maConstants::c_fAngleToRad * frame_time_delta;
		SetYaw(angle);
	}

	if( pKeyboard->IsHeld(inKeys::e_RIGHT) )		// orbit right
	{
		float angle = GetYaw();
		angle += orbit_multiplier * maConstants::c_fAngleToRad * frame_time_delta;
		SetYaw(angle);
	}

	if( pKeyboard->IsHeld(inKeys::e_UP) )		// orbit up
	{
		float angle = GetPitch();
		angle += orbit_multiplier * maConstants::c_fAngleToRad * frame_time_delta;
		SetPitch(angle);
	}

	if( pKeyboard->IsHeld(inKeys::e_DOWN) )		// orbit down
	{
		float angle = GetPitch();
		angle += -orbit_multiplier * maConstants::c_fAngleToRad * frame_time_delta;
		SetPitch(angle);
	}

	// Handle mouse input
	if ( pMouse )
	{
		if ( pKeyboard->IsHeld(inKeys::e_LALT) || pKeyboard->IsHeld(inKeys::e_RALT))
		{
			if( pMouse->IsHeld( e_MouseLeft ) && pMouse->IsHeld( e_MouseRight ) )
			{
				// ZOOM
				m_bHoldingLeftInput = true;
				m_bHoldingRightInput = true;

				int dX, dY;
				pMouse->GetAnalogDir(0, dX, dY);

				IncrementRadius( lc_ZoomRadiusModifier * (dY-dX));

			}
			else if( pMouse->IsHeld(e_MouseLeft) )
			{
				// ROTATE
				m_bHoldingLeftInput = true;
				int dX, dY;
				pMouse->GetAnalogDir(0, dX, dY);
				
				IncrementPitch( lc_RotatePitchModifier * dY);
				IncrementYaw( -lc_RotateYawModifier * dX);
			}
			else if( pMouse->IsHeld(e_MouseRight) )
			{
				// PAN
				m_bHoldingRightInput = true;
				int dX, dY;
				pMouse->GetAnalogDir(0, dX, dY);
				scCamera& camera = (*GetCamera());
				maVector3d x_shift = camera.GetLeft();
				maVector3d y_shift = camera.GetUp();
				
				const maPoint3d& old_pos = this->GetTarget();
				float pan_mul = GetRadius() * lc_fPanMultiplier;
				this->SafeSetTarget( old_pos + 
					(x_shift * pan_mul * dX) + 
					(y_shift * pan_mul * dY) );
			}
		}

		//stop holding input once the button is released
		if (pMouse->IsReleased(e_MouseLeft))
		{
			m_bHoldingLeftInput = false;
		}

		//stop holding input once the button is released
		if (pMouse->IsReleased(e_MouseRight))
		{
			m_bHoldingRightInput = false;
		}
	}

	scCameraManipOrbit::Think();
}

//====================================================================
//	SafeSetTarget - only set if valid target position
//====================================================================
void
camCameraManipOrbit::SafeSetTarget(const maPoint3d &i_Target)
{
	SetTarget(i_Target);
}

void
camCameraManipOrbit::IncrementPitch(float i_Radians)
{
	float pitch = GetPitch() + i_Radians;
	if (pitch > c_MaxPitch) pitch = c_MaxPitch;
	else if (pitch < c_MinPitch) pitch = c_MinPitch;
	SetPitch( pitch );
}

void
camCameraManipOrbit::IncrementYaw(float i_Radians)
{
	SetYaw( GetYaw() + i_Radians);
}

void
camCameraManipOrbit::IncrementRadius(float i_Radius)
{
	float radius = GetRadius() + i_Radius;
	if (radius > c_MaxRadius) radius = c_MaxRadius;
	else if (radius < c_MinRadius) radius = c_MinRadius;
	SetRadius( radius );
}
