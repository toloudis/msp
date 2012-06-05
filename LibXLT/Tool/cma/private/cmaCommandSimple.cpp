/*****************************************************************************
**	cmaCommandSimple.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Tool/cma/cmaCommandSimple.hpp"

#include "Core/dbg/dbgMsg.hpp"


//============================================================================
// Event Handler(s)
//============================================================================
namespace cmaCommandSimpleNS
{
	//--------------------------------------------------------------------
	//
	//--------------------------------------------------------------------
	void CommandExecuteHandler( cmaCommand* pCmd )
	{
		cmaCommandSimple* pCSM = dynamic_cast<cmaCommandSimple*>(pCmd);
		DBG_ASSERT( pCSM != NULL, "Invalid command hooked up to command simple" );

		(*pCSM->m_pExecuteFunction)();
	}
}


//--------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------
cmaCommandSimple::cmaCommandSimple(const std::string &i_CommandTagName,
				 const std::string &i_Category,
				 const std::string &i_Description,
				 ExecuteFunctionPtr i_pExecuteFunction)
:	cmaCommand(  i_CommandTagName,
				 cmaCommandSimpleNS::CommandExecuteHandler,
				 NULL ),
	m_pExecuteFunction(i_pExecuteFunction)
{
	DBG_ASSERT(m_pExecuteFunction!=NULL, "Simple command needs execute function pointer. " << i_CommandTagName.c_str());

	this->SetCategory(i_Category);
	this->SetDescription(i_Description);
}
