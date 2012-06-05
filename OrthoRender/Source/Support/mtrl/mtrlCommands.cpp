/*****************************************************************************
**  mtrlCommands.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Support/mtrl/mtrlCommands.hpp"

#include "Support/mtrl/GUI/mtrlDialogUtil.hpp"
#include "Support/mtrl/GUI/mtrlOperations.hpp"
#include "Support/mtrl/mtrlScriptObject.hpp"
#include "Systems/Common/GUI/cmmObjectDialogUtil.hpp"

#include "Tool/gui/guiCommandMgr.hpp"
#include "Tool/gui/guiMenuMgr.hpp"
#include "Tool/gui/guiMessageBox.hpp"
#include "Tool/gui/guiToolbarMgr.hpp"
#include "Tool/cma/cmaCommandToggle.hpp"
#include "Tool/cma/cmaCommandSimple.hpp"
#include "Tool/sel3d/sel3dCastUtil.hpp"


//============================================================================
//============================================================================
namespace mtrlCommands
{
	namespace
	{
		const char* c_Toolbar_Surfaces_Name = "Materials";

		int l_MenuIdOverride = -1;
		int l_MenuIdHighlight = -1;
		int l_MenuIdLock = -1;
		int l_MenuIdSave = -1;
		int l_MenuIdImport = -1;
		int l_MenuIdCopy = -1;
		int l_MenuIdPaste = -1;
		int l_MenuIdExportLib = -1;
		int l_MenuIdImportLib = -1;

		//--------------------------------------------------------------------
		// toggle highlighting of material
		//--------------------------------------------------------------------
		void highlight_material(bool i_bOn)
		{
			mtrlOperations::HighlightMaterial(i_bOn);
			mtrlDialogUtil::UpdateHighlightToggle();
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void Execute_OverrideMaterials()
		{
			if ( mtrlScriptObject *pMatObj = sel3dCastUtil::CastSelectedObject<mtrlScriptObject>() )
			{
				mtrlOperations::OverrideMaterials(pMatObj);
			}
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void Execute_SaveMaterials()
		{
			if ( mtrlScriptObject *pMatObj = sel3dCastUtil::CastSelectedObject<mtrlScriptObject>() )
			{
				if (pMatObj->GetNumMaterials() > 0)
					mtrlOperations::PromptAndSaveMaterials(pMatObj);
			}
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void Execute_ImportMaterials()
		{
			if ( mtrlScriptObject *pObject = sel3dCastUtil::CastSelectedObject<mtrlScriptObject>() )
			{
				if (pObject->HasMaterialAnimation())
				{
					guiMessageBox::Show( "This material is animated and cannot be changed in this way.", "Material AnimationExists", guiMessageBox::e_OK );
				}
				else
				{
					mtrlOperations::ImportMaterials(pObject);
					cmmObjectDialogUtil::UpdateDialog();
				}
			}
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void Execute_PasteMaterial()
		{
			if ( mtrlOperations::HaveClipboardData() )
			{
				if (mtrlOperations::HasMaterialAnimation())
				{
					guiMessageBox::Show( "This material is animated and cannot be changed in this way.", "Material AnimationExists", guiMessageBox::e_OK );
				}
				else
				{
					mtrlOperations::PasteMaterial();
					cmmObjectDialogUtil::UpdateDialog();
				}
			}
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void Execute_ImportMaterialLib()
		{
			if (mtrlOperations::HasMaterialAnimation())
			{
				guiMessageBox::Show( "This material is animated and cannot be changed in this way.", "Material AnimationExists", guiMessageBox::e_OK );
			}
			else
			{
				mtrlOperations::ImportFromLibrary();
				cmmObjectDialogUtil::UpdateDialog();
			}
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
		guiMenuMgr::AddMenu("Actions", "Materials");
			
		//	COMMAND: Override Materials
		cmaCommand* pCmd = new cmaCommandSimple("Override Materials", 
									"Materials", 
									"Override materials on selected object",
									
									&Execute_OverrideMaterials );
		l_MenuIdOverride = guiMenuMgr::AddMenuItem( "Materials", pCmd->GetTag().c_str(), 
			c_Toolbar_Surfaces_Name, "material-override.png" );
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), l_MenuIdOverride );
		//cmmSystemDialogUtil::AddSystemCommand( "Materials", "Override Materials", pCmd );
	
//Note; highlight button moved to pickmode toolbar
		//	COMMAND: Toggle Highlight of Materials
		//pCmd = new cmaCommandToggle("Highlight Material", 
		//	"Highlighting",
		//	"Toggle highlighting of selected material",
		//	highlight_material,
		//	mtrlOperations::IsHighlightMaterial);
		//l_MenuIdHighlight = guiMenuMgr::AddCheckableMenuItem( "Materials", pCmd->GetTag().c_str(), 
		//	c_Toolbar_Surfaces_Name, "material-highlight.png" );
		//guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), l_MenuIdHighlight );

		//	COMMAND: Toggle Lock of Materials
		pCmd = new cmaCommandToggle("Lock Materials", 
			"Materials",
			"Toggle lock of editing materials",
			mtrlOperations::LockMaterials,
			
			mtrlOperations::IsLockMaterials);
		l_MenuIdLock = guiMenuMgr::AddCheckableMenuItem( "Materials", pCmd->GetTag().c_str(), 
			c_Toolbar_Surfaces_Name, "material-lock.png" );
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), l_MenuIdLock );

		//	COMMAND: Save Materials
		pCmd = new cmaCommandSimple("Save Materials", 
									"Materials", 
									"Save materials of selected object",
									
									&Execute_SaveMaterials );
		l_MenuIdSave = guiMenuMgr::AddMenuItem( "Materials", pCmd->GetTag().c_str(), 
			c_Toolbar_Surfaces_Name, "material-save.png" );
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), l_MenuIdSave );

		//	COMMAND: Import Materials
		pCmd = new cmaCommandSimple("Import Materials", 
									"Materials", 
									"Import materials from a file into the selected object",
									
									&Execute_ImportMaterials );
		l_MenuIdImport = guiMenuMgr::AddMenuItem( "Materials", pCmd->GetTag().c_str(), 
			c_Toolbar_Surfaces_Name, "material-import.png" );
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), l_MenuIdImport );

		//	COMMAND: Copy To Clipboard
		pCmd = new cmaCommandSimple("Copy Material", 
									"Materials", 
									"Copy material into the clipboard",
									
									mtrlOperations::CopyMaterial );
		l_MenuIdCopy = guiMenuMgr::AddMenuItem( "Materials", pCmd->GetTag().c_str(), 
			c_Toolbar_Surfaces_Name, "material-copy.png" );
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), l_MenuIdCopy );

		//	COMMAND: Paste From Clipboard
		pCmd = new cmaCommandSimple("Paste Material", 
									"Materials", 
									"Paste material from the clipboard",
									
									&Execute_PasteMaterial );
		l_MenuIdPaste = guiMenuMgr::AddMenuItem( "Materials", pCmd->GetTag().c_str(), 
			c_Toolbar_Surfaces_Name, "material-paste.png" );
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), l_MenuIdPaste );

		//	COMMAND: Export To Library
		pCmd = new cmaCommandSimple("Export to Library", 
									"Materials", 
									"Export material from the material library into the selected object",
									
									mtrlOperations::ExportToLibrary );
		l_MenuIdExportLib = guiMenuMgr::AddMenuItem( "Materials", pCmd->GetTag().c_str(), 
			c_Toolbar_Surfaces_Name, "material-exportlib.png" );
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), l_MenuIdExportLib );

		//	COMMAND: Import From Library
		pCmd = new cmaCommandSimple("Import From Library", 
									"Materials", 
									"Import material from the material library into the selected object",
									
									&Execute_ImportMaterialLib );
		l_MenuIdImportLib = guiMenuMgr::AddMenuItem( "Materials", pCmd->GetTag().c_str(), 
			c_Toolbar_Surfaces_Name, "material-importlib.png" );
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), l_MenuIdImportLib );
	*/
	}

	//--------------------------------------------------------------------
	// EnableMenus -- enable menu items based on selection
	//--------------------------------------------------------------------
	void EnableMenus(bool i_bHaveMaterialObject, 
					 bool i_bHaveMaterials,
					 bool i_bHaveMaterialPart,
					 bool i_bMaterialsLocked)
	{
		//WXGUI
		/*
#ifndef _MANAGED
		guiToolbarMgr::Show(c_Toolbar_Surfaces_Name, i_bHaveMaterialObject);
#endif
		guiMenuMgr::MenuObjectsEnable(l_MenuIdOverride, mtrlOperations::CanOverrideMaterials() && !i_bMaterialsLocked);
		// highlight can stay enabled
		//guiMenuMgr::MenuObjectsEnable(l_MenuIdHighlight, i_bHaveMaterialPart);
		guiMenuMgr::MenuObjectsEnable(l_MenuIdLock, i_bHaveMaterials);
		guiMenuMgr::MenuObjectsEnable(l_MenuIdSave, i_bHaveMaterials && !i_bMaterialsLocked);
		guiMenuMgr::MenuObjectsEnable(l_MenuIdImport, i_bHaveMaterials && !i_bMaterialsLocked);
		guiMenuMgr::MenuObjectsEnable(l_MenuIdCopy, i_bHaveMaterialPart);
		guiMenuMgr::MenuObjectsEnable(l_MenuIdPaste, i_bHaveMaterialPart && mtrlOperations::HaveClipboardData() && !i_bMaterialsLocked);
		guiMenuMgr::MenuObjectsEnable(l_MenuIdExportLib, i_bHaveMaterialPart);
		guiMenuMgr::MenuObjectsEnable(l_MenuIdImportLib, i_bHaveMaterialPart && !i_bMaterialsLocked);
	*/
	}

}	// end of namespace
