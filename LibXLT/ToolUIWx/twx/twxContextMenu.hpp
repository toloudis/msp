/*****************************************************************************
**  twxContextMenu.hpp
**
**      Implementation of a context menu for wxWidgets
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef TWX_CONTEXTMENU_HPP
#error twxContextMenu.hpp multiply included
#endif
#define TWX_CONTEXTMENU_HPP

#ifndef GUI_CONTEXTMENU_HPP
#include "Tool/gui/guiContextMenu.hpp"
#endif

#ifndef TWX_WIDGETS_HPP
#include "ToolUIWx/twx/twxWidgets.hpp"
#endif

#include <map>

#ifdef USE_WXWIDGETS

//============================================================================
//============================================================================
class twxContextMenu : public wxMenu,
					   public guiContextMenu
{
public:
	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	twxContextMenu();

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	~twxContextMenu();

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

private:
	//---------------------------------------------------------------------------
	// Callback for when the menu item is chosen
	//---------------------------------------------------------------------------
	void menuItem_Click(wxCommandEvent& i_Event);

	std::map<int, guiContextMenu::CallbackFunction> m_MenuItemMap;
};

#endif	// USE_WXWIDGETS
