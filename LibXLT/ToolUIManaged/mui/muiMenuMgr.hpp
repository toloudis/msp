/*****************************************************************************
**  muiMenuMgr.hpp
**
**      Interface to the menu manager.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef MUI_MENUMGR_HPP
#error muiMenuMgr.hpp multiply included
#endif
#define MUI_MENUMGR_HPP

#ifndef GUI_MENUMGR_HPP
#include "Tool/gui/guiMenuMgr.hpp"
#endif

//============================================================================
//============================================================================
class muiMenuMgr : public guiMenuMgrImpl
{
public:
	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	muiMenuMgr();

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	~muiMenuMgr();

	//---------------------------------------------------------------------------
	//	AttachEventToMenuObjects()
	//---------------------------------------------------------------------------
	virtual void AttachEventToMenuObjects( int i_ObjectID, 
								   ControlCallback i_pFunction, 
								   ControlCallback i_pUpdate = 0 );

	//---------------------------------------------------------------------------
	//	AddMenu()
	//		add a menu or submenu to the Menu Tree.
	//		set the childname to "" is adding a parent item.
	//---------------------------------------------------------------------------
	virtual void AddMenu( const char * i_ParentName,
				  const char * i_ChildName );

	//---------------------------------------------------------------------------
	//	AddMenuItem()
	//		add a menu item to the Menu Tree.
	//	the function returns an ID
	//---------------------------------------------------------------------------
	virtual int AddMenuItem( const char * i_ParentName,
					 const char * i_ChildName,
				     const char * i_ToolbarName = 0,
					 const char * i_ToolbarButton_ImageFilename = 0 );

	//---------------------------------------------------------------------------
	//	AddCheckableMenuItem()
	//		add a menu item that can be checked to the Menu Tree.
	//	the function returns an ID
	//---------------------------------------------------------------------------
	virtual int AddCheckableMenuItem( const char * i_ParentName,
					 const char * i_ChildName,
				     const char * i_ToolbarName = 0,
					 const char * i_ToolbarButton_ImageFilename = 0 );

	//---------------------------------------------------------------------------
	//	RemoveMenuItem()
	//		remove a menu item from the Menu Tree + toolbar
	//---------------------------------------------------------------------------
	virtual void RemoveMenuItem( const char * i_ParentName,
						 const char * i_ChildName  );

	//---------------------------------------------------------------------------
	//	AddSeparator()
	//		add a separator to the menu with the given name.
	//---------------------------------------------------------------------------
	virtual void AddSeparator( const char * i_MenuName );

	//---------------------------------------------------------------------------
	//	EnableMenuItem()
	//		enable/disable a menu item
	//---------------------------------------------------------------------------
	virtual void EnableMenuItem( const char * i_ParentName,
						 const char * i_ChildName,
						 bool i_bEnable = true );

	//---------------------------------------------------------------------------
	//	GetMenuItemID()
	//		the function returns an ID
	//---------------------------------------------------------------------------
	virtual int GetMenuItemID( const char * i_ParentName,
					   const char * i_ChildName );

	//---------------------------------------------------------------------------
	//	GetMenuItemName()
	//		give the name of the menu item with the given ID.
	//---------------------------------------------------------------------------
	virtual std::string GetMenuItemName(int i_ObjectID);

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	virtual void SetMenuItemShortcut( int i_ObjectID, const char * i_ShortcutString );

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	virtual bool IsValidShortcut( const char * i_ShortcutString );

	//---------------------------------------------------------------------------
	// Set Help string to display when mouse is over given menu item
	//---------------------------------------------------------------------------
	virtual void SetMenuItemHelpString( int i_ObjectID, const char * i_HelpString );

	//---------------------------------------------------------------------------
	//	get a pointer to a ControlCallback
	//---------------------------------------------------------------------------
	//virtual ControlCallback* GetControlCallback( int i_ObjectID );

	//---------------------------------------------------------------------------
	//	MenuObjectsExist()
	//		based on the menu item ID.
	//
	//		returns true if the ID refers to a valid object
	//---------------------------------------------------------------------------
	virtual bool MenuObjectsExist( int i_ObjectID );

	//---------------------------------------------------------------------------
	//	MenuObjectsEnable()
	//---------------------------------------------------------------------------
	virtual void MenuObjectsEnable( int i_ObjectID, bool i_bEnabled );

	//---------------------------------------------------------------------------
	//	MenuObjectsCheck()
	//---------------------------------------------------------------------------
	virtual void MenuObjectsCheck( int i_ObjectID, bool i_bCheckd );

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	virtual void SetIconDirectory( const char * i_IconDirectory );
};

