/*****************************************************************************
**	tma3dCursorMgr.cpp
**
**		The tma3dCursorMgr provides access to the list of tile templates
**
**	StudioGPU
**	Copyright(C) 2000-2002 - All Rights Reserved
\****************************************************************************/
#include "Tool/tma3d/tma3dCursorMgr.hpp"

#include "Core/geo/geoRayIntersection.hpp"
#include "Input/in/inDeviceMgr.hpp"
#include "Input/in/inVirtualJoystick.hpp"
#include "Tool/cam3d/cam3dMgr.hpp"
#include "Tool/tma3d/tma3dScreenUtil.hpp"


//============================================================================
//============================================================================
namespace tma3dCursorMgr
{
	namespace
	{
		int l_nX = 0;
		int l_nY = 0;
		bool l_bCursorOverView = true; //bga - changing default to true

		maPoint3d l_MouseDownPickPoint( 0.0f, 0.0f, 0.0f );
		maPoint3d l_LastMouseDownPickPoint( 0.0f, 0.0f, 0.0f );
		maPoint3d l_CurrentPickPoint( 0.0f, 0.0f, 0.0f );

		void (*l_pCallback)(bool i_bWait) = NULL;
		bool l_bWait = false;

		const float c_PickRayLength = 1000.0f;
	}

	//============================================================================
	//	Think
	//============================================================================
	void Think()
	{ //PROFILE_FUNC();
		//TerrainIntersection(l_CurrentPickPoint);

		//inVirtualJoystick *pVJoy = inDeviceMgr::GetVirtualJoystick();

		// record the mouse press and release points
		//if (!l_bWait &&
		//	(pVJoy->IsPressed(mnmVJoystick::e_LeftClick)) )
		//{
		//	l_LastMouseDownPickPoint = l_MouseDownPickPoint;
		//	l_MouseDownPickPoint = l_CurrentPickPoint;
		//}

		// debug output of cursor information
		//char CursorPos[64];
		//sprintf( CursorPos, "screenPos: (%d,%d)", l_nX, l_nY );
		//g2dScreen::SetDebugInfo( 9, CursorPos );
		//sprintf( CursorPos, "Pos: %.2f %.2f %.2f", l_CurrentPickPoint.GetX(),l_CurrentPickPoint.GetY(),l_CurrentPickPoint.GetZ() );
		//g2dScreen::SetDebugInfo( 10, CursorPos );

//		maIntPoint2d grid_point;
//		tilGridSpec::PointToGridPosition(l_CurrentPickPoint,grid_point);
//
//		sprintf( CursorPos, "GridPos: %i %i", grid_point.GetX(),grid_point.GetY() );
//		g2dScreen::SetDebugInfo( 11, CursorPos );

	}

	//============================================================================
	// returns true if the cursor is over the terawatt window
	//============================================================================
	bool IsCursorOverView()
	{
		return l_bCursorOverView;
	}

	//============================================================================
	//	CursorPosChangedCallback gets called whenever the cursor is over the view
	//============================================================================
	void CursorOverViewCallback( bool i_OverView )
	{
		l_bCursorOverView = i_OverView;
	}

	//============================================================================
	//	GetSelectedItemIndex returns the x, y position of the cursor in client space.
	//============================================================================
	void GetCursorPos( int& o_nX, int& o_nY)
	{
		o_nX = l_nX;
		o_nY = l_nY;
	}

	//============================================================================
	//	CursorPosChangedCallback gets called whenever the cursor moves
	//============================================================================
	void CursorPosChangedCallback( int i_nX, int i_nY )
	{
		l_nX = i_nX;
		l_nY = i_nY;
	}

	//====================================================================
	// GetCurrentPickPoint returns the current pick point of the cursor
	//====================================================================
	maPoint3d GetCurrentPickPoint()
	{
		return l_CurrentPickPoint;
	}

	//====================================================================
	// GetCurrentPickPoint returns the mouse down pick point of the cursor
	//====================================================================
	maPoint3d GetMouseDownPickPoint()
	{
		return l_MouseDownPickPoint;
	}

	//====================================================================
	// GetLastMouseDownPickPoint returns the previous mouse down pick
	// point of the cursor
	//====================================================================
	maPoint3d GetLastMouseDownPickPoint()
	{
		return l_LastMouseDownPickPoint;
	}

	//====================================================================
	// GetCursorRay translates the cursor positon into a vector
	// from the camera position through the cursor position.  The length
	// is arbitrary but should be large.
	//====================================================================
	void GetCursorRay(maPoint3d& o_Start, maVector3d& o_Dir)
	{
		//	get cursor screen position
		maPoint2d cursor_pos = tma3dScreenUtil::WindowToScreenPosition(maPoint2d((float)l_nX,(float)l_nY));

//		DBG_LOG2("Window Pos: %d %d", l_nX, l_nY);
//		DBG_LOG2("Cursor Pos: %f %f", cursor_pos.GetX(), cursor_pos.GetY());

		camCamera &camera = cam3dMgr::GetCamera();
		float maxDist = camera.GetFarClip();	//use the far clip plane as the max ray distance

		//	make world ray
		maPoint3d screen_pos = camera.GetScreenPoint( cursor_pos.GetX() , cursor_pos.GetY() );
		if (camera.IsOrthographic())
		{
			o_Start = screen_pos;
			o_Dir = camera.GetDirection() * maxDist;
		}
		else
		{
			o_Start = camera.GetPosition();
			maVector3d dir = screen_pos - o_Start;
			dir.Normalize();
			o_Dir = dir * maxDist;
		}
	}

	//====================================================================
	// PlaneIntersection checks to see if the cursor ray intersects with
	// the plane defined by a point and a normal.
	//====================================================================
	bool PlaneIntersection(	const maPoint3d& i_PlanePoint,
							const maPoint3d& i_PlaneNormal,
							maPoint3d& o_PickPos)
	{
		float t = 0;
		return PlaneIntersection( i_PlanePoint, i_PlaneNormal, o_PickPos, t );
	}

	//====================================================================
	// PlaneIntersection checks to see if the cursor ray intersects with
	// the plane defined by a point and a normal.
	//====================================================================
	bool PlaneIntersection(	const maPoint3d& i_PlanePoint,
							const maPoint3d& i_PlaneNormal,
							maPoint3d& o_PickPos,
							float& o_fT)
	{
		float attempt_t = 2.0f;

		maPoint3d camera_position;
		maVector3d ray_dir;
		GetCursorRay(camera_position, ray_dir);

		bool intersect = geoRayIntersection::ProjectLineToPlane(camera_position,
																ray_dir,
																i_PlanePoint,
																i_PlaneNormal,
																attempt_t);

		if( intersect )
		{
			o_PickPos = camera_position + ray_dir * attempt_t;
			o_fT = attempt_t;
		}

		return intersect;
	}

	//====================================================================
	// LineIntersection finds closest point between cursor ray and 
	//	the given line
	//====================================================================
	bool LineIntersection(const maPoint3d &i_LinePoint, 
						  const maVector3d &i_LineDir,
						  maPoint3d& o_PickPos)
	{
		float ray_t = 2.0f, line_t = 0;

		maPoint3d camera_position;
		maVector3d ray_dir;
		GetCursorRay(camera_position, ray_dir);

		bool intersect = geoRayIntersection::ProjectLineToLine(	camera_position,
																ray_dir,
																i_LinePoint, 
																i_LineDir,
																ray_t,
																line_t);
		
		if( intersect )
		{
			DBG_LOG("Line t: " << line_t << " ray_t " << ray_t);
			o_PickPos = i_LinePoint + i_LineDir * line_t;
		}

		return intersect;
	}

	//====================================================================
	// TerrainIntersection checks to see if the cursor ray intersects with
	// the the terrain
	//====================================================================
	//bool TerrainIntersection(maPoint3d& o_PickPos)
	//{
	//	maPoint3d camera_position = cam3dMgr::GetCamera().GetPosition();
	//	maVector3d ray_dir;
	//	GetCursorRay(ray_dir);

	//	float dummy_t;
	//	if( mgzTerrain::SegmentIntersect(camera_position, camera_position + 100.0f * ray_dir, o_PickPos, dummy_t) )
	//		return true;

	//	//	failed to collide terrain, try the ground plane
	//	float t;
	//	if( geoRayIntersection::ProjectLineToPlane(	camera_position,
	//												ray_dir,
	//												maPoint3d(0, 0, 0),
	//												maVector3d(0, 1, 0),
	//												t) )
	//	{
	//		if( t > 0 )
	//		{
	//			o_PickPos = camera_position + ray_dir * t;
	//			return true;
	//		}
	//	}

	//	return false;
	//}

	//============================================================================
	// SetCursorCallback()
	//============================================================================
	void SetCursorCallback( void (*i_Callback)(bool i_bWait) )
	{
		l_pCallback = i_Callback;
	}

	//============================================================================
	// SetCursorCallback()
	//============================================================================
	void SetWaitCursor( bool bWait )
	{
		//???????//
		if (NULL != l_pCallback)
		{
			l_pCallback(bWait);
		}

		if ( true == bWait )
		{
			l_bWait = true;
		}
		else
		{
			l_bWait = false;
		}
	}
}