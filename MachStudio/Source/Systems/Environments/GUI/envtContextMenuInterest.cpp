/****************************************************************************\
**	envtContextMenuInterest.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "Systems/Environments/GUI/envtContextMenuInterest.hpp"
#include "Systems/Environments/Undo/envtOperations.hpp"

#include "Support/evmt/evmtEnvironmentMgr.hpp"
#include "Support/evmt/evmtInfluenceUtil.hpp"
#include "Support/rlyr/rlyrRenderLayerMgr.hpp"

#include "Core/Ma/maFunctions.hpp"
#include "Core/name/nameObject.hpp"
#include "Graphics/G3d/g3dPickInfo.hpp"
#include "Tool/gui/guiContextMenu.hpp"
#include "Tool/sel3d/sel3dCastUtil.hpp"

namespace
{
	const char* c_Environment_MenuName = "Environments";
	const char* c_Lighting_MenuName = "Lighting";
	const char* c_Influences_MenuName = "Influences";

}

//----------------------------------------------------------------------------
// Add commands to the context menu based on what was selected.
// i_pSelectedObject and i_pPickInfo might be NULL.
//----------------------------------------------------------------------------
void envtContextMenuInterest::AddToContextMenu(guiContextMenu &io_ContextMenu,
											  const sel3dObject* i_pChosenObject,
											  const g3dPickInfo* i_pPickInfo) const
{

	// The environment operations use the selection list, so it doesn't matter what
	// the chosen object is. As long as the selected list has something to put into a 
	// environment, we can add menu items.
	nameObject* pNameObj = sel3dCastUtil::CastSelectedObject<nameObject>();
	if (pNameObj)
	{
		bool bIsEnvironment = evmtEnvironmentMgr::IsEnvironment(pNameObj->GetName());
		bool bIsObject = evmtEnvironmentMgr::IsObject(pNameObj->GetName());

		if (bIsObject)
		{
			io_ContextMenu.AddMenu(c_Lighting_MenuName, "");
			//io_ContextMenu.AddMenuItem(c_Lighting_MenuName, "Hide Unselected Objects", 
			//	&envtOperations::HideGeometryExceptSelection);
			//io_ContextMenu.AddMenuItem(c_Lighting_MenuName, "Isolate Light Influences", 
			//	&envtOperations::IsolateInfluences);
			
			// Get environments for the selected geometry
			std::set<nameString> environment_names;
			evmtInfluenceUtil::GetEnvironmentsForSelection(environment_names);
			io_ContextMenu.AddMenu(c_Lighting_MenuName, c_Influences_MenuName);

			if (!environment_names.empty())
			{
				io_ContextMenu.AddMenu(c_Influences_MenuName, c_Environment_MenuName);

				std::set<nameString>::const_iterator it;
				for (it = environment_names.begin(); it != environment_names.end(); ++it)
				{
					io_ContextMenu.AddMenuItem(c_Environment_MenuName, it->GetString().c_str(), 
						std::bind(&envtOperations::SelectObject, (const nameString&)*it, false) );
				}
			}

		}
	}

}
