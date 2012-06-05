/*****************************************************************************
**  fgmtCommands.hpp
**
**      Sets up fragment related commands
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#ifdef FGMT_COMMANDS_HPP
#error fgmtCommands.hpp multiply included
#endif
#define FGMT_COMMANDS_HPP


//============================================================================
//============================================================================
namespace fgmtCommands
{
	//--------------------------------------------------------------------
	// SetupMenu --
	//--------------------------------------------------------------------
	void SetupMenu();

	//--------------------------------------------------------------------
	// EnableMenus -- enable menu items based on selection
	//--------------------------------------------------------------------
	void EnableMenus(bool i_bHaveSurfaceObject, bool i_bHaveSurfacePart);
};
