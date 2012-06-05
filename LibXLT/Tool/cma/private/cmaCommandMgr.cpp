/*****************************************************************************
**  cmaCommandMgr.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Tool/cma/cmaCommandMgr.hpp"

#include "Core/app/appApplication.hpp"
#include "Core/dbg/dbgMsg.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsXMLReader.hpp"
#include "Core/fs/fsXMLWriter.hpp"
#include "Core/gf/gfPaths.hpp"
#include "Tool/cma/cmaData.hpp"
#include "Tool/gui/guiDialogTabbedMgr.hpp"
#include "Tool/gui/guiMenuMgr.hpp"
#include "Tool/gui/guiMessageBox.hpp"

//	system
#include <algorithm>
#include <string>
#include <vector>


//============================================================================
// Windows API functions and constants
//============================================================================
#define DLLIMPORT __declspec(dllimport) 
//[DllImport("user32", SetLastError=true)]
//static extern int RegisterHotKey (IntPtr hwnd, int id, int fsModifiers, int vk);
//[DllImport("user32", SetLastError=true)]
//static extern int UnregisterHotKey (IntPtr hwnd, int id);
//__declspec( dllimport ) short GlobalAddAtom(std::string* lpString);
//[DllImport("kernel32", SetLastError=true)]
//static extern short GlobalAddAtom (string lpString);
//[DllImport("kernel32", SetLastError=true)]
//static extern short GlobalDeleteAtom (short nAtom);

//const int MOD_ALT = 1;
//const int MOD_CONTROL = 2;
//const int MOD_SHIFT = 4;
//const int MOD_WIN = 8;


//============================================================================
//============================================================================
namespace
{
	MenuCheckFunctionPtr	l_MenuCheckFunction;
	ValidHotKeyFunctionPtr	l_ValidHotKeyFunction;

	const char * lc_Key_HotKeys				= "Shortcuts";
	const char * lc_Key_HotKey_Command		= "Command";
	const char * lc_Key_HotKey_Combo		= "Key Combo";

	const char * lc_HotKeysFileName			= "HotKeys.cfg";
	const char * lc_HotKeysFileName_Default	= "HotKeys-Default.cfg";

	std::vector< std::string >	l_CommandTags;
	std::vector< cmaCommand* >	l_Commands;
	cmaHotKeyDataList			l_HotKeys;

	bool l_bValidHotKey = false;

	//--------------------------------------------------------------------
	//	get_index()
	//--------------------------------------------------------------------
	int get_index( const std::string& i_Command )
	{
		int index = 0;
		std::vector< std::string >::iterator it		= l_CommandTags.begin();
		std::vector< std::string >::iterator end	= l_CommandTags.end();
		while ( it != end )
		{
			if ( strcmp( (*(it)).c_str(), i_Command.c_str() ) == 0 )
			{
				break;
			}

			++it;
			index++;
		}

		if( it == end )
		{
			return -1;
		}

		return 	index;
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	int get_hotkey_index(const std::string& i_Command)
	{
		int num = l_HotKeys.Size();
		for ( int i = 0 ; i < num ; i++ )
		{
			if (l_HotKeys.GetCommandName(i) == i_Command)
			{
				return i;
			}
		}
		return -1;
	}
	

	//--------------------------------------------------------------------
	// Since the command name is about to be used as the key name
	// in an XML file, it is not allowed to have underscores.
	// There has to be a STL or boost way to do this in one line, right?
	//--------------------------------------------------------------------
	void convert_spaces(std::string& io_CommandName)
	{
		std::string::iterator it;
		for (it = io_CommandName.begin(); it != io_CommandName.end(); ++it)
		{
			if (*it == ' ') 
				*it = '_';
		}
	}
	void convert_underscores(std::string& io_CommandName)
	{
		std::string::iterator it;
		for (it = io_CommandName.begin(); it != io_CommandName.end(); ++it)
		{
			if (*it == '_') 
				*it = ' ';
		}
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void ReadHotKeysFromConfigFile(fsLocator& i_ConfigFile)
	{
		std::string cfgpath;
		fsFileUtil::LocatorToANSIFilename(i_ConfigFile,cfgpath);

		fsXMLReader pXMLReader(cfgpath);
		pXMLReader.Open();

		//	read in the preferences
		//
		fsXMLData::fs_Node_Type node_type;
		std::string keyname, strvalue;
		while ((node_type = pXMLReader.ReadNode(keyname,strvalue)) != fsXMLData::e_EOF)
		{
			if ((node_type == fsXMLData::e_Text) && (keyname.length() > 0))
			{
				convert_underscores(keyname);
				cmaCommandMgr::UpdateHotKey(keyname, strvalue);
			}
		}

		pXMLReader.Close();
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void MergeHotKeysFromConfigFile(fsLocator& i_ConfigFile)
	{
		std::string cfgpath;
		fsFileUtil::LocatorToANSIFilename(i_ConfigFile,cfgpath);

		fsXMLReader pXMLReader(cfgpath);
		pXMLReader.Open();

		//	read in the preferences
		//
		fsXMLData::fs_Node_Type node_type;
		std::string keyname, strvalue;
		while ((node_type = pXMLReader.ReadNode(keyname,strvalue)) != fsXMLData::e_EOF)
		{
			if ((node_type == fsXMLData::e_Text) && (keyname.length() > 0))
			{
				const bool cMERGE_IF_NEW = true;
				convert_underscores(keyname);
				cmaCommandMgr::UpdateHotKey(keyname, strvalue, cMERGE_IF_NEW);
			}
		}

		pXMLReader.Close();
	}


	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void WriteHotKeysToConfigFile(fsLocator& i_ConfigFile)
	{
		std::string cfgpath;
		fsFileUtil::LocatorToANSIFilename(i_ConfigFile,cfgpath);

		//	open + write the start element
		fsXMLWriter pXMLWriter(cfgpath);
		pXMLWriter.Open();
		pXMLWriter.WriteStartElement(std::string("HotKeys"));

		// Write out the hot keys
		//
		//System::String^ namestr;
		//System::String^ combo;
		for (int i = 0; i < l_HotKeys.Size(); ++i)
		{
			std::string command_name( l_HotKeys.GetCommandName(i) );
			convert_spaces(command_name);
			pXMLWriter.WriteElement(command_name, l_HotKeys.GetKeyCombo(i));
		}

		//	finish it up
		pXMLWriter.WriteEndElement();
		pXMLWriter.Close();
	}
	//-----------------------------------------
	//Determines if the shortcut is in the invalid set of hotkeys
	//-----------------------------------------
    bool is_invalid_hotkey( const std::string& i_KeyCombo )
	{
		//current set of invalid keys: Ctrl+A, Ctrl+C, Ctrl+V, Ctrl+X
		bool bCtrl;
		char keyValue[30];

		bCtrl = strstr(i_KeyCombo.c_str(), "Ctrl") != NULL;
		if(bCtrl)
		{
			//DBG_LOG("Ctrl Key: " << i_KeyCombo.c_str());
			bool bA, bC, bV, bX;
			
			sscanf( i_KeyCombo.c_str(), "Ctrl+%s", keyValue );
			bA = strcmp(keyValue, "A") == 0;
			bC = strcmp(keyValue, "C") == 0;
			bV = strcmp(keyValue, "V") == 0;
			bX = strcmp(keyValue, "X") == 0;
			
			if( bA || bC || bV || bX )
			{
				return true;
			}
		}
		return false;
	}
	//-----------------------------------------
	//Determines if the shortcut should be added as a shortcut into the menu
	//-----------------------------------------
	bool is_shortcut_worthy( const std::string& i_KeyCombo )
	{
		bool bCtrl, bShift, bAlt, bFunction;
		int keyNumber;
		if( i_KeyCombo == "" )
			return true;
		
		//DBG_LOG("Hot Key : " << i_KeyCombo.c_str());
		
		bCtrl = strstr(i_KeyCombo.c_str(), "Ctrl") != NULL;
		bShift = strstr(i_KeyCombo.c_str(), "Shift") != NULL;
		bAlt = strstr(i_KeyCombo.c_str(), "Alt") != NULL;
		bFunction = strstr(i_KeyCombo.c_str(), "F") != NULL;

		if(bCtrl)
		{
			//DBG_LOG("Ctrl Key: " << i_KeyCombo.c_str());		
			return true;
		}
		else if( bAlt )
			return true;

		else if( bShift )
			return false;
		
		else if(bFunction)
		{
			keyNumber = -1;
			sscanf( i_KeyCombo.c_str(), "F%d", &keyNumber );
			
			//DBG_LOG("Hot Key3 : " << keyNumber );
			
			if( keyNumber != -1 )
				return true;
		}

		return false;
	}
}


//
// 	cmaCommandMgr
//

//--------------------------------------------------------------------
// Initialize
//--------------------------------------------------------------------
void cmaCommandMgr::Initialize()
{
	//l_Commands.clear();
	envSTLHelpers::DeleteContainer(l_Commands);
	l_CommandTags.clear();
	l_HotKeys.Clear();

	// Setup idle processing
	//CommandMgrInfo::m_EventHandler = new EventHandler( this, cmaCommandMgr::OnIdle )
	//Application::Idle += CommandMgrInfo::m_EventHandler;
}

//--------------------------------------------------------------------
// DeInitialize
//--------------------------------------------------------------------
void cmaCommandMgr::DeInitialize()
{
	//l_Commands.clear();
	envSTLHelpers::DeleteContainer(l_Commands);
	l_CommandTags.clear();
	l_HotKeys.Clear();

	//Application::Idle -= CommandMgrInfo::m_EventHandler;
}

//--------------------------------------------------------------------
//	Add() - add the command to the list
//--------------------------------------------------------------------
void cmaCommandMgr::Add(cmaCommand* i_pCmd, 
						const std::string& i_Tag, 
						int i_ObjectID)
{
	DBG_ASSERT( i_pCmd != NULL, "Command cannot be NULL" );

	i_pCmd->SetTag( i_Tag );
	i_pCmd->SetObjectID( i_ObjectID );

	// add the command
	//
	l_Commands.push_back( i_pCmd );

	//	add the tag for faster searching
	l_CommandTags.push_back( i_pCmd->GetTag() );

	//	add the hotkey.  if it already exists update the key combo
	//
	int index = l_HotKeys.AddHotKey( i_pCmd->GetTag() );
	if (index != -1)
	{
		if ((i_pCmd->GetHotKey().GetValue().size() == 0))
		{
			//DBG_LOG2("menu item %s assigned hot key (%s)", i_pCmd->GetTag().c_str(), l_HotKeys.GetKeyCombo(index).c_str());

			i_pCmd->GetHotKey().SetValue(l_HotKeys.GetKeyCombo(index));
		}
		else
		{
			l_HotKeys.SetKeyCombo(index, i_pCmd->GetHotKey().GetValue());
		}
	}
}

//--------------------------------------------------------------------
//	RegisterObjectForMenu - register a GUI object with a particular command.
//--------------------------------------------------------------------
//void cmaCommandMgr::RegisterObjectForMenu( const std::string& i_Tag, int i_ObjectID )
//{
//	//DBG_LOG( "registering command (" << i_Tag.c_str() << ")" );
//
//	// -1 means no object id, skip out without trying
//	if (i_ObjectID == -1)
//		return;
//
//	// find the object from the MenuMgr to verify it exists.
//	//
//	if (!guiMenuMgr::MenuObjectsExist( i_ObjectID ))
//	{
//		DBG_ASSERT( false, "Could not find menu item to register (%d)", i_ObjectID );
//		return;
//	}
//
//	//
//	guiMenuMgr::AttachEventToMenuObjects( i_ObjectID, cma_control_Click, cma_control_Update);
//}

//--------------------------------------------------------------------
//	CommandSetEnabled - set a command
//--------------------------------------------------------------------
void cmaCommandMgr::CommandSetEnabled( cmaCommand* i_pCmd, bool i_bEnabled )
{
	DBG_ASSERT( i_pCmd != NULL, "Command is NULL, cannot set it" );

	i_pCmd->SetEnabled( i_bEnabled );
	guiMenuMgr::MenuObjectsEnable( i_pCmd->GetObjectID(), i_bEnabled );
}

//--------------------------------------------------------------------
//	CommandSetChecked - set a command
//--------------------------------------------------------------------
void cmaCommandMgr::CommandSetChecked( cmaCommand* i_pCmd, bool i_bChecked )
{
	DBG_ASSERT( i_pCmd != NULL, "Command is NULL, cannot set it" );

	i_pCmd->SetChecked( i_bChecked );

	if (l_MenuCheckFunction)
		l_MenuCheckFunction(i_pCmd->GetObjectID(), i_bChecked);
}

//--------------------------------------------------------------------
//	GetCommandFromObjectID() - get a command based on an objectID
//--------------------------------------------------------------------
cmaCommand* cmaCommandMgr::GetCommandFromObjectID( int i_ObjectID )
{
	int num = l_Commands.size();
	int i;
	for ( i = 0 ; i < num ; i++ )
	{
		//DBG_LOG4( "%02d) %02d vs %02d - %s", i, i_ObjectID, l_Commands[i]->GetObjectID(), l_Commands[i]->GetTag().c_str() );

		if ( i_ObjectID == l_Commands[i]->GetObjectID() )
		{
			//DBG_LOG( "====" );
			return l_Commands[i];
		}
	}

	//DBG_LOG( "----" );
	return NULL;
}


//--------------------------------------------------------------------
//	Execute a command based on the name
//--------------------------------------------------------------------
void cmaCommandMgr::ExecuteCommand( const std::string& i_Tag, int i_ObjectID )
{
	//DBG_LOG("Command Manager-------------EXECUTE");
	int num = l_Commands.size();
	for ( int i = 0 ; i < num ; i++ )
	{
		//DBG_LOG5( "%02d) %s vs %s - %03d vs %03d", i, i_Tag.c_str(), l_Commands[i]->GetTag().c_str(), i_ObjectID, l_Commands[i]->GetObjectID() );

		cmaCommand* pCommand = l_Commands[i];

		if (i_ObjectID == -1)
		{
			//	based on object tag
			if (pCommand->GetTag() == i_Tag)
			{
				pCommand->Execute();
				return;
			}
		}
		else
		{
			//	match by ID
			if (pCommand->GetObjectID() == i_ObjectID)
			{
				pCommand->Execute();
				return;
			}
		}
	}
}

//--------------------------------------------------------------------
//	Search through the hotkeys and if there is a match, execute it.
//--------------------------------------------------------------------
bool cmaCommandMgr::ExecuteCommand(const std::string& i_KeyCombo)
{
	//DBG_LOG("Command Manager-------------EXECUTE");
	int num = l_Commands.size();
	int i;
	for ( i = 0 ; i < num ; i++ )
	{
		//DBG_LOG4( "%d) %s vs %s - %s ", i, i_KeyCombo.c_str(), l_Commands[i]->GetHotKeyString().c_str(), l_Commands[i]->GetTag().c_str() );

		if (l_Commands[i]->GetHotKeyString() == i_KeyCombo)
		{
			l_Commands[i]->Execute();
			return true;
		}
	}
	return false;
}

//--------------------------------------------------------------------
//
//--------------------------------------------------------------------
void cmaCommandMgr::GetCommandPropertyUIInfoList( prtyPropertyUIInfoContainer& io_PropertyList )
{
	int num = l_Commands.size();
	for ( int i = 0 ; i < num ; i++ )
	{
		//	if the hotkey isn't enabled, don't return it in the list because this
		//	command is NOT supposed to be displayed
		//
		if (l_Commands[i]->GetEnableHotKey())
			io_PropertyList.Add( l_Commands[i]->GetList() );
	}
}

//--------------------------------------------------------------------
//	Get const list of commands available
//--------------------------------------------------------------------
const std::vector< cmaCommand* >& cmaCommandMgr::GetCommandList()
{
	return l_Commands;
}

//--------------------------------------------------------------------
//	Register the key combination for use
//--------------------------------------------------------------------
void cmaCommandMgr::RegisterHotKeyCombo(const std::string& i_Command, const std::string& i_KeyCombo)
{
	//	update the HotKey
	int hindex = get_hotkey_index( i_Command );

	if (hindex < 0)
	{
		DBG_LOG("Could not find command " << i_Command << " for hotkey " << i_KeyCombo.c_str() );
		return; // ignore an old or invalid hotkey 
	}
	
	l_HotKeys.SetKeyCombo(hindex, i_KeyCombo);
	
	//	update the Command + Menu Item
	int cindex = get_index( i_Command );
	if (cindex < 0) return; // ignore an old or invalid hotkey 

	int menu_id = l_Commands[cindex]->GetObjectID();	 

	//check if the hotkey meets the criteria to be a shortcut, else add no shortcut string
	//handles case if a hotkey is changed into a non shortcut key by setting the shortcut to an empty string
	if(is_shortcut_worthy( i_KeyCombo ))
		guiMenuMgr::SetMenuItemShortcut(menu_id, i_KeyCombo.c_str());
	else
		guiMenuMgr::SetMenuItemShortcut(menu_id, "");

	//DBG_ASSERT(cindex >= 0, "Invalid hotkey index (%s)", i_Command.c_str());
	l_Commands[cindex]->GetHotKey().SetValue(i_KeyCombo);
}

//--------------------------------------------------------------------
//	UnRegister the key combination for use
//--------------------------------------------------------------------
void cmaCommandMgr::UnRegisterHotKeyCombo(const std::string& i_Command)
{
	//	update the HotKey
	int hindex = get_hotkey_index( i_Command );

	if (hindex < 0) return; // ignore an old or invalid hotkey 
	//DBG_ASSERT(hindex >= 0, "Invalid hotkey index (%s)", i_Command.c_str());

	l_HotKeys.ClearKeyCombo(hindex);
}

//--------------------------------------------------------------------
//	Update the HotKey
//--------------------------------------------------------------------
void cmaCommandMgr::UpdateHotKey(const std::string& i_Command, 
								 const std::string& i_KeyCombo, 
								 bool i_bAddOnlyIfNew)
{
	//first check if it is a key in our invalid list, ctrl+A, ctrl+C, ctrl+X, ctrl+V
	/*
	if( is_invalid_hotkey( i_KeyCombo ) )
	{
		std::string invalidMsg = "Hotkey : " + i_KeyCombo + " cannot be used.";
		guiMessageBox::Show(invalidMsg.c_str(), "Invalid Hotkey", guiMessageBox::e_OKOnly);
		return;
	}
	*/

	l_bValidHotKey = false;
	
	//Check if the keycombo is previously used by another function
	std::string usedCommand = l_HotKeys.isUsedKeyCombo(i_Command, i_KeyCombo);
	if(usedCommand != "")
	{
		//if so, pop up a message box and don't register the new hot key
		std::string errorMsg = "Hotkey : " + i_KeyCombo + " for Command: " + i_Command + "\nis already in use by Command: " + usedCommand;
		guiMessageBox::Show(errorMsg.c_str(), "Hotkey in Use", guiMessageBox::e_OKOnly);
		l_bValidHotKey = false;
		return;
	}

	l_bValidHotKey = true;
	//	check if it is a valid combo (or empty). If not, bail
	//
	if (i_KeyCombo.size() > 0) 
	{
		if (   (l_ValidHotKeyFunction)
			&& !(l_ValidHotKeyFunction(i_KeyCombo.c_str())))
			return;
	}

	//	find the command
	//
	bool exists = false;
	int hkindex = get_hotkey_index(i_Command);
	if (   (hkindex != -1)
		&& (l_HotKeys.GetKeyCombo(hkindex).size() > 0))
	{
		exists = true;
	}

	//	unregister the old combo
	//
	if (!i_bAddOnlyIfNew && exists)
	{
		UnRegisterHotKeyCombo(i_Command);
		
		//since the hotkey was unregistered, lets make sure 
		//the hotkey shortcut is removed.
		if(is_shortcut_worthy( i_KeyCombo ))
		{
			int cindex = get_index( i_Command );
			int menu_id = l_Commands[cindex]->GetObjectID();		
			guiMenuMgr::SetMenuItemShortcut(menu_id, "");
		}
	}

	//	register the new one
	//
	if (   (!i_bAddOnlyIfNew)
		|| (i_bAddOnlyIfNew && !exists))
	{
		RegisterHotKeyCombo(i_Command, i_KeyCombo);

	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
bool cmaCommandMgr::IsValidHotKey()
{
	return l_bValidHotKey;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cmaCommandMgr::ResetValidHotKey()
{
	l_bValidHotKey = false;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cmaCommandMgr::ReadHotKeys()
{
	fsLocator cfgdir;
	cfgdir = gfPaths::GetPath(gfPaths::e_UserDataPath);
	cfgdir.Push("Configs");
	cfgdir.Push(lc_HotKeysFileName);

	//	see if the hot key config file exists
	if (fsFileUtil::FileExists(cfgdir))
	{
		ReadHotKeysFromConfigFile(cfgdir);

		// got the user hot key config, read the default and see if there are any new ones to merge
		//
		//	TODO - do this based on date/time instead so it doesn't have to go through all the entries
		//
		//cfgdir = gfPaths::GetPath(gfPaths::e_ExePath);
		//cfgdir.Push("Data");		// HACK - hard-coded path. Better: set path as cmgr func
		//cfgdir.Push("Configs");
		//cfgdir.Push( lc_HotKeysFileName_Default );
		//if (fsFileUtil::FileExists(cfgdir))
		//{
		//	MergeHotKeysFromConfigFile(cfgdir);
		//}
	}
	else
	{
		// no hot key config, so check for default
		cfgdir = gfPaths::GetPath(gfPaths::e_ExePath);
		cfgdir.Push("Data");		// HACK - hard-coded path. Better: set path as cmgr func
		cfgdir.Push("Configs");
		cfgdir.Push( lc_HotKeysFileName_Default );
		if (fsFileUtil::FileExists(cfgdir))
		{
			ReadHotKeysFromConfigFile(cfgdir);
		}
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cmaCommandMgr::WriteHotKeys()
{
	fsLocator cfgdir;
	cfgdir = gfPaths::GetPath(gfPaths::e_UserDataPath);
	cfgdir.Push("Configs");
	cfgdir.Push(lc_HotKeysFileName);

	WriteHotKeysToConfigFile(cfgdir);
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cmaCommandMgr::SetMenuCheckedFunction( MenuCheckFunctionPtr i_pMenuCheckFunction )
{
	l_MenuCheckFunction = i_pMenuCheckFunction;
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cmaCommandMgr::SetValidHotKeyFunction( ValidHotKeyFunctionPtr i_pValidHotKeyFunction )
{
	l_ValidHotKeyFunction = i_pValidHotKeyFunction;
}
