/*****************************************************************************
**  mnpInteraction.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "Features/ObjectManip/mnpInteraction.hpp"

#include "Core/Ma/maPoint2d.hpp"
#include "Graphics/Cam/camCamera.hpp"
#include "Graphics/G3d/g3dViewer.hpp"
#include "Graphics/G2d/g2dWindow.hpp"
#include "Tool/tma3d/tma3dCursorMgr.hpp"


namespace
{
	//------------------------------------------------------------------------
	// Convert from pixel coords to normalized screen coords
	//
	//		Converts between the window coordinate system and
	//		the screen coordinate system.
	//
	//									1
	//		0------Width				|
	//		|			    \			|
	//		|			 ---->	 -1	----0---- 1
	//		|			    /			|
	//		Height						|
	//								   -1
	//
	//------------------------------------------------------------------------
	maPoint2d window_to_screen_position( g2dWindow& i_Window, int i_X, int i_Y )
	{
		int screen_width = 0, screen_height = 0;
		i_Window.GetDimensions(screen_width, screen_height);

		maPoint2d ScreenPos(0,0);
		if (screen_width > 0)
			ScreenPos.SetX((( 2.0f * i_X - 1.0f ) /  (float)screen_width ) - 1.0f );
		if (screen_height > 0)
			ScreenPos.SetY( 1.0f - (( 2.0f * i_Y - 1.0f ) / (float)screen_height ));

		return ScreenPos;
	}
}

//------------------------------------------------------------------------
// An interaction is interacting as soon as it is created.
//------------------------------------------------------------------------
mnpInteraction::mnpInteraction()
:	m_bInteracting(true)
{
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
mnpInteraction::~mnpInteraction()
{
}

//------------------------------------------------------------------------
// Handle mouse motion. Mouse position is given in terms of a 
// position on the camera near plane and the direction of the
// ray that passes through that point into the camera view frustrum.
//------------------------------------------------------------------------
void mnpInteraction::MouseMotion(int i_X, int i_Y, g3dViewer *i_pViewer)
{
	if (m_bInteracting)
	{
		// Convert to ray and call other virtual function to allow 
		// derived classes to deal with the cursor ray only

		maPoint3d camera_ray_pos;
		maVector3d camera_ray_dir;

		if (!i_pViewer)
		{
			// Use tma3dCursorMgr to get camera ray. This should eventually
			// go away as we move away from modes and more to mouse events
			// in render windows.
			tma3dCursorMgr::GetCursorRay(camera_ray_pos, camera_ray_dir);
			this->RayMotion(camera_ray_pos, camera_ray_dir);
		}
		else
		{
			// Use camera and window of viewer to calculate cursor ray
			camCamera *pCamera = i_pViewer->GetCamera();
			g2dWindow *pWindow = i_pViewer->GetWindow();
			if (pCamera && pWindow)
			{
				maPoint2d cursor_pos = window_to_screen_position(*pWindow,i_X,i_Y);

				float maxDist = pCamera->GetFarClip();	//use the far clip plane as the max ray distance

				//	make world ray
				maPoint3d screen_pos = pCamera->GetScreenPoint( cursor_pos.GetX() , cursor_pos.GetY() );
				if (pCamera->IsOrthographic())
				{
					camera_ray_pos = screen_pos;
					camera_ray_dir = pCamera->GetDirection() * maxDist;
				}
				else
				{
					camera_ray_pos = pCamera->GetPosition();
					maVector3d dir = screen_pos - camera_ray_pos;
					dir.Normalize();
					camera_ray_dir = dir * maxDist;
				}
				this->RayMotion(camera_ray_pos, camera_ray_dir);
			}
		}
	}
}

//------------------------------------------------------------------------
// Abort the mouse interaction, return the objects back to their
//	original positions.
//------------------------------------------------------------------------
void mnpInteraction::AbortInteraction()
{
	m_bInteracting = false;
}

//------------------------------------------------------------------------
// Finish Interaction normally
//------------------------------------------------------------------------
void mnpInteraction::FinishInteraction()
{
	m_bInteracting = false;
}
