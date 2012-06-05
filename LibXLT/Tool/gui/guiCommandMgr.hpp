/*****************************************************************************
**	guiCommandMgr.hpp
**
**		command manager
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef GUI_COMMANDMGR_HPP
#error guiCommandMgr.hpp multiply included
#endif
#define GUI_COMMANDMGR_HPP

#ifndef CMA_COMMAND_HPP
#include "Tool/cma/cmaCommand.hpp"
#endif

#include <string>


//============================================================================
//============================================================================
namespace guiCommandMgr
{
	//--------------------------------------------------------------------
	//	Add() - add a command to the system.  The object ID is the menu ID
	//	if there is a menu item for this command.  Otherwise, any number
	//	(not used in this app) can be chosen.
	//
	//	TODO - resolve the "no menu item, but objectID needed" dilemma
	//	instead of using i_bRegisterInMenu flag
	//--------------------------------------------------------------------
	void Add(cmaCommand* i_pCmd, 
			 const std::string& i_Tag, 
			 int i_ObjectID,
			 bool i_bRegisterInMenu = true);

	//--------------------------------------------------------------------
	//	RegisterObjectForMenu - register a GUI object with a particular command.
	//--------------------------------------------------------------------
	void RegisterObjectForMenu( const std::string& i_Tag, int i_ObjectID );
};

