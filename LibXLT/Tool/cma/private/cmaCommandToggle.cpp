/*****************************************************************************
**	cmaCommandToggle.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Tool/cma/cmaCommandToggle.hpp"

#include "Core/dbg/dbgMsg.hpp"
#include "Tool/cma/cmaCommandMgr.hpp"
#include "Tool/gui/guiMenuMgr.hpp"


//============================================================================
// Event Handler(s)
//============================================================================
namespace cmaCommandToggleNS
{
	//--------------------------------------------------------------------
	//
	//--------------------------------------------------------------------
	void CommandExecuteHandler( cmaCommand* pCommand )
	{
		cmaCommandToggle* pCmd = dynamic_cast<cmaCommandToggle*>(pCommand);
		DBG_ASSERT( pCmd != NULL, "Invalid command hooked up to command toggle" );

		// Set value to opposite of the current value (toggle it!)
		(*pCmd->m_pSetFunction)(!(*pCmd->m_pGetFunction)());
	}

	//--------------------------------------------------------------------
	//
	//--------------------------------------------------------------------
	void CommandUpdateHandler( cmaCommand* pCommand )
	{
		cmaCommandToggle* pCmd = dynamic_cast<cmaCommandToggle*>(pCommand);
		DBG_ASSERT( pCmd != NULL, "Invalid command hooked up to command toggle" );

		// Set checked state of command
		bool bChecked = ((*pCmd->m_pGetFunction)());
		cmaCommandMgr::CommandSetChecked( pCmd, bChecked );
	}
}


//--------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------
cmaCommandToggle::cmaCommandToggle(const std::string &i_CommandTagName,
				 const std::string &i_Category,
				 const std::string &i_Description,
				 SetFunctionPtr i_pSetFunction,
				 GetFunctionPtr i_pGetFunction)
:	cmaCommand(  i_CommandTagName,
				 cmaCommandToggleNS::CommandExecuteHandler,
				 cmaCommandToggleNS::CommandUpdateHandler ),
	m_pGetFunction(i_pGetFunction),
	m_pSetFunction(i_pSetFunction)
{
	DBG_ASSERT(m_pGetFunction!=NULL, "Toggle command needs get function pointer. " << i_CommandTagName.c_str());
	DBG_ASSERT(m_pSetFunction!=NULL, "Toggle command needs set function pointer. " << i_CommandTagName.c_str());

	this->SetCategory(i_Category);
	this->SetDescription(i_Description);
}
