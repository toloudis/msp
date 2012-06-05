/*****************************************************************************
**  setsCommands.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "Systems/Sets/GUI/setsCommands.hpp"

#include "Systems/Sets/GUI/setsDialogUtil.hpp"

#include "Tool/cma/cmaCommandMgr.hpp"

#include "Core/dbg/dbgMsg.hpp"

namespace setsCommands
{

namespace
{

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void SetItemsExecute( cmaCommand* pCmd )
{
	//DBG_LOG( "SetItems Executed (" << pCmd->GetTag().c_str() << ")"  );
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