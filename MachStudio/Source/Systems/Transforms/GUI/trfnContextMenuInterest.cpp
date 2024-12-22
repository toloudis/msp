/****************************************************************************\
**	trfnContextMenuInterest.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "Systems/Transforms/GUI/trfnContextMenuInterest.hpp"
#include "Systems/Transforms/Undo/trfnOperations.hpp"
#include "Support/xfrm/xfrmTransformMgr.hpp"

#include "Core/Ma/maFunctions.hpp"
#include "Core/name/nameObject.hpp"
#include "Graphics/G3d/g3dPickInfo.hpp"
#include "Tool/gui/guiContextMenu.hpp"
#include "Tool/sel3d/sel3dCastUtil.hpp"

namespace
{
	// What are the maximum number of items we can put into a context menu?
	const int c_MaxNumSubMenuItems = 99;

	const char* c_MenuName = "Hierarchy";
	const char* c_AddMenuName = "Add Selection To Parent";

}

//----------------------------------------------------------------------------
// Add commands to the context menu based on what was selected.
// i_pSelectedObject and i_pPickInfo might be NULL.
//----------------------------------------------------------------------------
void trfnContextMenuInterest::AddToContextMenu(guiContextMenu &io_ContextMenu,
											  const sel3dObject* i_pChosenObject,
											  const g3dPickInfo* i_pPickInfo) const
{
	// The transform operations use the selection list, so it doesn't matter what
	// the chosen object is. As long as the selected list has something to put into a 
	// transform, we can add menu items.
	nameObject* pNameObj = sel3dCastUtil::CastSelectedObject<nameObject>();
	if (pNameObj)
	{
		if (xfrmTransformMgr::IsTransform(pNameObj->GetName())
		 || xfrmTransformMgr::IsObject(pNameObj->GetName()))
		{		
			io_ContextMenu.AddMenu(c_MenuName, "");
			io_ContextMenu.AddMenuItem(c_MenuName, "Create Parent from Selection", 
				&trfnOperations::CreateTransformFromSelection);
		}

		if (xfrmTransformMgr::IsObject(pNameObj->GetName()))
		{
			// Sub menus based on parent names
			std::vector<nameString> parent_names;
			xfrmTransformMgr::GetTransformNames(parent_names);
			if (!parent_names.empty())
			{
				io_ContextMenu.AddMenu(c_MenuName, c_AddMenuName);

				const int num_parents = maFunctions::Lowest((int)parent_names.size(), c_MaxNumSubMenuItems);
				for (int i=0; i<num_parents; ++i)
				{
					// Add Selected to Parent
					io_ContextMenu.AddMenuItem(c_AddMenuName, parent_names[i].GetString().c_str(), 
						std::bind(&trfnOperations::AddSelectedToTransform, parent_names[i]));
				}
			}
		}
	}

}
