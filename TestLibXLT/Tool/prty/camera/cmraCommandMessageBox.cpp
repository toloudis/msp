/*****************************************************************************
**  cmraCommandMessageBox.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "cmraCommandMessageBox.hpp"

#include "Tool/cma/cmaCommandMgr.hpp"
#include "Core/dbg/dbgAssert.hpp"
#include "Core/dbg/dbgLog.hpp"


//============================================================================
//============================================================================


///////////////////////////////////////////////////
// Event Handler(s)
//
namespace cmraCommandMessageBoxNS
{
	//--------------------------------------------------------------------
	//
	//--------------------------------------------------------------------
	void CommandExecuteHandler( cmaCommand* pCmd )
	{
		cmraCommandMessageBox* pCom = dynamic_cast<cmraCommandMessageBox*>(pCmd);
		DBG_ASSERT0( pCom != NULL, "Invalid command hooked up to Icons Visible command" );

		bool bChecked = !pCmd->GetChecked();
		cmaCommandMgr::CommandSetChecked( pCmd, bChecked );

		DBG_LOG2( "Command cmraCommandMessageBox Executed (%s) = %s", pCmd->GetTag().c_str(), (bChecked?"true":"false") );

		//
	}

	//--------------------------------------------------------------------
	//
	//--------------------------------------------------------------------
	void CommandUpdateHandler( cmaCommand* pCmd )
	{
		//cmaCommandMgr::CommandSetChecked( pCmd, cmraObjectMgr::IconsVisible() );
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
const std::string cmraCommandMessageBox::GetConstTagName()
{
	return std::string("cmraCommandMessageBox");
}

//--------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------
cmraCommandMessageBox::cmraCommandMessageBox()
:	cmaCommand( std::string("Camera MB"),
	cmraCommandMessageBoxNS::CommandExecuteHandler,
	cmraCommandMessageBoxNS::CommandUpdateHandler )
{
	this->SetTag( GetConstTagName() );
}


//--------------------------------------------------------------------
// destructor
//--------------------------------------------------------------------
//virtual
cmraCommandMessageBox::~cmraCommandMessageBox()
{
}

