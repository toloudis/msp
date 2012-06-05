/*****************************************************************************\
**	ctxmContextMenu.hpp
**
**		Manager for context menu interests. On right mouse click, the
**	windowing system should create a guiContextMenu, gather pick information
**	and then pass them into AddToContextMenu(). Each interest will then
**	have the option to add commands based on what was picked and what is
**	selected.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef CTXM_CONTEXTMENU_HPP
#error ctxmContextMenu.hpp multiply included
#endif
#define CTXM_CONTEXTMENU_HPP

#ifndef CTXM_CONTEXTMENUINTEREST_HPP
#include "Tool/ctxm/ctxmContextMenuInterest.hpp"
#endif

//============================================================================
//	Forward References
//============================================================================


//============================================================================
//============================================================================
namespace ctxmContextMenu
{
//	public:
		//--------------------------------------------------------------------
		//	RegisterInterest() - add an interest to the system
		//--------------------------------------------------------------------
		void RegisterInterest( ctxmContextMenuInterest* i_pInterest );

		//--------------------------------------------------------------------
		//	UnRegisterInterest() - remove an interest from the system.
		//
		//	Note: this will NOT delete the pick interest.  It is up to the
		//	registerer.
		//--------------------------------------------------------------------
		void UnRegisterInterest( ctxmContextMenuInterest* i_pInterest );

		//--------------------------------------------------------------------
		//	Clear() - clear the list
		//--------------------------------------------------------------------
		void Clear();
		
		//----------------------------------------------------------------------------
		// Add commands to the context menu based on what was selected.
		// i_pChosenObject and i_pPickInfo might be NULL.
		//----------------------------------------------------------------------------
		void AddToContextMenu(guiContextMenu &io_ContextMenu,
							  sel3dObject* i_pChosenObject,
							  const g3dPickInfo* i_pPickInfo);
}

