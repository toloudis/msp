/*****************************************************************************
**	tma3dScreenUtil.hpp
**
**		tma3dScreenUtil contains screen utility function for
**		for the gui package.
**
**	StudioGPU
**	Copyright(C) 2002 - All Rights Reserved
\****************************************************************************/
#ifdef TMA3D_SCREENUTIL_HPP
#error tma3dScreenUtil.hpp multiply included
#endif
#define TMA3D_SCREENUTIL_HPP

#ifndef MA_POINT2D_HPP
#include "Core/ma/maPoint2d.hpp"
#endif


//============================================================================
//============================================================================
class g2dWindow;


//============================================================================
//============================================================================
namespace tma3dScreenUtil
{
	//------------------------------------------------------------------------
	// Set window size
	//------------------------------------------------------------------------
	void SetWindowSize( const maPoint2d& i_WindowPosition, const maPoint2d& i_WindowSize );

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	const maPoint2d& GetWindowSize();
	const maPoint2d& GetWindowPosition();

	//--------------------------------------------------------------------
	//	Window / Screen Conversion Functions
	//
	//		Converts between the window coordinate system and
	//		the screen coordinate system.
	//
	//		Window					Screen
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
	maPoint2d WindowToScreenPosition( const maPoint2d& i_WindowPos );
	maPoint2d ScreenToWindowPosition( const maPoint2d& i_ScreenPos );
};
