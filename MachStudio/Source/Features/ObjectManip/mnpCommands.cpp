/*****************************************************************************
**	mnpCommands.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2007- All Rights Reserved
\****************************************************************************/
#include "Features/ObjectManip/mnpCommands.hpp"

#include "Features/ObjectManip/mnpModeObjectManip.hpp"
#include "Features/ObjectManip/mnpConstants.hpp"

#include "Features/FilmGates/fgtFrameMgr.hpp"
#include "Features/Prefs/PrefsMgr.hpp"
#include "Support/brsh/brshPaintBrushMgr.hpp"
#include "Support/brsh/brshColorDialogUtil.hpp"
#include "Support/fgmt/fgmtHighlight.hpp"
#include "Support/mnm/mnmObject.hpp"
#include "Support/mtrl/mtrlHighlight.hpp"
#include "Support/vis/visMgr.hpp"
#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"

#include "Input/in/inDeviceMgr.hpp"
#include "Support/mode/modeModeMgr.hpp"
#include "Tool/cma/cmaCommandSimple.hpp"
#include "Tool/cma/cmaCommandToggle.hpp"
#include "Tool/gui/guiCommandMgr.hpp"
#include "Tool/gui/guiMenuMgr.hpp"
#include "Tool/gui/guiToolbarMgr.hpp"
#include "Tool/sel3d/sel3dMgr.hpp"
#include "Tool/sel3d/sel3dObject.hpp"

#include <assert.h>


//============================================================================
//============================================================================
namespace
{
	int l_MenuIdPickModeObjects = -1;
	int l_MenuIdPickModeMaterials = -1;
	int l_MenuIdPickModeSurfaces = -1;
	int l_MenuIdPickModeHighlight = -1;
	bool l_bHighlightPickmode = false;

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	int add_menu_item_and_toolbarbutton(const char* i_ParentName,
										const char* i_ChildName,
										const char * i_ToolbarName,
										const char * i_ToolbarButton_ImageFilename )
	{
		int menuid = guiMenuMgr::AddMenuItem( i_ParentName, i_ChildName, 
			i_ToolbarName, i_ToolbarButton_ImageFilename );
		return menuid;
	}
	//--------------------------------------------------------------------
	// get pointer to object manip mode, or NULL if not current mode
	//--------------------------------------------------------------------
	mnpModeObjectManip* get_object_manip_mode()
	{
		modeMode* pMode = modeModeMgr::GetCurrentMode();
		mnpModeObjectManip* pModeOM = dynamic_cast<mnpModeObjectManip*>(pMode);
		return pModeOM;
	}

	//
	//	Command Functions
	//

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Execute_DecrementCompassScale()
	{
		mnpModeObjectManip* pModeOM = get_object_manip_mode();
		if ( pModeOM != 0 )
		{
			pModeOM->DecrementScaleOfCompass();
		}
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Execute_IncrementCompassScale()
	{
		mnpModeObjectManip* pModeOM = get_object_manip_mode();
		if ( pModeOM != 0 )
		{
			pModeOM->IncrementScaleOfCompass();
		}
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Execute_FlipCompass()
	{
		mnpModeObjectManip* pModeOM = get_object_manip_mode();
		if ( pModeOM != 0 )
		{
			pModeOM->ToggleCompassFlipState();
		}
	}
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	//void Execute_DecrementTimeline()
	//{
	//	mnpModeObjectManip* pModeOM = get_object_manip_mode();
	//	if ( pModeOM != 0 )
	//	{
	//		pModeOM->TimelineDecrementFrame();
	//	}
	//}

	////--------------------------------------------------------------------
	////--------------------------------------------------------------------
	//void Execute_IncrementTimeline()
	//{
	//	mnpModeObjectManip* pModeOM = get_object_manip_mode();
	//	if ( pModeOM != 0 )
	//	{
	//		pModeOM->TimelineIncrementFrame();
	//	}
	//}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Execute_ManipPlacement()
	{
		mnpModeObjectManip* pModeOM = get_object_manip_mode();
		if ( pModeOM != 0 )
		{
			pModeOM->SetStatePlacement();
		}
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Execute_ManipRotate()
	{
		mnpModeObjectManip* pModeOM = get_object_manip_mode();
		if ( pModeOM != 0 )
		{
			pModeOM->SetStateRotate();
		}
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Execute_ManipScale()
	{
		mnpModeObjectManip* pModeOM = get_object_manip_mode();
		if ( pModeOM != 0 )
		{
			pModeOM->SetStateScale();
		}
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Execute_ManipSelect()
	{
		mnpModeObjectManip* pModeOM = get_object_manip_mode();
		if ( pModeOM != 0 )
		{
			pModeOM->SetStateSelect();
		}
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Execute_ManipTranslate()
	{
		mnpModeObjectManip* pModeOM = get_object_manip_mode();
		if ( pModeOM != 0 )
		{
			pModeOM->SetStateTranslate();
		}
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Execute_ManipLockSelect()
	{
		mnpModeObjectManip* pModeOM = get_object_manip_mode();
		if ( pModeOM != 0 )
		{
			pModeOM->SetStateLockSelect();
		}
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Execute_CenterPivot()
	{
		// Maybe we need a mnpOperations namespace for this?
		const std::list<sel3dObject*> &selected_list = sel3dMgr::GetSelectedList();
		std::list<sel3dObject*>::const_iterator it, end = selected_list.end();
		for (it = selected_list.begin(); it != end; ++it)
		{
			if (mnmObject* pObject = dynamic_cast<mnmObject*>(*it))
			{
				pObject->CenterPivot();
			}
		}
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Execute_LocalSpace(bool i_bState)
	{
		mnpModeObjectManip::SetLocalSpaceTranslation(i_bState);
		PrefsMgr::GetDataSimple().m_bLocalSpaceTranslation = i_bState;
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	bool IsLocalSpace()
	{
		return mnpModeObjectManip::IsLocalSpaceTranslation();
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	//bga - Not valid command anymore, no pick list with GPU picking
	//void Execute_SelectNextPickedObject()
	//{
	//	mnpModeObjectManip* pModeOM = get_object_manip_mode();
	//	if ( pModeOM != 0 )
	//	{
	//		pModeOM->SelectNextObjectInPickList();
	//	}
	//}

	//--------------------------------------------------------------------
	// PickMode States
	//--------------------------------------------------------------------
	bool Get_PickObjects() 
	{
		if (mnpModeObjectManip* pModeOM = get_object_manip_mode())
			return (pModeOM->GetPickMode() == mnpModeObjectManip::e_PickObjects);
		return false;
	}
	bool Get_PickMaterials() 
	{
		if (mnpModeObjectManip* pModeOM = get_object_manip_mode())
			return (pModeOM->GetPickMode() == mnpModeObjectManip::e_PickMaterials);
		return false;
	}
	bool Get_PickSurfaces() 
	{
		if (mnpModeObjectManip* pModeOM = get_object_manip_mode())
			return (pModeOM->GetPickMode() == mnpModeObjectManip::e_PickSurfaces);
		return false;
	}
	bool Get_Highlight_Pickmode()
	{
		return l_bHighlightPickmode;
	}

	//--------------------------------------------------------------------
	// refresh toggle state of toolbar buttons
	//--------------------------------------------------------------------
	void refresh_pickmode_toolbar()
	{
		guiMenuMgr::MenuObjectsCheck(l_MenuIdPickModeObjects, Get_PickObjects());
		guiMenuMgr::MenuObjectsCheck(l_MenuIdPickModeMaterials, Get_PickMaterials());
		guiMenuMgr::MenuObjectsCheck(l_MenuIdPickModeSurfaces, Get_PickSurfaces());
		guiMenuMgr::MenuObjectsCheck(l_MenuIdPickModeHighlight, Get_Highlight_Pickmode());

		if (guiToolbarMgr::IsVisible(mnpConstants::mc_Toolbar_PickMode_Name))
			guiToolbarMgr::Refresh(mnpConstants::mc_Toolbar_PickMode_Name);
	}
	
	//--------------------------------------------------------------------
	// PickMode
	//--------------------------------------------------------------------
	void Set_PickObjects(bool i_bValue) 
	{
		if (mnpModeObjectManip* pModeOM = get_object_manip_mode())
		{
			pModeOM->SetPickMode(mnpModeObjectManip::e_PickObjects);
			//mtrlHighlight::HighlightMaterial(false);
			//fgmtHighlight::HighlightFragment(false);

			refresh_pickmode_toolbar();
		}
	}
	void Set_PickMaterials(bool i_bValue)
	{
		if (mnpModeObjectManip* pModeOM = get_object_manip_mode())
		{
			pModeOM->SetPickMode(mnpModeObjectManip::e_PickMaterials);
			//mtrlHighlight::HighlightMaterial( l_bHighlightPickmode );
			//fgmtHighlight::HighlightFragment(false);

			refresh_pickmode_toolbar();
		}
	}
	//void Set_HighlightMaterials(bool i_bValue)
	//{
	//	mtrlHighlight::HighlightMaterial(i_bValue);
	//	if (i_bValue)
	//		fgmtHighlight::HighlightFragment(false);

	//	refresh_pickmode_toolbar();
	//}

	void Set_PickSurfaces(bool i_bValue)
	{
		if (mnpModeObjectManip* pModeOM = get_object_manip_mode())
		{
			pModeOM->SetPickMode(mnpModeObjectManip::e_PickSurfaces);
			//fgmtHighlight::HighlightFragment( l_bHighlightPickmode );
			//mtrlHighlight::HighlightMaterial(false);

			refresh_pickmode_toolbar();
		}
	}
	//void Set_HighlightSurfaces(bool i_bValue)
	//{
	//	fgmtHighlight::HighlightFragment(i_bValue);
	//	if (i_bValue)
	//		mtrlHighlight::HighlightMaterial(false);

	//	refresh_pickmode_toolbar();
	//}
	//void Set_Highlight()
	//{
	//	if(	fgmtOperations::GetSelectedFragmentIndex() != -1 )
	//		Set_HighlightSurfaces( !fgmtOperations::IsHighlightFragment() );
	//	else if( mtrlOperations::GetSelectedMaterialIndex() != -1 )
	//		Set_HighlightMaterials( !mtrlOperations::IsHighlightMaterial() );
	//}

	// Single highlight button controls both material and fragment
	// highlighting based on current pick mode
	void Set_Highlight_Pickmode(bool i_bValue)
	{
		l_bHighlightPickmode = i_bValue;

		//mtrlHighlight::HighlightMaterial( Get_PickMaterials() && l_bHighlightPickmode );
		//fgmtHighlight::HighlightFragment( Get_PickSurfaces() && l_bHighlightPickmode );

		// Highlight can be "on" for both at the same time now, uses contents of selected list
		// to determine type of highlight
		mtrlHighlight::HighlightMaterial( l_bHighlightPickmode );
		fgmtHighlight::HighlightFragment( l_bHighlightPickmode );

		refresh_pickmode_toolbar();
	}
}	


//--------------------------------------------------------------------
// SetupMenu
//--------------------------------------------------------------------
void mnpCommands::SetupMenu()
{
	//
	//	commands
	//
	cmaCommand* pCmd;
	guiMenuMgr::AddMenu( "View", "Icons" );

	//	COMMAND: Decrement scale of compass
	pCmd = new cmaCommandSimple("Decrement Scale of Compass", 
								"Icons", 
								"Decrement the scale of the compass",
								&Execute_DecrementCompassScale );
	int menu_id = guiMenuMgr::AddMenuItem( "Icons", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Icons", "Decrement Scale of Compass", pCmd );

	//	COMMAND: Increment scale of compass
	pCmd = new cmaCommandSimple("Increment Scale of Compass", 
								"Icons", 
								"Increment the scale of the compass",
								&Execute_IncrementCompassScale );
	menu_id = guiMenuMgr::AddMenuItem( "Icons", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Icons", "Increment Scale of Compass", pCmd );

	//	COMMAND: Flip orientation of compass
	pCmd = new cmaCommandSimple("Flip Compass", 
								"Icons", 
								"Flip the compass",
								&Execute_FlipCompass );
	menu_id = guiMenuMgr::AddMenuItem( "Icons", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Icons", "Flip Compass", pCmd );

	//	COMMAND: Decrement Timeline
	//pCmd = new cmaCommandSimple("Decrement Timeline", 
	//							"Actions", 
	//							"Decrement the Timeline",
	//							&Execute_DecrementTimeline );
	//menu_id = guiMenuMgr::AddMenuItem( "Actions", pCmd->GetTag().c_str() );
	//guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	//cmmSystemDialogUtil::AddSystemCommand( "Actions", "Decrement time", pCmd );

	////	COMMAND: Increment Timeline
	//pCmd = new cmaCommandSimple("Increment Timeline", 
	//							"Actions", 
	//							"Increment the Timeline",
	//							&Execute_IncrementTimeline );
	//menu_id = guiMenuMgr::AddMenuItem( "Actions", pCmd->GetTag().c_str() );
	//guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	//cmmSystemDialogUtil::AddSystemCommand( "Actions", "Increment time", pCmd );
	
	//	COMMAND: Select Manipulation
	pCmd = new cmaCommandSimple("Select", 
								"Objects", 
								"Select manipulation for objects",
								&Execute_ManipSelect );
	menu_id = add_menu_item_and_toolbarbutton( "Objects", pCmd->GetTag().c_str(), mnpConstants::mc_Toolbar_Actions_Name, "edit-select.png" );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Objects", "Select", pCmd );

	//	COMMAND: Translate Manipulation
	pCmd = new cmaCommandSimple("Translate", 
								"Objects", 
								"Translate manipulation for objects",
								&Execute_ManipTranslate );
	menu_id = add_menu_item_and_toolbarbutton( "Objects", pCmd->GetTag().c_str(), mnpConstants::mc_Toolbar_Actions_Name, "edit-translate.png" );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Objects", "Translate", pCmd );

	//	COMMAND: Rotate Manipulation
	pCmd = new cmaCommandSimple("Rotate", 
								"Objects", 
								"Rotate manipulation for objects",
								&Execute_ManipRotate );
	menu_id = add_menu_item_and_toolbarbutton( "Objects", pCmd->GetTag().c_str(), mnpConstants::mc_Toolbar_Actions_Name, "edit-rotate.png" );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Objects", "Rotate", pCmd );

	//	COMMAND: Scale Manipulation
	pCmd = new cmaCommandSimple("Scale", 
								"Objects", 
								"Scale manipulation for objects",
								&Execute_ManipScale );
	menu_id = add_menu_item_and_toolbarbutton( "Objects", pCmd->GetTag().c_str(), mnpConstants::mc_Toolbar_Actions_Name, "edit-scale.png" );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Objects", "Scale", pCmd );

	//	COMMAND: Placement Manipulation
	pCmd = new cmaCommandSimple("Placement", 
								"Objects", 
								"Placement manipulation for objects",
								&Execute_ManipPlacement );
	menu_id = add_menu_item_and_toolbarbutton( "Objects", pCmd->GetTag().c_str(), mnpConstants::mc_Toolbar_Actions_Name, "edit-placement.png" );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Objects", "Placement", pCmd );

	//	COMMAND: Select Next Picked Object
	//pCmd = new cmaCommandSimple("Select Next Picked Object", 
	//							"Objects", 
	//							"Select the next picked object from the picked list",
	//								
	//							&Execute_SelectNextPickedObject );
	//menu_id = guiMenuMgr::AddMenuItem( "Objects", pCmd->GetTag().c_str() );
	//guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	//cmmSystemDialogUtil::AddSystemCommand( "Objects", "Select Next Picked Object", pCmd );

	//	COMMAND: Clear Selection
	pCmd = new cmaCommandSimple("Clear Selection", 
								"Objects", 
								"Clear the current selection list",
								&sel3dMgr::ClearSelection );
	menu_id = guiMenuMgr::AddMenuItem( "Objects", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );

	//	COMMAND: Lock Selection
	pCmd = new cmaCommandSimple("Toggle Selection Lock", 
								"Objects", 
								"Lock or Unlock the current selection list",
								&Execute_ManipLockSelect );
	menu_id = guiMenuMgr::AddMenuItem( "Objects", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );

	//	COMMAND: Center Pivot
	pCmd = new cmaCommandSimple("Center Pivot", 
								"Objects", 
								"Move pivot to center of bounding box",
								&Execute_CenterPivot );
	menu_id = guiMenuMgr::AddMenuItem( "Objects", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );

	//	COMMAND: Translate in world or local space
	pCmd = new cmaCommandToggle("Local Space Translation", 
								"Objects", 
								"Switch between local and world space translation",	
								&Execute_LocalSpace,	
								&IsLocalSpace );
	menu_id = guiMenuMgr::AddCheckableMenuItem( "Objects", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );

	//	COMMAND: view Icons
	guiMenuMgr::AddSeparator("Icons");
	pCmd = new cmaCommandToggle("Icons Visible", 
								"Icons", 
								"Toggle Icons Visibility",
								&visMgr::ShowIcons, 
								&visMgr::IsShowIcons);
	menu_id = guiMenuMgr::AddCheckableMenuItem( "Icons", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Icons", "Toggle Icons Visibility", pCmd );

	// Pick Mode Group - feature of only the wxWidgets version
	guiMenuMgr::AddMenu( "Edit", "Pick Mode" );

	//	COMMAND: Pick Object
	pCmd = new cmaCommandToggle("Pick Object", 
								"Pick Mode", 
								"Pick objects with mouse clicks",
								&Set_PickObjects,
								&Get_PickObjects );
	l_MenuIdPickModeObjects = guiMenuMgr::AddCheckableMenuItem( "Pick Mode", pCmd->GetTag().c_str(), 
		mnpConstants::mc_Toolbar_PickMode_Name, "pickmode-object.png" );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), l_MenuIdPickModeObjects );

	//	COMMAND: Pick Material
	pCmd = new cmaCommandToggle("Pick Material", 
								"Pick Mode", 
								"Pick material with mouse clicks",
								&Set_PickMaterials,
								&Get_PickMaterials );
	l_MenuIdPickModeMaterials = guiMenuMgr::AddCheckableMenuItem( "Pick Mode", pCmd->GetTag().c_str(), 
		mnpConstants::mc_Toolbar_PickMode_Name, "pickmode-material.png" );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), l_MenuIdPickModeMaterials );

	//	COMMAND: Toggle Highlight of Materials
	//pCmd = new cmaCommandToggle("Highlight Material", 
	//	"Materials",
	//	"Toggle highlighting of selected material",
	//	Set_HighlightMaterials,
	//	mtrlOperations::IsHighlightMaterial);
	//menu_id = guiMenuMgr::AddCheckableMenuItem( "Materials", pCmd->GetTag().c_str(), 
	//	mnpConstants::mc_Toolbar_PickMode_Name, "material-highlight.png" );
	//guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );

	//	COMMAND: Pick Surface
	pCmd = new cmaCommandToggle("Pick Surface", 
								"Pick Mode", 
								"Pick surface with mouse clicks",
								&Set_PickSurfaces,
								&Get_PickSurfaces );
	l_MenuIdPickModeSurfaces = guiMenuMgr::AddCheckableMenuItem( "Pick Mode", pCmd->GetTag().c_str(), 
		mnpConstants::mc_Toolbar_PickMode_Name, "pickmode-surface.png" );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), l_MenuIdPickModeSurfaces );
	
	//	COMMAND: Toggle Highlight of Surfaces
	//pCmd = new cmaCommandToggle("Highlight Surface", 
	//	"Surfaces",
	//	"Toggle highlighting of selected surface",
	//	Set_HighlightSurfaces,
	//	fgmtOperations::IsHighlightFragment);
	//menu_id = guiMenuMgr::AddCheckableMenuItem( "Surfaces", pCmd->GetTag().c_str(), 
	//	mnpConstants::mc_Toolbar_PickMode_Name, "surface-highlight.png" ); 
	//guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );

	//	COMMAND: Toggle Highlight of Surfaces or Materials based on pickmode
	pCmd = new cmaCommandToggle("Highlight Selected", 
		"Pick Mode",
		"Toggle highlighting of selected surface or material",
		Set_Highlight_Pickmode,
		Get_Highlight_Pickmode);
	l_MenuIdPickModeHighlight = guiMenuMgr::AddCheckableMenuItem( "Pick Mode", pCmd->GetTag().c_str(), 
		mnpConstants::mc_Toolbar_PickMode_Name, "pickmode-highlight.png" ); 
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), l_MenuIdPickModeHighlight );
}
