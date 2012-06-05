/*****************************************************************************
**  CommandPrefs.hpp
**
**      command to launch the capture options dialog before the capture mode.
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef _COMMANDPREFS_HPP
#error CommandPrefs.hpp multiply included
#endif
#define _COMMANDPREFS_HPP

#ifndef CMA_COMMAND_HPP
#include "cmaCommand.hpp"
#endif

#include <string>


//============================================================================
//============================================================================
class CommandPrefs : public cmaCommand
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		static const std::string GetConstTagName();

	public:
		//--------------------------------------------------------------------
		// constructor
		//--------------------------------------------------------------------
		CommandPrefs();

		//--------------------------------------------------------------------
		// destructor
		//--------------------------------------------------------------------
		virtual ~CommandPrefs();
};

