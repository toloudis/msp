/*****************************************************************************
**	prjltCommands.hpp
**
**	Sets up menu buttons for system prjlt
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef PRJLT_COMMANDS_HPP
#error prjltCommands.hpp multiply included
#endif
#define PRJLT_COMMANDS_HPP


//============================================================================
//============================================================================
namespace prjltCommands
{
	//--------------------------------------------------------------------
	//  Add commands specific to system to menu bar and tabs
	//--------------------------------------------------------------------
	void  SetupMenu();
		
	//--------------------------------------------------------------------
	// CleanUp 
	//--------------------------------------------------------------------
	void CleanUp();

}	// end of namespace
