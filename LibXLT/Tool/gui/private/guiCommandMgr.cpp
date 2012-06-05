/*****************************************************************************
**	guiCommandMgr.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Tool/gui/guiCommandMgr.hpp"

#include "Tool/cma/cmaCommandMgr.hpp"
#include "Tool/gui/guiMenuMgr.hpp"

//	system
#include <string>


//============================================================================
// Windows API functions and constants
//============================================================================
#define DLLIMPORT __declspec(dllimport) 


//============================================================================
//============================================================================
namespace
{
	//--------------------------------------------------------------------
	// the callback function for menu items in cma
	//--------------------------------------------------------------------
	void cma_control_Click( int i_ObjectID )
	{
		cmaCommand* cmd = cmaCommandMgr::GetCommandFromObjectID( i_ObjectID );
		cmd->Execute();
	}
	void cma_control_Update( int i_ObjectID )
	{
		cmaCommand* cmd = cmaCommandMgr::GetCommandFromObjectID( i_ObjectID );
		cmd->ProcessUpdates();
	}

}


//
// 	guiCommandMgr
//

//--------------------------------------------------------------------
//	Add() - do the Add() and Register()
//--------------------------------------------------------------------
void guiCommandMgr::Add(cmaCommand* i_pCmd, 
						const std::string& i_Tag, 
						int i_ObjectID,
						bool i_bRegisterInMenu )
{
	DBG_ASSERT( i_pCmd != NULL, "Command cannot be NULL" );

	//	set the tag of the command based on the menu item...if it exists
	//
	std::string tagstr(i_Tag);
	std::string new_cmd_tag = guiMenuMgr::GetMenuItemName(i_ObjectID);
	if (!new_cmd_tag.empty())
	{
		tagstr = new_cmd_tag;
	}

	cmaCommandMgr::Add(i_pCmd, tagstr, i_ObjectID);

	if (i_bRegisterInMenu)
	{
		RegisterObjectForMenu( tagstr, i_ObjectID );

		// If there is a description for this command, set this as the
		// help string for the menu item
		if (!i_pCmd->GetDescription().empty())
		{
			guiMenuMgr::SetMenuItemHelpString( i_ObjectID, i_pCmd->GetDescription().c_str());
		}
	}
}

//--------------------------------------------------------------------
//	RegisterObjectForMenu - register a GUI object with a particular command.
//--------------------------------------------------------------------
void guiCommandMgr::RegisterObjectForMenu( const std::string& i_Tag, int i_ObjectID )
{
	//DBG_LOG( "registering command (" << i_Tag.c_str() << ")" );

	// -1 means no object id, skip out without trying
	if (i_ObjectID == -1)
		return;

	// find the object from the MenuMgr to verify it exists.
	//
	if (!guiMenuMgr::MenuObjectsExist( i_ObjectID ))
	{
		DBG_ASSERT( false, "Could not find menu item to register - " << i_ObjectID );
		return;
	}

	//
	guiMenuMgr::AttachEventToMenuObjects( i_ObjectID, cma_control_Click, cma_control_Update);
}

