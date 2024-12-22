/****************************************************************************\
**	lsetContextMenuInterest.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "Systems/LightSets/GUI/lsetContextMenuInterest.hpp"
#include "Systems/LightSets/Undo/lsetOperations.hpp"

#include "Support/ltst/ltstIsolateMgr.hpp"
#include "Support/ltst/ltstLightSetMgr.hpp"
#include "Support/ltst/ltstInfluenceUtil.hpp"
#include "Support/rlyr/rlyrRenderLayerMgr.hpp"

#include "Core/Ma/maFunctions.hpp"
#include "Core/name/nameObject.hpp"
#include "Graphics/G3d/g3dPickInfo.hpp"
#include "Tool/gui/guiContextMenu.hpp"
#include "Tool/sel3d/sel3dCastUtil.hpp"

namespace
{
	// What are the maximum number of items we can put into a context menu?
	const int c_MaxNumSubMenuItems = 99;

	const char* c_LightSet_MenuName = "Light Sets";
	const char* c_AddMenuName = "Add Selection To Light Set";
	const char* c_RemoveMenuName = "Remove Selection From Light Set";
	const char* c_Lighting_MenuName = "Lighting";
	const char* c_Influences_MenuName = "Influences";

}

//----------------------------------------------------------------------------
// Add commands to the context menu based on what was selected.
// i_pSelectedObject and i_pPickInfo might be NULL.
//----------------------------------------------------------------------------
void lsetContextMenuInterest::AddToContextMenu(guiContextMenu &io_ContextMenu,
											  const sel3dObject* i_pChosenObject,
											  const g3dPickInfo* i_pPickInfo) const
{
	if (ltstIsolateMgr::HasIsolatedSet())
	{
		// Clear Isolation in any selection if there is an isolated set.
		// This is a top level menu item
		io_ContextMenu.AddMenuItem("", "Clear Isolated Lights", 
			&lsetOperations::ClearIsolatedLights);
	}
	if (rlyrRenderLayerMgr::HaveHiddenObjects())
	{
		io_ContextMenu.AddMenuItem("", "Unhide Objects", 
			&lsetOperations::ClearHidden);
	}

	// The light set operations use the selection list, so it doesn't matter what
	// the chosen object is. As long as the selected list has something to put into a 
	// light set, we can add menu items.
	nameObject* pNameObj = sel3dCastUtil::CastSelectedObject<nameObject>();
	if (pNameObj)
	{
		bool bIsLight = ltstLightSetMgr::IsLight(pNameObj->GetName());
		bool bIsLightSet = ltstLightSetMgr::IsLightSet(pNameObj->GetName());
		bool bIsObject = ltstLightSetMgr::IsObject(pNameObj->GetName());
		if (bIsLight || bIsObject)
		{		
			io_ContextMenu.AddMenu(c_LightSet_MenuName, "");
			io_ContextMenu.AddMenuItem(c_LightSet_MenuName, "Create Light Set from Selection", 
				&lsetOperations::CreateLightSetFromSelection);

			// Sub menus based on light set names
			std::vector<nameString> lset_names;
			ltstLightSetMgr::GetLightSetNames(lset_names);
			if (!lset_names.empty())
			{
				io_ContextMenu.AddMenu(c_LightSet_MenuName, c_AddMenuName);
				io_ContextMenu.AddMenu(c_LightSet_MenuName, c_RemoveMenuName);

				const int num_lsets = maFunctions::Lowest((int)lset_names.size(), c_MaxNumSubMenuItems);
				for (int i=0; i<num_lsets; ++i)
				{
					// Add Selected to Light Set
					io_ContextMenu.AddMenuItem(c_AddMenuName, lset_names[i].GetString().c_str(), 
						std::bind(&lsetOperations::AddSelectionToLightSet, i));

					// Remove Selected from Light Set
					io_ContextMenu.AddMenuItem(c_RemoveMenuName, lset_names[i].GetString().c_str(), 
						std::bind(&lsetOperations::RemoveSelectionFromLightSet, i));
				}
			}
		}

		if (bIsLight || bIsLightSet)
		{
			io_ContextMenu.AddMenu(c_Lighting_MenuName, "");
			io_ContextMenu.AddMenuItem(c_Lighting_MenuName, "Isolate Selected Lights", 
				&lsetOperations::IsolateSelectedLights);
			//io_ContextMenu.AddMenuItem(c_Lighting_MenuName, "Isolate Light Influence", 
			//	&lsetOperations::HideGeometryInfluencedBySelection);
		}

		if (bIsObject)
		{
			io_ContextMenu.AddMenu(c_Lighting_MenuName, "");
			io_ContextMenu.AddMenuItem(c_Lighting_MenuName, "Hide Unselected Objects", 
				&lsetOperations::HideGeometryExceptSelection);
			io_ContextMenu.AddMenuItem(c_Lighting_MenuName, "Isolate Influencing Lights", 
				&lsetOperations::IsolateInfluences);
			
			// Get light sets for the selected geometry
			std::set<nameString> light_sets;
			ltstInfluenceUtil::GetLightSetsForSelectedObjects(light_sets);
			io_ContextMenu.AddMenu(c_Lighting_MenuName, c_Influences_MenuName);

			// Add in the unassigned lights
			std::vector<nameString> light_names;
			nameString unassigned_set;
			ltstLightSetMgr::GetLightsInSet(unassigned_set, light_names);
			if (!light_names.empty())
			{
				const int num_lights = maFunctions::Lowest((int)light_names.size(), c_MaxNumSubMenuItems);
				if (num_lights > 0)
				{
					io_ContextMenu.AddMenu(c_Influences_MenuName, "Unassigned");

					for (int i=0; i<num_lights; ++i)
					{
						io_ContextMenu.AddMenuItem("Unassigned", light_names[i].GetString().c_str(), 
							std::bind(&ltstLightSetMgr::SelectLight,light_names[i]) );
					}
				}
			}

			// For each light set, get the containing lights. Organize submenus
			// such that each each light set is a submenu with its lights within
			std::set<nameString>::const_iterator it;
			for (it = light_sets.begin(); it != light_sets.end(); ++it)
			{
				ltstLightSetMgr::GetLightsInSet(*it, light_names);
				
				std::string lset_name = it->GetString();
				io_ContextMenu.AddMenu(c_Influences_MenuName, lset_name.c_str());

				const int num_lights = maFunctions::Lowest((int)light_names.size(), c_MaxNumSubMenuItems);
				for (int i=0; i<num_lights; ++i)
				{
					io_ContextMenu.AddMenuItem(lset_name.c_str(), light_names[i].GetString().c_str(), 
						std::bind(&ltstLightSetMgr::SelectLight,light_names[i]) );
				}
			}
		}
	}

}
