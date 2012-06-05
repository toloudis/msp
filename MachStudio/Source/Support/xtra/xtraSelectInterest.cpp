/****************************************************************************\
**	xtraSelectInterest.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Support/xtra/xtraSelectInterest.hpp"

#include "Support/xtra/GUI/xtraCommands.hpp"
#include "Support/xtra/xtraScriptObject.hpp"

#include "Tool/sel3d/sel3dCastUtil.hpp"


//--------------------------------------------------------------------
//	SelectionChanged - called when the selection list is changed
//--------------------------------------------------------------------
//virtual 
void xtraSelectInterest::SelectionChanged()
{
	if ( xtraScriptObject *pMatObj = sel3dCastUtil::CastSelectedObject<xtraScriptObject>() )
	{
		xtraCommands::EnableMenus(true);
	}
	else
	{
		xtraCommands::EnableMenus(false);
	}
}

//--------------------------------------------------------------------
//	AddedToSelection - called when an object is added to the
//		selection list.
//--------------------------------------------------------------------
//virtual 
void xtraSelectInterest::AddedToSelection( sel3dObject* i_pSelObj )
{
}

//--------------------------------------------------------------------
//	RemovedFromSelection - called when an object is removed from
//		the selection list.
//--------------------------------------------------------------------
//virtual 
void xtraSelectInterest::RemovedFromSelection( sel3dObject* i_pSelObj )
{
}

//--------------------------------------------------------------------
//	SelectFromPickCode - check the pick code of the selected
//	object and set the selected material index to match.
//--------------------------------------------------------------------
bool xtraSelectInterest::SelectFromPickCode(sel3dObject *i_pPicked,
											envType::UInt32 i_PickCode,
											bool i_bAppendSelection)
{
	return false;
}
