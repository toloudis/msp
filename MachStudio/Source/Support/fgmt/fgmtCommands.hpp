/*****************************************************************************
**  fgmtCommands.hpp
**
**      Sets up fragment related commands
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#ifdef FGMT_COMMANDS_HPP
#error fgmtCommands.hpp multiply included
#endif
#define FGMT_COMMANDS_HPP

#ifndef CTXM_CONTEXTMENUINTEREST_HPP
#include "Tool/ctxm/ctxmContextMenuInterest.hpp"
#endif 


//============================================================================
//============================================================================
namespace fgmtCommands
{
	//--------------------------------------------------------------------
	// SetupMenu --
	//--------------------------------------------------------------------
	void SetupMenu();

	//--------------------------------------------------------------------
	// EnableMenus -- enable menu items based on selection
	//--------------------------------------------------------------------
	void EnableMenus(bool i_bHaveSurfaceObject, bool i_bHaveSurfacePart);
};

//============================================================================
//============================================================================
class fgmtContextMenuInterest : public ctxmContextMenuInterest
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


