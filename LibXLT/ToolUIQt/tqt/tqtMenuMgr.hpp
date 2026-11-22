/*****************************************************************************
**  tqtMenuMgr.hpp
**
**      Qt menu manager. Menu items are QActions; a toolbar button for a
**	menu item reuses the same QAction, so enabling or checking the item
**	updates both.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef TQT_MENUMGR_HPP
#error tqtMenuMgr.hpp multiply included
#endif
#define TQT_MENUMGR_HPP

#ifndef TQT_WIDGETS_HPP
#include "ToolUIQt/tqt/tqtWidgets.hpp"
#endif

#ifndef GUI_CONSTANTS_HPP
#include "Tool/gui/guiConstants.hpp"
#endif

#include <map>
#include <set>
#include <vector>

#ifdef USE_QT

//============================================================================
//============================================================================
class tqtMenuMgr
{
public:
	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	tqtMenuMgr();

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	~tqtMenuMgr();

	//---------------------------------------------------------------------------
	//	AddMenu()
	//		add a menu or submenu to the Menu Tree.
	//		set the childname to "" is adding a parent item.
	//---------------------------------------------------------------------------
	void AddMenu( const char * i_ParentName,
				  const char * i_ChildName );

	//---------------------------------------------------------------------------
	//	AddMenuItem()
	//		add a menu item to the Menu Tree.
	//		set the toolbar name to "" to not add a toolbar button
	//	the function returns a unique ObjectID for the GUI object.
	//---------------------------------------------------------------------------
	int AddMenuItem( const char * i_ParentName,
					 const char * i_ChildName,
				     const char * i_ToolbarName = NULL,
					 const char * i_ToolbarButton_ImageFilename = NULL,
					 bool i_bCheckable = false );

	//---------------------------------------------------------------------------
	//	RemoveMenuItem()
	//		remove a menu item from the Menu Tree
	//---------------------------------------------------------------------------
	void RemoveMenuItem( const char * i_ParentName,
						 const char * i_ChildName  );

	//---------------------------------------------------------------------------
	//	FindMenuItem()
	//		returns whether or not the menu item can be found in the menu system
	//---------------------------------------------------------------------------
	bool FindMenuItem( const char * i_ParentName,
					   const char * i_ChildName  );

	//---------------------------------------------------------------------------
	//	AddSeparator()
	//		add a separator to the menu with the given name.
	//---------------------------------------------------------------------------
	void AddSeparator( const char * i_MenuName );

	//---------------------------------------------------------------------------
	//	AttachEventToMenuObjects()
	//---------------------------------------------------------------------------
	void AttachEventToMenuObjects( int i_ObjectID,
								   ControlCallback i_pFunction,
								   ControlCallback i_pUpdate );

	//---------------------------------------------------------------------------
	//	MenuObjectsExist()
	//		based on the menu item ID.
	//		returns true if the ID refers to a valid menu item
	//---------------------------------------------------------------------------
	bool MenuObjectsExist( int i_ObjectID );

	//---------------------------------------------------------------------------
	//	MenuObjectsEnable() / MenuObjectsCheck()
	//---------------------------------------------------------------------------
	void MenuObjectsEnable( int i_ObjectID, bool i_bEnabled );
	void MenuObjectsCheck( int i_ObjectID, bool i_bChecked );

	//---------------------------------------------------------------------------
	//	EnableMenuItem()
	//		enable/disable a menu item
	//---------------------------------------------------------------------------
	void EnableMenuItem( const char * i_ParentName,
						 const char * i_ChildName,
						 bool i_bEnable );

	//---------------------------------------------------------------------------
	//	GetMenuItemID()
	//		the function returns an ID, or -1 if not found
	//---------------------------------------------------------------------------
	int GetMenuItemID( const char * i_ParentName,
					   const char * i_ChildName );

	//---------------------------------------------------------------------------
	//	GetMenuItemName()
	//		give the name of the menu item with the given ID.
	//---------------------------------------------------------------------------
	std::string GetMenuItemName(int i_ObjectID);

	//---------------------------------------------------------------------------
	//	GetMenuItem()
	//		return the menu item with the given ID.
	//---------------------------------------------------------------------------
	QAction* GetMenuItem(int i_ObjectID);

	//---------------------------------------------------------------------------
	// Set Help string to display when mouse is over given menu item
	//---------------------------------------------------------------------------
	void SetMenuItemHelpString( int i_ObjectID, const char * i_HelpString );

	//---------------------------------------------------------------------------
	// Change the text associated to a menu item to show a shortcut string.
	//	The hot key itself is handled by tqtMessaging, not by Qt.
	//---------------------------------------------------------------------------
	void SetMenuItemShortcut(int i_ObjectID, const char* i_pShortcutString);

private:
	//---------------------------------------------------------------------------
	// Callbacks for menu items
	//---------------------------------------------------------------------------
	void menuItem_Click(int i_ObjectID);
	void menuItem_Update(int i_ObjectID);

	//---------------------------------------------------------------------------
	// Run the update callbacks of the items in the menu (before it shows)
	// or on toolbars (periodically, as wxWidgets did in idle time).
	//---------------------------------------------------------------------------
	void watch_menu(QMenu* i_pMenu);
	void update_menu_items(QMenu* i_pMenu);
	void update_toolbar_items();

	struct sMenuItemInfo
	{
		// Null once the main window (and its menus) is destroyed, which
		// happens before the application stops sending menu updates
		QPointer<QAction>	m_pMenuItem;
		ControlCallback m_ExecuteCallback;
		ControlCallback m_UpdateCallback;
		bool		m_bOnToolbar;
	};
	std::map<int, sMenuItemInfo> m_MenuItemMap;
	std::set<QMenu*> m_WatchedMenus;
	std::vector<QMetaObject::Connection> m_Connections;
	QTimer* m_pToolbarUpdateTimer;
};

#endif // USE_QT
