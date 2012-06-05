/*****************************************************************************
**  quiDialogTabbedMgr.hpp
**
**      Interface to the tabbed dialog manager.
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef QUI_DIALOGTABBEDMGR_HPP
#error quiDialogTabbedMgr.hpp multiply included
#endif
#define QUI_DIALOGTABBEDMGR_HPP

#ifndef GUI_MESSAGEBOX_HPP
#include "Tool/gui/guiDialogTabbedMgr.hpp"
#endif

//============================================================================
//============================================================================
class quiDialogTabbedMgr : public guiDialogTabbedMgrImpl
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
	// Hide the dialog with the given name
	//---------------------------------------------------------------------------
	virtual void Hide(const char* i_DialogName);

	//---------------------------------------------------------------------------
	// Returns true if the dialog is currently visible
	//---------------------------------------------------------------------------
	virtual bool IsVisible(const char* i_DialogName);

	//---------------------------------------------------------------------------
	// Returns true if the dialog is existed
	//---------------------------------------------------------------------------
	virtual bool IsExisted(const char* i_DialogName);

	//---------------------------------------------------------------------------
	// Add new tab page to dialog with given name
	//---------------------------------------------------------------------------
	virtual void AddTabPage(const char* i_DialogName, const char* i_TabName);

	//---------------------------------------------------------------------------
	// Remove tab page from dialog with given name
	//---------------------------------------------------------------------------
	virtual void RemoveTabPage(const char* i_DialogName, const char* i_TabName);

	//---------------------------------------------------------------------------
	// Remove tab page from dialog with given name
	//---------------------------------------------------------------------------
	virtual void DeleteTabPage(const char* i_DialogName, const char* i_TabName);
	
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
							const prtyPropertyUIInfoContainer& i_PropertyContainer,
							bool i_bShowCategory = false,
							bool i_bAutoCollapse = false);

	//---------------------------------------------------------------------------
	// Stop the dialog from updating
	//---------------------------------------------------------------------------
	virtual void Freeze(const char* i_DialogName);

	//---------------------------------------------------------------------------
	// Allow dialog to update
	//---------------------------------------------------------------------------
	virtual void Thaw(const char* i_DialogName);
};

