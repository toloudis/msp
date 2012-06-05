/*****************************************************************************
**  tma3dScreenUtil.cpp
**
**      tma3dScreenUtil contains screen utility function for
**		for the gui package.
**
**	StudioGPU
**	Copyright(C) 2002 - All Rights Reserved
\****************************************************************************/
#include "Tool/tma3d/tma3dScreenUtil.hpp"


//============================================================================
//============================================================================
namespace tma3dScreenUtil
{

//============================================================================
//============================================================================
namespace
{
	maPoint2d	l_WindowPos(0,0);
	maPoint2d	l_WindowSize(800,600);
}

	//------------------------------------------------------------------------
	// Set window size
	//------------------------------------------------------------------------
	void SetWindowSize( const maPoint2d& i_WindowPosition, const maPoint2d& i_WindowSize )
	{
		//DBG_LOG2("Window Pos: %f %f", i_WindowPosition.GetX(), i_WindowPosition.GetY());
		//DBG_LOG2("Window Size: %f %f", i_WindowSize.GetX(), i_WindowSize.GetY());

		l_WindowPos = i_WindowPosition;
		l_WindowSize = i_WindowSize;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	const maPoint2d& GetWindowSize()
	{
		return l_WindowSize;
	}
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	const maPoint2d& GetWindowPosition()
	{
		return l_WindowPos;
	}

	//------------------------------------------------------------------------
	//	Conversion Functions
	//
	//		Converts between the window coordinate system and
	//		the screen coordinate system.
	//
	//		Window					Screen
	//
	//									1
	//		0------Width				|
	//		|			  /  \			|
	//		|			 <====>	 -1	----0---- 1
	//		|			  \  /			|
	//		Height						|
	//								   -1
	//
	//------------------------------------------------------------------------
	maPoint2d WindowToScreenPosition( const maPoint2d& i_WindowPos )
	{
		//DBG_ASSERT(l_pWindow, "No window for screen util");
		int screen_width, screen_height;
		screen_width	= (int) l_WindowSize.GetX();
		screen_height	= (int) l_WindowSize.GetY();

		//DBG_LOG2( "window (%03dx%03d)", screen_width, screen_height );

		float xpos = (i_WindowPos.GetX() - l_WindowPos.GetX());
		float ypos = (i_WindowPos.GetY() - l_WindowPos.GetY());

		maPoint2d ScreenPos;
		ScreenPos.SetX((( 2.0f * xpos - 1.0f ) /  screen_width ) - 1.0f );
		ScreenPos.SetY( 1.0f - (( 2.0f * ypos - 1.0f ) / screen_height ));

		return ScreenPos;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	maPoint2d ScreenToWindowPosition( const maPoint2d& i_ScreenPos )
	{
		//DBG_ASSERT(l_pWindow, "No window for screen util");
		int screen_width, screen_height;
		screen_width	= (int)l_WindowSize.GetX();
		screen_height	= (int)l_WindowSize.GetY();

		maPoint2d WindowPos;
		WindowPos.SetX((( i_ScreenPos.GetX() + 1.0f ) * screen_width + 1.0f ) / 2.0f );
		WindowPos.SetY((( 1.0f - i_ScreenPos.GetY() ) * screen_height + 1.0f ) / 2.0f );

		WindowPos += l_WindowPos; // offset

		return WindowPos;
	}

};
