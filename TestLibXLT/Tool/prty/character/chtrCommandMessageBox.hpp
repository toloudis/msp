/*****************************************************************************
**  chtrCommandMessageBox.hpp
**
**      command to put up a message box
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef CHTR_COMMANDMESSAGEBOX_HPP
#error chtrCommandMessageBox.hpp multiply included
#endif
#define CHTR_COMMANDMESSAGEBOX_HPP

#ifndef CMA_COMMAND_HPP
#include "Tool/cma/cmaCommand.hpp"
#endif

#include <string>


//============================================================================
//============================================================================
class chtrCommandMessageBox : public cmaCommand
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		static const std::string GetConstTagName();

	public:
		//--------------------------------------------------------------------
		// constructor
		//--------------------------------------------------------------------
		chtrCommandMessageBox();

		//--------------------------------------------------------------------
		// destructor
		//--------------------------------------------------------------------
		virtual ~chtrCommandMessageBox();
};
