/*****************************************************************************
**  setsCommands.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "Systems/Sets/GUI/setsCommands.hpp"

#include "Systems/Sets/GUI/setsDialogUtil.hpp"

#include "Tool/cma/cmaCommandMgr.hpp"

#include "Core/dbg/dbgLog.hpp"

namespace setsCommands
{

namespace
{

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void SetItemsExecute( cmaCommand* pCmd )
{
	//DBG_LOG1( "SetItems Executed (%s)", pCmd->GetTag().c_str() );
	//setsDialogUtil::ShowSetItemsDialog();
}

}


//--------------------------------------------------------------------
// SetupMenu
//--------------------------------------------------------------------
void setsCommands::SetupMenu()
{
}

}	// end of namespace