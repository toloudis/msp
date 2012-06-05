/*****************************************************************************
**  wuiDialogTabbedMgr.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "ToolUIWx/wui/wuiDialogTabbedMgr.hpp"

#include "ToolUIWx/pwx/pwxFormControlBuilder.hpp"
#include "ToolUIWx/twx/twxDialogTabbedMgr.hpp"

namespace
{

}

//---------------------------------------------------------------------------
// Create the dialog with the given name, but will not show it
//---------------------------------------------------------------------------
void wuiDialogTabbedMgr::Create(const char* i_DialogName, const char* i_Title)
{
#ifdef USE_WXWIDGETS
	twxDialogTabbedMgr::Create( i_DialogName, i_Title );
#endif
}

//---------------------------------------------------------------------------
// Show the dialog with the given name
//---------------------------------------------------------------------------
void wuiDialogTabbedMgr::Show(const char* i_DialogName)
{
#ifdef USE_WXWIDGETS
	twxDialogTabbedMgr::Show( i_DialogName );
#endif
}

//---------------------------------------------------------------------------
// Hide the dialog with the given name
//---------------------------------------------------------------------------
void wuiDialogTabbedMgr::Hide(const char* i_DialogName)
{
#ifdef USE_WXWIDGETS
	twxDialogTabbedMgr::Hide( i_DialogName );
#endif
}

//---------------------------------------------------------------------------
// Returns true if the dialog is currently visible
//---------------------------------------------------------------------------
bool wuiDialogTabbedMgr::IsVisible(const char* i_DialogName) 
{
#ifdef USE_WXWIDGETS
	return twxDialogTabbedMgr::IsVisible( i_DialogName );
#endif
	return false;
}

//---------------------------------------------------------------------------
// Returns true if the dialog is existed
//---------------------------------------------------------------------------
bool wuiDialogTabbedMgr::IsExisted(const char* i_DialogName)
{
#ifdef USE_WXWIDGETS
	return twxDialogTabbedMgr::IsExisted( i_DialogName );
#endif
	return false;
}

//---------------------------------------------------------------------------
// Add new tab page to dialog with given name
//---------------------------------------------------------------------------
void wuiDialogTabbedMgr::AddTabPage(const char* i_DialogName, const char* i_TabName)
{
#ifdef USE_WXWIDGETS
	twxDialogTabbedMgr::AddTabPage( i_DialogName, i_TabName );
#endif
}

//---------------------------------------------------------------------------
// Remove tab page from dialog with given name
//---------------------------------------------------------------------------
void wuiDialogTabbedMgr::RemoveTabPage(const char* i_DialogName, const char* i_TabName)
{
#ifdef USE_WXWIDGETS
	twxDialogTabbedMgr::RemoveTabPage( i_DialogName, i_TabName );
#endif
}

//---------------------------------------------------------------------------
// Remove tab page from dialog with given name
//---------------------------------------------------------------------------
void wuiDialogTabbedMgr::DeleteTabPage(const char* i_DialogName, const char* i_TabName)
{
#ifdef USE_WXWIDGETS
	twxDialogTabbedMgr::DeleteTabPage( i_DialogName, i_TabName );
#endif
}

//---------------------------------------------------------------------------
// Remove all tab pages for the dialog with the given name
//---------------------------------------------------------------------------
void wuiDialogTabbedMgr::RemoveTabPages(const char* i_DialogName)
{
#ifdef USE_WXWIDGETS
	twxDialogTabbedMgr::RemoveTabPages( i_DialogName );
#endif
}

//--------------------------------------------------------------------
//	Init the form and then add the list to it.
//	At the end of this function, the default label width will
//	get reset to the default width.
//--------------------------------------------------------------------
void wuiDialogTabbedMgr::BuildForm(	const char* i_DialogName, 
									const char* i_TabName,
									const prtyPropertyUIInfoContainer& i_PropertyContainer,
									bool i_bShowCategory,
									bool i_bAutoCollapse)
{
#ifdef USE_WXWIDGETS	
	wxPanel* pTabPage = twxDialogTabbedMgr::GetTabPage( i_DialogName,  i_TabName );
	if (pTabPage)
	{

		pwxFormControlBuilder::BuildForm(pTabPage, i_DialogName, i_PropertyContainer, i_bShowCategory, i_bAutoCollapse );
	}
#endif
}

//---------------------------------------------------------------------------
// Stop the dialog from updating
//---------------------------------------------------------------------------
void wuiDialogTabbedMgr::Freeze(const char* i_DialogName) 
{
#ifdef USE_WXWIDGETS
	twxDialogTabbedMgr::Freeze(i_DialogName);
#endif
}

//---------------------------------------------------------------------------
// Allow dialog to update
//---------------------------------------------------------------------------
void wuiDialogTabbedMgr::Thaw(const char* i_DialogName) 
{
#ifdef USE_WXWIDGETS
	twxDialogTabbedMgr::Thaw(i_DialogName);
#endif
}