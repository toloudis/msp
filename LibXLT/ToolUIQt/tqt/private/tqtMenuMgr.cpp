/*****************************************************************************
**  tqtMenuMgr.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/tqt/tqtMenuMgr.hpp"

#include "ToolUIQt/tqt/tqtSystem.hpp"
#include "ToolUIQt/tqt/tqtToolbarMgr.hpp"

#include "Core/Dbg/dbgMsg.hpp"
#include "Core/Env/envExceptionX.hpp"
#include "Tool/gui/guiMessageBox.hpp"
#include "Tool/gui/guiStatusBarMgr.hpp"


#ifdef USE_QT

//============================================================================
//============================================================================
namespace
{
	// Not sure what a safe id number to start with is
	int l_MenuCounter = 300;

	// How often to refresh the state of toolbar buttons, in milliseconds
	const int c_ToolbarUpdateInterval = 100;

	//-----------------------------------------------------------------------
	// Menu text without the mnemonic markers or shortcut, for comparing
	//-----------------------------------------------------------------------
	QString plain_text(const QString& i_Text)
	{
		QString text = i_Text;
		text.remove(QLatin1Char('&'));
		int tab = text.indexOf(QLatin1Char('\t'));
		if (tab >= 0)
			text.truncate(tab);
		return text;
	}

	//-----------------------------------------------------------------------
	// Find menu item (not submenu) somewhere within the given menu
	//-----------------------------------------------------------------------
	QAction* find_menu_item(QMenu* i_pParent, const QString& i_MenuName)
	{
		const QList<QAction*> actions = i_pParent->actions();
		for (QAction* pAction : actions)
		{
			if (pAction->isSeparator())
				continue;
			if (plain_text(pAction->text()) == i_MenuName)
				return pAction;
			if (pAction->menu())
			{
				if (QAction* pFound = find_menu_item(pAction->menu(), i_MenuName))
					return pFound;
			}
		}
		return NULL;
	}

	//-----------------------------------------------------------------------
	// Find menu somewhere within the given menu
	//-----------------------------------------------------------------------
	QMenu* find_menu(QMenu* i_pParent, const QString& i_MenuName)
	{
		// we actually want to find the last occurrence of the menu,
		// not the first. This helps sort out issues when two submenus
		// has the same name.
		QMenu *ret_val = NULL;
		const QList<QAction*> actions = i_pParent->actions();
		for (QAction* pAction : actions)
		{
			if (QMenu* pSubMenu = pAction->menu())
			{
				if (plain_text(pAction->text()) == i_MenuName)
					ret_val = pSubMenu;

				if (QMenu* pFound = find_menu(pSubMenu, i_MenuName))
					ret_val = pFound;
			}
		}
		return ret_val;
	}

	//-----------------------------------------------------------------------
	// Find top-level menu with given name in the menu bar
	//-----------------------------------------------------------------------
	QMenu* find_top_menu(QMenuBar* i_pParent, const QString& i_MenuName)
	{
		const QList<QAction*> actions = i_pParent->actions();
		for (QAction* pAction : actions)
		{
			if (pAction->menu() && plain_text(pAction->text()) == i_MenuName)
				return pAction->menu();
		}
		return NULL;
	}

	//-----------------------------------------------------------------------
	// Find menu with given name within the menu bar and its submenus
	//-----------------------------------------------------------------------
	QMenu* find_menu(QMenuBar* i_pParent, const QString& i_MenuName)
	{
		if (QMenu* pMenu = find_top_menu(i_pParent, i_MenuName))
			return pMenu;

		const QList<QAction*> actions = i_pParent->actions();
		for (QAction* pAction : actions)
		{
			if (pAction->menu())
			{
				if (QMenu* pMenu = find_menu(pAction->menu(), i_MenuName))
					return pMenu;
			}
		}
		return NULL;
	}

	//-----------------------------------------------------------------------
	// Id stored on the menu item's action, or -1
	//-----------------------------------------------------------------------
	int get_action_id(QAction* i_pAction)
	{
		bool bOk = false;
		int id = i_pAction->data().toInt(&bOk);
		return bOk ? id : -1;
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
tqtMenuMgr::tqtMenuMgr()
:	m_pToolbarUpdateTimer(NULL)
{
	DBG_ASSERT( tqtSystem::g_pMainMenu != NULL, "tqtMenuMgr needs a main frame in order to init." );

	// wxWidgets updated toolbar buttons in idle time; do the same
	// periodically so the button states follow the application.
	m_pToolbarUpdateTimer = new QTimer;
	m_pToolbarUpdateTimer->setInterval(c_ToolbarUpdateInterval);
	m_Connections.push_back(
		QObject::connect(m_pToolbarUpdateTimer, &QTimer::timeout, [this]() { update_toolbar_items(); }));
	m_pToolbarUpdateTimer->start();
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
tqtMenuMgr::~tqtMenuMgr()
{
	for (size_t i = 0; i < m_Connections.size(); ++i)
		QObject::disconnect(m_Connections[i]);
	m_Connections.clear();

	delete m_pToolbarUpdateTimer;
	m_pToolbarUpdateTimer = NULL;
}

//---------------------------------------------------------------------------
//	AddMenu()
//		add a menu or submenu to the Menu Tree.
//		set the childname to "" is adding a parent item.
//---------------------------------------------------------------------------
void tqtMenuMgr::AddMenu( const char * i_ParentName,
						  const char * i_ChildName )
{
	DBG_ASSERT( tqtSystem::g_pMainMenu != NULL, "tqtMenuMgr not initialized" );

	QString parent_name = tqtQString(i_ParentName);

	// If child name is empty, create top-level menu
	if (!i_ChildName || !(*i_ChildName))
	{
		if (!find_top_menu(tqtSystem::g_pMainMenu, parent_name))
		{
			QMenu* pNewMenu = tqtSystem::g_pMainMenu->addMenu(parent_name);
			watch_menu(pNewMenu);
		}
		return;
	}
	else
	{
		QString child_name = tqtQString(i_ChildName);
		QMenu* pMenu = find_menu(tqtSystem::g_pMainMenu, parent_name);
		if (pMenu)
		{
			// Need to make sure the same menu isn't added more than once
			if (!find_menu_item(pMenu, child_name))
			{
				QMenu* pSubMenu = pMenu->addMenu(child_name);
				watch_menu(pSubMenu);
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
//---------------------------------------------------------------------------
int tqtMenuMgr::AddMenuItem( const char * i_ParentName,
							const char * i_ChildName,
							const char * i_ToolbarName,
							const char * i_ToolbarButton_ImageFilename,
							bool i_bCheckable )
{
	DBG_ASSERT( tqtSystem::g_pMainMenu != NULL, "tqtMenuMgr not initialized" );
	DBG_ASSERT( (i_ChildName && (*i_ChildName)), "MenuItems need to include a child name.");

	QString parent_name = tqtQString(i_ParentName);
	QMenu* pMenu = find_menu(tqtSystem::g_pMainMenu, parent_name);
	if (pMenu)
	{
		int id = -1;

		// See if the menu item was already created (when laying out menus earlier)
		QString child_name = tqtQString(i_ChildName);
		QAction *pItem = find_menu_item(pMenu, child_name);
		if (pItem && get_action_id(pItem) >= 0)
		{
			id = get_action_id(pItem);
		}
		else
		{
			id = l_MenuCounter++;
			if (!pItem)
				pItem = pMenu->addAction(child_name);
			pItem->setData(id);
			pItem->setCheckable(i_bCheckable);
			pItem->setIconVisibleInMenu(false);

			m_Connections.push_back(
				QObject::connect(pItem, &QAction::triggered, [this, id]() { menuItem_Click(id); }));
		}
		watch_menu(pMenu);

		std::map<int, sMenuItemInfo>::iterator it = m_MenuItemMap.find(id);
		if (it == m_MenuItemMap.end())
		{
			sMenuItemInfo menu_info = { pItem, NULL, NULL, false };
			m_MenuItemMap[id] = menu_info;
		}

		if (i_ToolbarName && i_ToolbarButton_ImageFilename)
		{
			if (tqtToolbarMgr::AddToolBarButton(pItem,
												i_ToolbarName,
												i_ToolbarButton_ImageFilename,
												i_ChildName,
												i_bCheckable))
			{
				m_MenuItemMap[id].m_bOnToolbar = true;
			}
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
void tqtMenuMgr::RemoveMenuItem( const char * i_ParentName,
								 const char * i_ChildName  )
{
	DBG_LOG("RemoveMenuItem: parent " << i_ParentName <<" child " << i_ChildName);
}

//---------------------------------------------------------------------------
//	FindMenuItem()
//		returns whether or not the menu item can be found in the menu system
//---------------------------------------------------------------------------
bool tqtMenuMgr::FindMenuItem( const char * i_ParentName,
							   const char * i_ChildName  )
{
	QMenu* pMenu = find_menu(tqtSystem::g_pMainMenu, tqtQString(i_ParentName));
	if (pMenu)
	{
		// See if the menu item was already created (when laying out menus earlier)
		if ( (!i_ChildName || !(*i_ChildName)) ||
			 find_menu_item(pMenu, tqtQString(i_ChildName)) )
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
void tqtMenuMgr::AddSeparator( const char * i_MenuName )
{
	DBG_ASSERT( tqtSystem::g_pMainMenu != NULL, "tqtMenuMgr not initialized" );

	QMenu* pMenu = find_menu(tqtSystem::g_pMainMenu, tqtQString(i_MenuName));
	if (pMenu)
	{
		pMenu->addSeparator();
		return;
	}
	DBG_LOG("Could not add separator, menu="<< i_MenuName);
}

//---------------------------------------------------------------------------
//	AttachEventToMenuObjects()
//---------------------------------------------------------------------------
void tqtMenuMgr::AttachEventToMenuObjects( int i_ObjectID,
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
//---------------------------------------------------------------------------
bool tqtMenuMgr::MenuObjectsExist( int i_ObjectID )
{
	std::map<int, sMenuItemInfo>::iterator it = m_MenuItemMap.find(i_ObjectID);
	return (it != m_MenuItemMap.end());
}

//---------------------------------------------------------------------------
//	MenuObjectsEnable()
//---------------------------------------------------------------------------
void tqtMenuMgr::MenuObjectsEnable( int i_ObjectID, bool i_bEnabled )
{
	QAction* pItem = GetMenuItem( i_ObjectID );
	if (pItem)
	{
		// The toolbar button shares the action
		if (pItem->isEnabled() != i_bEnabled)
			pItem->setEnabled( i_bEnabled );
	}
}

//---------------------------------------------------------------------------
//	MenuObjectsCheck()
//---------------------------------------------------------------------------
void tqtMenuMgr::MenuObjectsCheck( int i_ObjectID, bool i_bChecked )
{
	QAction* pItem = GetMenuItem( i_ObjectID );
	if (pItem)
	{
		if (pItem->isCheckable())
		{
			if (pItem->isChecked() != i_bChecked)
				pItem->setChecked( i_bChecked );
		}
		else
		{
			DBG_WARNING("Menu item " << tqtStdString(pItem->text()) << " is not checkable.");
		}
	}
}

//---------------------------------------------------------------------------
//	EnableMenuItem()
//		enable or disable a menu item from the Menu Tree
//---------------------------------------------------------------------------
void tqtMenuMgr::EnableMenuItem( const char * i_ParentName,
								 const char * i_ChildName,
								 bool i_bEnable )
{
	int id = GetMenuItemID(i_ParentName, i_ChildName);
	if (id >= 0)
	{
		MenuObjectsEnable(id, i_bEnable);
	}
}

//---------------------------------------------------------------------------
//	GetMenuItemID()
//		get the menu item ID
//---------------------------------------------------------------------------
int tqtMenuMgr::GetMenuItemID( const char * i_ParentName,
							   const char * i_ChildName )
{
	if (tqtSystem::g_pMainMenu)
	{
		// Like wxMenuBar::FindMenuItem, look in the top-level menu with
		// the parent name (and all of its submenus).
		QMenu* pMenu = find_top_menu(tqtSystem::g_pMainMenu, tqtQString(i_ParentName));
		if (pMenu)
		{
			QAction* pItem = find_menu_item(pMenu, tqtQString(i_ChildName));
			if (pItem)
				return get_action_id(pItem);
		}
	}
	return -1;
}

//---------------------------------------------------------------------------
//	GetMenuItemName()
//		give the name of the menu item with the given ID.
//---------------------------------------------------------------------------
std::string tqtMenuMgr::GetMenuItemName(int i_ObjectID)
{
	QAction* pItem = GetMenuItem( i_ObjectID );
	if (pItem)
	{
		return tqtStdString(plain_text(pItem->text()));
	}
	return "";
}

//---------------------------------------------------------------------------
//	GetMenuItem()
//		return the menu item with the given ID.
//---------------------------------------------------------------------------
QAction* tqtMenuMgr::GetMenuItem(int i_ObjectID)
{
	std::map<int, sMenuItemInfo>::iterator it = m_MenuItemMap.find(i_ObjectID);
	if (it != m_MenuItemMap.end())
	{
		return it->second.m_pMenuItem;
	}
	return NULL;
}

//---------------------------------------------------------------------------
// Set Help string to display when mouse is over given menu item
//---------------------------------------------------------------------------
void tqtMenuMgr::SetMenuItemHelpString( int i_ObjectID, const char * i_HelpString )
{
	QAction* pItem = GetMenuItem( i_ObjectID );
	if (pItem)
	{
		// shared with the toolbar button, if there is one
		pItem->setStatusTip( tqtQString(i_HelpString) );
	}
}

//---------------------------------------------------------------------------
// Change the text associated to a menu item to create a shortcut string
//---------------------------------------------------------------------------
void tqtMenuMgr::SetMenuItemShortcut(int i_ObjectID, const char* i_pShortcutString)
{
	QAction* pItem = GetMenuItem( i_ObjectID );
	if (!pItem)
		return;

	// Text after a tab is shown in the shortcut column of the menu.
	// The key is not given to Qt as a real shortcut: hot keys are
	// handled by tqtMessaging, and both would run the command.
	QString label = plain_text(pItem->text());
	if (i_pShortcutString && *i_pShortcutString)
	{
		label += QLatin1Char('\t');
		label += tqtQString(i_pShortcutString);
	}
	pItem->setText(label);
}

//---------------------------------------------------------------------------
// Callback for when the menu item is chosen
//---------------------------------------------------------------------------
void tqtMenuMgr::menuItem_Click(int i_ObjectID)
{
	std::map<int, sMenuItemInfo>::iterator it = m_MenuItemMap.find(i_ObjectID);
	if (it != m_MenuItemMap.end())
	{
		ControlCallback callback = it->second.m_ExecuteCallback;
		if (callback)
		{
			try
			{
				// Execute callback associated with menu item
				guiStatusBarMgr::ClearErrorMessage();
				(*callback)(i_ObjectID);
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

		// The command may have changed the state the item shows
		menuItem_Update(i_ObjectID);
	}
}

//---------------------------------------------------------------------------
// Callback for when the menu item needs to update state
//---------------------------------------------------------------------------
void tqtMenuMgr::menuItem_Update(int i_ObjectID)
{
	std::map<int, sMenuItemInfo>::iterator it = m_MenuItemMap.find(i_ObjectID);
	if (it != m_MenuItemMap.end())
	{
		ControlCallback callback = it->second.m_UpdateCallback;
		if (callback)
		{
			// Call update callback associated with menu item
			(*callback)(i_ObjectID);
		}
	}
}

//---------------------------------------------------------------------------
// Update the items in the menu each time before it is shown
//---------------------------------------------------------------------------
void tqtMenuMgr::watch_menu(QMenu* i_pMenu)
{
	if (i_pMenu && m_WatchedMenus.insert(i_pMenu).second)
	{
		m_Connections.push_back(
			QObject::connect(i_pMenu, &QMenu::aboutToShow, [this, i_pMenu]() { update_menu_items(i_pMenu); }));
		m_Connections.push_back(
			QObject::connect(i_pMenu, &QObject::destroyed, [this, i_pMenu]() { m_WatchedMenus.erase(i_pMenu); }));
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void tqtMenuMgr::update_menu_items(QMenu* i_pMenu)
{
	const QList<QAction*> actions = i_pMenu->actions();
	for (QAction* pAction : actions)
	{
		int id = get_action_id(pAction);
		if (id >= 0)
			menuItem_Update(id);
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void tqtMenuMgr::update_toolbar_items()
{
	if (!tqtSystem::g_pMainForm || !tqtSystem::g_pMainForm->isVisible())
		return;

	// copy the ids, an update callback could add menu items
	std::vector<int> ids;
	std::map<int, sMenuItemInfo>::iterator it;
	for (it = m_MenuItemMap.begin(); it != m_MenuItemMap.end(); ++it)
	{
		if (it->second.m_bOnToolbar && it->second.m_UpdateCallback)
			ids.push_back(it->first);
	}
	for (size_t i = 0; i < ids.size(); ++i)
	{
		menuItem_Update(ids[i]);
	}
}

#endif // USE_QT
