/*****************************************************************************
**  cmraCommandMessageBox.hpp
**
**      command to put up a message box
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef CMRA_COMMANDMESSAGEBOX_HPP
#error cmraCommandMessageBox.hpp multiply included
#endif
#define CMRA_COMMANDMESSAGEBOX_HPP

#ifndef CMA_COMMAND_HPP
#include "Tool/cma/cmaCommand.hpp"
#endif

#include <string>


//============================================================================
//============================================================================
class cmraCommandMessageBox : public cmaCommand
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		static const std::string GetConstTagName();

	public:
		//--------------------------------------------------------------------
		// constructor
		//--------------------------------------------------------------------
		cmraCommandMessageBox();

		//--------------------------------------------------------------------
		// destructor
		//--------------------------------------------------------------------
		virtual ~cmraCommandMessageBox();
};
