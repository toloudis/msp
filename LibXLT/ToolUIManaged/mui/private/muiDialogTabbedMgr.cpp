/*****************************************************************************
**  muiDialogTabbedMgr.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "ToolUIManaged/mui/muiDialogTabbedMgr.hpp"

#include "ToolUIManaged/tma/tmaDialogTabbedMgr.hpp"
#include "ToolUIManaged/prtym/prtyFormControlBuilder.hpp"


//---------------------------------------------------------------------------
// Create the dialog with the given name, but will not show it
//---------------------------------------------------------------------------
void muiDialogTabbedMgr::Create(const char* i_DialogName, const char* i_Title)
{

}

//---------------------------------------------------------------------------
// Show the dialog with the given name
//---------------------------------------------------------------------------
void muiDialogTabbedMgr::Show(const char* i_DialogName)
{

}

//---------------------------------------------------------------------------
// Returns true if the dialog is currently visible
//---------------------------------------------------------------------------
bool muiDialogTabbedMgr::IsVisible(const char* i_DialogName) 
{
	return false;
}

//---------------------------------------------------------------------------
// Add new tab page to dialog with given name
//---------------------------------------------------------------------------
void muiDialogTabbedMgr::AddTabPage(const char* i_DialogName, const char* i_TabName)
{

}

//---------------------------------------------------------------------------
// Remove tab page from dialog with given name
//---------------------------------------------------------------------------
void muiDialogTabbedMgr::RemoveTabPage(const char* i_DialogName, const char* i_TabName)
{

}

//---------------------------------------------------------------------------
// Remove all tab pages for the dialog with the given name
//---------------------------------------------------------------------------
void muiDialogTabbedMgr::RemoveTabPages(const char* i_DialogName)
{

}

//--------------------------------------------------------------------
//	Init the form and then add the list to it.
//	At the end of this function, the default label width will
//	get reset to the default width.
//--------------------------------------------------------------------
void muiDialogTabbedMgr::BuildForm(	const char* i_DialogName, 
									const char* i_TabName,
									const std::list<shared_ptr<prtyPropertyUIInfo> >& i_List,
									bool i_bShowCategory,
									bool i_bAutoCollapse)
{

}

