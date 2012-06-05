/*****************************************************************************
**	cmaCommandSimple.hpp
**
**		Utility class for creating a command that calls a function pointer
**	when executed. Simple way to add a hotkey to an existing function.
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef CMA_COMMANDSIMPLE_HPP
#error cmaCommandSimple.hpp multiply included
#endif
#define CMA_COMMANDSIMPLE_HPP

#ifndef CMA_COMMAND_HPP
#include "Tool/cma/cmaCommand.hpp"
#endif


//============================================================================
//============================================================================
class cmaCommandSimple : public cmaCommand
{
	public:
		//--------------------------------------------------------------------
		// This command calls functions with no arguments
		//--------------------------------------------------------------------
		typedef void (*ExecuteFunctionPtr)();

	public:
		//--------------------------------------------------------------------
		// constructor
		//--------------------------------------------------------------------
		cmaCommandSimple(const std::string &i_CommandTagName,
						 const std::string &i_Category,
						 const std::string &i_Description,
						 ExecuteFunctionPtr i_pExecuteFunction);

	public:
		ExecuteFunctionPtr m_pExecuteFunction;
};

