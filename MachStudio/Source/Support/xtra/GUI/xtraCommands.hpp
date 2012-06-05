/*****************************************************************************
**  xtraCommands.hpp
**
**      Sets up custom property related commands
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifdef XTRA_COMMANDS_HPP
#error xtraCommands.hpp multiply included
#endif
#define XTRA_COMMANDS_HPP


//============================================================================
//============================================================================
namespace xtraCommands
{
	//--------------------------------------------------------------------
	// SetupMenu --
	//--------------------------------------------------------------------
	void SetupMenu();

	//--------------------------------------------------------------------
	// EnableMenus -- enable menu items based on selection
	//--------------------------------------------------------------------
	void EnableMenus(bool i_bHaveObject);
};
