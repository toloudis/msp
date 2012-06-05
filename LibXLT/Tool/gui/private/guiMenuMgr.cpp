/*****************************************************************************
**	guiMenuMgr.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Tool/gui/guiMenuMgr.hpp"


//===========================================================================
//===========================================================================
namespace
{
	std::vector<fsLocator> l_IconPathList;
}


//===========================================================================
//	guiMenuMgr functions
//===========================================================================

//---------------------------------------------------------------------------
//	AttachEventToMenuObjects()
//---------------------------------------------------------------------------
void guiMenuMgr::AttachEventToMenuObjects( int i_ObjectID, 
										  ControlCallback i_pFunction, 
										  ControlCallback i_pUpdate )
{
	DBG_ASSERT(sm_pImplementation, "guiMenuMgr: No implementation");
	if (sm_pImplementation)
	{
		sm_pImplementation->AttachEventToMenuObjects(i_ObjectID, i_pFunction, i_pUpdate);
	}
}

//---------------------------------------------------------------------------
//	AddMenu()
//		add a menu or submenu to the Menu Tree.
//		set the childname to "" is adding a parent item.
//---------------------------------------------------------------------------
void guiMenuMgr::AddMenu( const char * i_ParentName,
			  const char * i_ChildName )
{
	DBG_ASSERT(sm_pImplementation, "guiMenuMgr: No implementation");
	if (sm_pImplementation)
	{
		sm_pImplementation->AddMenu(i_ParentName, i_ChildName);
	}
}

//---------------------------------------------------------------------------
//	AddMenuItem()
//		add a menu item to the Menu Tree.
//		set the childname to "" is adding a parent item.
//	the function returns a unique ObjectID for the GUI object.
//---------------------------------------------------------------------------
int guiMenuMgr::AddMenuItem( const char * i_ParentName,
							  const char * i_ChildName,
						      const char * i_ToolbarName,
							  const char * i_ToolbarButton_ImageFilename )
{
	DBG_ASSERT(sm_pImplementation, "guiMenuMgr: No implementation");
	if (sm_pImplementation)
	{
		return sm_pImplementation->AddMenuItem(i_ParentName, i_ChildName, i_ToolbarName, i_ToolbarButton_ImageFilename);
	}
	return -1;
}

//---------------------------------------------------------------------------
//	AddCheckableMenuItem()
//		add a menu item to the Menu Tree.
//		set the childname to "" is adding a parent item.
//	the function returns a unique ObjectID for the GUI object.
//---------------------------------------------------------------------------
int guiMenuMgr::AddCheckableMenuItem( const char * i_ParentName,
									  const char * i_ChildName,
									  const char * i_ToolbarName,
									  const char * i_ToolbarButton_ImageFilename )
{
	DBG_ASSERT(sm_pImplementation, "guiMenuMgr: No implementation");
	if (sm_pImplementation)
	{
		return sm_pImplementation->AddCheckableMenuItem(i_ParentName, i_ChildName, i_ToolbarName, i_ToolbarButton_ImageFilename);
	}
	return -1;
}

//---------------------------------------------------------------------------
//	RemoveMenuItem()
//		remove a menu item from the Menu Tree
//---------------------------------------------------------------------------
void guiMenuMgr::RemoveMenuItem( const char * i_ParentName,
								 const char * i_ChildName  )
{
	DBG_ASSERT(sm_pImplementation, "guiMenuMgr: No implementation");
	if (sm_pImplementation)
	{
		sm_pImplementation->RemoveMenuItem(i_ParentName, i_ChildName);
	}
}

//---------------------------------------------------------------------------
//	FindMenuItem()
//		returns whether or not the menu item can be found in the menu system
//---------------------------------------------------------------------------
bool guiMenuMgr::FindMenuItem( const char * i_ParentName,
							   const char * i_ChildName  )
{
	DBG_ASSERT(sm_pImplementation, "guiMenuMgr: No implementation");
	if (sm_pImplementation)
	{
		return sm_pImplementation->FindMenuItem(i_ParentName, i_ChildName);
	}
	return false;
}

//---------------------------------------------------------------------------
//	AddSeparator()
//		add a separator to the menu with the given name.
//---------------------------------------------------------------------------
void guiMenuMgr::AddSeparator( const char * i_MenuName )
{
	DBG_ASSERT(sm_pImplementation, "guiMenuMgr: No implementation");
	if (sm_pImplementation)
	{
		sm_pImplementation->AddSeparator(i_MenuName);
	}
}

//---------------------------------------------------------------------------
//	EnableMenuItem()
//		enable/disable a menu item
//---------------------------------------------------------------------------
void guiMenuMgr::EnableMenuItem( const char * i_ParentName,
								 const char * i_ChildName,
								 bool i_bEnable )
{
	DBG_ASSERT(sm_pImplementation, "guiMenuMgr: No implementation");
	if (sm_pImplementation)
	{
		sm_pImplementation->EnableMenuItem(i_ParentName, i_ChildName, i_bEnable);
	}
}

//---------------------------------------------------------------------------
//	GetMenuItemID()
//		the function returns an ID
//---------------------------------------------------------------------------
int guiMenuMgr::GetMenuItemID( const char * i_ParentName,
							   const char * i_ChildName )
{
	DBG_ASSERT(sm_pImplementation, "guiMenuMgr: No implementation");
	if (sm_pImplementation)
	{
		return sm_pImplementation->GetMenuItemID(i_ParentName, i_ChildName);
	}
	return 0;
}

//---------------------------------------------------------------------------
//	GetMenuItemName()
//		give the name of the menu item with the given ID.
//---------------------------------------------------------------------------
std::string guiMenuMgr::GetMenuItemName(int i_ObjectID)
{
	DBG_ASSERT(sm_pImplementation, "guiMenuMgr: No implementation");
	if (sm_pImplementation)
	{
		return sm_pImplementation->GetMenuItemName(i_ObjectID);
	}
	return std::string("");
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void guiMenuMgr::SetMenuItemShortcut( int i_ObjectID, const char * i_ShortcutString )
{
	DBG_ASSERT(sm_pImplementation, "guiMenuMgr: No implementation");
	if (sm_pImplementation)
	{
		sm_pImplementation->SetMenuItemShortcut(i_ObjectID, i_ShortcutString);
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool guiMenuMgr::IsValidShortcut( const char * i_ShortcutString )
{
	DBG_ASSERT(sm_pImplementation, "guiMenuMgr: No implementation");
	if (sm_pImplementation)
	{
		return sm_pImplementation->IsValidShortcut(i_ShortcutString);
	}
	return false;
}

//---------------------------------------------------------------------------
// Set Help string to display when mouse is over given menu item
//---------------------------------------------------------------------------
void guiMenuMgr::SetMenuItemHelpString( int i_ObjectID, const char * i_HelpString )
{
	DBG_ASSERT(sm_pImplementation, "guiMenuMgr: No implementation");
	if (sm_pImplementation)
	{
		return sm_pImplementation->SetMenuItemHelpString(i_ObjectID, i_HelpString);
	}

}

//---------------------------------------------------------------------------
//	get a pointer to a ControlCallback
//---------------------------------------------------------------------------
//ControlCallback* guiMenuMgr::GetControlCallback( int i_ObjectID )
//{
//}

//---------------------------------------------------------------------------
//	MenuObjectsExist()
//		based on the menu item ID.
//
//		returns true if the ID refers to a valid tmaMenuObjects
//---------------------------------------------------------------------------
bool guiMenuMgr::MenuObjectsExist( int i_ObjectID )
{
	DBG_ASSERT(sm_pImplementation, "guiMenuMgr: No implementation");
	if (sm_pImplementation)
	{
		return sm_pImplementation->MenuObjectsExist(i_ObjectID);
	}
	return false;
}

//---------------------------------------------------------------------------
//	MenuObjectsEnable()
//---------------------------------------------------------------------------
void guiMenuMgr::MenuObjectsEnable( int i_ObjectID, bool i_bEnabled )
{
	DBG_ASSERT(sm_pImplementation, "guiMenuMgr: No implementation");
	if (sm_pImplementation)
	{
		sm_pImplementation->MenuObjectsEnable(i_ObjectID, i_bEnabled);
	}
}

//---------------------------------------------------------------------------
//	MenuObjectsCheck()
//---------------------------------------------------------------------------
void guiMenuMgr::MenuObjectsCheck( int i_ObjectID, bool i_bCheckd )
{
	DBG_ASSERT(sm_pImplementation, "guiMenuMgr: No implementation");
	if (sm_pImplementation)
	{
		sm_pImplementation->MenuObjectsCheck(i_ObjectID, i_bCheckd);
	}
}


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void guiMenuMgr::SetIconDirectory( std::vector<fsLocator>& i_IconPathList )
{
	l_IconPathList = i_IconPathList;

	DBG_ASSERT(sm_pImplementation, "guiMenuMgr: No implementation");
	if (sm_pImplementation)
	{
		sm_pImplementation->SetIconDirectory(i_IconPathList);
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
const fsLocator& guiMenuMgr::GetIconDirectory()
{
	return l_IconPathList[0];
}


const std::vector<fsLocator>& GetIconPathList()
{
	return l_IconPathList;	
}