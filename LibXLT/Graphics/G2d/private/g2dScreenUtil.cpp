/*****************************************************************************
**  g2dScreenUtil.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Graphics/g2d/g2dScreenUtil.hpp"

#include "Graphics/g2d/g2dWindow.hpp"


//============================================================================
//============================================================================
namespace g2dScreenUtil
{
	//--------------------------------------------------------------------
	//	Conversion Functions
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
	maPoint3d WindowToScreenPosition( const g2dWindow &i_Window, const maPoint3d& i_WindowPos )
	{
		int nVirtWidth, nVirtHeight;
		i_Window.GetVirtualResolution( nVirtWidth, nVirtHeight );

		maPoint3d ScreenPos;

		ScreenPos.SetX((( 2.0f * i_WindowPos.GetX() - 1.0f ) /  (float)nVirtWidth ) - 1.0f );
		ScreenPos.SetY( 1.0f - (( 2.0f * i_WindowPos.GetY() - 1.0f ) / (float)nVirtHeight ));
		ScreenPos.SetZ( 1 - 1 / ( i_WindowPos.GetZ() + 1 ) );

		return ScreenPos;
	}

	maPoint2d WindowToScreenPosition( const g2dWindow &i_Window, const maPoint2d& i_WindowPos )
	{
		int nVirtWidth, nVirtHeight;
		i_Window.GetVirtualResolution( nVirtWidth, nVirtHeight );

		maPoint2d ScreenPos;

		ScreenPos.SetX((( 2.0f * i_WindowPos.GetX() - 1.0f ) /  (float)nVirtWidth ) - 1.0f );
		ScreenPos.SetY( 1.0f - (( 2.0f * i_WindowPos.GetY() - 1.0f ) / (float)nVirtHeight ));

		return ScreenPos;
	}

	maPoint3d ScreenToWindowPosition( const g2dWindow &i_Window, const maPoint3d& i_ScreenPos )
	{
		int nVirtWidth, nVirtHeight;
		i_Window.GetVirtualResolution( nVirtWidth, nVirtHeight );

		maPoint3d WindowPos;

		WindowPos.SetX((( i_ScreenPos.GetX() + 1.0f ) * nVirtWidth + 1.0f ) / 2.0f );
		WindowPos.SetY((( 1.0f - i_ScreenPos.GetY() ) * nVirtHeight + 1.0f ) / 2.0f );
		WindowPos.SetZ( 1 + 1 / ( 1 - i_ScreenPos.GetZ() ) );

		return WindowPos;
	}

	maMatrix4x4 WindowToScreenMatrix( const g2dWindow &i_Window, const maMatrix4x4& i_WindowMatrix )
	{
		maPoint3d WindowPos( i_WindowMatrix( 3, 0 ), i_WindowMatrix( 3, 1 ), i_WindowMatrix( 3, 2) );
		maPoint3d ScreenPos = WindowToScreenPosition( i_Window, WindowPos );

		maMatrix4x4 ScreenMatrix = i_WindowMatrix;
		ScreenMatrix( 3, 0 ) = ScreenPos.GetX();
		ScreenMatrix( 3, 1 ) = ScreenPos.GetY();
		ScreenMatrix( 3, 2 ) = ScreenPos.GetZ();

		return ScreenMatrix;
	}

	//--------------------------------------------------------------------
	//	Resolution Conversion Functions
	//
	//		Converts between the actual and the virtual resoltuion.
	//--------------------------------------------------------------------

	maPoint3d ActualToVirtualPosition( const g2dWindow &i_Window, const maPoint3d& i_ActualPos )
	{
		int nScreenWidth, nScreenHeight;
		i_Window.GetDimensions( nScreenWidth, nScreenHeight );
		int nVirtWidth, nVirtHeight;
		i_Window.GetVirtualResolution( nVirtWidth, nVirtHeight );

		maPoint3d VirtualPos;

		VirtualPos.SetX( ( nVirtWidth * i_ActualPos.GetX() ) / (float)nScreenWidth );
		VirtualPos.SetY( ( nVirtHeight * i_ActualPos.GetY() ) / (float)nScreenHeight );
		VirtualPos.SetZ( i_ActualPos.GetZ() );

		return VirtualPos;
	}

	maPoint2d ActualToVirtualPosition( const g2dWindow &i_Window, const maPoint2d& i_ActualPos )
	{
		int nScreenWidth, nScreenHeight;
		i_Window.GetDimensions( nScreenWidth, nScreenHeight );
		int nVirtWidth, nVirtHeight;
		i_Window.GetVirtualResolution( nVirtWidth, nVirtHeight );

		maPoint2d VirtualPos;

		VirtualPos.SetX( ( nVirtWidth * i_ActualPos.GetX() ) / (float)nScreenWidth );
		VirtualPos.SetY( ( nVirtHeight * i_ActualPos.GetY() ) / (float)nScreenHeight );

		return VirtualPos;
	}

	maPoint3d VirtualToActualPosition( const g2dWindow &i_Window, const maPoint3d& i_VirtualPos )
	{
		int nScreenWidth, nScreenHeight;
		i_Window.GetDimensions( nScreenWidth, nScreenHeight );
		int nVirtWidth, nVirtHeight;
		i_Window.GetVirtualResolution( nVirtWidth, nVirtHeight );

		maPoint3d ActualPos;

		ActualPos.SetX( ( nScreenWidth * i_VirtualPos.GetX() ) / (float)nVirtWidth );
		ActualPos.SetY( ( nScreenHeight * i_VirtualPos.GetY() ) / (float)nVirtHeight );
		ActualPos.SetZ( i_VirtualPos.GetZ() );

		return ActualPos;
	}

	maPoint2d VirtualToActualPosition( const g2dWindow &i_Window, const maPoint2d& i_VirtualPos )
	{
		int nScreenWidth, nScreenHeight;
		i_Window.GetDimensions( nScreenWidth, nScreenHeight );
		int nVirtWidth, nVirtHeight;
		i_Window.GetVirtualResolution( nVirtWidth, nVirtHeight );

		maPoint2d ActualPos;

		ActualPos.SetX( ( nScreenWidth * i_VirtualPos.GetX() ) / (float)nVirtWidth );
		ActualPos.SetY( ( nScreenHeight * i_VirtualPos.GetY() ) / (float)nVirtHeight );

		return ActualPos;
	}
};
