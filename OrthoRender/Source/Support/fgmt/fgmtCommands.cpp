/*****************************************************************************
**  fgmtCommands.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Support/fgmt/fgmtCommands.hpp"

#include "Support/fgmt/GUI/fgmtDialogUtil.hpp"
#include "Support/fgmt/GUI/fgmtOperations.hpp"
#include "Support/fgmt/fgmtScriptObject.hpp"

#include "Graphics/g3d/g3dPrefs.hpp"
#include "Tool/gui/guiCommandMgr.hpp"
#include "Tool/gui/guiMenuMgr.hpp"
#include "Tool/gui/guiToolbarMgr.hpp"
#include "Tool/cma/cmaCommandToggle.hpp"
#include "Tool/cma/cmaCommandSimple.hpp"
#include "Tool/sel3d/sel3dCastUtil.hpp"


//============================================================================
//============================================================================
namespace fgmtCommands
{
	namespace
	{
		const char* c_Toolbar_Surfaces_Name = "Surfaces";
		
		int l_MenuIdOverride = -1;
		int l_MenuIdHighlight = -1;
		int l_MenuIdApplyFlags = -1;
		int l_MenuIdApplyFlagsAO = -1;
		int l_MenuIdSaveAO = -1;
		int l_MenuIdAutoImportAO = -1;
		int l_MenuIdClearAO = -1;
		int l_MenuIdAOGrid = -1;

		void highlight_fragment(bool i_bOn)
		{
			fgmtOperations::HighlightFragment(i_bOn);
			fgmtDialogUtil::UpdateHighlightToggle();
		}
	
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void Execute_OverrideSurfaces()
		{
			if ( fgmtScriptObject *pObject = sel3dCastUtil::CastSelectedObject<fgmtScriptObject>() )
			{
				fgmtOperations::OverrideFragments(pObject);
			}
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void Execute_ComputeAO()
		{
			// this signals that we want to process any invalid ao solutions.
			g3dPrefs::CurrentPrefs().m_RecalcAORequested = true;
		}
	}

	//--------------------------------------------------------------------
	// SetupMenu --
	//--------------------------------------------------------------------
	void SetupMenu()
	{
		//WXGUI
		/*
		// Create commands for surface operations
		guiMenuMgr::AddMenu("Actions", "Surfaces");
			
		//	COMMAND: Override Surfaces
		cmaCommand* pCmd = new cmaCommandSimple("Override Surface Flags", 
									"Surfaces", 
									"Override surfaces on selected object",
									
									&Execute_OverrideSurfaces );
		l_MenuIdOverride = guiMenuMgr::AddMenuItem( "Surfaces", pCmd->GetTag().c_str() 
			//,c_Toolbar_Surfaces_Name, "surface-override.png" 
		); 
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), l_MenuIdOverride );
		//cmmSystemDialogUtil::AddSystemCommand( "Surfaces", "Override Surfaces", pCmd );

//Note: Moved highlight toggle to the pick mode toolbar
		//	COMMAND: Toggle Highlight of Surfaces
		//pCmd = new cmaCommandToggle("Highlight Surface", 
		//	"Highlighting",
		//	"Toggle highlighting of selected surface",
		//	highlight_fragment,
		//	fgmtOperations::IsHighlightFragment);
		//l_MenuIdHighlight = guiMenuMgr::AddCheckableMenuItem( "Surfaces", pCmd->GetTag().c_str(), 
		//	c_Toolbar_Surfaces_Name, "surface-highlight.png" ); 
		//guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), l_MenuIdHighlight );

		//	COMMAND: Apply Flags To All
		pCmd = new cmaCommandSimple("Apply Flags To All", 
									"Surfaces", 
									"Apply surface info from selected surface to all surfaces in object",
									
									&fgmtOperations::SetAllFragments );
		l_MenuIdApplyFlags = guiMenuMgr::AddMenuItem( "Surfaces", pCmd->GetTag().c_str(), 
			c_Toolbar_Surfaces_Name, "surface-applyflags.png" ); 
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), l_MenuIdApplyFlags );

		//	COMMAND: Apply AO Flags To All
		pCmd = new cmaCommandSimple("Apply AO Flags To All", 
									"Surfaces", 
									"Apply ambient occlusion info from selected surface to all surfaces in object",
									
									&fgmtOperations::SetAllFragmentsAO );
		l_MenuIdApplyFlagsAO = guiMenuMgr::AddMenuItem( "Surfaces", pCmd->GetTag().c_str(), 
			c_Toolbar_Surfaces_Name, "surface-applyflagsAO.png" );
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), l_MenuIdApplyFlagsAO  );

		//	COMMAND: Save AO Textures
		pCmd = new cmaCommandSimple("Save AO Textures", 
									"Surfaces", 
									"Save ambient occlusion textures to files",
									
									&fgmtOperations::SaveAOTextures );
		l_MenuIdSaveAO = guiMenuMgr::AddMenuItem( "Surfaces", pCmd->GetTag().c_str(), 
			c_Toolbar_Surfaces_Name, "surface-saveAOtextures.png" );
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), l_MenuIdSaveAO );

		//	COMMAND: AutoImport AO Textures
		pCmd = new cmaCommandSimple("AutoImport AO Textures", 
									"Surfaces", 
									"Load ambient occlusion textures from Save folder",
									
									&fgmtOperations::AutoImportAOTextures );
		l_MenuIdAutoImportAO = guiMenuMgr::AddMenuItem( "Surfaces", pCmd->GetTag().c_str(), 
			c_Toolbar_Surfaces_Name, "surface-saveAOtextures.png" );
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), l_MenuIdAutoImportAO );

		//	COMMAND: Clear AO Textures
		pCmd = new cmaCommandSimple("Clear AO Textures", 
									"Surfaces", 
									"Remove texture filenames from fragments",
									
									&fgmtOperations::ClearAOTextures );
		l_MenuIdClearAO = guiMenuMgr::AddMenuItem( "Surfaces", pCmd->GetTag().c_str(), 
			c_Toolbar_Surfaces_Name, "surface-saveAOtextures.png" );
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), l_MenuIdClearAO );

		pCmd = new cmaCommandSimple("Compute AO", 
									"Surfaces", 
									"Compute Ambient Occlusion",
									
									&Execute_ComputeAO );
		// TODO: get toolbar button icon!
		int menu_id = guiMenuMgr::AddMenuItem( "Surfaces", pCmd->GetTag().c_str() );
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );

		//	COMMAND: Show AO Grid
		pCmd = new cmaCommandSimple("Show AO Grid", 
									"Surfaces", 
									"Show grid of ambient occlusion settings",
									
									&fgmtOperations::ShowAOGrid );
		l_MenuIdAOGrid = guiMenuMgr::AddMenuItem( "Surfaces", pCmd->GetTag().c_str(), 
			c_Toolbar_Surfaces_Name, "surface-saveAOtextures.png" );
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), l_MenuIdAOGrid );

#ifndef _MANAGED
		// Start out with the toolbar hidden
		guiToolbarMgr::Show(c_Toolbar_Surfaces_Name, false);
#endif
	*/
	}

	//--------------------------------------------------------------------
	// EnableMenus -- enable menu items based on selection
	//--------------------------------------------------------------------
	void EnableMenus(bool i_bHaveSurfaceObject, bool i_bHaveSurfacePart)
	{
		//WXGUI
		/*
#ifndef _MANAGED
		// When to display the toolbar depends on if the override command has a button,
		// the other commands all are only valid when a surface part is selected.
		guiToolbarMgr::Show(c_Toolbar_Surfaces_Name, i_bHaveSurfacePart);
#endif
		guiMenuMgr::MenuObjectsEnable(l_MenuIdOverride, fgmtOperations::CanOverrideFragments());
		// highlight can stay enabled
		//guiMenuMgr::MenuObjectsEnable(l_MenuIdHighlight, i_bHaveSurfacePart); 
		guiMenuMgr::MenuObjectsEnable(l_MenuIdApplyFlags, i_bHaveSurfacePart);
		guiMenuMgr::MenuObjectsEnable(l_MenuIdApplyFlagsAO, i_bHaveSurfacePart);
		guiMenuMgr::MenuObjectsEnable(l_MenuIdSaveAO, i_bHaveSurfaceObject); // does this really need a surface selected?
		guiMenuMgr::MenuObjectsEnable(l_MenuIdAutoImportAO, i_bHaveSurfaceObject); // does this really need a surface selected?
		guiMenuMgr::MenuObjectsEnable(l_MenuIdClearAO, i_bHaveSurfaceObject); // does this really need a surface selected?

		guiMenuMgr::MenuObjectsEnable(l_MenuIdAOGrid, i_bHaveSurfaceObject); // does this really need a surface selected?
	*/
	}

}	// end of namespace