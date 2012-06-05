/*****************************************************************************
**  g2dScreenUtil.hpp
**
**      g2dScreenUtil contains screen utility function for
**		for gettng screen calculations.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef G2D_SCREENUTIL_HPP
#error g2dScreenUtil.hpp multiply included
#endif
#define G2D_SCREENUTIL_HPP

#ifndef MA_MATRIX4X4_HPP
#include "Core/ma/maMatrix4x4.hpp"
#endif

#ifndef MA_POINT2D_HPP
#include "Core/ma/maPoint2d.hpp"
#endif

#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif


//============================================================================
//============================================================================
class g2dWindow;


//============================================================================
//============================================================================
namespace g2dScreenUtil
{
	//--------------------------------------------------------------------
	//	Window / Screen Conversion Functions
	//
	//		Converts between the window coordinate system and
	//		the screen coordinate system.
	//
	//									1
	//		0------Width				|
	//		|			  /  \			|
	//		|			 <---->	 -1	----0---- 1
	//		|			  \  /			|
	//		Height						|
	//								   -1
	//
	//--------------------------------------------------------------------

	maPoint3d WindowToScreenPosition( const g2dWindow &i_Window, const maPoint3d& i_WindowPos );
	maPoint2d WindowToScreenPosition( const g2dWindow &i_Window, const maPoint2d& i_WindowPos );
	maPoint3d ScreenToWindowPosition( const g2dWindow &i_Window, const maPoint3d& i_ScreenPos );

	maMatrix4x4 WindowToScreenMatrix( const g2dWindow &i_Window, const maMatrix4x4& i_WindowMatrix );

	//--------------------------------------------------------------------
	//	Resolution Conversion Functions
	//
	//		Converts between the actual and the virtual resoltuion.
	//--------------------------------------------------------------------

	maPoint3d ActualToVirtualPosition( const g2dWindow &i_Window, const maPoint3d& i_ActualPos );
	maPoint2d ActualToVirtualPosition( const g2dWindow &i_Window, const maPoint2d& i_ActualPos );
	maPoint3d VirtualToActualPosition( const g2dWindow &i_Window, const maPoint3d& i_VirtualPos );
	maPoint2d VirtualToActualPosition( const g2dWindow &i_Window, const maPoint2d& i_VirtualPos );
};
