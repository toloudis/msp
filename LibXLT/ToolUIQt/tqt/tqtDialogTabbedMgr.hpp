/*****************************************************************************
**	tqtDialogTabbedMgr.hpp
**
**		Interface to the tabbed dialog manager.
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef TQT_DIALOGTABBEDMGR_HPP
#error tqtDialogTabbedMgr.hpp multiply included
#endif
#define TQT_DIALOGTABBEDMGR_HPP

#ifndef TQT_WIDGETS_HPP
#include "ToolUIQt/tqt/tqtWidgets.hpp"
#endif


#include <list>



//============================================================================
//	Forward References
//============================================================================
class prtyPropertyUIInfo;
class tqtDialogTabbed;


//============================================================================
//============================================================================
namespace tqtDialogTabbedMgr
{
	//---------------------------------------------------------------------------
	// Create the dialog with the given name, but will not show it
	//---------------------------------------------------------------------------
	void Create(const char* i_DialogName, const char* i_Title);

	//---------------------------------------------------------------------------
	// Show the dialog with the given name
	//---------------------------------------------------------------------------
	void Show(const char* i_DialogName);

	//---------------------------------------------------------------------------
	// Hide the dialog with the given name
	//---------------------------------------------------------------------------
	void Hide(const char* i_DialogName);

	//---------------------------------------------------------------------------
	// Returns true if the dialog is currently visible
	//---------------------------------------------------------------------------
	bool IsVisible(const char* i_DialogName);

	//---------------------------------------------------------------------------
	// Returns true if the dialog is existed
	//---------------------------------------------------------------------------
	bool IsExisted(const char* i_DialogName);

	//---------------------------------------------------------------------------
	// Add new tab page to dialog with given name
	//---------------------------------------------------------------------------
	void AddTabPage(const char* i_DialogName, const char* i_TabName);

	//---------------------------------------------------------------------------
	// Remove tab page from dialog with given name
	//---------------------------------------------------------------------------
	void RemoveTabPage(const char* i_DialogName, const char* i_TabName);

	//--------------------------------------------------------------------
	// Delete tab page from dialog with given name
	//--------------------------------------------------------------------
	void DeleteTabPage(const char* i_DialogName, const char* i_TabName);
	
	//---------------------------------------------------------------------------
	// Remove all tab pages for the dialog with the given name
	//---------------------------------------------------------------------------
	void RemoveTabPages(const char* i_DialogName);

#ifdef QT_FINISH_PORT
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	wxPanel* GetTabPage(const char* i_DialogName, const std::string& i_Title);
#endif

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	void NotifyDialogDeleted(tqtDialogTabbed* i_pDialog);

	//---------------------------------------------------------------------------
	// Stop the dialog from updating
	//---------------------------------------------------------------------------
	void Freeze(const char* i_DialogName);

	//---------------------------------------------------------------------------
	// Allow dialog to update
	//---------------------------------------------------------------------------
	void Thaw(const char* i_DialogName);
}
