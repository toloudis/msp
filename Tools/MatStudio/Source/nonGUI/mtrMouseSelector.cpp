/*****************************************************************************
**  mtrMouseSelector.hpp
**
**		Picks with mouse input
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "mtrMouseSelector.hpp"

//#include "mtAppManager.hpp"
//#include "tma3dCursorMgr.hpp"
#include "mtrLevel.hpp"
//#include "mtObjectManipulate.hpp"
//#include "mtObjectOp.hpp"
//#include "mtVJoystick.hpp"
//#include "mtViewer.hpp"

#include "appTime.hpp"
#include "cam3dMgr.hpp"
#include "dbgLog.hpp"
#include "inDeviceMgr.hpp"
#include "inVirtualJoystick.hpp"
#include "maPoint2d.hpp"
#include "tma3dCursorMgr.hpp"
#include "tma3dScreenUtil.hpp"

//====================================================================
// Local variables and functions
//====================================================================
namespace
{
float	c_ClickTime = 1.0f; // minimum time in seconds between press and release
int		c_MaxMotion = 5;	// maximum pixels of motion between press and release

float	l_MouseDownTime = 0;
int		l_MouseDownX = 0;
int		l_MouseDownY = 0;

//====================================================================
// get_pick_ray - get pick ray from camera and screen position
//====================================================================
void get_pick_ray(const camCamera &i_Camera,
				   int i_MouseX, int i_MouseY, 
				   int i_ScreenWidth, int i_ScreenHeight,
				   maPoint3d &o_CameraPos, maPoint3d &o_RayEnd)
{
	//bool ctrl =		inDeviceMgr::GetKeyboard(0)->IsHeld(inKeys::e_LCTRL) ||
	//				inDeviceMgr::GetKeyboard(0)->IsHeld(inKeys::e_RCTRL);

	// Convert x,y to normalized screen coordinates
	float halfwidth = 0.5f * i_ScreenWidth;
	float halfheight = 0.5f * i_ScreenHeight;
	float u = (i_MouseX - halfwidth) / halfwidth;
	float v = (halfheight - i_MouseY) / halfheight;

	// Find eye point and point on near plane
	o_CameraPos = i_Camera.GetPosition();
	maPoint3d nearPt = i_Camera.GetScreenPoint(u, v);
	o_RayEnd = ((nearPt - o_CameraPos) * 1000.0f) + o_CameraPos;
}


//====================================================================
// handle_mouse_click - select object based on mouse click
//====================================================================
void handle_mouse_click(const camCamera &i_Camera,
					   int i_MouseX, int i_MouseY, 
					   int i_ScreenWidth, int i_ScreenHeight)
{
	maPoint3d camera_pos, ray_end;
	get_pick_ray(i_Camera, i_MouseX, i_MouseY, 
				 i_ScreenWidth, i_ScreenHeight, 
				 camera_pos, ray_end);
	mtrLevel::DoMaterialPick(camera_pos, ray_end);
}

}	// end of namespace

//====================================================================
//	GetPickDir() - returns position and direction for
//		picking ray.  Ray length is near plane of camera,
//		so it needs to be resized to pick in world enviroment.
//====================================================================
void mtrMouseSelector::GetPickDir(maPoint3d &o_Pos, maVector3d &o_Dir)
{
	int dX, dY;
	tma3dCursorMgr::GetCursorPos(dX, dY);
	maPoint2d size = tma3dScreenUtil::GetWindowSize();

	// Convert x,y to normalized screen coordinates
	float halfwidth = 0.5f * size.GetX();
	//float halfheight = 0.5f * width;
	float halfheight = 0.5f * size.GetY();
	float u = (dX - halfwidth) / halfwidth;
	float v = (halfheight - dY) / halfheight;

//	DBG_LOG4("Mouse(%d, %d) Normalized(%f, %f)", dX, dY, u, v);

	// Find eye point and point on near plane
	camCamera camera = cam3dMgr::GetCamera();
	o_Pos = camera.GetPosition();
	maPoint3d nearPt = camera.GetScreenPoint(u, v);
	o_Dir = nearPt - o_Pos;
}
	
//====================================================================
//	GetPickRay() - returns start and end of pick ray
//====================================================================
void mtrMouseSelector::GetPickRay(maPoint3d &o_RayStart, maPoint3d &o_RayEnd)
{
	maVector3d dir;
	mtrMouseSelector::GetPickDir(o_RayStart, dir);
	o_RayEnd = o_RayStart + dir * 3000.0f;
}

//====================================================================
//	Think()
//====================================================================
void
mtrMouseSelector::Think()
{
	
	if (/*mtAppManager::HasViewFocus() &&*/ tma3dCursorMgr::IsCursorOverView())
	{
		//inVirtualJoystick *pVJoy = inDeviceMgr::GetVirtualJoystick();

		int dX, dY;
		tma3dCursorMgr::GetCursorPos(dX, dY);
/*		maPoint3d ray_start, ray_end, pick_pos;
		mtrMouseSelector::GetPickRay(ray_start, ray_end);

		if (mtObjectManipulate::IsActiveDrag())
		{
			if ( pVJoy->IsReleased(mtVJoystick::e_LeftClick) )
				mtObjectManipulate::EndDrag();
			else
				mtObjectManipulate::RayUpdate(ray_start, ray_end);
		}
		else*/
		{
			inKeyboard* pKeyBoard = inDeviceMgr::GetKeyboard();
			inMouse* pMouse = inDeviceMgr::GetMouse();
			
			if ( !pKeyBoard->IsHeld(inKeys::e_LALT) && 
				 !pKeyBoard->IsHeld(inKeys::e_RALT))
			{
				if ( pMouse->IsPressed( 0 ) )
				{
					//float tval = 99.9f;
					//if (mtObjectManipulate::PickManipulate( ray_start, ray_end, tval ))
					//{
					//	mtObjectManipulate::BeginDrag();
					//}

					// On mouse down, record mouse position
					// and time
					//
					l_MouseDownTime = appTime::GetTime();
					l_MouseDownX = dX;
					l_MouseDownY = dY;
				}
				else if ( pMouse->IsReleased( 0 ))
				{	
					// On mouse up, check to see if mouse
					// is in roughly same position, and 
					// if little time has passed
					//
					float time = appTime::GetTime();
					if (time - l_MouseDownTime < c_ClickTime)
					{
						if ((abs(dX-l_MouseDownX) < c_MaxMotion) && 
							(abs(dY-l_MouseDownY) < c_MaxMotion) )
						{
							//mtObjectOp::DoPick(ray_start, ray_end);

							// If not light chosen, pick material
							//if (!mtObjectManipulate::GetObject())
							{
								maPoint3d ray_start, ray_end, pick_pos;
								mtrMouseSelector::GetPickRay(ray_start, ray_end);

								mtrLevel::DoMaterialPick(ray_start, ray_end);
							}
						}
					}
				}
				//else
				//{
				//	// Update manipulator
				//	mtObjectManipulate::RayUpdate(ray_start, ray_end);
				//}
			}

			// TAB selected objects
			//if ( pKeyBoard->IsReleased(inKeys::e_TAB) )
			//{
			//	mtObjectOp::SelectNextPick();
			//}

			// Delete Selected Object
			//if ( pKeyBoard->IsReleased(inKeys::e_DELETE) )
			//{
			//	if (mtObjectManipulate::GetObject())
			//	{
			//		mtObjectOp::DeleteObject(mtObjectManipulate::GetObject());
			//	}
			//}
		}
	}	
	
}
