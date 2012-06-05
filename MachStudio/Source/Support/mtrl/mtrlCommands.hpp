/*****************************************************************************
**  mtrlCommands.hpp
**
**      Sets up material related commands
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#ifdef MTRL_COMMANDS_HPP
#error mtrlCommands.hpp multiply included
#endif
#define MTRL_COMMANDS_HPP

#ifndef CTXM_CONTEXTMENUINTEREST_HPP
#include "Tool/ctxm/ctxmContextMenuInterest.hpp"
#endif 


//============================================================================
//============================================================================
namespace mtrlCommands
{
	//--------------------------------------------------------------------
	// SetupMenu --
	//--------------------------------------------------------------------
	void SetupMenu();

	//--------------------------------------------------------------------
	// EnableMenus -- enable menu items based on selection
	//--------------------------------------------------------------------
	void EnableMenus(bool i_bHaveMaterialObject, 
					 bool i_bHaveMaterials,
					 bool i_bHaveMaterialPart,
					 bool i_bMaterialsLocked);
};


//============================================================================
//============================================================================
class mtrlContextMenuInterest : public ctxmContextMenuInterest
{
	public:
		//----------------------------------------------------------------------------
		// Add commands to the context menu based on what was selected.
		// i_pChosenObject and i_pPickInfo might be NULL.
		//----------------------------------------------------------------------------
		virtual void AddToContextMenu(guiContextMenu &io_ContextMenu,
									  const sel3dObject* i_pChosenObject,
									  const g3dPickInfo* i_pPickInfo) const;
};
