/****************************************************************************\
**	envtContextMenuInterest.hpp
**
**		Interface for context menu interests. On a call to AddToContextMenu(),
**	the interest has the option to add commands to the guiContextMenu based 
**	on what was picked and what is	selected.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef ENVT_CONTEXTMENUINTEREST_HPP
#error envtContextMenuInterest.hpp multiply included
#endif
#define ENVT_CONTEXTMENUINTEREST_HPP

#ifndef CTXM_CONTEXTMENUINTEREST_HPP
#include "Tool/ctxm/ctxmContextMenuInterest.hpp"
#endif 


//============================================================================
//	Forward References
//============================================================================


//============================================================================
//============================================================================
class envtContextMenuInterest : public ctxmContextMenuInterest
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
