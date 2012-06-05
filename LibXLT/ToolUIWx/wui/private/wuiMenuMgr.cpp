/*****************************************************************************
**  wuiMenuMgr.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "ToolUIWx/wui/wuiMenuMgr.hpp"

#include "ToolUIWx/twx/twxMenuMgr.hpp"
#include "ToolUIWx/twx/twxSystem.hpp"
#include "ToolUIWx/twx/twxToolbarMgr.hpp"

namespace
{
#ifdef USE_WXWIDGETS
	twxMenuMgr *l_pMenuMgr = NULL;
#endif
}	

//===========================================================================
//	wuiMenuMgr functions
//===========================================================================

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
wuiMenuMgr::wuiMenuMgr()
{
#ifdef USE_WXWIDGETS
	l_pMenuMgr = new twxMenuMgr();
#endif
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
wuiMenuMgr::~wuiMenuMgr()
{
#ifdef USE_WXWIDGETS
	delete l_pMenuMgr;
	l_pMenuMgr = NULL;
#endif
}

//---------------------------------------------------------------------------
//	AttachEventToMenuObjects()
//---------------------------------------------------------------------------
void wuiMenuMgr::AttachEventToMenuObjects( int i_ObjectID, 
										  ControlCallback i_pFunction, 
										  ControlCallback i_pUpdate )
{
#ifdef USE_WXWIDGETS
	l_pMenuMgr->AttachEventToMenuObjects( i_ObjectID, i_pFunction, i_pUpdate );
#endif
}

//---------------------------------------------------------------------------
//	AddMenu()
//		add a menu or submenu to the Menu Tree.
//		set the childname to "" is adding a parent item.
//---------------------------------------------------------------------------
void wuiMenuMgr::AddMenu( const char * i_ParentName,
			  const char * i_ChildName )
{
#ifdef USE_WXWIDGETS
	l_pMenuMgr->AddMenu( i_ParentName, i_ChildName );
#endif // USE_WXWIDGETS
}

//---------------------------------------------------------------------------
//	AddMenuItem()
//		add a menu item to the Menu Tree.
//		set the childname to "" is adding a parent item.
//	the function returns a unique ObjectID for the GUI object.
//---------------------------------------------------------------------------
int wuiMenuMgr::AddMenuItem( const char * i_ParentName,
							  const char * i_ChildName,
						      const char * i_ToolbarName,
							  const char * i_ToolbarButton_ImageFilename )
{
	
#ifdef USE_WXWIDGETS
	const bool bCheckable = false;
	return l_pMenuMgr->AddMenuItem( i_ParentName, 
									i_ChildName, 
									i_ToolbarName, 
									i_ToolbarButton_ImageFilename, 
									bCheckable );
#else // USE_WXWIDGETS
	return -1;
#endif // USE_WXWIDGETS

}

//---------------------------------------------------------------------------
//	AddCheckableMenuItem()
//		add a menu item to the Menu Tree.
//		set the childname to "" is adding a parent item.
//	the function returns a unique ObjectID for the GUI object.
//---------------------------------------------------------------------------
int wuiMenuMgr::AddCheckableMenuItem( const char * i_ParentName,
									  const char * i_ChildName,
									  const char * i_ToolbarName,
									  const char * i_ToolbarButton_ImageFilename )
{
	
#ifdef USE_WXWIDGETS
	const bool bCheckable = true;
	return l_pMenuMgr->AddMenuItem( i_ParentName, i_ChildName, i_ToolbarName, i_ToolbarButton_ImageFilename, bCheckable );
#else // USE_WXWIDGETS
	return -1;
#endif // USE_WXWIDGETS

}

//---------------------------------------------------------------------------
//	RemoveMenuItem()
//		remove a menu item from the Menu Tree
//---------------------------------------------------------------------------
void wuiMenuMgr::RemoveMenuItem( const char * i_ParentName,
								 const char * i_ChildName  )
{
#ifdef USE_WXWIDGETS
	l_pMenuMgr->RemoveMenuItem( i_ParentName, i_ChildName );
#endif
}

//---------------------------------------------------------------------------
//	FindMenuItem()
//		returns whether or not the menu item can be found in the menu system
//---------------------------------------------------------------------------
bool wuiMenuMgr::FindMenuItem( const char * i_ParentName,
							   const char * i_ChildName  )
{
#ifdef USE_WXWIDGETS
	return l_pMenuMgr->FindMenuItem( i_ParentName, i_ChildName );
#endif
	return false;
}

//---------------------------------------------------------------------------
//	AddSeparator()
//		add a separator to the menu with the given name.
//---------------------------------------------------------------------------
void wuiMenuMgr::AddSeparator( const char * i_MenuName )
{
#ifdef USE_WXWIDGETS
	l_pMenuMgr->AddSeparator( i_MenuName );
#endif
}

//---------------------------------------------------------------------------
//	EnableMenuItem()
//		enable/disable a menu item
//---------------------------------------------------------------------------
void wuiMenuMgr::EnableMenuItem( const char * i_ParentName,
								 const char * i_ChildName,
								 bool i_bEnable )
{
#ifdef USE_WXWIDGETS
	l_pMenuMgr->EnableMenuItem( i_ParentName, i_ChildName, i_bEnable );
#endif
}

//---------------------------------------------------------------------------
//	GetMenuItemID()
//		the function returns an ID
//---------------------------------------------------------------------------
int wuiMenuMgr::GetMenuItemID( const char * i_ParentName,
							   const char * i_ChildName )
{
#ifdef USE_WXWIDGETS
	return l_pMenuMgr->GetMenuItemID(i_ParentName, i_ChildName);
#else
	return 0;
#endif
}

//---------------------------------------------------------------------------
//	GetMenuItemName()
//		give the name of the menu item with the given ID.
//---------------------------------------------------------------------------
std::string wuiMenuMgr::GetMenuItemName(int i_ObjectID)
{
#ifdef USE_WXWIDGETS
	return l_pMenuMgr->GetMenuItemName(i_ObjectID);
#else
	return std::string("");
#endif
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void wuiMenuMgr::SetMenuItemShortcut( int i_ObjectID, const char * i_ShortcutString )
{
#ifdef USE_WXWIDGETS
	l_pMenuMgr->SetMenuItemShortcut(i_ObjectID, i_ShortcutString);
#endif
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool wuiMenuMgr::IsValidShortcut( const char * i_ShortcutString )
{
	return true;
}

//---------------------------------------------------------------------------
// Set Help string to display when mouse is over given menu item
//---------------------------------------------------------------------------
void wuiMenuMgr::SetMenuItemHelpString( int i_ObjectID, const char * i_HelpString )
{
#ifdef USE_WXWIDGETS
	l_pMenuMgr->SetMenuItemHelpString( i_ObjectID, i_HelpString );
#endif
}

//---------------------------------------------------------------------------
//	get a pointer to a ControlCallback
//---------------------------------------------------------------------------
//ControlCallback* wuiMenuMgr::GetControlCallback( int i_ObjectID )
//{
//}

//---------------------------------------------------------------------------
//	MenuObjectsExist()
//		based on the menu item ID.
//
//		returns true if the ID refers to a valid tmaMenuObjects
//---------------------------------------------------------------------------
bool wuiMenuMgr::MenuObjectsExist( int i_ObjectID )
{
#ifdef USE_WXWIDGETS
	return l_pMenuMgr->MenuObjectsExist( i_ObjectID );
#else
	return false;
#endif
}

//---------------------------------------------------------------------------
//	MenuObjectsEnable()
//---------------------------------------------------------------------------
void wuiMenuMgr::MenuObjectsEnable( int i_ObjectID, bool i_bEnabled )
{
#ifdef USE_WXWIDGETS
	l_pMenuMgr->MenuObjectsEnable( i_ObjectID, i_bEnabled );
#endif
}

//---------------------------------------------------------------------------
//	MenuObjectsCheck()
//---------------------------------------------------------------------------
void wuiMenuMgr::MenuObjectsCheck( int i_ObjectID, bool i_bCheckd )
{
#ifdef USE_WXWIDGETS
	l_pMenuMgr->MenuObjectsCheck( i_ObjectID, i_bCheckd );
#endif
}


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void wuiMenuMgr::SetIconDirectory( std::vector<fsLocator>& i_IconPathList )
{
#ifdef USE_WXWIDGETS
	twxToolbarMgr::SetIconDirectory(i_IconPathList);
#endif
}
