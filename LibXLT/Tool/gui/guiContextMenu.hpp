/*****************************************************************************
**	guiContextMenu.hpp
**
**		An windowing system independent interface to a context menu
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef GUI_CONTEXTMENU_HPP
#error guiContextMenu.hpp multiply included
#endif
#define GUI_CONTEXTMENU_HPP

#include <functional>

//============================================================================
// forward declaration
//============================================================================


//============================================================================
// static functions define API
//============================================================================
class guiContextMenu 
{
public:
	//---------------------------------------------------------------------------
	//	AddMenu()
	//		add a menu or submenu to the Menu Tree.
	//		set the childname to "" is adding a parent item.
	//---------------------------------------------------------------------------
	virtual void AddMenu( const char * i_ParentName,
						  const char * i_ChildName ) = 0;

	//---------------------------------------------------------------------------
	//	AddMenuItem()
	//		add a menu item to the Menu Tree which will call the given 
	//	callback function when clicked.
	//---------------------------------------------------------------------------
	typedef std::function<void()> CallbackFunction;
	virtual int AddMenuItem( const char * i_ParentName,
							 const char * i_ChildName,
							 CallbackFunction i_Callback ) = 0;

	//---------------------------------------------------------------------------
	//	AddSeparator()
	//		add a separator to the menu with the given name.
	//---------------------------------------------------------------------------
	virtual void AddSeparator( const char * i_MenuName ) = 0;

	//---------------------------------------------------------------------------
	// Returns true if the context menu does not have any menus added.
	//---------------------------------------------------------------------------
	virtual bool IsEmpty() = 0;
};
