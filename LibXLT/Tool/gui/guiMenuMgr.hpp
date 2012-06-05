/*****************************************************************************
**	guiMenuMgr.hpp
**
**		A non-managed interface to the managed menu manager.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef GUI_MENUMGR_HPP
#error guiMenuMgr.hpp multiply included
#endif
#define GUI_MENUMGR_HPP

#ifndef ENV_ABSTRACTION_HPP
#include "Core/env/envAbstraction.hpp"
#endif
#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif
#ifndef GUI_CONSTANTS_HPP
#include "Tool/gui/guiConstants.hpp"
#endif

#include <string>
#include <vector>


//============================================================================
// forward declaration
//============================================================================
class guiMenuMgrImpl;


//============================================================================
// static functions define API
//============================================================================
class guiMenuMgr : public envAbstraction<guiMenuMgrImpl>
{
public:
	//---------------------------------------------------------------------------
	//	AttachEventToMenuObjects()
	//---------------------------------------------------------------------------
	static void AttachEventToMenuObjects( int i_ObjectID, 
										   ControlCallback i_pFunction, 
										   ControlCallback i_pUpdate = 0 );

	//---------------------------------------------------------------------------
	//	AddMenu()
	//		add a menu or submenu to the Menu Tree.
	//		set the childname to "" is adding a parent item.
	//---------------------------------------------------------------------------
	static void AddMenu( const char * i_ParentName,
						  const char * i_ChildName );

	//---------------------------------------------------------------------------
	//	AddMenuItem()
	//		add a menu item to the Menu Tree.
	//	the function returns an ID
	//---------------------------------------------------------------------------
	static int AddMenuItem( const char * i_ParentName,
							 const char * i_ChildName,
							 const char * i_ToolbarName = 0,
							 const char * i_ToolbarButton_ImageFilename = 0 );

	//---------------------------------------------------------------------------
	//	AddCheckableMenuItem()
	//		add a menu item that can be checked to the Menu Tree.
	//	the function returns an ID
	//---------------------------------------------------------------------------
	static int AddCheckableMenuItem( const char * i_ParentName,
									 const char * i_ChildName,
									 const char * i_ToolbarName = 0,
									 const char * i_ToolbarButton_ImageFilename = 0 );

	//---------------------------------------------------------------------------
	//	RemoveMenuItem()
	//		remove a menu item from the Menu Tree + toolbar
	//---------------------------------------------------------------------------
	static void RemoveMenuItem( const char * i_ParentName,
								const char * i_ChildName  );

	//---------------------------------------------------------------------------
	//	FindMenuItem()
	//		returns whether or not the menu item can be found in the menu system
	//---------------------------------------------------------------------------
	static bool FindMenuItem( const char * i_ParentName,
							  const char * i_ChildName  );

	//---------------------------------------------------------------------------
	//	AddSeparator()
	//		add a separator to the menu with the given name.
	//---------------------------------------------------------------------------
	static void AddSeparator( const char * i_MenuName );

	//---------------------------------------------------------------------------
	//	EnableMenuItem()
	//		enable/disable a menu item
	//---------------------------------------------------------------------------
	static void EnableMenuItem( const char * i_ParentName,
								 const char * i_ChildName,
								 bool i_bEnable = true );

	//---------------------------------------------------------------------------
	//	GetMenuItemID()
	//		the function returns an ID
	//---------------------------------------------------------------------------
	static int GetMenuItemID( const char * i_ParentName,
							   const char * i_ChildName );

	//---------------------------------------------------------------------------
	//	GetMenuItemName()
	//		give the name of the menu item with the given ID.
	//---------------------------------------------------------------------------
	static std::string GetMenuItemName(int i_ObjectID);

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	static void SetMenuItemShortcut( int i_ObjectID, const char * i_ShortcutString );

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	static bool IsValidShortcut( const char * i_ShortcutString );

	//---------------------------------------------------------------------------
	// Set Help string to display when mouse is over given menu item
	//---------------------------------------------------------------------------
	static void SetMenuItemHelpString( int i_ObjectID, const char * i_HelpString );

	//---------------------------------------------------------------------------
	//	get a pointer to a ControlCallback
	//---------------------------------------------------------------------------
	//static ControlCallback* GetControlCallback( int i_ObjectID );

	//---------------------------------------------------------------------------
	//	MenuObjectsExist()
	//		based on the menu item ID.
	//
	//		returns true if the ID refers to a valid object
	//---------------------------------------------------------------------------
	static bool MenuObjectsExist( int i_ObjectID );

	//---------------------------------------------------------------------------
	//	MenuObjectsEnable()
	//---------------------------------------------------------------------------
	static void MenuObjectsEnable( int i_ObjectID, bool i_bEnabled );

	//---------------------------------------------------------------------------
	//	MenuObjectsCheck()
	//---------------------------------------------------------------------------
	static void MenuObjectsCheck( int i_ObjectID, bool i_bCheckd );

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	static void SetIconDirectory( std::vector<fsLocator>& i_IconPathList );
	static const fsLocator& GetIconDirectory();
	static const std::vector<fsLocator>& GetIconPathList();
};

//============================================================================
// Implementation class, needs to be derived in implementation library
//============================================================================
class guiMenuMgrImpl
{
public:
	//---------------------------------------------------------------------------
	//	AttachEventToMenuObjects()
	//---------------------------------------------------------------------------
	virtual void AttachEventToMenuObjects( int i_ObjectID, 
										   ControlCallback i_pFunction, 
										   ControlCallback i_pUpdate )= 0;

	//---------------------------------------------------------------------------
	//	AddMenu()
	//		add a menu or submenu to the Menu Tree.
	//		set the childname to "" is adding a parent item.
	//---------------------------------------------------------------------------
	virtual void AddMenu( const char * i_ParentName,
						  const char * i_ChildName )= 0;

	//---------------------------------------------------------------------------
	//	AddMenuItem()
	//		add a menu item to the Menu Tree.
	//	the function returns an ID
	//---------------------------------------------------------------------------
	virtual int AddMenuItem( const char * i_ParentName,
							 const char * i_ChildName,
							 const char * i_ToolbarName,
							 const char * i_ToolbarButton_ImageFilename)= 0;

	//---------------------------------------------------------------------------
	//	AddCheckableMenuItem()
	//		add a menu item that can be checked to the Menu Tree.
	//	the function returns an ID
	//---------------------------------------------------------------------------
	virtual int AddCheckableMenuItem( const char * i_ParentName,
									 const char * i_ChildName,
									 const char * i_ToolbarName,
									 const char * i_ToolbarButton_ImageFilename ) = 0;

	//---------------------------------------------------------------------------
	//	RemoveMenuItem()
	//		remove a menu item from the Menu Tree + toolbar
	//---------------------------------------------------------------------------
	virtual void RemoveMenuItem( const char * i_ParentName,
								 const char * i_ChildName  )= 0;

	//---------------------------------------------------------------------------
	//	FindMenuItem()
	//		returns whether or not the menu item can be found in the menu system
	//---------------------------------------------------------------------------
	virtual bool FindMenuItem( const char * i_ParentName,
							  const char * i_ChildName  ) = 0;

	//---------------------------------------------------------------------------
	//	AddSeparator()
	//		add a separator to the menu with the given name.
	//---------------------------------------------------------------------------
	virtual void AddSeparator( const char * i_MenuName ) = 0;

	//---------------------------------------------------------------------------
	//	EnableMenuItem()
	//		enable/disable a menu item
	//---------------------------------------------------------------------------
	virtual void EnableMenuItem( const char * i_ParentName,
								 const char * i_ChildName,
								 bool i_bEnable )= 0;

	//---------------------------------------------------------------------------
	//	GetMenuItemID()
	//		the function returns an ID
	//---------------------------------------------------------------------------
	virtual int GetMenuItemID( const char * i_ParentName,
							   const char * i_ChildName )= 0;

	//---------------------------------------------------------------------------
	//	GetMenuItemName()
	//		give the name of the menu item with the given ID.
	//---------------------------------------------------------------------------
	virtual std::string GetMenuItemName(int i_ObjectID)= 0;

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	virtual void SetMenuItemShortcut( int i_ObjectID, const char * i_ShortcutString )= 0;

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	virtual bool IsValidShortcut( const char * i_ShortcutString )= 0;

	//---------------------------------------------------------------------------
	// Set Help string to display when mouse is over given menu item
	//---------------------------------------------------------------------------
	virtual void SetMenuItemHelpString( int i_ObjectID, const char * i_HelpString )= 0;

	//---------------------------------------------------------------------------
	//	get a pointer to a ControlCallback
	//---------------------------------------------------------------------------
	//virtual ControlCallback* GetControlCallback( int i_ObjectID )= 0;

	//---------------------------------------------------------------------------
	//	MenuObjectsExist()
	//		based on the menu item ID.
	//
	//		returns true if the ID refers to a valid object
	//---------------------------------------------------------------------------
	virtual bool MenuObjectsExist( int i_ObjectID )= 0;

	//---------------------------------------------------------------------------
	//	MenuObjectsEnable()
	//---------------------------------------------------------------------------
	virtual void MenuObjectsEnable( int i_ObjectID, bool i_bEnabled )= 0;

	//---------------------------------------------------------------------------
	//	MenuObjectsCheck()
	//---------------------------------------------------------------------------
	virtual void MenuObjectsCheck( int i_ObjectID, bool i_bCheckd )= 0;

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	virtual void SetIconDirectory( std::vector<fsLocator>& i_IconPathList )= 0;
};

