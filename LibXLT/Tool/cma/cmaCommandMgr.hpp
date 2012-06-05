/*****************************************************************************
**	cmaCommandMgr.hpp
**
**		command manager
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef CMA_COMMANDMGR_HPP
#error cmaCommandMgr.hpp multiply included
#endif
#define CMA_COMMANDMGR_HPP

#ifndef CMA_COMMAND_HPP
#include "Tool/cma/cmaCommand.hpp"
#endif
#ifndef PRTY_PROPERTYUIINFOCONTAINER_HPP
#include "Core/prty/prtyPropertyUIInfoContainer.hpp"
#endif

#include <string>


//============================================================================
//============================================================================
namespace
{
	//--------------------------------------------------------------------
	//	This namespace can set a function that allows menu items to be 
	//	checked.
	//--------------------------------------------------------------------
	typedef void (*MenuCheckFunctionPtr)(int i_ObjectID, bool i_bCheckd);

	//--------------------------------------------------------------------
	//	This namespace can set a function that allows a shortcut to be
	//	checked for being valid.
	//--------------------------------------------------------------------
	typedef bool (*ValidHotKeyFunctionPtr)(const char * i_ShortcutString);
}


//============================================================================
//	Forward References
//============================================================================


//============================================================================
//============================================================================
namespace cmaCommandMgr
{
	//--------------------------------------------------------------------
	// Initialize
	//--------------------------------------------------------------------
	void Initialize();

	//--------------------------------------------------------------------
	// DeInitialize
	//--------------------------------------------------------------------
	void DeInitialize();

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
			 int i_ObjectID);

	//--------------------------------------------------------------------
	//	RegisterObjectForMenu - register a GUI object with a particular command.
	//--------------------------------------------------------------------
	//void RegisterObjectForMenu( const std::string& i_Tag, int i_ObjectID );

	//--------------------------------------------------------------------
	//	CommandSetEnabled - set a command (and all of it's objects)
	//
	//	i_ObjectID if set to -1 it affects all Object IDs
	//--------------------------------------------------------------------
	void CommandSetEnabled( cmaCommand* i_pCmd, bool i_bEnabled );

	//--------------------------------------------------------------------
	//	CommandSetChecked - set a command (and all of it's objects)
	//
	//	i_ObjectID if set to -1 it affects all Object IDs
	//--------------------------------------------------------------------
	void CommandSetChecked( cmaCommand* i_pCmd, bool i_bChecked );

	//--------------------------------------------------------------------
	//	GetCommandFromObjectID() - get a command based on an objectID
	//--------------------------------------------------------------------
	cmaCommand* GetCommandFromObjectID( int i_ObjectID );

	//--------------------------------------------------------------------
	//	Execute a command based on the name
	//--------------------------------------------------------------------
	void ExecuteCommand( const std::string& i_Tag, int i_ObjectID );

	//--------------------------------------------------------------------
	//	Search through the hotkeys and if there is a match, execute it.
	//--------------------------------------------------------------------
	bool ExecuteCommand(const std::string& i_KeyCombo);

	//--------------------------------------------------------------------
	//	Build one list of all of the command propertyUIInfos
	//--------------------------------------------------------------------
	void GetCommandPropertyUIInfoList( prtyPropertyUIInfoContainer& io_PropertyList );

	//--------------------------------------------------------------------
	//	Get const list of commands available
	//--------------------------------------------------------------------
	const std::vector< cmaCommand* >& GetCommandList();

	//--------------------------------------------------------------------
	//	Register the key combination for use
	//--------------------------------------------------------------------
	void RegisterHotKeyCombo(const std::string& i_Command, const std::string& i_KeyCombo);

	//--------------------------------------------------------------------
	//	UnRegister the key combination for use
	//--------------------------------------------------------------------
	void UnRegisterHotKeyCombo(const std::string& i_Command);

	//--------------------------------------------------------------------
	//	Update the HotKey
	//--------------------------------------------------------------------
	void UpdateHotKey(const std::string& i_Command, const std::string& i_KeyCombo, bool i_bAddOnlyIfNew = false );

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	bool IsValidHotKey();
	
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void ResetValidHotKey();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void ReadHotKeys();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void WriteHotKeys();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void SetMenuCheckedFunction( MenuCheckFunctionPtr i_pMenuCheckFunction );

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void SetValidHotKeyFunction( ValidHotKeyFunctionPtr i_pValidHotKeyFunction );


};

