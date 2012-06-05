/*****************************************************************************
**  mtrlCommands.hpp
**
**      Sets up material related commands
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#ifdef MTRL_COMMANDS_HPP
#error mtrlCommands.hpp multiply included
#endif
#define MTRL_COMMANDS_HPP


//============================================================================
//============================================================================
namespace mtrlCommands
{
	//--------------------------------------------------------------------
	// SetupMenu --
	//--------------------------------------------------------------------
	void SetupMenu();

	//--------------------------------------------------------------------
	// EnableMenus -- enable menu items based on selection
	//--------------------------------------------------------------------
	void EnableMenus(bool i_bHaveMaterialObject, 
					 bool i_bHaveMaterials,
					 bool i_bHaveMaterialPart,
					 bool i_bMaterialsLocked);
};
