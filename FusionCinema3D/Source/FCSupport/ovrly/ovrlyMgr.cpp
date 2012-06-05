/*****************************************************************************
**	ovrlyMgr.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "FCSupport/ovrly/ovrlyMgr.hpp"

#include "FCSupport/ovrly/ovrlyObjectRotate.hpp"
#include "FCSupport/ovrly/ovrlyObjectZoom.hpp"
#include "FCSupport/ovrly/ovrlyObjectColor.hpp"

#include "Support/mnm/mnmVJoystick.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Input/in/inDeviceMgr.hpp"
#include "Input/in/inVirtualJoystick.hpp"
#include "Tool/tma3d/tma3dCursorMgr.hpp"
#include "Tool/tma3d/tma3dScreenUtil.hpp"

#include <stdio.h>


///-----------------------------------------------------------------------
/// constructors
///-----------------------------------------------------------------------
ovrlyMgr::ovrlyMgr()
:	m_SelectedOverlay(-1)
{
}

///-----------------------------------------------------------------------
/// destructors
///-----------------------------------------------------------------------
ovrlyMgr::~ovrlyMgr()
{
	envSTLHelpers::DeleteContainer( this->m_Overlays );
}

///---------------------------------------------------------------------------
///	the current instance of the mode mgr singleton
///---------------------------------------------------------------------------
ovrlyMgr* ovrlyMgr::Instance = NULL;

//--------------------------------------------------------------------
//	Deinitialize/Initialize
//--------------------------------------------------------------------
void ovrlyMgr::Initialize()
{
}
void ovrlyMgr::DeInitialize()
{
}

//---------------------------------------------------------------------------
///	AddOverlay
//---------------------------------------------------------------------------
void ovrlyMgr::AddOverlay( ovrlyObject* i_pObject )
{
	//	TODO check for duplicates
	//
	m_Overlays.push_back( i_pObject );
}

///---------------------------------------------------------------------------
/// Make overlays all visible or not
///---------------------------------------------------------------------------
void ovrlyMgr::Show(bool i_bVisible)
{
	for (int i=0; i < m_Overlays.size(); ++i)
	{
		m_Overlays[i]->Show( i_bVisible );
	}
}

//----------------------------------------------------------------------------
//	returns true if left button pressed over view window
//----------------------------------------------------------------------------
bool is_left_click(inVirtualJoystick *i_pVJoy)
{
	return ( i_pVJoy->IsPressed(mnmVJoystick::e_LeftClick) &&
			 tma3dCursorMgr::IsCursorOverView() );
}

///---------------------------------------------------------------------------
/// Check for picks
///
///	return true if an overlay has been picked
///---------------------------------------------------------------------------
void ovrlyMgr::CheckPicks()
{
	inVirtualJoystick *pVJoy = inDeviceMgr::GetVirtualJoystick();
	inMouse* pMouse = inDeviceMgr::GetMouse();

	// check to see if there is something we are selecting
	if ( is_left_click(pVJoy) )
	{
		int x,y;
		tma3dCursorMgr::GetCursorPos(x,y);
		maPoint2d cursor_point((float)x,(float)y);
		DBG_TRACE("---Click pick at (" << x << "," << y << ")  " << cursor_point);

		//	Look through all the overlays and check for picks
		//
		bool picked = false;
		for (int i=0; i < m_Overlays.size(); ++i)
		{
			picked |= m_Overlays[i]->CheckForPick( cursor_point );
			if (picked)
			{
				m_SelectedOverlay = i;
				m_Overlays[i]->State_Picked();
				break;
			}
		}
	}
	else
	{
		if ((pMouse != NULL) && pMouse->IsHeld(0))
		{
			int dX, dY, dZ;
			pMouse->GetAnalogDir(0, dX, dY, dZ);

			if ((m_SelectedOverlay != -1) && (m_Overlays[m_SelectedOverlay]->IsPickable()))
				m_Overlays[m_SelectedOverlay]->DoMovement(dX, dY, dZ);

			//DBG_TRACE(" Mouse Move " << dX << " " << dY << " " << dZ);
			//if(dZ != 0)
			//{
			//	o_CamManip.m_bScrollWheel = true;
			//	o_CamManip.m_CamManipMode = e_ZOOM;
			//}
		}
		else
		{
#if 1
			int x,y;
			tma3dCursorMgr::GetCursorPos(x,y);
			maPoint2d curpos(x,y);
			maPoint2d winpos = tma3dScreenUtil::GetWindowPosition();
			curpos += winpos;
			DBG_TRACE("Screen pick at (" << x << "," << y << ")");

			//	Look through all the overlays and check for picks
			//
			bool picked = false;
			for (int i=0; i < m_Overlays.size(); ++i)
			{
				picked |= m_Overlays[i]->CheckForPick( curpos );
				if (picked)
				{
					DBG_TRACE("    Highlight " << i);
					m_Overlays[i]->State_Highlight();
					picked = false;
				}
				else
				{
					if (m_Overlays[i]->IsPickable())
						m_Overlays[i]->State_Normal();
				}
			}
#endif
		}
	}
}

///---------------------------------------------------------------------------
/// 
/// 
///---------------------------------------------------------------------------
void ovrlyMgr::Something()
{
	ovrlyObject* pOO;
	//pOO = new ovrlyObject(itString("overlay_ring.png"), itString("overlay_ring.png"), itString("overlay_ring.png"));
	//pOO->SetPosition(maPoint2d(0.0f,0.0f));
	//AddOverlay(pOO);
	pOO = new ovrlyObjectRotate(itString("overlay_rotate.png"), itString("overlay_rotate_highlight.png"), itString("overlay_rotate_disabled.png"));
	pOO->SetPosition(maPoint2d(0.25f,0.50f));
	AddOverlay(pOO);
	pOO = new ovrlyObjectZoom(itString("overlay_zoom.png"), itString("overlay_zoom_highlight.png"), itString("overlay_zoom_disabled.png"));
	pOO->SetPosition(maPoint2d(0.25f,-0.50f));
	AddOverlay(pOO);
	//pOO = new ovrlyObject(itString("dotpink.png"), itString("dotpink.png"), itString("dotpink.png"));
	//pOO->SetPosition(maPoint2d(0.25f,-0.50f));
	//AddOverlay(pOO);

	//for (int i=0; i < 16; ++i)
	//{
	//	maFloatRGBA color(1.0f-(i*0.1f), 0.1f+(i*0.1f), 0.6f-(i*0.04f), 1.0f);

	//	pOO = new ovrlyObjectColor(itString("Overlay_ColorSlice.png"), itString("Overlay_ColorSlice.png"), itString("Overlay_ColorSlice.png"), color);
	//	pOO->SetColor(color);
	//	pOO->SetPosition(maPoint2d(-0.019f+(-0.03f*i),0.58f+(-0.019f*i)));

	//	//	pOO->get
	//	//effTexturedData* pData = dynamic_cast<effTexturedData*>(pTexMat->GetEffectData());
	//	//DBG_ASSERT(pData != NULL, "cptrModeRender not using effTextureData");
	//	//const float fALPHA = 0.5f;
	//	//const maFloatRGBA objectcolor( 1.0f, 1.0f, 1.0f, fALPHA );
	//	//pData->m_Color = objectcolor;

	//	AddOverlay(pOO);
	//}

	Show(true);
}

