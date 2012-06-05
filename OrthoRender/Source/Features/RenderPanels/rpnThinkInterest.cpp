/****************************************************************************\
**	rpnThinkInterest.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Features/RenderPanels/rpnThinkInterest.hpp"

#include "Features/RenderPanels/rpnCommandChangeLayout.hpp"
#include "Features/RenderPanels/rpnPanelLayout.hpp"

#include "Core/dbg/dbgLog.hpp"
#include "Support/mnm/mnmObject.hpp"

#ifdef _MANAGED
using namespace StudioFramework;
#endif

namespace
{
	const float lc_fFocusRadius	= 10.0f;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
rpnThinkInterest::rpnThinkInterest()
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//virtual
rpnThinkInterest::~rpnThinkInterest()
{
}


//--------------------------------------------------------------------
//	Think
//--------------------------------------------------------------------
//virtual 
void rpnThinkInterest::Think( )
{
	inKeyboard* pKeyboard = inDeviceMgr::GetKeyboard();

	// Hotkeys
	if (tma3dCursorMgr::IsCursorOverView())
	{
		// SWITCH PANEL LAYOUT
		if (   pKeyboard->IsDown(inKeys::e_LCTRL) 
			|| pKeyboard->IsDown(inKeys::e_RCTRL) )
		{
			if ( pKeyboard->IsReleased(inKeys::e_1) )
			{		
				rpnCommandChangeLayout::ChangeLayout(rpnCommandChangeLayout::e_SinglePane);
			}
			else if ( pKeyboard->IsReleased(inKeys::e_2) )
			{		
				rpnCommandChangeLayout::ChangeLayout(rpnCommandChangeLayout::e_TwoStacked);
			}
			else if ( pKeyboard->IsReleased(inKeys::e_3) )
			{		
				rpnCommandChangeLayout::ChangeLayout(rpnCommandChangeLayout::e_TwoSideBySide);
			}
			else if ( pKeyboard->IsReleased(inKeys::e_4) )
			{		
				rpnCommandChangeLayout::ChangeLayout(rpnCommandChangeLayout::e_FourPanels);
			}
		}

		/*
		// CAMERA FOCUS
		if ( pKeyboard->IsReleased(inKeys::e_F) )
		{		
			// Get bounding box of selected objects
			maAxisBox bbox;
			const std::list<pick3dPickObject*> &selected_list = sel3dMgr::GetSelectedList();
			std::list<pick3dPickObject*>::const_iterator it, end = selected_list.end();
			for (it = selected_list.begin(); it != end; ++it)
			{
				mnmObject* pObject = dynamic_cast<mnmObject*>(*it);
				if (pObject)
				{
					bbox.Union( pObject->GetWorldBox() );
				}
			}

			// Enforce a minimum zoom
			maPoint3d center(0.0f,0.0f,0.0f);
			float radius = lc_fFocusRadius;
			if (!bbox.IsEmpty())
			{
				center = bbox.GetCenter();
				radius = bbox.GetRadius() * 1.5f;
				if (radius < lc_fFocusRadius)
					radius = lc_fFocusRadius;
			}

			// If SHIFT is pressed, focus on all panels
			if (   pKeyboard->IsDown(inKeys::e_LSHIFT) 
				|| pKeyboard->IsDown(inKeys::e_RSHIFT) )
			{
#ifdef _MANAGED
				// Do camera focus for all panels
				for (int i=0; i<4; ++i)
				{
					if (rpnPanelLayout::g_RenderPanes[i]->Visible)
					{
						rpnPanelLayout::g_RenderPanes[i]->FocusCamera( center, radius );
					}
				}
#endif
			}
			else
			{
				// Focus just the active camera
				cam3dMgr::FocusCamera( center, radius );
			}
		}
		*/
	}

#ifdef _MANAGED
	for (int i=0; i<4; ++i)
	{
		// Update the camera manipulator so that it tracks changes in the scripted camera
		rpnPanelLayout::g_RenderPanes[i]->ConfirmCameraManip();
	}
#endif
}
