/*****************************************************************************
**  muiMenuMgr.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "ToolUIManaged/mui/muiMenuMgr.hpp"

#include "ToolUIManaged/tma/tmaMenuMgr.hpp"

namespace
{
}	

//===========================================================================
//	muiMenuMgr functions
//===========================================================================

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
muiMenuMgr::muiMenuMgr()
{
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
muiMenuMgr::~muiMenuMgr()
{
}

//---------------------------------------------------------------------------
//	AttachEventToMenuObjects()
//---------------------------------------------------------------------------
void muiMenuMgr::AttachEventToMenuObjects( int i_ObjectID, 
										  ControlCallback i_pFunction, 
										  ControlCallback i_pUpdate )
{
}

//---------------------------------------------------------------------------
//	AddMenu()
//		add a menu or submenu to the Menu Tree.
//		set the childname to "" is adding a parent item.
//---------------------------------------------------------------------------
void muiMenuMgr::AddMenu( const char * i_ParentName,
			  const char * i_ChildName )
{
	
}

//---------------------------------------------------------------------------
//	AddMenuItem()
//		add a menu item to the Menu Tree.
//		set the childname to "" is adding a parent item.
//	the function returns a unique ObjectID for the GUI object.
//---------------------------------------------------------------------------
int muiMenuMgr::AddMenuItem( const char * i_ParentName,
							  const char * i_ChildName,
						      const char * i_ToolbarName,
							  const char * i_ToolbarButton_ImageFilename )
{
	return -1;
}

//---------------------------------------------------------------------------
//	AddCheckableMenuItem()
//		add a menu item to the Menu Tree.
//		set the childname to "" is adding a parent item.
//	the function returns a unique ObjectID for the GUI object.
//---------------------------------------------------------------------------
int muiMenuMgr::AddCheckableMenuItem( const char * i_ParentName,
							  const char * i_ChildName,
						      const char * i_ToolbarName,
							  const char * i_ToolbarButton_ImageFilename )
{
	// .NET does not handle checkable menu items differently, 
	// just call main AddMenuItem function
	return AddMenuItem(i_ParentName, i_ChildName, i_ToolbarName, i_ToolbarButton_ImageFilename);
} 

//---------------------------------------------------------------------------
//	RemoveMenuItem()
//		remove a menu item from the Menu Tree
//---------------------------------------------------------------------------
void muiMenuMgr::RemoveMenuItem( const char * i_ParentName,
								 const char * i_ChildName  )
{
}

//---------------------------------------------------------------------------
//	AddSeparator()
//		add a separator to the menu with the given name.
//---------------------------------------------------------------------------
void muiMenuMgr::AddSeparator( const char * i_MenuName )
{

}

//---------------------------------------------------------------------------
//	EnableMenuItem()
//		enable/disable a menu item
//---------------------------------------------------------------------------
void muiMenuMgr::EnableMenuItem( const char * i_ParentName,
								 const char * i_ChildName,
								 bool i_bEnable )
{
}

//---------------------------------------------------------------------------
//	GetMenuItemID()
//		the function returns an ID
//---------------------------------------------------------------------------
int muiMenuMgr::GetMenuItemID( const char * i_ParentName,
							   const char * i_ChildName )
{

	return 0;
}

//---------------------------------------------------------------------------
//	GetMenuItemName()
//		give the name of the menu item with the given ID.
//---------------------------------------------------------------------------
std::string muiMenuMgr::GetMenuItemName(int i_ObjectID)
{
	return std::string("");
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void muiMenuMgr::SetMenuItemShortcut( int i_ObjectID, const char * i_ShortcutString )
{
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool muiMenuMgr::IsValidShortcut( const char * i_ShortcutString )
{
	return false;
}

//---------------------------------------------------------------------------
// Set Help string to display when mouse is over given menu item
//---------------------------------------------------------------------------
void muiMenuMgr::SetMenuItemHelpString( int i_ObjectID, const char * i_HelpString )
{
	// no implementation in managed code
}

//---------------------------------------------------------------------------
//	get a pointer to a ControlCallback
//---------------------------------------------------------------------------
//ControlCallback* muiMenuMgr::GetControlCallback( int i_ObjectID )
//{
//}

//---------------------------------------------------------------------------
//	MenuObjectsExist()
//		based on the menu item ID.
//
//		returns true if the ID refers to a valid tmaMenuObjects
//---------------------------------------------------------------------------
bool muiMenuMgr::MenuObjectsExist( int i_ObjectID )
{
	return false;
}

//---------------------------------------------------------------------------
//	MenuObjectsEnable()
//---------------------------------------------------------------------------
void muiMenuMgr::MenuObjectsEnable( int i_ObjectID, bool i_bEnabled )
{
}

//---------------------------------------------------------------------------
//	MenuObjectsCheck()
//---------------------------------------------------------------------------
void muiMenuMgr::MenuObjectsCheck( int i_ObjectID, bool i_bCheckd )
{
}


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void muiMenuMgr::SetIconDirectory( const char * i_IconDirectory )
{
}
