/*****************************************************************************
**  tqtContextMenu.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/tqt/tqtContextMenu.hpp"

#include "Core/dbg/dbgMsg.hpp"
#include "Core/env/envExceptionX.hpp"
#include "Tool/gui/guiMessageBox.hpp"
#include "Tool/gui/guiStatusBarMgr.hpp"


#ifdef QT_FINISH_PORT

namespace
{
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

//===========================================================================
//	tqtContextMenu functions
//===========================================================================

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
tqtContextMenu::tqtContextMenu()
{
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
tqtContextMenu::~tqtContextMenu()
{
}


//---------------------------------------------------------------------------
//	AddMenu()
//		add a menu or submenu to the Menu Tree.
//		set the childname to "" is adding a parent item.
//---------------------------------------------------------------------------
void tqtContextMenu::AddMenu( const char * i_ParentName,
							  const char * i_ChildName )
{
	//wxString menu_name(i_ParentName, wxConvUTF8);
	//this->Append(wxID_ANY,  menu_name);

	wxString parent_name(i_ParentName, wxConvUTF8);

	// If child name is empty, create top-level menu
	if (!i_ChildName || !(*i_ChildName))
	{
		int index = this->FindItem(parent_name);
		if (index == wxNOT_FOUND)
		{
			wxMenu *pNewMenu = new wxMenu;
			this->AppendSubMenu(pNewMenu, parent_name);
		}
	}	
	else
	{
		wxString child_name(i_ChildName, wxConvUTF8);
		wxMenu* pMenu = find_menu(this, parent_name);
		if (pMenu)
		{
			// Need to make sure the same menu isn't added more than once
			if (!find_menu_item(pMenu, child_name))
			{
				wxMenu *pSubMenu = new wxMenu();
				pMenu->AppendSubMenu(pSubMenu, child_name);
			}
		}
	}

}

//---------------------------------------------------------------------------
//	AddMenuItem()
//		add a menu item to the Menu Tree which will call the given 
//	callback function when clicked.
//---------------------------------------------------------------------------
int tqtContextMenu::AddMenuItem( const char * i_ParentName,
								 const char * i_ChildName,
								 guiContextMenu::CallbackFunction i_Callback )
{
	DBG_ASSERT( (i_ChildName && (*i_ChildName)), "MenuItems need to include a child name.");

	//DBG_LOG2("AddMenuItem: parent %s child %s", i_ParentName, i_ChildName);

	wxMenu* pMenu = this;
	if (i_ParentName && (*i_ParentName))
	{
		wxString parent_name(i_ParentName, wxConvUTF8);
		pMenu = find_menu(this, parent_name);
	}
	if (pMenu)
	{
		// See if the menu item was already created (when laying out menus earlier)
		wxString child_name(i_ChildName, wxConvUTF8);
		wxMenuItem *pItem = find_menu_item(pMenu, child_name);
		if (!pItem)
		{
			//pItem = (i_bCheckable) ? pMenu->AppendCheckItem(-1, child_name) 
			//								: pMenu->Append(-1, child_name);
			pItem = pMenu->Append(wxID_ANY, child_name);
		}
		int id = pItem->GetId();
		m_MenuItemMap[id] = i_Callback;

		// Connect our callback to the menu item we just created
		this->Connect( id,
					wxEVT_COMMAND_MENU_SELECTED,
					wxCommandEventHandler(tqtContextMenu::menuItem_Click) );

		return id;
	}

	DBG_LOG("Could not add menu item " << i_ChildName << ", parent=" << i_ParentName);
	return -1;
}

//---------------------------------------------------------------------------
//	AddSeparator()
//		add a separator to the menu with the given name.
//---------------------------------------------------------------------------
void tqtContextMenu::AddSeparator( const char * i_MenuName )
{
	this->AppendSeparator();
}

//---------------------------------------------------------------------------
// Returns true if the context menu does not have any menus added.
//---------------------------------------------------------------------------
bool tqtContextMenu::IsEmpty()
{
	// If no menu items are registered, then return true. 
	// This will skip the Popup of the menu when empty.
	return m_MenuItemMap.empty();
}

//---------------------------------------------------------------------------
// Callback for when the menu item is chosen
//---------------------------------------------------------------------------
void tqtContextMenu::menuItem_Click(wxCommandEvent& i_Event)
{
	int id = i_Event.GetId();
	std::map<int, guiContextMenu::CallbackFunction>::iterator it = m_MenuItemMap.find(id);
	if (it != m_MenuItemMap.end())
	{
		guiContextMenu::CallbackFunction callback = it->second;
		if (callback)
		{
			try
			{
				// Execute callback associated with menu item
				guiStatusBarMgr::ClearErrorMessage();
				(callback)();
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

#endif
