/*****************************************************************************
**  twxMenuMgr.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "ToolUIWx/twx/twxMenuMgr.hpp"

#include "ToolUIWx/twx/twxSystem.hpp"
#include "ToolUIWx/twx/twxToolbarMgr.hpp"

#include "Core/Env/envExceptionX.hpp"
#include "Tool/gui/guiMessageBox.hpp"
#include "Tool/gui/guiStatusBarMgr.hpp"


#ifdef USE_WXWIDGETS

//============================================================================
//============================================================================
namespace
{
	// Not sure what a safe id number to start with is
	int l_MenuCounter = 300;

	//-----------------------------------------------------------------------
	// Find menu item within the given menu
	//-----------------------------------------------------------------------
	wxMenuItem* find_menu_item(wxMenu* i_pParent, const wxString& i_MenuName)
	{
		int index = i_pParent->FindItem(i_MenuName);
		if (index != wxNOT_FOUND)
		{
			//DBG_LOG("Found menu item index: " << index);
			wxMenuItem* pMenuItem = i_pParent->FindItem(index);
			if (pMenuItem)
			{
				return pMenuItem;
			}
		}
		return NULL;
	}

	//-----------------------------------------------------------------------
	// Find menu somewhere within the given menu
	//-----------------------------------------------------------------------
	wxMenu* find_menu(wxMenu* i_pParent, const wxString& i_MenuName)
	{
		// we actually want to find the last occurrence of the menu,
		// not the first. This helps sort out issues when two submenus
		// has the same name.
		wxMenu *ret_val = NULL;

		int index = i_pParent->FindItem(i_MenuName);
		if (index != wxNOT_FOUND)
		{
			//DBG_LOG("Found menu item index: " << index);
			wxMenuItem* pMenuItem = i_pParent->FindItem(index);
			if (pMenuItem && pMenuItem->GetSubMenu())
			{
				ret_val = pMenuItem->GetSubMenu();
			}
		}

		// Look through submenus
		wxMenuItemList &list = i_pParent->GetMenuItems(); 
		wxMenuItemList::iterator iter;
		for (iter = list.begin(); iter != list.end(); ++iter)
		{
			if ((*iter)->GetSubMenu())
			{
				wxMenu* pMenu = find_menu((*iter)->GetSubMenu(), i_MenuName);
				if (pMenu)
				{
					ret_val = pMenu;
				}
			}
		}
		return ret_val;
	}
	//-----------------------------------------------------------------------
	// Find menu with given name within the menu bar and its submenus
	//-----------------------------------------------------------------------
	wxMenu* find_menu(wxMenuBar* i_pParent, const wxString& i_MenuName)
	{
		int index = i_pParent->FindMenu(i_MenuName);
		if (index != wxNOT_FOUND)
		{
			wxMenu* pMenu = i_pParent->GetMenu(index);
			return pMenu;
		}
		for (int i=0; i<i_pParent->GetMenuCount(); i++)
		{
			wxMenu* pMenu = find_menu(i_pParent->GetMenu(i), i_MenuName);
			if (pMenu)
				return pMenu;
		}
		return NULL;
	}

}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
twxMenuMgr::twxMenuMgr()
{
	DBG_ASSERT( twxSystem::g_pMainMenu != NULL, "twxMenuMgr needs a main frame in order to init." );

	// Add ourself as an event handler for menu events on the main form
	twxSystem::g_pMainForm->PushEventHandler(this);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
twxMenuMgr::~twxMenuMgr()
{
}

//---------------------------------------------------------------------------
//	AddMenu()
//		add a menu or submenu to the Menu Tree.
//		set the childname to "" is adding a parent item.
//---------------------------------------------------------------------------
void twxMenuMgr::AddMenu( const char * i_ParentName,
						  const char * i_ChildName )
{
	DBG_ASSERT( twxSystem::g_pMainMenu != NULL, "twxMenuMgr not initialized" );

	wxString parent_name(i_ParentName, wxConvUTF8);

	// If child name is empty, create top-level menu
	if (!i_ChildName || !(*i_ChildName))
	{
		int index = twxSystem::g_pMainMenu->FindMenu(parent_name);
		if (index == wxNOT_FOUND)
		{
			wxMenu *pNewMenu = new wxMenu;
			twxSystem::g_pMainMenu->Append(pNewMenu, parent_name);
			return;
		}
	}	
	else
	{
		wxString child_name(i_ChildName, wxConvUTF8);
		wxMenu* pMenu = find_menu(twxSystem::g_pMainMenu, parent_name);
		if (pMenu)
		{
			// Need to make sure the same menu isn't added more than once
			if (!find_menu_item(pMenu, child_name))
			{
				wxMenu *pSubMenu = new wxMenu();
				pMenu->AppendSubMenu(pSubMenu, child_name);
			}
			return;
		}
	}
	DBG_LOG("Could not add menu " << i_ChildName << ", parent=" << i_ParentName);
}

//---------------------------------------------------------------------------
//	AddMenuItem()
//		add a menu item to the Menu Tree.
//		set the toolbar name to "" to not add a toolbar button
//	the function returns a unique ObjectID for the GUI object.
//	
//---------------------------------------------------------------------------
int twxMenuMgr::AddMenuItem( const char * i_ParentName,
							const char * i_ChildName,
							const char * i_ToolbarName,
							const char * i_ToolbarButton_ImageFilename,
							bool i_bCheckable )
{
	DBG_ASSERT( twxSystem::g_pMainMenu != NULL, "twxMenuMgr not initialized" );
	DBG_ASSERT( (i_ChildName && (*i_ChildName)), "MenuItems need to include a child name.");

	//DBG_LOG2("AddMenuItem: parent %s child %s", i_ParentName, i_ChildName);

	wxString parent_name(i_ParentName, wxConvUTF8);
	wxMenu* pMenu = find_menu(twxSystem::g_pMainMenu, parent_name);
	if (pMenu)
	{
		int id = -1;

		// See if the menu item was already created (when laying out menus earlier)
		wxString child_name(i_ChildName, wxConvUTF8);
		wxMenuItem *pItem = find_menu_item(pMenu, child_name);
		if (pItem)
		{
			id = pItem->GetId();
		}
		else
		{
			id = l_MenuCounter++;
			pItem = (i_bCheckable) ? pMenu->AppendCheckItem(id, child_name) 
											: pMenu->Append(id, child_name);
		}

		sMenuItemInfo menu_info = { pItem, NULL, NULL };
		m_MenuItemMap[id] = menu_info;

		// Connect our callback to the menu item we just created
		this->Connect( id,
					wxEVT_COMMAND_MENU_SELECTED,
					wxCommandEventHandler(twxMenuMgr::menuItem_Click) );
		this->Connect( id,
					wxEVT_UPDATE_UI,
					wxUpdateUIEventHandler(twxMenuMgr::menuItem_Update) );

		if (i_ToolbarName && i_ToolbarButton_ImageFilename)
		{
			twxToolbarMgr::AddToolBarButton(id, 
											i_ToolbarName, 
											i_ToolbarButton_ImageFilename,
											i_ChildName,
											i_bCheckable);
		}

		return id;
	}

	DBG_LOG("Could not add menu item " << i_ChildName << ", parent=" << i_ParentName);
	return -1;
}

//---------------------------------------------------------------------------
//	RemoveMenuItem()
//		remove a menu item from the Menu Tree
//---------------------------------------------------------------------------
void twxMenuMgr::RemoveMenuItem( const char * i_ParentName,
					const char * i_ChildName  )
{
	DBG_LOG("RemoveMenuItem: parent " << i_ParentName <<" child " << i_ChildName);
}

//---------------------------------------------------------------------------
//	FindMenuItem()
//		returns whether or not the menu item can be found in the menu system
//---------------------------------------------------------------------------
bool twxMenuMgr::FindMenuItem( const char * i_ParentName,
							   const char * i_ChildName  )
{
	wxString parent_name(i_ParentName, wxConvUTF8);
	wxMenu* pMenu = find_menu(twxSystem::g_pMainMenu, parent_name);
	if (pMenu)
	{
		// See if the menu item was already created (when laying out menus earlier)
		wxString child_name(i_ChildName, wxConvUTF8);
		if ( find_menu_item(pMenu, child_name) || (!i_ChildName || !(*i_ChildName)) )
		{
			return true;
		}
	}
	return false;
}

//---------------------------------------------------------------------------
//	AddSeparator()
//		add a separator to the menu with the given name.
//---------------------------------------------------------------------------
void twxMenuMgr::AddSeparator( const char * i_MenuName )
{
	DBG_ASSERT( twxSystem::g_pMainMenu != NULL, "twxMenuMgr not initialized" );

	wxString menu_name(i_MenuName, wxConvUTF8);
	wxMenu* pMenu = find_menu(twxSystem::g_pMainMenu, menu_name);
	if (pMenu)
	{
		pMenu->AppendSeparator();
		return;
	}
	DBG_LOG("Could not add separator, menu="<< i_MenuName);
}


//---------------------------------------------------------------------------
//	AttachEventToMenuObjects()
//---------------------------------------------------------------------------
void twxMenuMgr::AttachEventToMenuObjects( int i_ObjectID, 
										   ControlCallback i_pFunction, 
										   ControlCallback i_pUpdate )
{
	std::map<int, sMenuItemInfo>::iterator it = m_MenuItemMap.find(i_ObjectID);
	if (it != m_MenuItemMap.end())
	{
		it->second.m_ExecuteCallback = i_pFunction;
		it->second.m_UpdateCallback = i_pUpdate;
	}
}

//---------------------------------------------------------------------------
//	MenuObjectsExist()
//		based on the menu item ID.
//
//		returns true if the ID refers to a valid tmaMenuObjects
//---------------------------------------------------------------------------
bool twxMenuMgr::MenuObjectsExist( int i_ObjectID )
{
	std::map<int, sMenuItemInfo>::iterator it = m_MenuItemMap.find(i_ObjectID);
	return (it != m_MenuItemMap.end());
}


//---------------------------------------------------------------------------
//	MenuObjectsEnable()
//---------------------------------------------------------------------------
void twxMenuMgr::MenuObjectsEnable( int i_ObjectID, bool i_bEnabled )
{
	if (twxSystem::g_pMainMenu)
	{
		std::map<int, sMenuItemInfo>::iterator it = m_MenuItemMap.find(i_ObjectID);
		if (it != m_MenuItemMap.end())
		{
			it->second.m_pMenuItem->Enable( i_bEnabled );
		}

		// Set toolbar also, if needed
		twxToolbarMgr::EnableButton(i_ObjectID, i_bEnabled);
	}
}

//---------------------------------------------------------------------------
//	MenuObjectsCheck()
//---------------------------------------------------------------------------
void twxMenuMgr::MenuObjectsCheck( int i_ObjectID, bool i_bChecked )
{
	if (twxSystem::g_pMainMenu)
	{
		std::map<int, sMenuItemInfo>::iterator it = m_MenuItemMap.find(i_ObjectID);
		if (it != m_MenuItemMap.end())
		{
			if (it->second.m_pMenuItem->IsCheckable())
			{
				it->second.m_pMenuItem->Check( i_bChecked );
			}
			else
			{
				DBG_WARNING("Menu item " << it->second.m_pMenuItem->GetItemLabel().c_str() << " is not checkable.");
			}
		}

		// Set toolbar also, if needed
		twxToolbarMgr::CheckButton(i_ObjectID, i_bChecked);
	}
}


//---------------------------------------------------------------------------
//	EnableMenuItem()
//		enable or disable a menu item from the Menu Tree
//---------------------------------------------------------------------------
void twxMenuMgr::EnableMenuItem( const char * i_ParentName,
					const char * i_ChildName,
					bool i_bEnable )
{
	if (twxSystem::g_pMainMenu)
	{
		int id = GetMenuItemID(i_ParentName, i_ChildName);
		if (id >= 0)
		{
			std::map<int, sMenuItemInfo>::iterator it = m_MenuItemMap.find(id);
			if (it != m_MenuItemMap.end())
			{
				it->second.m_pMenuItem->Enable(i_bEnable);
			}

			// Set toolbar also, if needed
			twxToolbarMgr::EnableButton(id, i_bEnable);
		}
	}
}

//---------------------------------------------------------------------------
//	GetMenuItemID()
//		get the menu item ID
//---------------------------------------------------------------------------
int twxMenuMgr::GetMenuItemID( const char * i_ParentName,
							   const char * i_ChildName )
{
	if (twxSystem::g_pMainMenu)
	{
		wxString parent_name(i_ParentName, wxConvUTF8);
		wxString child_name(i_ChildName, wxConvUTF8);
		int id = twxSystem::g_pMainMenu->FindMenuItem( parent_name, child_name );
		return (id == wxNOT_FOUND) ? -1 : id;
	}
	return -1;
}

//---------------------------------------------------------------------------
//	GetMenuItemName()
//		give the name of the menu item with the given ID.
//---------------------------------------------------------------------------
std::string twxMenuMgr::GetMenuItemName(int i_ObjectID)
{
	if (twxSystem::g_pMainMenu)
	{
		std::map<int, sMenuItemInfo>::iterator it = m_MenuItemMap.find(i_ObjectID);
		if (it != m_MenuItemMap.end())
		{
			wxString menu_label = it->second.m_pMenuItem->GetItemLabelText();
			return std::string(menu_label.utf8_str());
		}
	}

	return "";
}

//---------------------------------------------------------------------------
//	GetMenuItem()
//		return the menu item with the given ID.
//---------------------------------------------------------------------------
wxMenuItem* twxMenuMgr::GetMenuItem(int i_ObjectID)
{
	if (twxSystem::g_pMainMenu)
	{
		std::map<int, sMenuItemInfo>::iterator it = m_MenuItemMap.find(i_ObjectID);
		if (it != m_MenuItemMap.end())
		{
			return it->second.m_pMenuItem;
		}
	}

	return NULL;
}

//---------------------------------------------------------------------------
// Set Help string to display when mouse is over given menu item
//---------------------------------------------------------------------------
void twxMenuMgr::SetMenuItemHelpString( int i_ObjectID, const char * i_HelpString )
{
	if (twxSystem::g_pMainMenu)
	{
		std::map<int, sMenuItemInfo>::iterator it = m_MenuItemMap.find(i_ObjectID);
		if (it != m_MenuItemMap.end())
		{
			it->second.m_pMenuItem->SetHelp( wxString(i_HelpString, wxConvUTF8) );
		}

		// Set toolbar also, if needed
		twxToolbarMgr::SetToolItemHelpString(i_ObjectID, i_HelpString);
	}
}

//---------------------------------------------------------------------------
// Callback for when the menu item is chosen
//---------------------------------------------------------------------------
void twxMenuMgr::menuItem_Click(wxCommandEvent& i_Event)
{
	int id = i_Event.GetId();
	std::map<int, sMenuItemInfo>::iterator it = m_MenuItemMap.find(id);
	if (it != m_MenuItemMap.end())
	{
		ControlCallback callback = it->second.m_ExecuteCallback;
		if (callback)
		{
			try
			{
				// Execute callback associated with menu item
				guiStatusBarMgr::ClearErrorMessage();
				(*callback)(id);
			}
			catch ( const envExceptionX& i_Ex)
			{
				DBG_ERROR("Problem occurred executing menu command: " << i_Ex.GetErrorMessage());
				guiMessageBox::Show(i_Ex.GetErrorMessage().c_str(), "Error");
			}
			catch (const std::bad_alloc&)
			{
				std::string msg = "Out of system memory, could not execute menu command";
				DBG_ERROR(msg);
				guiMessageBox::Show(msg.c_str(), "Error");
			}
			catch (const std::exception& i_Ex)
			{
				std::string msg = "Problem occurred executing menu command: " + std::string(i_Ex.what());
				DBG_ERROR(msg);
				guiMessageBox::Show(msg.c_str(), "Error");
			}
		}
	}
}

//---------------------------------------------------------------------------
// Callback for when the menu item needs to update state
//---------------------------------------------------------------------------
void twxMenuMgr::menuItem_Update(wxUpdateUIEvent& i_Event)
{
	int id = i_Event.GetId();
	std::map<int, sMenuItemInfo>::iterator it = m_MenuItemMap.find(id);
	if (it != m_MenuItemMap.end())
	{
		ControlCallback callback = it->second.m_UpdateCallback;
		if (callback)
		{
			// Call update callback associated with menu item
			(*callback)(id);
		}
	}
	i_Event.Skip(); // allow others to update UI also
}

//---------------------------------------------------------------------------
// Change the text associated to a menu item to create a shortcut string
//---------------------------------------------------------------------------
void twxMenuMgr::SetMenuItemShortcut(int i_ObjectID, const char* i_pShortcutString)
{
	//Get the menu item associated with the id
	wxMenuItem* _menuItem = GetMenuItem( i_ObjectID );
	
	if(!_menuItem)
		return;
	
	//alter the string to add a tab and then add the shortcut string
	wxString _newMenuString = _menuItem->GetItemLabel();//.BeforeFirst(wxChar("\t"));
	_newMenuString = _menuItem->GetLabelText(_newMenuString);

	if( i_pShortcutString != "" )
	{
		wxString _menuShortcutString = wxString(i_pShortcutString, wxConvUTF8);
		_newMenuString += wxString(L"\t");
		_newMenuString += _menuShortcutString;
	}

	_menuItem->SetItemLabel(_newMenuString);
}

/*
//---------------------------------------------------------------------------
//	AttachEventToMenuObjects()
//---------------------------------------------------------------------------
void twxMenuMgr::AttachEventToMenuObjects( int i_ObjectID, ControlCallback i_pFunction, ControlCallback i_pUpdate )
{
	tmaMenuObjects^ pmmaMI = tmaMenuObjectsMgr::g_pMgr->get_menuitem( i_ObjectID );
	DBG_ASSERT( pmmaMI != nullptr, "MenuItem is 0" );

	pmmaMI->SetCallback( i_pFunction );
	if (i_pUpdate != nullptr)
	{
		pmmaMI->SetUpdateCallback( i_pUpdate );
	}
}


//---------------------------------------------------------------------------
//	AddMenuItem()
//		add a menu item to the Menu Tree.
//		set the childname to "" is adding a parent item.
//		set the toolbar name to "" to not add a toolbar button
//	the function returns a unique ObjectID for the GUI object.
//	
//---------------------------------------------------------------------------
int twxMenuMgr::AddMenuItem( const char * i_ParentName,
				const char * i_ChildName,
				const char * i_ToolbarName,
				const char * i_ToolbarButton_ImageFilename )
{
	DBG_ASSERT( tmaSystem::g_pMainForm != nullptr, "twxMenuMgr not initialized" );
	DBG_ASSERT( i_ParentName != 0, "Parent Name cannot be 0" );
	DBG_ASSERT( (*i_ParentName != 0), "Parent Name cannot be empty" );

	tmaMenuObjects^ pMITop = nullptr;
	tmaMenuObjects^ pMISub = nullptr;

	// find if the parent name exists already
	//
	tmaMenuObjects^ pMO;

	System::String^ parent_name = gcnew System::String(i_ParentName);
	System::String^ child_name = gcnew System::String(i_ChildName);
	System::Collections::IEnumerator^ myEnumerator = tmaMenuObjectsMgr::g_pMgr->GetMenuObjectsList()->GetEnumerator();
	while ( myEnumerator->MoveNext() )
	{
		pMO = dynamic_cast<tmaMenuObjects^>(myEnumerator->Current);
		if ( String::CompareOrdinal( pMO->GetMenuItem()->Text, parent_name ) == 0 )
		{
			pMITop = pMO;
		}
		if (	(String::CompareOrdinal( pMO->GetMenuItem()->Text, child_name ) == 0)
			&&	(pMITop != nullptr))
		{
			pMISub = pMO;
		}
	}

	tmaSystem::g_pMainForm->SuspendLayout();

	// the top doesn't exist yet, so create it.
	if ( pMITop == nullptr )
	{
		//	check if the menu item was created in the designer instead of programmatically
		//	if so, create a tmaMenuObject and use the ToolStripItem that is already there.
		//
		ToolStripItem^ pTSI = nullptr;
		int ndx = tmaSystem::g_pMainForm->MainMenuStrip->Items->Count;
		System::String ^parent_name = gcnew System::String(i_ParentName);
		for (int i = 0; i < ndx; ++i)
		{
			pTSI = tmaSystem::g_pMainForm->MainMenuStrip->Items[i];
			if (pTSI->Text->Equals(parent_name))
			{
				break;
			}
			pTSI = nullptr;
		}

		// generate the menu item
		pMITop = generate_menuitem(pTSI);
		DBG_ASSERT( pMITop != nullptr, "generation of menu item failed" );
		pMITop->GetMenuItem()->Text = parent_name;

		// attach popup callback in order to get callback for updates
		pMITop->GetMenuItem()->DropDownOpening += gcnew System::EventHandler( this, &twxMenuMgr::menuItem_Popup );

		//	add the top level menu item to the list
		if (pTSI == nullptr)
			tmaSystem::g_pMainForm->MainMenuStrip->Items->Add( pMITop->GetMenuItem() );
	}

	//	sub menu item (if not an empty string passed in)
	//
	if ( *i_ChildName != 0 )
	{
		if ( pMISub == nullptr )
		{
			pMISub = generate_menuitem(nullptr);
			DBG_ASSERT( pMISub != nullptr, "generation of sub menu item failed" );
			pMISub->GetMenuItem()->Text = gcnew System::String(i_ChildName);

			pMISub->GetMenuItem()->MergeIndex = 0;
			//pMITop->GetMenuItem()->DropDownItems->AddRange( __mcTemp__2 );
			pMITop->GetMenuItem()->DropDownItems->Add( pMISub->GetMenuItem() );

			// Attach popup callback in order to get callback for updates.
			// This particular callback won't get called until other items
			// are added to this as children, so this is a little earlier, 
			// but atleast we'll be ready.
			pMISub->GetMenuItem()->DropDownOpening += gcnew System::EventHandler( this, &twxMenuMgr::menuItem_Popup );
		}
		else
		{
			tmaSystem::g_pMainForm->ResumeLayout();

			//	the item already exists so don't drop below and try to
			//	add another button
			return pMISub->GetID();
		}
	}

	//	set-up the event handling for the menu items
	//
	tmaMenuObjects ^pMIAdded;

	if ( pMISub != nullptr )
	{
		pMIAdded = pMISub;
	}
	else
	{
		pMIAdded = pMITop;
	}

	//pMIAdded->GetMenuItem()->Click += new EventHandler( (pMIAdded), tmaMenuObjects::Callback );
	//pMIAdded->GetMenuItem()->Select += new EventHandler( (pMIAdded), tmaMenuObjects::Callback );
	//pSWFMI->PerformClick();
	//pSWFMI->PerformSelect();

	//
	//	add buttons to the toolbar
	//
	if ( i_ToolbarName != 0 )
	{
		std::string icon_dir;
		tmaManagedStringUtils::ManagedStringToStdString( m_IconDir, icon_dir );
		icon_dir += i_ToolbarButton_ImageFilename;

		tmaToolBarMgr::g_pMgr->add_toolstrip_button( pMIAdded, i_ToolbarName, icon_dir.c_str() );
	}

	AttachCallbacks( pMIAdded->GetID() );

	tmaSystem::g_pMainForm->ResumeLayout();

	return pMIAdded->GetID();
}

//---------------------------------------------------------------------------
//	RemoveMenuItem()
//		remove a menu item from the Menu Tree
//---------------------------------------------------------------------------
void twxMenuMgr::RemoveMenuItem( const char * i_ParentName,
					const char * i_ChildName  )
{
	DBG_ASSERT( tmaSystem::g_pMainForm != nullptr, "twxMenuMgr not initialized" );

	//	find the menu items
	//
	tmaMenuObjects^ pMI_top = nullptr;
	tmaMenuObjects^ pMI = nullptr;
	tmaMenuObjects^ pMO;

	System::String ^parent_name = gcnew System::String( i_ParentName );
	System::String ^child_name = gcnew System::String( i_ChildName );
	System::Collections::IEnumerator^ myEnumerator = tmaMenuObjectsMgr::g_pMgr->GetMenuObjectsList()->GetEnumerator();
	while ( myEnumerator->MoveNext() )
	{
		pMO = dynamic_cast<tmaMenuObjects^>(myEnumerator->Current);
		if ( String::CompareOrdinal( pMO->GetMenuItem()->Text, parent_name ) == 0 )
		{
			pMI_top = pMO;
		}
		if (	(String::CompareOrdinal( pMO->GetMenuItem()->Text, child_name ) == 0)
			&&	(pMI_top != nullptr))
		{
			pMI = pMO;
		}
	}

	if ( !pMI_top )
	{
		return;
	}

	//	if the sub menu item is null that means we want to remove the root menu item
	//
	if ( pMI == nullptr )
	{
		//	remove the MenuItem + toolbarbutton
		//
		tmaSystem::g_pMainForm->MainMenuStrip->Items->Remove( pMI_top->GetMenuItem() );

//FINISH			tmaToolBarMgr::g_pMgr->remove_toolstrip_button( pMI_top );

		tmaMenuObjectsMgr::g_pMgr->GetMenuObjectsList()->Remove( pMI_top );

		delete pMI_top;
	}
	else
	{
		//	remove the sub MenuItem + toolbarbutton
		//
		pMI_top->GetMenuItem()->DropDownItems->Remove( pMI->GetMenuItem() );

//FINISH			tmaToolBarMgr::g_pMgr->remove_toolstrip_button( pMI );

		tmaMenuObjectsMgr::g_pMgr->GetMenuObjectsList()->Remove( pMI );

		delete pMI;
	}
}

//---------------------------------------------------------------------------
//	EnableMenuItem()
//		enable or disable a menu item from the Menu Tree
//---------------------------------------------------------------------------
void twxMenuMgr::EnableMenuItem( const char * i_ParentName,
					const char * i_ChildName,
					bool i_bEnable )
{
	DBG_ASSERT( tmaSystem::g_pMainForm != nullptr, "twxMenuMgr not initialized" );

	//	find the menu items
	//
	tmaMenuObjects^ pMI_top = nullptr;
	tmaMenuObjects^ pMI = nullptr;
	tmaMenuObjects^ pMO;

	System::String ^parent_name = gcnew System::String( i_ParentName );
	System::String ^child_name = gcnew System::String( i_ChildName );
	System::Collections::IEnumerator^ myEnumerator = tmaMenuObjectsMgr::g_pMgr->GetMenuObjectsList()->GetEnumerator();
	while ( myEnumerator->MoveNext() )
	{
		pMO = dynamic_cast<tmaMenuObjects^>(myEnumerator->Current);
		if ( String::CompareOrdinal( pMO->GetMenuItem()->Text, parent_name ) == 0 )
		{
			pMI_top = pMO;
		}
		if (	(String::CompareOrdinal( pMO->GetMenuItem()->Text, child_name ) == 0)
			&&	(pMI_top != nullptr))
		{
			pMI = pMO;
		}
	}

	if ( !pMI_top )
	{
		return;
	}

	DBG_ASSERT( pMI != nullptr, "Cannot enable/disable a menu item that is NULL" );

	//	enable/disable the menu item + the toolbar
	//
	pMI->GetMenuItem()->Enabled = i_bEnable;

	tmaToolBarMgr::g_pMgr->enable_toolstrip_button( pMI, i_bEnable );
}

//---------------------------------------------------------------------------
//	GetMenuItemID()
//		get the menu item ID
//---------------------------------------------------------------------------
int twxMenuMgr::GetMenuItemID( const char * i_ParentName,
				const char * i_ChildName )
{
	DBG_ASSERT( tmaSystem::g_pMainForm != nullptr, "twxMenuMgr not initialized" );

	//	find the menu items
	//
	tmaMenuObjects^ pMI_top = nullptr;
	tmaMenuObjects^ pMI = nullptr;
	tmaMenuObjects^ pMO;

	System::String ^parent_name = gcnew System::String( i_ParentName );
	System::String ^child_name = gcnew System::String( i_ChildName );
	System::Collections::IEnumerator^ myEnumerator = tmaMenuObjectsMgr::g_pMgr->GetMenuObjectsList()->GetEnumerator();
	while ( myEnumerator->MoveNext() )
	{
		pMO = dynamic_cast<tmaMenuObjects^>(myEnumerator->Current);
		if ( String::CompareOrdinal( pMO->GetMenuItem()->Text, parent_name ) == 0 )
		{
			pMI_top = pMO;
		}
		if (	(String::CompareOrdinal( pMO->GetMenuItem()->Text, child_name ) == 0)
			&&	(pMI_top != nullptr))
		{
			pMI = pMO;
		}
	}

	if (pMI != nullptr)
		return pMI->GetID();
	else
		return -1;
	
	//DBG_ASSERT( pMI_top != nullptr, "Cannot get a menu item with that ID" );
	//DBG_ASSERT( pMI != nullptr, "Cannot get a menu item with that ID" );
	//return pMI->GetID();
}


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
tmaMenuObjects^ twxMenuMgr::GetMenuObjects( int i_ObjectID )
{
	return tmaMenuObjectsMgr::g_pMgr->get_menuitem( i_ObjectID );
}


//---------------------------------------------------------------------------
//	MenuObjectsEnable()
//---------------------------------------------------------------------------
void twxMenuMgr::MenuObjectsEnable( int i_ObjectID, bool i_bEnabled )
{
	tmaMenuObjects^ pmmaMI = tmaMenuObjectsMgr::g_pMgr->get_menuitem( i_ObjectID );
	DBG_ASSERT( pmmaMI != nullptr, "Cannot attach an event to a 0 menu item" );

	// menu item
	ToolStripMenuItem^ mi = pmmaMI->GetMenuItem();
	mi->Enabled =  i_bEnabled;

	// toolbar button
	ToolStripButton^ tbb = pmmaMI->GetToolStripButton();
	if ( tbb )
	{
		tbb->Enabled =  i_bEnabled;
	}
}

//---------------------------------------------------------------------------
//	MenuObjectsCheck()
//---------------------------------------------------------------------------
void twxMenuMgr::MenuObjectsCheck( int i_ObjectID, bool i_bChecked )
{
	tmaMenuObjects^ pmmaMI = tmaMenuObjectsMgr::g_pMgr->get_menuitem( i_ObjectID );
	DBG_ASSERT( pmmaMI != nullptr, "Cannot attach an event to a 0 menu item" );

	// menu item
	ToolStripMenuItem^ mi = pmmaMI->GetMenuItem();
	mi->Checked =  i_bChecked;

	// toolbar button
	//	Not Applicable?
	//ToolBarButton^ tbb = pmmaMI->GetToolbarButton();
	//if ( tbb )
	//{
	//	tbb->set_Pushed( i_bChecked );
	//}
}

//---------------------------------------------------------------------------
//	AttachCallbacks()
//---------------------------------------------------------------------------
void twxMenuMgr::AttachCallbacks( int i_ObjectID )
{
	tmaMenuObjects^ pmmaMI = tmaMenuObjectsMgr::g_pMgr->get_menuitem( i_ObjectID );
	DBG_ASSERT( pmmaMI != nullptr, "Cannot attach an event to a 0 menu item" );

	// menu item
	ToolStripMenuItem^ mi = pmmaMI->GetMenuItem();
	mi->Click += gcnew System::EventHandler( this, &twxMenuMgr::menuItem_Click );

	// toolbar
	tmaToolBarMgr::g_pMgr->AttachHandler( pmmaMI );
}

//---------------------------------------------------------------------------
//	GetMenuObjectsList()
//---------------------------------------------------------------------------
ArrayList^ twxMenuMgr::GetMenuObjectsList()
{
	return tmaMenuObjectsMgr::g_pMgr->GetMenuObjectsList();
}

//---------------------------------------------------------------------------
//	AddDesignerMenu()
//---------------------------------------------------------------------------
void twxMenuMgr::AddDesignerMenu(System::Windows::Forms::ToolStripMenuItem ^pMenu)
{
	tmaMenuObjectsMgr::g_pMgr->AddDesignerMenu(pMenu);

	// attach popup callback in order to get callback for updates
	pMenu->DropDownOpening += gcnew System::EventHandler( this, &twxMenuMgr::menuItem_Popup );
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void twxMenuMgr::SetIconDirectory( const char * i_IconDirectory )
{
	if ( m_IconDir == nullptr )
	{
		m_IconDir = gcnew System::String( i_IconDirectory );
	}
	else
	{
		m_IconDir->Copy( gcnew System::String(i_IconDirectory) );
	}
}

//---------------------------------------------------------------------------
//	Find a menu item.
//	NOTE: can return incorrect results if multiple menu items have the same
//	name.
//---------------------------------------------------------------------------
int twxMenuMgr::FindMenuItem(String^ i_pMenuItemName)
{
	// loop through the menus and try to find a matching item
	DBG_ASSERT( tmaSystem::g_pMainForm != nullptr, "twxMenuMgr not initialized" );

	//	find the menu items
	//
	tmaMenuObjects^ pMI = nullptr;
	tmaMenuObjects^ pMO;

	System::Collections::IEnumerator^ myEnumerator = tmaMenuObjectsMgr::g_pMgr->GetMenuObjectsList()->GetEnumerator();
	while ( myEnumerator->MoveNext() )
	{
		pMO = dynamic_cast<tmaMenuObjects^>(myEnumerator->Current);
		if (String::CompareOrdinal( pMO->GetMenuItem()->Text, i_pMenuItemName ) == 0)
		{
			pMI = pMO;
		}
	}

	if (pMI == nullptr)
		return -1;
	else
		return pMI->GetID();
}


	//_menuItem.SetLableString
	

	//tmaMenuObjects^ pmmaMI = tmaMenuObjectsMgr::g_pMgr->get_menuitem( i_ObjectID );
	//wxMenuItem
	DBG_ASSERT( pmmaMI != nullptr, "Cannot attach an event to a 0 menu item" );

	// menu item
	ToolStripMenuItem^ pMI = pmmaMI->GetMenuItem();

	if (pMI != nullptr && i_pShortcutString != nullptr)
	{
		//	set-up a converter for shortcuts
		//
		TypeConverter^ conv = TypeDescriptor::GetConverter(Keys::typeid);
		Object^ obj = conv->ConvertFromString(i_pShortcutString);
		if (obj != nullptr)
		{
			Keys^ kys = dynamic_cast<Keys^>(conv->ConvertFromString( i_pShortcutString ));
			if (kys != nullptr)
			{
				try
				{
					//pMI->set_ShortcutKeys(^(kys));
					pMI->ShortcutKeys = *(kys);

					int kysvalue = (int)(*kys);
					std::string skstr;
					tmaManagedStringUtils::ManagedStringToStdString( conv->ConvertToString(pMI->ShortcutKeys), skstr);
					std::string menustr;
					tmaManagedStringUtils::ManagedStringToStdString( pMI->Text, menustr );
					//DBG_LOG4("    shortcut menu-id=%d-%s scut=%d-%s", i_ObjectID, menustr.c_str(), kysvalue, skstr.c_str());
				}
				catch(...)
				{
				}
			}
		}
	}
	else
	{
		pMI->ShortcutKeys = System::Windows::Forms::Keys::None;
	}
	*/

/*
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool twxMenuMgr::IsValidShortcut( String^ i_ShortcutString )
{
	if (i_ShortcutString == nullptr)
		return false;
	if (i_ShortcutString->Length == 0)
		return false;

	TypeConverter^ conv = TypeDescriptor::GetConverter(Keys::typeid);
	return conv->IsValid(i_ShortcutString);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void twxMenuMgr::GetDropDownItemShortcut(MenuItem^ i_pMenuItem, String^ o_pShortcutString)
{
	if (i_pMenuItem != nullptr && o_pShortcutString != nullptr)
	{
		//TypeConverter^ keyconv = TypeDescriptor::GetConverter(Keys::typeid);//new TypeConverter;
		//Shortcut scut = i_pMenuItem->Shortcut;
		//Keys k;
		//k = static_cast<Keys>(i_pMenuItem->Shortcut);
		//String^ st = gcnew String("");
		//st->Concat(st, scut);
		//o_pShortcutString = keyconv->ConvertToString(k);

//Shortcut sc = Shortcut.CtrlShiftF1;
//string s = TypeDescriptor.GetConverter(typeof(Keys)).ConvertToString((Keys) sc);
	}
	else
	{
		if (o_pShortcutString != nullptr)
		{
			o_pShortcutString = "";
		}
	}
}

//---------------------------------------------------------------------------
//	generate a menu item
//---------------------------------------------------------------------------
tmaMenuObjects^ twxMenuMgr::generate_menuitem(ToolStripItem^ i_pTSI)
{
	tmaMenuObjects^ pMI = tmaMenuObjectsMgr::g_pMgr->generate_menuitem();
	
	//	if a toolstripitem was passed in, then use it otherwise create a new one
	if (i_pTSI == nullptr)
	{
		System::Windows::Forms::ToolStripMenuItem^ pSWFMI = gcnew System::Windows::Forms::ToolStripMenuItem;
		pSWFMI->ShowShortcutKeys = true;
		pMI->SetMenuItem( pSWFMI );
	}
	else
	{
		dynamic_cast<ToolStripMenuItem^>(i_pTSI)->ShowShortcutKeys = true;
		pMI->SetMenuItem( dynamic_cast<ToolStripMenuItem^>(i_pTSI) );
	}

	return pMI;
}


//---------------------------------------------------------------------------
// Execution event handler
//---------------------------------------------------------------------------
void twxMenuMgr::menuItem_Click( Object^ Sender, System::EventArgs^ e )
{
	//System::Windows::Forms::MessageBox::Show( "menu item clicked!" );

	tmaMenuObjects^ pmmaMI = tmaMenuObjectsMgr::g_pMgr->get_menuitem( dynamic_cast<ToolStripMenuItem^>(Sender) );
	if ( pmmaMI->GetCallback() )
	{
		(*(pmmaMI->GetCallback()))( pmmaMI->GetID() );
	}
}
//---------------------------------------------------------------------------
// Popup event handler
//---------------------------------------------------------------------------
void twxMenuMgr::menuItem_Popup(System::Object ^  Sender, System::EventArgs ^  e)
{
	// Popup is called on the parent, but Update is registered on the child items.
	// Go through the 
	ToolStripMenuItem^ parent = dynamic_cast<ToolStripMenuItem^>(Sender);
	int num_kids = parent->DropDownItems->Count;
	for (int k=0; k<num_kids; k++)
	{
		ToolStripMenuItem ^child = dynamic_cast<ToolStripMenuItem^>(parent->DropDownItems[k]);

		tmaMenuObjects^ pItem = tmaMenuObjectsMgr::g_pMgr->get_menuitem( child );
		if ( pItem && pItem->GetUpdateCallback() )
		{
			(*(pItem->GetUpdateCallback()))( pItem->GetID() );
		}
	}
}
*/

#endif // USE_WXWIDGETS
