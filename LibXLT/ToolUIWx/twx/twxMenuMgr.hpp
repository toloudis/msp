/*****************************************************************************
**  twxMenuMgr.hpp
**
**      The main application menu manager
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef TWX_MENUMGR_HPP
#error twxMenuMgr.hpp multiply included
#endif
#define TWX_MENUMGR_HPP

#ifndef TWX_WIDGETS_HPP
#include "ToolUIWx/twx/twxWidgets.hpp"
#endif
#ifndef GUI_CONSTANTS_HPP
#include "Tool/gui/guiConstants.hpp"
#endif

#include <map>

#ifdef USE_WXWIDGETS

//============================================================================
//============================================================================
class twxMenuMgr : public wxEvtHandler
{
public:
	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	twxMenuMgr();

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	~twxMenuMgr();

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
	//
	//		returns true if the ID refers to a valid tmaMenuObjects
	//---------------------------------------------------------------------------
	bool MenuObjectsExist( int i_ObjectID );

	//---------------------------------------------------------------------------
	//	MenuObjectsEnable()
	//---------------------------------------------------------------------------
	void MenuObjectsEnable( int i_ObjectID, bool i_bEnabled );

	//---------------------------------------------------------------------------
	//	MenuObjectsCheck()
	//---------------------------------------------------------------------------
	void MenuObjectsCheck( int i_ObjectID, bool i_bChecked );

	//---------------------------------------------------------------------------
	//	EnableMenuItem()
	//		enable or disable a menu item from the Menu Tree
	//---------------------------------------------------------------------------
	void EnableMenuItem( const char * i_ParentName,
						const char * i_ChildName,
						bool i_bEnable );

	//---------------------------------------------------------------------------
	//	GetMenuItemID()
	//		get the menu item ID
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
	wxMenuItem* twxMenuMgr::GetMenuItem(int i_ObjectID);

	//---------------------------------------------------------------------------
	// Set Help string to display when mouse is over given menu item
	//---------------------------------------------------------------------------
	void SetMenuItemHelpString( int i_ObjectID, const char * i_HelpString );

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	void SetMenuItemShortcut(int i_ObjectID, const char* i_pShortcutString);

/*

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	tmaMenuObjects^ GetMenuObjects( int i_ObjectID );

	//---------------------------------------------------------------------------
	//	AttachCallbacks()
	//---------------------------------------------------------------------------
	void AttachCallbacks( int i_ObjectID );

	//---------------------------------------------------------------------------
	//	GetMenuObjectsList()
	//---------------------------------------------------------------------------
	ArrayList^ GetMenuObjectsList();

	//---------------------------------------------------------------------------
	//	AddDesignerMenu()
	//---------------------------------------------------------------------------
	void AddDesignerMenu(System::Windows::Forms::ToolStripMenuItem ^pMenu);

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	void SetIconDirectory( const char * i_IconDirectory );

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	void SetMenuItemShortcut(int i_ObjectID, String^ i_pShortcutString);

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	bool IsValidShortcut( String^ i_ShortcutString );

private:
	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	void GetDropDownItemShortcut(MenuItem^ i_pMenuItem, String^ o_pShortcutString);

	//---------------------------------------------------------------------------
	//	generate a menu item
	//---------------------------------------------------------------------------
	tmaMenuObjects^ generate_menuitem(ToolStripItem^ i_pTSI);


	//===========================================================================
	//	twxMenuMgr functions
	//===========================================================================

	//---------------------------------------------------------------------------
	// Execution event handler
	//---------------------------------------------------------------------------
	void menuItem_Click( Object^ Sender, System::EventArgs^ e );
	//---------------------------------------------------------------------------
	// Popup event handler
	//---------------------------------------------------------------------------
	void menuItem_Popup(System::Object ^  Sender, System::EventArgs ^  e);

private:
	ArrayList^ l_pMenuObjects;	// tmaMenuObjects

	System::String^	m_IconDir;
	*/

	//---------------------------------------------------------------------------
	// Callback for when the menu item is chosen
	//---------------------------------------------------------------------------
	void menuItem_Click(wxCommandEvent& i_Event);

	//---------------------------------------------------------------------------
	// Callback for when the menu item needs to update state
	//---------------------------------------------------------------------------
	void menuItem_Update(wxUpdateUIEvent& i_Event);

private:
	struct sMenuItemInfo
	{
		wxMenuItem*	m_pMenuItem;
		ControlCallback m_ExecuteCallback;
		ControlCallback m_UpdateCallback;
	};
	std::map<int, sMenuItemInfo> m_MenuItemMap;
};

#endif // USE_WXWIDGETS
