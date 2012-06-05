/*****************************************************************************
**  dirltCommandIconsVisible.hpp
**
**      command to toggle the icons of a system
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef DIRLT_COMMANDICONSVISIBLE_HPP
#error dirltCommandIconsVisible.hpp multiply included
#endif
#define DIRLT_COMMANDICONSVISIBLE_HPP

#ifndef CMA_COMMAND_HPP
#include "cmaCommand.hpp"
#endif

#include <string>


//============================================================================
//============================================================================
class dirltCommandIconsVisible : public cmaCommand
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		static const std::string GetConstTagName();

	public:
		//--------------------------------------------------------------------
		// constructor
		//--------------------------------------------------------------------
		dirltCommandIconsVisible();

		//--------------------------------------------------------------------
		// destructor
		//--------------------------------------------------------------------
		virtual ~dirltCommandIconsVisible();
};
