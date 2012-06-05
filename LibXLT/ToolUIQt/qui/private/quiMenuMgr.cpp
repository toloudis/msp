/*****************************************************************************
**  quiMenuMgr.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/qui/quiMenuMgr.hpp"

#include "ToolUIQt/tqt/tqtMenuMgr.hpp"
#include "ToolUIQt/tqt/tqtSystem.hpp"
#include "ToolUIQt/tqt/tqtToolbarMgr.hpp"

namespace
{
#ifdef QT_FINISH_PORT
	tqtMenuMgr *l_pMenuMgr = NULL;
#endif
}	

//===========================================================================
//	quiMenuMgr functions
//===========================================================================

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
quiMenuMgr::quiMenuMgr()
{
#ifdef QT_FINISH_PORT
	l_pMenuMgr = new tqtMenuMgr();
#endif
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
quiMenuMgr::~quiMenuMgr()
{
#ifdef QT_FINISH_PORT
	delete l_pMenuMgr;
	l_pMenuMgr = NULL;
#endif
}

//---------------------------------------------------------------------------
//	AttachEventToMenuObjects()
//---------------------------------------------------------------------------
void quiMenuMgr::AttachEventToMenuObjects( int i_ObjectID, 
										  ControlCallback i_pFunction, 
										  ControlCallback i_pUpdate )
{
#ifdef QT_FINISH_PORT
	l_pMenuMgr->AttachEventToMenuObjects( i_ObjectID, i_pFunction, i_pUpdate );
#endif
}

//---------------------------------------------------------------------------
//	AddMenu()
//		add a menu or submenu to the Menu Tree.
//		set the childname to "" is adding a parent item.
//---------------------------------------------------------------------------
void quiMenuMgr::AddMenu( const char * i_ParentName,
			  const char * i_ChildName )
{
#ifdef QT_FINISH_PORT
	l_pMenuMgr->AddMenu( i_ParentName, i_ChildName );
#endif // USE_QT
}

//---------------------------------------------------------------------------
//	AddMenuItem()
//		add a menu item to the Menu Tree.
//		set the childname to "" is adding a parent item.
//	the function returns a unique ObjectID for the GUI object.
//---------------------------------------------------------------------------
int quiMenuMgr::AddMenuItem( const char * i_ParentName,
							  const char * i_ChildName,
						      const char * i_ToolbarName,
							  const char * i_ToolbarButton_ImageFilename )
{
	
#ifdef QT_FINISH_PORT
	const bool bCheckable = false;
	return l_pMenuMgr->AddMenuItem( i_ParentName, 
									i_ChildName, 
									i_ToolbarName, 
									i_ToolbarButton_ImageFilename, 
									bCheckable );
#else // USE_WXWIDGETS
	return -1;
#endif // USE_QT

}

//---------------------------------------------------------------------------
//	AddCheckableMenuItem()
//		add a menu item to the Menu Tree.
//		set the childname to "" is adding a parent item.
//	the function returns a unique ObjectID for the GUI object.
//---------------------------------------------------------------------------
int quiMenuMgr::AddCheckableMenuItem( const char * i_ParentName,
									  const char * i_ChildName,
									  const char * i_ToolbarName,
									  const char * i_ToolbarButton_ImageFilename )
{
	
#ifdef QT_FINISH_PORT
	const bool bCheckable = true;
	return l_pMenuMgr->AddMenuItem( i_ParentName, i_ChildName, i_ToolbarName, i_ToolbarButton_ImageFilename, bCheckable );
#else // USE_WXWIDGETS
	return -1;
#endif // USE_QT

}

//---------------------------------------------------------------------------
//	RemoveMenuItem()
//		remove a menu item from the Menu Tree
//---------------------------------------------------------------------------
void quiMenuMgr::RemoveMenuItem( const char * i_ParentName,
								 const char * i_ChildName  )
{
#ifdef QT_FINISH_PORT
	l_pMenuMgr->RemoveMenuItem( i_ParentName, i_ChildName );
#endif
}

//---------------------------------------------------------------------------
//	FindMenuItem()
//		returns whether or not the menu item can be found in the menu system
//---------------------------------------------------------------------------
bool quiMenuMgr::FindMenuItem( const char * i_ParentName,
							   const char * i_ChildName  )
{
#ifdef QT_FINISH_PORT
	return l_pMenuMgr->FindMenuItem( i_ParentName, i_ChildName );
#endif
	return false;
}

//---------------------------------------------------------------------------
//	AddSeparator()
//		add a separator to the menu with the given name.
//---------------------------------------------------------------------------
void quiMenuMgr::AddSeparator( const char * i_MenuName )
{
#ifdef QT_FINISH_PORT
	l_pMenuMgr->AddSeparator( i_MenuName );
#endif
}

//---------------------------------------------------------------------------
//	EnableMenuItem()
//		enable/disable a menu item
//---------------------------------------------------------------------------
void quiMenuMgr::EnableMenuItem( const char * i_ParentName,
								 const char * i_ChildName,
								 bool i_bEnable )
{
#ifdef QT_FINISH_PORT
	l_pMenuMgr->EnableMenuItem( i_ParentName, i_ChildName, i_bEnable );
#endif
}

//---------------------------------------------------------------------------
//	GetMenuItemID()
//		the function returns an ID
//---------------------------------------------------------------------------
int quiMenuMgr::GetMenuItemID( const char * i_ParentName,
							   const char * i_ChildName )
{
#ifdef QT_FINISH_PORT
	return l_pMenuMgr->GetMenuItemID(i_ParentName, i_ChildName);
#else
	return 0;
#endif
}

//---------------------------------------------------------------------------
//	GetMenuItemName()
//		give the name of the menu item with the given ID.
//---------------------------------------------------------------------------
std::string quiMenuMgr::GetMenuItemName(int i_ObjectID)
{
#ifdef QT_FINISH_PORT
	return l_pMenuMgr->GetMenuItemName(i_ObjectID);
#else
	return std::string("");
#endif
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void quiMenuMgr::SetMenuItemShortcut( int i_ObjectID, const char * i_ShortcutString )
{
#ifdef QT_FINISH_PORT
	l_pMenuMgr->SetMenuItemShortcut(i_ObjectID, i_ShortcutString);
#endif
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool quiMenuMgr::IsValidShortcut( const char * i_ShortcutString )
{
	return true;
}

//---------------------------------------------------------------------------
// Set Help string to display when mouse is over given menu item
//---------------------------------------------------------------------------
void quiMenuMgr::SetMenuItemHelpString( int i_ObjectID, const char * i_HelpString )
{
#ifdef QT_FINISH_PORT
	l_pMenuMgr->SetMenuItemHelpString( i_ObjectID, i_HelpString );
#endif
}

//---------------------------------------------------------------------------
//	get a pointer to a ControlCallback
//---------------------------------------------------------------------------
//ControlCallback* quiMenuMgr::GetControlCallback( int i_ObjectID )
//{
//}

//---------------------------------------------------------------------------
//	MenuObjectsExist()
//		based on the menu item ID.
//
//		returns true if the ID refers to a valid tmaMenuObjects
//---------------------------------------------------------------------------
bool quiMenuMgr::MenuObjectsExist( int i_ObjectID )
{
#ifdef QT_FINISH_PORT
	return l_pMenuMgr->MenuObjectsExist( i_ObjectID );
#else
	return false;
#endif
}

//---------------------------------------------------------------------------
//	MenuObjectsEnable()
//---------------------------------------------------------------------------
void quiMenuMgr::MenuObjectsEnable( int i_ObjectID, bool i_bEnabled )
{
#ifdef QT_FINISH_PORT
	l_pMenuMgr->MenuObjectsEnable( i_ObjectID, i_bEnabled );
#endif
}

//---------------------------------------------------------------------------
//	MenuObjectsCheck()
//---------------------------------------------------------------------------
void quiMenuMgr::MenuObjectsCheck( int i_ObjectID, bool i_bCheckd )
{
#ifdef QT_FINISH_PORT
	l_pMenuMgr->MenuObjectsCheck( i_ObjectID, i_bCheckd );
#endif
}


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void quiMenuMgr::SetIconDirectory( std::vector<fsLocator>& i_IconPathList )
{
#ifdef QT_FINISH_PORT
	tqtToolbarMgr::SetIconDirectory(i_IconPathList);
#endif
}
