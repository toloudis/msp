/*****************************************************************************
**  mnpCommands.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2007- All Rights Reserved
\****************************************************************************/
#include "Features/ObjectManip/mnpCommands.hpp"

#include "Features/ObjectManip/orthoModeObjectManip.hpp"
#include "Features/ObjectManip/mnpConstants.hpp"

#include "Features/FilmGates/fgtFrameMgr.hpp"
#include "Support/fgmt/GUI/fgmtOperations.hpp"
#include "Support/mtrl/GUI/mtrlOperations.hpp"
#include "Support/vis/visMgr.hpp"
#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"

#include "Core/dbg/dbgLog.hpp"
#include "Support/mode/modeModeMgr.hpp"
#include "Tool/cma/cmaCommandSimple.hpp"
#include "Tool/cma/cmaCommandToggle.hpp"
#include "Tool/gui/guiCommandMgr.hpp"
#include "Tool/gui/guiMenuMgr.hpp"
#include "Tool/sel3d/sel3dMgr.hpp"
#include "ToolUIManaged/tma/tmaCommandTabControlUtil.hpp"

#include <assert.h>


//============================================================================
//============================================================================
namespace
{
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
	orthoModeObjectManip* get_object_manip_mode()
	{
		modeMode* pMode = modeModeMgr::GetCurrentMode();
		orthoModeObjectManip* pModeOM = dynamic_cast<orthoModeObjectManip*>(pMode);
		return pModeOM;
	}

	//
	//	Command Functions
	//

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Execute_DecrementCompassScale()
	{
		orthoModeObjectManip* pModeOM = get_object_manip_mode();
		if ( pModeOM != 0 )
		{
			pModeOM->DecrementScaleOfCompass();
		}
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Execute_IncrementCompassScale()
	{
		orthoModeObjectManip* pModeOM = get_object_manip_mode();
		if ( pModeOM != 0 )
		{
			pModeOM->IncrementScaleOfCompass();
		}
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Execute_DecrementTimeline()
	{
		orthoModeObjectManip* pModeOM = get_object_manip_mode();
		if ( pModeOM != 0 )
		{
			pModeOM->TimelineDecrementFrame();
		}
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Execute_IncrementTimeline()
	{
		orthoModeObjectManip* pModeOM = get_object_manip_mode();
		if ( pModeOM != 0 )
		{
			pModeOM->TimelineIncrementFrame();
		}
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Execute_ManipPlacement()
	{
		orthoModeObjectManip* pModeOM = get_object_manip_mode();
		if ( pModeOM != 0 )
		{
			pModeOM->SetStatePlacement();
		}
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Execute_ManipRotate()
	{
		orthoModeObjectManip* pModeOM = get_object_manip_mode();
		if ( pModeOM != 0 )
		{
			pModeOM->SetStateRotate();
		}
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Execute_ManipScale()
	{
		orthoModeObjectManip* pModeOM = get_object_manip_mode();
		if ( pModeOM != 0 )
		{
			pModeOM->SetStateScale();
		}
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Execute_ManipSelect()
	{
		orthoModeObjectManip* pModeOM = get_object_manip_mode();
		if ( pModeOM != 0 )
		{
			pModeOM->SetStateSelect();
		}
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Execute_ManipTranslate()
	{
		orthoModeObjectManip* pModeOM = get_object_manip_mode();
		if ( pModeOM != 0 )
		{
			pModeOM->SetStateTranslate();
		}
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Execute_SelectNextPickedObject()
	{
		orthoModeObjectManip* pModeOM = get_object_manip_mode();
		if ( pModeOM != 0 )
		{
			pModeOM->SelectNextObjectInPickList();
		}
	}

	
	//--------------------------------------------------------------------
	// PickMode
	//--------------------------------------------------------------------
	void Set_PickObjects(bool i_bValue) 
	{
		if (orthoModeObjectManip* pModeOM = get_object_manip_mode())
		{
//			pModeOM->SetPickMode(orthoModeObjectManip::e_PickObjects);
			mtrlOperations::HighlightMaterial(false);
			fgmtOperations::HighlightFragment(false);
		}
	}
	bool Get_PickObjects() 
	{
//		if (orthoModeObjectManip* pModeOM = get_object_manip_mode())
//			return (pModeOM->GetPickMode() == orthoModeObjectManip::e_PickObjects);
		return false;
	}
	void Set_PickMaterials(bool i_bValue)
	{
		if (orthoModeObjectManip* pModeOM = get_object_manip_mode())
		{
//			pModeOM->SetPickMode(orthoModeObjectManip::e_PickMaterials);
			fgmtOperations::HighlightFragment(false);
		}
	}
	bool Get_PickMaterials() 
	{
//		if (orthoModeObjectManip* pModeOM = get_object_manip_mode())
//			return (pModeOM->GetPickMode() == orthoModeObjectManip::e_PickMaterials);
		return false;
	}
	void Set_PickSurfaces(bool i_bValue)
	{
		if (orthoModeObjectManip* pModeOM = get_object_manip_mode())
		{
//			pModeOM->SetPickMode(orthoModeObjectManip::e_PickSurfaces);
			mtrlOperations::HighlightMaterial(false);
		}
	}
	bool Get_PickSurfaces() 
	{
//		if (orthoModeObjectManip* pModeOM = get_object_manip_mode())
//			return (pModeOM->GetPickMode() == orthoModeObjectManip::e_PickSurfaces);
		return false;
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
	pCmd = new cmaCommandSimple("Decrement Scale Of Compass", 
								"Icons", 
								"Decrement the scale Of the compass",
									
								&Execute_DecrementCompassScale );
	int menu_id = guiMenuMgr::AddMenuItem( "Icons", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Icons", "Decrement Scale Of Compass", pCmd );

	//	COMMAND: Increment scale of compass
	pCmd = new cmaCommandSimple("Increment Scale Of Compass", 
								"Icons", 
								"Increment the scale Of the compass",
									
								&Execute_IncrementCompassScale );
	menu_id = guiMenuMgr::AddMenuItem( "Icons", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Icons", "Increment Scale Of Compass", pCmd );

	//	COMMAND: Decrement Timeline
	pCmd = new cmaCommandSimple("Decrement Timeline", 
								"Actions", 
								"Decrement the Timeline",
									
								&Execute_DecrementTimeline );
	menu_id = guiMenuMgr::AddMenuItem( "Actions", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Actions", "Decrement time", pCmd );

	//	COMMAND: Increment Timeline
	pCmd = new cmaCommandSimple("Increment Timeline", 
								"Actions", 
								"Increment the Timeline",
									
								&Execute_IncrementTimeline );
	menu_id = guiMenuMgr::AddMenuItem( "Actions", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Actions", "Increment time", pCmd );

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

	//	COMMAND: Select Manipulation
	pCmd = new cmaCommandSimple("Select", 
								"Objects", 
								"Select manipulation for objects",
									
								&Execute_ManipSelect );
	menu_id = add_menu_item_and_toolbarbutton( "Objects", pCmd->GetTag().c_str(), mnpConstants::mc_Toolbar_Actions_Name, "edit-select.png" );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Objects", "Select", pCmd );

	//	COMMAND: Placement Manipulation
	pCmd = new cmaCommandSimple("Placement", 
								"Objects", 
								"Placement manipulation for objects",
									
								&Execute_ManipPlacement );
	menu_id = add_menu_item_and_toolbarbutton( "Objects", pCmd->GetTag().c_str(), mnpConstants::mc_Toolbar_Actions_Name, "edit-placement.png" );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Objects", "Placement", pCmd );

	//	COMMAND: Select Next Picked Object
	pCmd = new cmaCommandSimple("Select Next Picked Object", 
								"Objects", 
								"Select the next picked object from the picked list",
									
								&Execute_SelectNextPickedObject );
	menu_id = guiMenuMgr::AddMenuItem( "Objects", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Objects", "Select Next Picked Object", pCmd );

	//	COMMAND: Clear Selection
	pCmd = new cmaCommandSimple("Clear Selection", 
								"Objects", 
								"Clear the current selection list",
									
								&sel3dMgr::ClearSelection );
	menu_id = guiMenuMgr::AddMenuItem( "Objects", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );

	//	COMMAND: view File Gates
	pCmd = new cmaCommandToggle("Film Gates Visibile", 
								"Icons", 
								"Toggle Film Gates Visibility",
								&fgtFrameMgr::Show, 
								
								&fgtFrameMgr::IsVisible);
	menu_id = guiMenuMgr::AddCheckableMenuItem( "Icons", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Icons", "Toggle Film Gates Visibility", pCmd );

	//	COMMAND: view Icons
	pCmd = new cmaCommandToggle("Icons Visibile", 
								"Icons", 
								"Toggle Icons Visibility",
								&visMgr::ShowIcons, 
								
								&visMgr::IsShowIcons);
	menu_id = guiMenuMgr::AddCheckableMenuItem( "Icons", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Icons", "Toggle Icons Visibility", pCmd );

#ifndef _MANAGED
	// Pick Mode Group - feature of only the wxWidgets version
	guiMenuMgr::AddMenu( "View", "Pick Mode" );

	//	COMMAND: Pick Object
	pCmd = new cmaCommandToggle("Pick Object", 
								"Pick Mode", 
								"Pick objects with mouse clicks",
								&Set_PickObjects,
								
								&Get_PickObjects );
	
	menu_id = guiMenuMgr::AddCheckableMenuItem( "Pick Mode", pCmd->GetTag().c_str(), 
		mnpConstants::mc_Toolbar_PickMode_Name, "pickmode-object.png" );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );

	//	COMMAND: Pick Material
	pCmd = new cmaCommandToggle("Pick Material", 
								"Pick Mode", 
								"Pick material with mouse clicks",
								&Set_PickMaterials,
								
								&Get_PickMaterials );
	menu_id = guiMenuMgr::AddCheckableMenuItem( "Pick Mode", pCmd->GetTag().c_str(), 
		mnpConstants::mc_Toolbar_PickMode_Name, "pickmode-material.png" );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );

	//	COMMAND: Toggle Highlight of Materials
	pCmd = new cmaCommandToggle("Highlight Material", 
		"Highlighting",
		"Toggle highlighting of selected material",
		mtrlOperations::HighlightMaterial,
		
		mtrlOperations::IsHighlightMaterial);
	menu_id = guiMenuMgr::AddCheckableMenuItem( "Materials", pCmd->GetTag().c_str(), 
		mnpConstants::mc_Toolbar_PickMode_Name, "material-highlight.png" );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );

	//	COMMAND: Pick Surface
	pCmd = new cmaCommandToggle("Pick Surface", 
								"Pick Mode", 
								"Pick surface with mouse clicks",
								&Set_PickSurfaces,
								
								&Get_PickSurfaces );
	menu_id = guiMenuMgr::AddCheckableMenuItem( "Pick Mode", pCmd->GetTag().c_str(), 
		mnpConstants::mc_Toolbar_PickMode_Name, "pickmode-surface.png" );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	
	//	COMMAND: Toggle Highlight of Surfaces
	pCmd = new cmaCommandToggle("Highlight Surface", 
		"Highlighting",
		"Toggle highlighting of selected surface",
		fgmtOperations::HighlightFragment,
		
		fgmtOperations::IsHighlightFragment);
	menu_id = guiMenuMgr::AddCheckableMenuItem( "Surfaces", pCmd->GetTag().c_str(), 
		mnpConstants::mc_Toolbar_PickMode_Name, "surface-highlight.png" ); 
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
#endif
}
