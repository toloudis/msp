/****************************************************************************\
**	chtrContextMenuInterest.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "Systems/Character/GUI/chtrContextMenuInterest.hpp"
#include "Systems/Character/Object/chtrScriptObject.hpp"

#include "Support/fgmt/GUI/fgmtPropertyObject.hpp"
#include "Support/mtrl/GUI/mtrlPropertyObject.hpp"

#include "Core/Ma/maFunctions.hpp"
#include "Graphics/G3d/g3dPickInfo.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Tool/gui/guiContextMenu.hpp"
#include "Tool/sel3d/sel3dCastUtil.hpp"

namespace
{
	// What are the maximum number of items we can put into a context menu?
	const int c_MaxNumSubMenuItems = 99;

	const char* c_Context_Menu_Name = "Materials";
	const char* c_Context_Select_SubMenu_Name = "Select Material";

	//--------------------------------------------------------------------
	// Select material in currently selected object with given name,
	// callback from context menu
	//--------------------------------------------------------------------
	void SelectMaterialByName(const std::string& i_MaterialName)
	{
		if ( mtrlScriptObject *pMatObj = sel3dCastUtil::CastSelectedObject<mtrlScriptObject>() )
		{
			sel3dObject *pPart  = pMatObj->GetMaterialUI(i_MaterialName);
			if (pPart)
			{
				sel3dMgr::CreateUndoOperation();
				sel3dMgr::Select(pPart);
			}
		}
	}

	//--------------------------------------------------------------------
	// Create submenu with all materials in the given selection
	//--------------------------------------------------------------------
	void create_select_materials_menu(guiContextMenu &io_ContextMenu,
									  const chtrScriptObject *i_pCharacter,
									  const fgmtPropertyObject *i_pSelectedSurface)
	{
		if (i_pSelectedSurface)
		{
			// List just the one material for this fragment
			int frag_index = i_pCharacter->fgmtScriptObject::GetIndexForName(i_pSelectedSurface->GetName());
			matMaterial *pMat = i_pCharacter->GetMaterialForSurface(frag_index);
			if (pMat)
			{
				std::string mat_name = pMat->GetName();
				std::string menu_item_name =  "Select " + pMat->GetName();
				io_ContextMenu.AddMenu(c_Context_Menu_Name, "");
				io_ContextMenu.AddMenuItem(c_Context_Menu_Name, menu_item_name.c_str(), 
					std::bind(&SelectMaterialByName, mat_name));
			}
		}
		else
		{
			// List all materials in object
			int num_materials = maFunctions::Lowest(i_pCharacter->GetNumMaterials(), c_MaxNumSubMenuItems);
			if (num_materials > 0)
			{
				io_ContextMenu.AddMenu(c_Context_Menu_Name, "");
				io_ContextMenu.AddMenu(c_Context_Menu_Name, c_Context_Select_SubMenu_Name);
				for (int i=0; i<num_materials; ++i)
				{
					std::string mat_name = i_pCharacter->GetMaterialName(i);
					io_ContextMenu.AddMenuItem(c_Context_Select_SubMenu_Name, mat_name.c_str(), 
						std::bind(&SelectMaterialByName, mat_name));
				}
			}
		}
	}

}

//----------------------------------------------------------------------------
// Add commands to the context menu based on what was selected.
// i_pSelectedObject and i_pPickInfo might be NULL.
//----------------------------------------------------------------------------
void chtrContextMenuInterest::AddToContextMenu(guiContextMenu &io_ContextMenu,
											  const sel3dObject* i_pChosenObject,
											  const g3dPickInfo* i_pPickInfo) const
{
	if ( chtrScriptObject *pCharacter = sel3dCastUtil::CastSelectedObject<chtrScriptObject>() )
	{
		//DBG_LOG( "selected an mtrlScriptObject" );

		// We don't want to show a selection menu if a material is already selected.
		if ( NULL == sel3dCastUtil::CastSelectedObject<mtrlPropertyObject>() )
		{
			// Create submenu with all materials in the given selection,
			// If the selection is a single surface then only list the material for
			// that surface.
			fgmtPropertyObject *pSurfaceObj = sel3dCastUtil::CastSelectedObject<fgmtPropertyObject>();
			create_select_materials_menu(io_ContextMenu, pCharacter, pSurfaceObj);
		}
	}
}
