/*****************************************************************************
**	tma3dCursorMgr.hpp
**
**		The tma3dCursorMgr keeps track of the cursor
**
**	StudioGPU
**	Copyright(C) 2000-2002 - All Rights Reserved
\****************************************************************************/
#ifdef TMA3D_CURSORMGR_HPP
#error tma3dCursorMgr.hpp multiply included
#endif
#define TMA3D_CURSORMGR_HPP

#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif


//============================================================================
//============================================================================
namespace tma3dCursorMgr
{
	//----------------------------------------------------------------------------
	//	Think
	//----------------------------------------------------------------------------
	void Think();

	//----------------------------------------------------------------------------
	// IsCursorOverView returns true if the cursor is over the terawatt window
	//----------------------------------------------------------------------------
	bool IsCursorOverView();

	//----------------------------------------------------------------------------
	//	CursorPosChangedCallback gets called whenever the cursor is over the view
	//----------------------------------------------------------------------------
	void CursorOverViewCallback( bool i_OverView );

	//----------------------------------------------------------------------------
	//	GetSelectedItemIndex returns the x, y position of the cursor in client space.
	//----------------------------------------------------------------------------
	void GetCursorPos( int& o_nX, int& o_nY);

	//----------------------------------------------------------------------------
	//	CursorPosChangedCallback gets called whenever the cursor moves
	//----------------------------------------------------------------------------
	void CursorPosChangedCallback( int i_nX, int i_nY );

	//--------------------------------------------------------------------
	// GetCurrentPickPoint returns the current pick point of the cursor
	//--------------------------------------------------------------------
	maPoint3d GetCurrentPickPoint();

	//--------------------------------------------------------------------
	// GetCurrentPickPoint returns the mouse down pick point of the cursor
	//--------------------------------------------------------------------
	maPoint3d GetMouseDownPickPoint();

	//--------------------------------------------------------------------
	// GetLastMouseDownPickPoint returns the previous mouse down pick
	// point of the cursor
	//--------------------------------------------------------------------
	maPoint3d GetLastMouseDownPickPoint();

	//--------------------------------------------------------------------
	// GetCursorRay translates the cursor positon into a vector
	// from the camera position through the cursor position.  The length
	// is arbitrary but should be large.
	//--------------------------------------------------------------------
	void GetCursorRay(maPoint3d& o_Start, maVector3d& o_Dir);

	//--------------------------------------------------------------------
	// PlaneIntersection checks to see if the cursor ray intersects with
	// the plane defined by a point and a normal.
	//--------------------------------------------------------------------
	bool PlaneIntersection(	const maPoint3d& i_PlanePoint,
							const maPoint3d& i_PlaneNormal,
							maPoint3d& o_PickPos);

	//--------------------------------------------------------------------
	// PlaneIntersection checks to see if the cursor ray intersects with
	// the plane defined by a point and a normal.
	//--------------------------------------------------------------------
	bool PlaneIntersection(	const maPoint3d& i_PlanePoint,
							const maPoint3d& i_PlaneNormal,
							maPoint3d& o_PickPos,
							float& o_fT);

	//--------------------------------------------------------------------
	// LineIntersection finds closest point between cursor ray and 
	//	the given line
	//--------------------------------------------------------------------
	bool LineIntersection(const maPoint3d &i_LinePoint, 
						  const maVector3d &i_LineDir,
						  maPoint3d& o_PickPos);

	//--------------------------------------------------------------------
	// TerrainIntersection checks to see if the cursor ray intersects with
	// the the terrain
	//--------------------------------------------------------------------
	bool TerrainIntersection( maPoint3d& o_PickPos );

	//----------------------------------------------------------------------------
	// SetCursorCallback()
	//----------------------------------------------------------------------------
	void SetCursorCallback( void (*i_Callback)(bool i_bWait) );

	//----------------------------------------------------------------------------
	// SetCursorCallback()
	//----------------------------------------------------------------------------
	void SetWaitCursor( bool bWait );
}
