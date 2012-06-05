/*****************************************************************************
**	cmraCommands.hpp
**
**	Sets up menu buttons for system cmra
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/


#ifdef CMRA_COMMANDS_HPP
#error cmraCommands.hpp multiply included
#endif
#define CMRA_COMMANDS_HPP


namespace cmraCommands
{

	//--------------------------------------------------------------------
	//  Add commands specific to system to menu bar
	//--------------------------------------------------------------------
	void  SetupMenu();

	//--------------------------------------------------------------------
	// CleanUp 
	//--------------------------------------------------------------------
	void CleanUp();

}	// end of namespace
