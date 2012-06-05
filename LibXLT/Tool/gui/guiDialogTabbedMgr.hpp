/*****************************************************************************
**	guiDialogTabbedMgr.hpp
**
**		A non-managed interface to the tabbed dialog manager.
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef GUI_DIALOGTABBEDMGR_HPP
#error guiDialogTabbedMgr.hpp multiply included
#endif
#define GUI_DIALOGTABBEDMGR_HPP

#ifndef ENV_ABSTRACTION_HPP
#include "Core/env/envAbstraction.hpp"
#endif
#ifndef ENV_BOOST_HPP
#include "Core/Env/envBoost.hpp"
#endif 

#include <list>


//============================================================================
// forward declaration
//============================================================================
class guiDialogTabbedMgrImpl;
class prtyPropertyUIInfoContainer;


//============================================================================
// static functions define API
//============================================================================
class guiDialogTabbedMgr : public envAbstraction<guiDialogTabbedMgrImpl>
{
public:
	//---------------------------------------------------------------------------
	// Create the dialog with the given name, but will not show it
	//---------------------------------------------------------------------------
	static void Create(const char* i_DialogName, const char* i_Title);

	//---------------------------------------------------------------------------
	// Show the dialog with the given name
	//---------------------------------------------------------------------------
	static void Show(const char* i_DialogName);

	//---------------------------------------------------------------------------
	// Hide the dialog with the given name
	//---------------------------------------------------------------------------
	static void Hide(const char* i_DialogName);

	//---------------------------------------------------------------------------
	// Returns true if the dialog is currently visible
	//---------------------------------------------------------------------------
	static bool IsVisible(const char* i_DialogName);

	//---------------------------------------------------------------------------
	// Returns true if the dialog is existed
	//---------------------------------------------------------------------------
	static bool IsExisted(const char* i_DialogName);

	//---------------------------------------------------------------------------
	// Add new tab page to dialog with given name
	//---------------------------------------------------------------------------
	static void AddTabPage(const char* i_DialogName, const char* i_TabName);

	//---------------------------------------------------------------------------
	// Remove tab page from dialog with given name
	//---------------------------------------------------------------------------
	static void RemoveTabPage(const char* i_DialogName, const char* i_TabName);

	//---------------------------------------------------------------------------
	// Delete tab page from dialog with given name
	//---------------------------------------------------------------------------
	static void DeleteTabPage(const char* i_DialogName, const char* i_TabName);
	
	//---------------------------------------------------------------------------
	// Remove all tab pages for the dialog with the given name
	//---------------------------------------------------------------------------
	static void RemoveTabPages(const char* i_DialogName);

	//--------------------------------------------------------------------
	//	Init the form and then add the list to it.
	//	At the end of this function, the default label width will
	//	get reset to the default width.
	//--------------------------------------------------------------------
	static void BuildForm(	const char* i_DialogName, 
							const char* i_TabName,
							const prtyPropertyUIInfoContainer& i_PropertyContainer,
							bool i_bShowCategory = false,
							bool i_bAutoCollapse = false);

	//---------------------------------------------------------------------------
	// Stop the dialog from updating
	//---------------------------------------------------------------------------
	static void Freeze(const char* i_DialogName);

	//---------------------------------------------------------------------------
	// Allow dialog to update
	//---------------------------------------------------------------------------
	static void Thaw(const char* i_DialogName);
};

//============================================================================
// Implementation class, needs to be derived in implementation library
//============================================================================
class guiDialogTabbedMgrImpl
{
public:
		//---------------------------------------------------------------------------
	// Create the dialog with the given name, but will not show it
	//---------------------------------------------------------------------------
	virtual void Create(const char* i_DialogName, const char* i_Title) = 0;

	//---------------------------------------------------------------------------
	// Show the dialog with the given name
	//---------------------------------------------------------------------------
	virtual void Show(const char* i_DialogName) = 0;

	//---------------------------------------------------------------------------
	// Hide the dialog with the given name
	//---------------------------------------------------------------------------
	virtual void Hide(const char* i_DialogName) = 0;

	//---------------------------------------------------------------------------
	// Returns true if the dialog is existed
	//---------------------------------------------------------------------------
	virtual bool IsExisted(const char* i_DialogName) = 0;

	//---------------------------------------------------------------------------
	// Returns true if the dialog is currently visible
	//---------------------------------------------------------------------------
	virtual bool IsVisible(const char* i_DialogName) = 0;

	//---------------------------------------------------------------------------
	// Add new tab page to dialog with given name
	//---------------------------------------------------------------------------
	virtual void AddTabPage(const char* i_DialogName, const char* i_TabName) = 0;

	//---------------------------------------------------------------------------
	// Remove tab page from dialog with given name
	//---------------------------------------------------------------------------
	virtual void RemoveTabPage(const char* i_DialogName, const char* i_TabName) = 0;

	//---------------------------------------------------------------------------
	// Delete tab page from dialog with given name
	//---------------------------------------------------------------------------
	virtual void DeleteTabPage(const char* i_DialogName, const char* i_TabName) = 0;
	
	//---------------------------------------------------------------------------
	// Remove all tab pages for the dialog with the given name
	//---------------------------------------------------------------------------
	virtual void RemoveTabPages(const char* i_DialogName) = 0;

	//--------------------------------------------------------------------
	//	Init the form and then add the list to it.
	//	At the end of this function, the default label width will
	//	get reset to the default width.
	//--------------------------------------------------------------------
	virtual void BuildForm(	const char* i_DialogName, 
							const char* i_TabName,
							const prtyPropertyUIInfoContainer& i_PropertyContainer,
							bool i_bShowCategory,
							bool i_bAutoCollapse) = 0;

	//---------------------------------------------------------------------------
	// Stop the dialog from updating
	//---------------------------------------------------------------------------
	virtual void Freeze(const char* i_DialogName) = 0;

	//---------------------------------------------------------------------------
	// Allow dialog to update
	//---------------------------------------------------------------------------
	virtual void Thaw(const char* i_DialogName) = 0;

};


