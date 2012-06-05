/*****************************************************************************
**	cam3dManipOrthoPan.cpp
**
**		cam3dManipOrthoPan allows user to freely move camera
**	by orbiting around target point and translating target.
**
**	StudioGPU
**	Copyright(C) 2000-2002 - All Rights Reserved
\****************************************************************************/
#include "Tool/cam3d/cam3dManipOrthoPan.hpp"

#include "Core/app/appSimTime.hpp"
#include "Core/ma/maConstants.hpp"
#include "Graphics/Cam/camCameraManipTarget.hpp"
#include "Input/in/inDeviceMgr.hpp"
#include "Tool/cam3d/cam3dMgr.hpp"
#include "Tool/tma3d/tma3dCursorMgr.hpp"
#include "Tool/tma3d/tma3dScreenUtil.hpp"


//============================================================================
//============================================================================
namespace
{
	const float c_MinWidth = 0.2f;
	//const float c_PanModifier = 0.2f;
	
	float l_CameraPanRate = 1.0f;

	//--------------------------------------------------------------------
	// get_shift_from_cursor_drag - convert mouse motion in screen
	//	space into a world vector motion at distance of target point.
	//--------------------------------------------------------------------
	maVector3d get_shift_from_cursor_drag(const camCameraManipTarget &i_Camera,
										  int i_DeltaX,
										  int i_DeltaY)
	{
		// Get the delta vector in screen space
		maPoint2d origin = tma3dScreenUtil::ScreenToWindowPosition( maPoint2d(0,0) );
		origin.m_X  += i_DeltaX;
		origin.m_Y  += i_DeltaY;
		maPoint2d sc_diff = tma3dScreenUtil::WindowToScreenPosition(origin);

		maVector3d Diff = -sc_diff.GetX() * i_Camera.GetLeft() + sc_diff.GetY() * i_Camera.GetUp();
		Diff *= i_Camera.GetOrthoWidth() * 0.5f;
	
		return Diff;
	}

	//------------------------------------------------------------------------
	// clamp_value clamps x within the bounds of min and max
	//------------------------------------------------------------------------
	float clamp_value( float x, float min, float max )
	{
		return x < min ? min : ( x > max ? max : x );
	}
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
cam3dManipOrthoPan::cam3dManipOrthoPan()
:	m_MouseDownPosition(0,0,0)
{
}


//--------------------------------------------------------------------
//	Think should be called to update camera and push
//	changes into g3d system.
//--------------------------------------------------------------------
void
cam3dManipOrthoPan::Think()
{
	camCameraManipTarget *pCamera = GetCamera();
	if (!pCamera) return;

	// dont do anything if the window does not have focus
	if (tma3dCursorMgr::IsCursorOverView())
	{
		inKeyboard* pKeyboard = inDeviceMgr::GetKeyboard();
		inMouse* pMouse = inDeviceMgr::GetMouse();

		cam3dManipUtil::GetCameraControls(pMouse, pKeyboard, m_ManipControls);
		//inPCVJoy *pVJoy = inDeviceMgr::GetPCVJoy();

		// Handle mouse input
		//
		if ( pMouse )
		{
			//if (   pKeyboard->IsDown(inKeys::e_LALT) 
			//	|| pKeyboard->IsDown(inKeys::e_RALT))
			if( m_ManipControls.m_bValidControls )
			{
				// record the right mouse click for posterity
				//if ( pVJoy->IsPressed(cam3dVJoystick::e_RightClick) )
				/*if ( pMouse->IsPressed( 1 ) )
				{
					tma3dCursorMgr::PlaneIntersection(	pCamera->GetTarget(),
														-( pCamera->GetDirection() ),
														m_MouseDownPosition);
				}*/

				// mouse position can zoom in/out
				//
				int mx,my;
				float mz;
				pMouse->GetPosition(mx,my,mz);
				if (mz != 0.0f)
				{
					static float r_modifier = 30.0f; //0.9f;
					IncrementWidth(mz, r_modifier);
				}

				//if( pVJoy->IsHeld(cam3dVJoystick::e_LeftClick) &&  pVJoy->IsHeld(cam3dVJoystick::e_RightClick))
				//if (   ( pMouse->IsHeld( 1 ) )
				//	&& ( pMouse->IsHeld( 0 ) ) )
				if( m_ManipControls.m_CamManipMode == cam3dManipUtil::e_ZOOM )
				{
					// ZOOM
					int dX, dY, dZ;
					//pVJoy->GetAnalogDir(0, dX, dY);	//stick #0
					if(!m_ManipControls.m_bScrollWheel)
						pMouse->GetAnalogDir(0, dX, dY);	//stick #0
					else
						pMouse->GetAnalogDir(0, dX, dY, dZ);	//stick #0
						
					//DBG_LOG2( "mouse %d %d", dX, dY );
					//pVJoy->GetAnalogDir(0, dX, dY);	//stick #0
					//DBG_LOG2( "mouse %d %d", dX, dY );

					if( m_ManipControls.m_bInverseZoom )
						dY *= -1;

					static float r_modifier = 5.0f; // 0.5f;
					if(!m_ManipControls.m_bScrollWheel && !m_ManipControls.m_bXYZoom)
					{
						IncrementWidth((float)dY, r_modifier);
					}
					else if(!m_ManipControls.m_bScrollWheel && m_ManipControls.m_bXYZoom)
					{
						//make it so positive x movement, zooms in.
						dX *= -1;
						float zoom_val = (float)(dY + dX);
						if( dY > dX )
							zoom_val = clamp_value(zoom_val, (float)dX, (float)dY);
						else if( dY < dX )
							zoom_val = clamp_value(zoom_val, (float)dY, (float)dX);
						else
							zoom_val = (float)dY;
						
						IncrementWidth(zoom_val, r_modifier);
					}
					else if(dZ != 0)
					{
						IncrementWidth((float)(dZ*-10), r_modifier);
					}
					
				}
				//else if( pVJoy->IsHeld(cam3dVJoystick::e_LeftClick) )
				//else if( pMouse->IsHeld( 0 ) )
				else if( m_ManipControls.m_CamManipMode == cam3dManipUtil::e_ROTATE )
				{
					// NO ROTATION IN THIS MANIP
				}
				//else if( pVJoy->IsHeld(cam3dVJoystick::e_RightClick) )
				//else if( pMouse->IsHeld( 1 ) )
				else if( m_ManipControls.m_CamManipMode == cam3dManipUtil::e_PAN )
				{
					// PAN
					int dX, dY;
					//pVJoy->GetAnalogDir(0, dX, dY);//stick #0
					pMouse->GetAnalogDir(0, dX, dY);

					//float c_PanModifier = 1.0f * l_CameraPanRate;
					//IncrementPan( c_PanModifier * dX, c_PanModifier * dY )

					maVector3d plane_shift = get_shift_from_cursor_drag(*GetCamera(), dX, dY);
					const maPoint3d& old_tgt = GetCamera()->GetTarget();
					const maPoint3d& old_pos = GetCamera()->GetPosition();
					pCamera->LookAt(old_pos-plane_shift, old_tgt-plane_shift, GetCamera()->GetUp());
				}
				
				float c_DollyModifier = 50.0f;
				if ( pKeyboard->IsReleased( inKeys::e_UP ) )
				{
					// Dolly forward target and position
					IncrementDolly(c_DollyModifier);
				}
				else if ( pKeyboard->IsReleased( inKeys::e_DOWN ) )
				{
					// Dolly backwards target and position
					IncrementDolly(-c_DollyModifier);
				}
				//else if ( pKeyboard->IsReleased( inKeys::e_RIGHT ) )
				//{
				//	// ROTATE
				//	int dX = 1;
				//	int dY = 0;
				//	float p_modifier = 0.010f;
				//	float y_modifier = 0.010f;

				//	IncrementPitch( p_modifier * dY);
				//	IncrementYaw( y_modifier * dX);
				//}
				//else if ( pKeyboard->IsReleased( inKeys::e_LEFT ) )
				//{
				//	// ROTATE
				//	int dX = -1;
				//	int dY = 0;
				//	float p_modifier = 0.010f;
				//	float y_modifier = 0.010f;

				//	IncrementPitch( p_modifier * dY);
				//	IncrementYaw( y_modifier * dX);
				//}
			}
		}
	}

	camCameraManip::Think();
}

//--------------------------------------------------------------------
// Focus_Camera centers camera with respect to the point
//--------------------------------------------------------------------
void cam3dManipOrthoPan::FocusCamera(const maPoint3d& i_Focus, float i_Radius, maAxisBox i_Box)
{
	camCameraManipTarget* pCamera = GetCamera();

	// Same view direction, just shifted to new target
	maVector3d offset = pCamera->GetPosition() - pCamera->GetTarget();
	maPoint3d new_pos = i_Focus + offset;
	pCamera->LookAt(new_pos, i_Focus, pCamera->GetUp());

	static float width_modifier = 1.6f;
	pCamera->SetOrthoWidth( width_modifier * i_Radius );

	cam3dMgr::SetManipDepth( (new_pos - i_Focus).Length() );
}

//--------------------------------------------------------------------
// SetPanRate sets the speed of camera panning (tracking)
//--------------------------------------------------------------------
void cam3dManipOrthoPan::SetPanRate( float i_Rate )
{
	l_CameraPanRate = i_Rate;
}

//--------------------------------------------------------------------
// Shift position and target by given amounts along left and
//	up direction of camera.
//--------------------------------------------------------------------
void
cam3dManipOrthoPan::IncrementPan(float i_LeftDelta, float i_UpDelta)
{
	camCameraManipTarget* pCamera = GetCamera();
	maVector3d shift = i_LeftDelta * pCamera->GetLeft() +
						i_UpDelta * pCamera->GetUp();

	maPoint3d new_pos = pCamera->GetPosition() + shift;
	maPoint3d new_target = pCamera->GetTarget() + shift;

	pCamera->LookAt(new_pos, new_target, pCamera->GetUp());
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cam3dManipOrthoPan::IncrementWidth(float i_ZoomDiff, float i_RadModifier)
{
	camCameraManipTarget* pCamera = GetCamera();
	float new_width = pCamera->GetOrthoWidth() + i_RadModifier * i_ZoomDiff;
	if (new_width > c_MinWidth)
	{
		pCamera->SetOrthoWidth( new_width );
	}
}

//--------------------------------------------------------------------
// Move camera forward/back on view direction
//--------------------------------------------------------------------
void cam3dManipOrthoPan::IncrementDolly(float i_Motion)
{
	camCameraManipTarget* pCamera = GetCamera();
	maVector3d shift = pCamera->GetDirection() * i_Motion;

	maPoint3d new_pos = pCamera->GetPosition() + shift;
	maPoint3d new_target = pCamera->GetTarget() + shift;
	pCamera->LookAt(new_pos, new_target, pCamera->GetUp());
}
