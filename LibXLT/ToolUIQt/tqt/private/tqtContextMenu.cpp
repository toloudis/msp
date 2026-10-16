/*****************************************************************************
**	tqtContextMenu.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/tqt/tqtContextMenu.hpp"

#include "Core/dbg/dbgMsg.hpp"
#include "Core/env/envExceptionX.hpp"
#include "Tool/gui/guiMessageBox.hpp"
#include "Tool/gui/guiStatusBarMgr.hpp"

#ifdef USE_QT

namespace
{
	//-----------------------------------------------------------------------
	// Menu text without the mnemonic markers, for comparing names
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
			if (pAction->menu())
			{
				if (QAction* pFound = find_menu_item(pAction->menu(), i_MenuName))
					return pFound;
			}
			else if (!pAction->isSeparator() && plain_text(pAction->text()) == i_MenuName)
			{
				return pAction;
			}
		}
		return NULL;
	}

	//-----------------------------------------------------------------------
	// Find submenu somewhere within the given menu
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
}


//===========================================================================
//	tqtContextMenu functions
//===========================================================================

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
tqtContextMenu::tqtContextMenu()
:	m_NextId(1)
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
	QString parent_name = tqtQString(i_ParentName);

	// If child name is empty, create top-level menu
	if (!i_ChildName || !(*i_ChildName))
	{
		if (!find_menu(this, parent_name))
		{
			this->addMenu(parent_name);
		}
	}
	else
	{
		QString child_name = tqtQString(i_ChildName);
		QMenu* pMenu = find_menu(this, parent_name);
		if (pMenu)
		{
			// Need to make sure the same menu isn't added more than once
			bool bExists = false;
			for (QAction* pAction : pMenu->actions())
			{
				if (pAction->menu() && plain_text(pAction->text()) == child_name)
					bExists = true;
			}
			if (!bExists)
			{
				pMenu->addMenu(child_name);
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

	QMenu* pMenu = this;
	if (i_ParentName && (*i_ParentName))
	{
		pMenu = find_menu(this, tqtQString(i_ParentName));
	}

	if (pMenu)
	{
		// See if the menu item was already created (when laying out menus earlier)
		QString child_name = tqtQString(i_ChildName);
		QAction *pItem = find_menu_item(pMenu, child_name);
		if (!pItem)
		{
			pItem = pMenu->addAction(child_name);
		}

		m_MenuItemMap[pItem] = i_Callback;
		return m_NextId++;
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
	this->addSeparator();
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
// Show the menu and run the chosen item's callback once it has closed
//---------------------------------------------------------------------------
bool tqtContextMenu::Popup(QWidget* i_pWidget, const QPoint& i_Pos)
{
	QPoint global_pos = i_pWidget ? i_pWidget->mapToGlobal(i_Pos) : i_Pos;
	QAction* pChosen = this->exec(global_pos);
	if (pChosen)
	{
		menuItem_Click(pChosen);
		return true;
	}
	return false;
}

//---------------------------------------------------------------------------
// Callback for when the menu item is chosen
//---------------------------------------------------------------------------
void tqtContextMenu::menuItem_Click(QAction* i_pAction)
{
	std::map<QAction*, guiContextMenu::CallbackFunction>::iterator it = m_MenuItemMap.find(i_pAction);
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
