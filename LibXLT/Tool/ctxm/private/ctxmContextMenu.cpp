/****************************************************************************\
**	ctxmContextMenu.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "Tool/ctxm/ctxmContextMenu.hpp"

#include "Core/dbg/dbgMsg.hpp"
#include "Core/env/envSTLHelpers.hpp"


//============================================================================
//============================================================================
namespace
{
	std::vector<ctxmContextMenuInterest*>	m_ContextMenuInterestList;
}


//----------------------------------------------------------------------------
//	RegisterInterest()
//----------------------------------------------------------------------------
void ctxmContextMenu::RegisterInterest( ctxmContextMenuInterest* i_pInterest )
{
	DBG_ASSERT( i_pInterest != 0, "Cannot register a NULL Pick Interest" );

	m_ContextMenuInterestList.push_back( i_pInterest );
}

//--------------------------------------------------------------------
//	UnRegisterInterest() - remove a pick interest from the system.
//
//	Note: this will NOT delete the pick interest.  It is up to the
//	registerer.
//--------------------------------------------------------------------
void ctxmContextMenu::UnRegisterInterest( ctxmContextMenuInterest* i_pInterest )
{
	envSTLHelpers::RemoveOneValue( m_ContextMenuInterestList, i_pInterest );
}

//----------------------------------------------------------------------------
//	Clear() - clear the list
//----------------------------------------------------------------------------
void ctxmContextMenu::Clear()
{
	m_ContextMenuInterestList.clear();
}

//----------------------------------------------------------------------------
// Add commands to the context menu based on what was selected.
// i_pSelectedObject and i_pPickInfo might be NULL.
//----------------------------------------------------------------------------
void ctxmContextMenu::AddToContextMenu(guiContextMenu &io_ContextMenu,
									   sel3dObject* i_pSelectedObject,
									   const g3dPickInfo* i_pPickInfo)
{
	std::vector<ctxmContextMenuInterest*>::iterator it, end = m_ContextMenuInterestList.end();
	for ( it  = m_ContextMenuInterestList.begin(); it != end; it++ )
	{
		(*it)->AddToContextMenu( io_ContextMenu, i_pSelectedObject, i_pPickInfo );
	}
}
