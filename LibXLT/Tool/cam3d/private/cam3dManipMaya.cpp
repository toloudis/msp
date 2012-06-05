/*****************************************************************************
**	cam3dManipMaya.cpp
**
**		cam3dManipMaya allows user to freely move camera
**	by orbiting around target point and translating target.
**
**	StudioGPU
**	Copyright(C) 2000-2002 - All Rights Reserved
\****************************************************************************/
#include "Tool/cam3d/cam3dManipMaya.hpp"

#include "Core/app/appSimTime.hpp"
#include "Core/ma/maConstants.hpp"
#include "Graphics/Cam/camCameraManipTarget.hpp"
#include "Graphics/g3d/g3dPrefs.hpp"
#include "Graphics/g3d/g3dSingleLightRendering.hpp"
#include "Input/in/inDeviceMgr.hpp"
#include "Tool/cam3d/cam3dMgr.hpp"
#include "Tool/gpx/gpxRenderControl.hpp"
#include "Tool/tma3d/tma3dCursorMgr.hpp"
#include "Tool/tma3d/tma3dScreenUtil.hpp"

#include <limits>


//============================================================================
//============================================================================
namespace
{
	const float c_MaxPitch = 89.0f * maConstants::c_fAngleToRad;
	const float c_MinPitch = -89.0f * maConstants::c_fAngleToRad;
	const float c_MaxRadius = FLT_MAX; //10000.0f;
	const float c_MinRadius = 0.2f;
	const float c_MinRadiusMod = 2.0f;

	const float c_MinAboveGround = 5.0f;

	float l_CameraPanRate = 1.0f;

	float l_CameraDynamicZoomRate = 0.18f;
	float l_CameraFixedZoomRate = 0.0001f;

	//maVector3d l_MouseDownPosition(0,0,0);
	//------------------------------------------------------------------------
	// clamp_value clamps x within the bounds of min and max
	//------------------------------------------------------------------------
	float clamp_value( float x, float min, float max )
	{
		return x < min ? min : ( x > max ? max : x );
	}

	//----------------------------------------------------------------------------
	// point_outofbounds - return true if point is out of bounds
	//----------------------------------------------------------------------------
	bool
	point_outofbounds(const maPoint3d &i_Pt)
	{
		return false;
	}

	//--------------------------------------------------------------------
	// Project current screen cursor position to camera plane position
	//--------------------------------------------------------------------
	//void get_camera_plane_point(const maPoint3d i_Target,
	//							const camCamera &i_Camera,
	//							maPoint3d &o_MousePosition)
	//{
	//	tma3dCursorMgr::PlaneIntersection(	i_Target,
	//										-( i_Camera.GetDirection() ),
	//										o_MousePosition);
	//}
				

	//--------------------------------------------------------------------
	// get_left_from_cursor_drag
	//--------------------------------------------------------------------
	//float get_left_from_cursor_drag( const maPoint3d& i_Target )
	//{
	//	maVector3d camera_dir = cam3dMgr::GetCamera().GetDirection();
	//	maVector3d camera_left = cam3dMgr::GetCamera().GetLeft();

	//	// Get the current mouse position on the plane
	//	maPoint3d intersect_point;
	//	tma3dCursorMgr::PlaneIntersection(	i_Target,
	//										-(camera_dir),
	//										intersect_point );

	//	// Get the difference between the two points
	//	maVector3d Diff = intersect_point - l_MouseDownPosition;

	//	return camera_left * Diff; // length directionalized
	//}

	//--------------------------------------------------------------------
	// get_up_from_cursor_drag
	//--------------------------------------------------------------------
	//float get_up_from_cursor_drag( const maPoint3d& i_Target )
	//{
	//	maVector3d camera_dir = cam3dMgr::GetCamera().GetDirection();
	//	maVector3d camera_up = cam3dMgr::GetCamera().GetUp();

	//	// Get the current mouse position on the plane
	//	maPoint3d intersect_point;
	//	tma3dCursorMgr::PlaneIntersection(	i_Target,
	//										-(camera_dir),
	//										intersect_point);

	//	// Get the difference between the two points
	//	maVector3d Diff = intersect_point - l_MouseDownPosition;

	//	return camera_up * Diff; // length directionalized
	//}
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
cam3dManipMaya::cam3dManipMaya()
{
	m_ManipPrefs = new g3dPrefs::g3dRenderPrefs;
	m_ManipPrefs->m_bMultipassOn = false;
	m_ManipPrefs->m_bHeadlightOn = true;
	m_ManipPrefs->m_bEnableReflection = false;

	m_RestorePrefs = NULL;

	SetTarget(maPoint3d(0, 0, 0));
	SetPitch(45.0f * maConstants::c_fAngleToRad);
	SetYaw(180.0f * maConstants::c_fAngleToRad);
	SetRadius(10+0.0f);
	SetZoomFactor(10+0.0f);
}

//--------------------------------------------------------------------
// get_shift_from_cursor_drag - convert mouse motion in screen
//	space into a world vector motion at distance of target point.
//--------------------------------------------------------------------
maVector3d cam3dManipMaya::get_shift_from_cursor_drag(const camCameraManipTarget &i_Camera,
									  const maPoint3d& i_Target,
									  int i_DeltaX,
									  int i_DeltaY,
									  float i_Depth)
{
	// Get the delta vector in screen space
	maPoint2d origin = tma3dScreenUtil::ScreenToWindowPosition( maPoint2d(0,0) );
	origin.m_X  += i_DeltaX;
	origin.m_Y  += i_DeltaY;
	maPoint2d sc_diff = tma3dScreenUtil::WindowToScreenPosition(origin);

	maVector3d leftVec = i_Camera.GetLeft();
	maVector3d upVec = i_Camera.GetUp();

	maVector3d Diff = -sc_diff.GetX() * leftVec + sc_diff.GetY() * upVec;

	//float target_distance = (i_Camera.GetPosition() - i_Target).Length();
	float target_distance = abs( i_Depth - i_Camera.GetNearClip() );
	Diff *= target_distance;

	return Diff;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
cam3dManipMaya::~cam3dManipMaya()
{
	DBG_ASSERT(&g3dPrefs::CurrentPrefs() != m_ManipPrefs, "Manipulator preferences still installed on viewport");
	delete m_ManipPrefs;
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cam3dManipMaya::GetRadiusMod(float* o_PositionMod, float* o_TargetMod)
{
	camCameraManipTarget& camera = (*GetCamera());

	float factor = camera.GetFarClip() * l_CameraFixedZoomRate;

	float tNear = 0;
	float tFar = 0;
	float * pNear = &tNear;
	float * pFar = &tFar;

	bool hit =  m_CurrBox.IntersectRay_Suffern( camera.GetPosition(), 
											    camera.GetDirection()/(camera.GetDirection().Length()),
												pNear,
												pFar );

	//DBG_LOG("tNear = " << tNear);
	//DBG_LOG("tFar = " << tFar);
	//DBG_LOG( " " );

	if ( hit )
	{
		*o_PositionMod = tFar * l_CameraDynamicZoomRate;

		float d = abs(tFar - tNear) / 2;
		float myDist = tFar / camera.GetFarClip();

		if ( myDist == 1 )
		{
			*o_TargetMod = 0;
		}
		else
		{
			*o_TargetMod = d * ( 1 - myDist );
		}

	}
	else
	{
		*o_PositionMod = (camera.GetPosition() - m_CurrBox.GetCenter()).Length() * l_CameraDynamicZoomRate;
		*o_TargetMod = 0;
	}

	float targetMod = *o_TargetMod;

	DBG_LOG("targetMod = " << targetMod);

}

//--------------------------------------------------------------------
//	Think should be called to update camera and push
//	changes into g3d system.
//--------------------------------------------------------------------
void
cam3dManipMaya::Think()
{
	if (m_RestorePrefs != NULL)
	{
		// stop any render threads
		gpxRenderControl::ConfirmSingleThread();

		g3dSingleLightRendering::SetDoSingleLightRendering(m_RestorePrefs->m_bMultipassOn);

		g3dPrefs::SetPrefs(m_RestorePrefs);
		m_RestorePrefs = NULL;
	}

	if (!GetCamera()) return;

	// dont do anything if the window does not have focus
	if (tma3dCursorMgr::IsCursorOverView())
	{
		inKeyboard* pKeyboard = inDeviceMgr::GetKeyboard();
		inMouse* pMouse = inDeviceMgr::GetMouse();

		camCameraManipTarget& camera = (*GetCamera());
		cam3dManipUtil::GetCameraControls(pMouse, pKeyboard, m_ManipControls);
		//
		// Handle user input
		//

		////if( pVJoy->IsHeld(cam3dVJoystick::e_OrbitLeft) )
		//if ( pKeyboard->IsHeld( inKeys::e_LEFT ) )
		//{
		//	float angle = GetYaw();
		//	angle += -orbit_multiplier * maConstants::c_fAngleToRad * delta;
		//	SetYaw(angle);
		//}

		////if( pVJoy->IsHeld(cam3dVJoystick::e_OrbitRight) )
		//if ( pKeyboard->IsHeld( inKeys::e_RIGHT ) )
		//{
		//	float angle = GetYaw();
		//	angle += orbit_multiplier * maConstants::c_fAngleToRad * delta;
		//	SetYaw(angle);
		//}

		////if( pVJoy->IsHeld(cam3dVJoystick::e_OrbitUp) )
		//if ( pKeyboard->IsHeld( inKeys::e_UP ) )
		//{
		//	float angle = GetPitch();
		//	angle += orbit_multiplier * maConstants::c_fAngleToRad * delta;
		//	SetPitch(angle);
		//}

		////if( pVJoy->IsHeld(cam3dVJoystick::e_OrbitDown) )
		//if ( pKeyboard->IsHeld( inKeys::e_DOWN ) )
		//{
		//	float angle = GetPitch();
		//	angle += -orbit_multiplier * maConstants::c_fAngleToRad * delta;
		//	SetPitch(angle);
		//}

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

				// There is also a state where both left and right were
				// down and then the left was released. This begins the
				// PAN motion again and needs to reset the l_MouseDownPosition.
				//if ( pMouse->IsPressed( 1 ) 
				// || (pMouse->IsReleased(0) && pMouse->IsHeld(1)) )
				//{
				//	get_camera_plane_point(	GetTarget(), *GetCamera(), l_MouseDownPosition);
				//}

				// mouse position can zoom in/out
				//
				int mx,my;
				float mz;
				pMouse->GetPosition(mx,my,mz);
				if (mz != 0.0f)
				{
					float r_modifier = camera.GetFarClip() * l_CameraFixedZoomRate;
					maVector3d dir = camera.GetDirection();
					const maPoint3d& old_pos = this->GetTarget();

					// Note: if someone want to reverse the direction
					// of motion when doing dolly, change the "mz" here
					// to "-mz".
					float factor = r_modifier * (float) mz;
					this->SafeSetTarget( old_pos + dir * factor );
				}

				//if( pVJoy->IsHeld(cam3dVJoystick::e_LeftClick) &&  pVJoy->IsHeld(cam3dVJoystick::e_RightClick))
				//if (   ( pMouse->IsHeld( 1 ) )
				//	&& ( pMouse->IsHeld( 0 ) ) )
				if( m_ManipControls.m_CamManipMode == cam3dManipUtil::e_ZOOM )
				{
					SetManipPrefs();

					// ZOOM
					int dX, dY, dZ;
					//pVJoy->GetAnalogDir(0, dX, dY);	//stick #0
					if(!m_ManipControls.m_bScrollWheel)
						pMouse->GetAnalogDir(0, dX, dY);	//stick #0
					else
						pMouse->GetAnalogDir(0, dX, dY, dZ);	//stick #0
						
					//DBG_LOG2( "mouse %d %d", dX, dY );
					if( m_ManipControls.m_bInverseZoom )
						dY *= -1;

					float maxDist = camera.GetFarClip();	//use the far clip plane as the max ray distance
					float r_modifier = 0;
					float t_modifier = 0;
					float * p_rModifier = &r_modifier;
					float * p_tModifier = &t_modifier;

					//GetRadiusMod(p_rModifier,p_tModifier);

					r_modifier = camera.GetFarClip() * l_CameraFixedZoomRate;

					if(!m_ManipControls.m_bScrollWheel && !m_ManipControls.m_bXYZoom)
					{
						zoom((float)dY, r_modifier, t_modifier);
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
						
						zoom(zoom_val, r_modifier, t_modifier);
					}
					else if(dZ != 0)
					{
						zoom((float)(dZ*-10), r_modifier, t_modifier);
					}
				}
				//else if( pVJoy->IsHeld(cam3dVJoystick::e_LeftClick) )
				//else if( pMouse->IsHeld( 0 ) )
				else if( m_ManipControls.m_CamManipMode == cam3dManipUtil::e_ROTATE )
				{
					SetManipPrefs();

					// ROTATE
					int dX, dY;
					//pVJoy->GetAnalogDir(0, dX, dY);//stick #0
					pMouse->GetAnalogDir( 0, dX, dY);

					// base modifiers on screen size
					float p_modifier = 0.010f;
					float y_modifier = 0.010f;
					IncrementPitch( p_modifier * dY);
					IncrementYaw( -y_modifier * dX);
					
				}
				//else if( pVJoy->IsHeld(cam3dVJoystick::e_RightClick) )
				//else if( pMouse->IsHeld( 1 ) )
				else if( m_ManipControls.m_CamManipMode == cam3dManipUtil::e_PAN )
				{
					SetManipPrefs();

					// PAN
					int dX, dY;
					//pVJoy->GetAnalogDir(0, dX, dY);//stick #0
					pMouse->GetAnalogDir(0, dX, dY);
					//DBG_LOG("drag pan " << dX << " " << dY);
					camCameraManipTarget& camera = (*GetCamera());
					//float multiplier = (0.1f * l_CameraPanRate);
					//maVector3d x_shift = camera.GetLeft() * get_left_from_cursor_drag(GetTarget());
					//maVector3d y_shift = camera.GetUp() * get_up_from_cursor_drag(GetTarget());
					//maVector3d plane_shift = get_shift_from_cursor_drag(camera, GetTarget());
					maVector3d plane_shift = get_shift_from_cursor_drag(camera, GetTarget(), dX, dY, m_ClickDepth);

					const maPoint3d& old_tgt = this->GetTarget();
					const maPoint3d& old_pos = camera.GetPosition();

//					this->SafeSetTarget( old_tgt + (camera.GetLeft() * multiplier * (float)dX) + (camera.GetUp() * multiplier * (float)dY) );
//					maVector3d worldDelta = (camera.GetLeft() * multiplier * (float)dX) + (camera.GetUp() * multiplier * (float)dY);
//					this->SetPositionTarget(old_pos+worldDelta, old_tgt+worldDelta);

					const maPoint3d& new_tgt = old_tgt-plane_shift;
					const maPoint3d& new_pos = old_pos-plane_shift;
					this->SetPositionTarget(new_pos, new_tgt);
					
					//this->SetPositionTarget(old_pos-x_shift-y_shift, old_tgt-x_shift-y_shift);
					//this->SafeSetTarget( old_pos - x_shift - y_shift );
				
				}
				
				if ( pKeyboard->IsReleased( inKeys::e_UP ) )
				{
					// ROTATE
					int dX = 0;
					int dY = 1;
					float p_modifier = 0.010f;
					float y_modifier = 0.010f;

					IncrementPitch( p_modifier * dY);
					IncrementYaw( y_modifier * dX);
				}
				else if ( pKeyboard->IsReleased( inKeys::e_DOWN ) )
				{
					// ROTATE
					int dX = 0;
					int dY = -1;
					float p_modifier = 0.010f;
					float y_modifier = 0.010f;

					IncrementPitch( p_modifier * dY);
					IncrementYaw( y_modifier * dX);
				}
				else if ( pKeyboard->IsReleased( inKeys::e_RIGHT ) )
				{
					// ROTATE
					int dX = 1;
					int dY = 0;
					float p_modifier = 0.010f;
					float y_modifier = 0.010f;

					IncrementPitch( p_modifier * dY);
					IncrementYaw( y_modifier * dX);
				}
				else if ( pKeyboard->IsReleased( inKeys::e_LEFT ) )
				{
					// ROTATE
					int dX = -1;
					int dY = 0;
					float p_modifier = 0.010f;
					float y_modifier = 0.010f;

					IncrementPitch( p_modifier * dY);
					IncrementYaw( y_modifier * dX);
				}
			
			//else if (   pKeyboard->IsDown(inKeys::e_LCTRL) 
			//		 || pKeyboard->IsDown(inKeys::e_RCTRL))
			//{
				//if( pVJoy->IsHeld(cam3dVJoystick::e_LeftClick) )
				//if( pMouse->IsHeld( 0 ) )
				else if( m_ManipControls.m_CamManipMode == cam3dManipUtil::e_PIVOT )
				{
					SetManipPrefs();

					// ROTATE
					int dX, dY;
					//pVJoy->GetAnalogDir(0, dX, dY);//stick #0
					pMouse->GetAnalogDir( 0, dX, dY);

					// TODO base modifiers on screen size
					//
					maVector3d shift = maVector3d((float)dX,(float)dY,0);

					const maPoint3d& old_pos = this->GetTarget();
					float multiplier = 1.0f;
					float length = (camera.GetPosition() - old_pos).Length();
					if (length < 20.0f)
					{
						multiplier = length / 20.0f;
					}
					maVector3d temp;
					temp = camera.GetLeft();
					temp.Normalize();
					maVector3d x_shift = temp * shift.GetX() * multiplier;
					temp = camera.GetUp();
					temp.Normalize();
					maVector3d y_shift = temp * shift.GetY() * multiplier;

					SetPositionTarget( camera.GetPosition(),  (old_pos - x_shift - y_shift) );
				}
			}

		}
	}
	else
	{
		// restore old view prefs
	}

	// Test if camera is above ground
	//
/*	maPoint3d pos = GetCamera()->GetPosition();
	float ground_height = 0.0f;
//	coTerrain::GetHeightOfTerrain(pos.m_X, pos.m_Z, ground_height);

	if(	pos.m_Y < ground_height + c_MinAboveGround )
	{
		const maPoint3d& cur_target = this->GetTarget();
		maPoint3d new_target = cur_target;
		new_target.m_Y += ground_height - pos.m_Y + c_MinAboveGround;
		this->SetTarget(new_target);
	}
*/

	camCameraManipOrbit::Think();
}

//--------------------------------------------------------------------
// SetPanRate sets the speed of camera panning (tracking)
//--------------------------------------------------------------------
void cam3dManipMaya::SetPanRate( float i_Rate )
{
	l_CameraPanRate = i_Rate;
}

//--------------------------------------------------------------------
//	SafeSetTarget - only set if valid target position
//--------------------------------------------------------------------
void
cam3dManipMaya::SafeSetTarget(const maPoint3d &i_Target)
{
	if (point_outofbounds(i_Target)) return;

	SetTarget(i_Target);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void
cam3dManipMaya::IncrementPitch(float i_Radians)
{
	float pitch = GetPitch() + i_Radians;
	if (pitch > c_MaxPitch)
	{
		pitch = c_MaxPitch;
	}
	else if (pitch < c_MinPitch)
	{
		pitch = c_MinPitch;
	}
	SetPitch( pitch );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void
cam3dManipMaya::IncrementYaw(float i_Radians)
{
	SetYaw( GetYaw() + i_Radians );
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cam3dManipMaya::zoom(float i_ZoomDiff, float i_RadModifier, float i_TargetModifier)
{
	//DBG_LOG( "i_ZoomDiff = " << i_ZoomDiff );
	//DBG_LOG( "i_RadModifier = " << i_RadModifier );
	//DBG_LOG( " " );

	/*
	camCameraManipTarget& camera = (*GetCamera());

	maVector3d dir = camera.GetDirection() / (camera.GetDirection().Length());
	const maPoint3d& old_target = this->GetTarget();
	float factor = i_TargetModifier;
	float posNegDir = 0;
	
	if ( i_ZoomDiff > 0 )
	{
		posNegDir = -1;
	} 
	else if ( i_ZoomDiff < 0 )
	{
		posNegDir = 1;
	}

	factor *= posNegDir;

	DBG_LOG("Factor = " << factor );

	this->SafeSetTarget( old_target + dir * factor );
	
	SetRadius( i_RadModifier );
	*/	

	
	// if the radius is all the way in then move the
	// target
	if ( i_ZoomDiff < 0 && GetRadius() <= c_MinRadius )
	{
		camCameraManipTarget& camera = (*GetCamera());
		maVector3d dir = camera.GetDirection();
		const float multiplier = 0.4f;
		const maPoint3d& old_pos = this->GetTarget();
		float factor = -i_ZoomDiff * multiplier;
		this->SafeSetTarget( old_pos + dir * factor );
	}
	else
	{
		// move the radius in
		IncrementRadius( i_RadModifier * i_ZoomDiff);
	}
	
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cam3dManipMaya::SetManipPrefs()
{
	if (m_bManipFastRender)
	{
		g3dPrefs::g3dRenderPrefs* prefs = &g3dPrefs::CurrentPrefs();
		if (prefs != m_ManipPrefs)
			m_RestorePrefs = prefs;

		// stop any render threads
		gpxRenderControl::ConfirmSingleThread();

		g3dPrefs::SetPrefs(m_ManipPrefs);
		g3dSingleLightRendering::SetDoSingleLightRendering(false);
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void
cam3dManipMaya::IncrementRadius(float i_Radius)
{
	float radius = GetRadius() + i_Radius;
	if (radius > c_MaxRadius)
	{
		radius = c_MaxRadius;
	}
	else if (radius < c_MinRadius)
	{
		radius = c_MinRadius;
	}
	SetRadius( radius );
}

