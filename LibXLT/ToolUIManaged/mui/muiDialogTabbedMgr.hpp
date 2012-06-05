/*****************************************************************************
**  muiDialogTabbedMgr.hpp
**
**      A non-managed interface to the tabbed dialog manager.
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef MUI_DIALOGTABBEDMGR_HPP
#error muiDialogTabbedMgr.hpp multiply included
#endif
#define MUI_DIALOGTABBEDMGR_HPP

#ifndef GUI_DIALOGTABBEDMGR_HPP
#include "Tool/gui/guiDialogTabbedMgr.hpp"
#endif

//============================================================================
//============================================================================
class muiDialogTabbedMgr : public guiDialogTabbedMgrImpl
{
public:
	//---------------------------------------------------------------------------
	// Create the dialog with the given name, but will not show it
	//---------------------------------------------------------------------------
	virtual void Create(const char* i_DialogName, const char* i_Title);

	//---------------------------------------------------------------------------
	// Show the dialog with the given name
	//---------------------------------------------------------------------------
	virtual void Show(const char* i_DialogName);

	//---------------------------------------------------------------------------
	// Returns true if the dialog is currently visible
	//---------------------------------------------------------------------------
	virtual bool IsVisible(const char* i_DialogName);

	//---------------------------------------------------------------------------
	// Add new tab page to dialog with given name
	//---------------------------------------------------------------------------
	virtual void AddTabPage(const char* i_DialogName, const char* i_TabName);

	//---------------------------------------------------------------------------
	// Remove tab page from dialog with given name
	//---------------------------------------------------------------------------
	virtual void RemoveTabPage(const char* i_DialogName, const char* i_TabName);
	
	//---------------------------------------------------------------------------
	// Remove all tab pages for the dialog with the given name
	//---------------------------------------------------------------------------
	virtual void RemoveTabPages(const char* i_DialogName);

	//--------------------------------------------------------------------
	//	Init the form and then add the list to it.
	//	At the end of this function, the default label width will
	//	get reset to the default width.
	//--------------------------------------------------------------------
	virtual void BuildForm(	const char* i_DialogName, 
							const char* i_TabName,
							const std::list<shared_ptr<prtyPropertyUIInfo> >& i_List,
							bool i_bShowCategory = false,
							bool i_bAutoCollapse = false);

};

