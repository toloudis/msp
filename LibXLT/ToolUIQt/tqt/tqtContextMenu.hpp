/*****************************************************************************
**	tqtContextMenu.hpp
**
**		Qt implementation of a context menu that can be filled by
**	the guiContextMenu interface.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef TQT_CONTEXTMENU_HPP
#error tqtContextMenu.hpp multiply included
#endif
#define TQT_CONTEXTMENU_HPP

#ifndef GUI_CONTEXTMENU_HPP
#include "Tool/gui/guiContextMenu.hpp"
#endif

#ifndef TQT_WIDGETS_HPP
#include "ToolUIQt/tqt/tqtWidgets.hpp"
#endif

#include <map>

#ifdef USE_QT

//============================================================================
//============================================================================
class tqtContextMenu : public QMenu,
					   public guiContextMenu
{
public:
	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	tqtContextMenu();

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	~tqtContextMenu();

	//---------------------------------------------------------------------------
	//	AddMenu()
	//		add a menu or submenu to the Menu Tree.
	//		set the childname to "" is adding a parent item.
	//---------------------------------------------------------------------------
	virtual void AddMenu( const char * i_ParentName,
						  const char * i_ChildName );

	//---------------------------------------------------------------------------
	//	AddMenuItem()
	//		add a menu item to the Menu Tree which will call the given
	//	callback function when clicked.
	//---------------------------------------------------------------------------
	virtual int AddMenuItem( const char * i_ParentName,
							 const char * i_ChildName,
							 guiContextMenu::CallbackFunction i_Callback );

	//---------------------------------------------------------------------------
	//	AddSeparator()
	//		add a separator to the menu with the given name.
	//---------------------------------------------------------------------------
	virtual void AddSeparator( const char * i_MenuName );

	//---------------------------------------------------------------------------
	// Returns true if the context menu does not have any menus added.
	//---------------------------------------------------------------------------
	virtual bool IsEmpty();

	//---------------------------------------------------------------------------
	// Show the menu at the given position in i_pWidget's coordinates and
	//	run the callback of the chosen item after the menu has closed.
	//	Returns true if an item was chosen.
	//---------------------------------------------------------------------------
	bool Popup(QWidget* i_pWidget, const QPoint& i_Pos);

private:
	//---------------------------------------------------------------------------
	// Run the callback for the chosen menu item
	//---------------------------------------------------------------------------
	void menuItem_Click(QAction* i_pAction);

	std::map<QAction*, guiContextMenu::CallbackFunction> m_MenuItemMap;
	int m_NextId;
};

#endif	// USE_QT
