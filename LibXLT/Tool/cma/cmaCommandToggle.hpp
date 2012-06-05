/*****************************************************************************
**	cmaCommandToggle.hpp
**
**		Utility class for creating a command that toggles a boolean flag.
**	Sets the checked state of the menu item based on the state of that flag. 
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef CMA_COMMANDTOGGLE_HPP
#error cmaCommandToggle.hpp multiply included
#endif
#define CMA_COMMANDTOGGLE_HPP

#ifndef CMA_COMMAND_HPP
#include "Tool/cma/cmaCommand.hpp"
#endif


//============================================================================
//============================================================================
class cmaCommandToggle : public cmaCommand
{
	public:
		//--------------------------------------------------------------------
		// This command needs two functions,
		//  1 - sets the boolean flag state
		//	2 - gets the boolean flag state
		//--------------------------------------------------------------------
		typedef void (*SetFunctionPtr)(bool);
		typedef bool (*GetFunctionPtr)();

	public:
		//--------------------------------------------------------------------
		// constructor
		//--------------------------------------------------------------------
		cmaCommandToggle(const std::string &i_CommandTagName,
						 const std::string &i_Category,
						 const std::string &i_Description,
						 SetFunctionPtr i_pSetFunction,
						 GetFunctionPtr i_pGetFunction);

	public:
		SetFunctionPtr m_pSetFunction;
		GetFunctionPtr m_pGetFunction;
};

