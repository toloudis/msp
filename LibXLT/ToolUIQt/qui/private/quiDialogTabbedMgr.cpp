/*****************************************************************************
**  quiDialogTabbedMgr.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/qui/quiDialogTabbedMgr.hpp"

#include "ToolUIQt/pqt/pqtFormControlBuilder.hpp"
#include "ToolUIQt/tqt/tqtDialogTabbedMgr.hpp"

namespace
{

}

//---------------------------------------------------------------------------
// Create the dialog with the given name, but will not show it
//---------------------------------------------------------------------------
void quiDialogTabbedMgr::Create(const char* i_DialogName, const char* i_Title)
{
#ifdef QT_FINISH_PORT
	tqtDialogTabbedMgr::Create( i_DialogName, i_Title );
#endif
}

//---------------------------------------------------------------------------
// Show the dialog with the given name
//---------------------------------------------------------------------------
void quiDialogTabbedMgr::Show(const char* i_DialogName)
{
#ifdef QT_FINISH_PORT
	tqtDialogTabbedMgr::Show( i_DialogName );
#endif
}

//---------------------------------------------------------------------------
// Hide the dialog with the given name
//---------------------------------------------------------------------------
void quiDialogTabbedMgr::Hide(const char* i_DialogName)
{
#ifdef QT_FINISH_PORT
	tqtDialogTabbedMgr::Hide( i_DialogName );
#endif
}

//---------------------------------------------------------------------------
// Returns true if the dialog is currently visible
//---------------------------------------------------------------------------
bool quiDialogTabbedMgr::IsVisible(const char* i_DialogName) 
{
#ifdef QT_FINISH_PORT
	return tqtDialogTabbedMgr::IsVisible( i_DialogName );
#endif
	return false;
}

//---------------------------------------------------------------------------
// Returns true if the dialog is existed
//---------------------------------------------------------------------------
bool quiDialogTabbedMgr::IsExisted(const char* i_DialogName)
{
#ifdef QT_FINISH_PORT
	return tqtDialogTabbedMgr::IsExisted( i_DialogName );
#endif
	return false;
}

//---------------------------------------------------------------------------
// Add new tab page to dialog with given name
//---------------------------------------------------------------------------
void quiDialogTabbedMgr::AddTabPage(const char* i_DialogName, const char* i_TabName)
{
#ifdef QT_FINISH_PORT
	tqtDialogTabbedMgr::AddTabPage( i_DialogName, i_TabName );
#endif
}

//---------------------------------------------------------------------------
// Remove tab page from dialog with given name
//---------------------------------------------------------------------------
void quiDialogTabbedMgr::RemoveTabPage(const char* i_DialogName, const char* i_TabName)
{
#ifdef QT_FINISH_PORT
	tqtDialogTabbedMgr::RemoveTabPage( i_DialogName, i_TabName );
#endif
}

//---------------------------------------------------------------------------
// Remove tab page from dialog with given name
//---------------------------------------------------------------------------
void quiDialogTabbedMgr::DeleteTabPage(const char* i_DialogName, const char* i_TabName)
{
#ifdef QT_FINISH_PORT
	tqtDialogTabbedMgr::DeleteTabPage( i_DialogName, i_TabName );
#endif
}

//---------------------------------------------------------------------------
// Remove all tab pages for the dialog with the given name
//---------------------------------------------------------------------------
void quiDialogTabbedMgr::RemoveTabPages(const char* i_DialogName)
{
#ifdef QT_FINISH_PORT
	tqtDialogTabbedMgr::RemoveTabPages( i_DialogName );
#endif
}

//--------------------------------------------------------------------
//	Init the form and then add the list to it.
//	At the end of this function, the default label width will
//	get reset to the default width.
//--------------------------------------------------------------------
void quiDialogTabbedMgr::BuildForm(	const char* i_DialogName, 
									const char* i_TabName,
									const prtyPropertyUIInfoContainer& i_PropertyContainer,
									bool i_bShowCategory,
									bool i_bAutoCollapse)
{
#ifdef QT_FINISH_PORT	
	wxPanel* pTabPage = tqtDialogTabbedMgr::GetTabPage( i_DialogName,  i_TabName );
	if (pTabPage)
	{

		pqtFormControlBuilder::BuildForm(pTabPage, i_DialogName, i_PropertyContainer, i_bShowCategory, i_bAutoCollapse );
	}
#endif
}

//---------------------------------------------------------------------------
// Stop the dialog from updating
//---------------------------------------------------------------------------
void quiDialogTabbedMgr::Freeze(const char* i_DialogName) 
{
#ifdef QT_FINISH_PORT
	tqtDialogTabbedMgr::Freeze(i_DialogName);
#endif
}

//---------------------------------------------------------------------------
// Allow dialog to update
//---------------------------------------------------------------------------
void quiDialogTabbedMgr::Thaw(const char* i_DialogName) 
{
#ifdef QT_FINISH_PORT
	tqtDialogTabbedMgr::Thaw(i_DialogName);
#endif
}