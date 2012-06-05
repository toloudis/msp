/*****************************************************************************
**	guiDialogTabbedMgr.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Tool/gui/guiDialogTabbedMgr.hpp"

#include "Core/dbg/dbgMsg.hpp"


//---------------------------------------------------------------------------
// Create the dialog with the given name, but will not show it
//---------------------------------------------------------------------------
void guiDialogTabbedMgr::Create(const char* i_DialogName, const char* i_Title)
{
	DBG_ASSERT(sm_pImplementation, "guiDialogTabbedMgr: No implementation");
	if (sm_pImplementation)
	{
		sm_pImplementation->Create(i_DialogName, i_Title);
	}
}

//---------------------------------------------------------------------------
// Show the dialog with the given name
//---------------------------------------------------------------------------
void guiDialogTabbedMgr::Show(const char* i_DialogName)
{
	DBG_ASSERT(sm_pImplementation, "guiDialogTabbedMgr: No implementation");
	if (sm_pImplementation)
	{
		sm_pImplementation->Show(i_DialogName);
	}
}

//---------------------------------------------------------------------------
// Hide the dialog with the given name
//---------------------------------------------------------------------------
void guiDialogTabbedMgr::Hide(const char* i_DialogName)
{
	//DBG_ASSERT(sm_pImplementation, "guiDialogTabbedMgr: No implementation");
	if (sm_pImplementation)
	{
		sm_pImplementation->Hide(i_DialogName);
	}
}

//---------------------------------------------------------------------------
// Returns true if the dialog is currently visible
//---------------------------------------------------------------------------
bool guiDialogTabbedMgr::IsVisible(const char* i_DialogName)
{
	DBG_ASSERT(sm_pImplementation, "guiDialogTabbedMgr: No implementation");
	if (sm_pImplementation)
	{
		return sm_pImplementation->IsVisible(i_DialogName);
	}
	return false;
}

//---------------------------------------------------------------------------
// Returns true if the dialog is existed
//---------------------------------------------------------------------------
bool guiDialogTabbedMgr::IsExisted(const char* i_DialogName)
{
	DBG_ASSERT(sm_pImplementation, "guiDialogTabbedMgr: No implementation");
	if (sm_pImplementation)
	{
		return sm_pImplementation->IsExisted(i_DialogName);
	}
	return false;
}

//---------------------------------------------------------------------------
// Add new tab page to dialog with given name
//---------------------------------------------------------------------------
void guiDialogTabbedMgr::AddTabPage(const char* i_DialogName, const char* i_TabName)
{
	DBG_ASSERT(sm_pImplementation, "guiDialogTabbedMgr: No implementation");
	if (sm_pImplementation)
	{
		sm_pImplementation->AddTabPage(i_DialogName, i_TabName);
	}
}

//---------------------------------------------------------------------------
// Remove tab page from dialog with given name
//---------------------------------------------------------------------------
void guiDialogTabbedMgr::RemoveTabPage(const char* i_DialogName, const char* i_TabName)
{
	DBG_ASSERT(sm_pImplementation, "guiDialogTabbedMgr: No implementation");
	if (sm_pImplementation)
	{
		sm_pImplementation->RemoveTabPage(i_DialogName, i_TabName);
	}
}

//---------------------------------------------------------------------------
// Delete tab page from dialog with given name
//---------------------------------------------------------------------------
void guiDialogTabbedMgr::DeleteTabPage(const char* i_DialogName, const char* i_TabName)
{
	DBG_ASSERT(sm_pImplementation, "guiDialogTabbedMgr: No implementation");
	if (sm_pImplementation)
	{
		sm_pImplementation->DeleteTabPage(i_DialogName, i_TabName);
	}
}

//---------------------------------------------------------------------------
// Remove all tab pages for the dialog with the given name
//---------------------------------------------------------------------------
void guiDialogTabbedMgr::RemoveTabPages(const char* i_DialogName)
{
	DBG_ASSERT(sm_pImplementation, "guiDialogTabbedMgr: No implementation");
	if (sm_pImplementation)
	{
		sm_pImplementation->RemoveTabPages(i_DialogName);
	}
}

//--------------------------------------------------------------------
//	Init the form and then add the list to it.
//	At the end of this function, the default label width will
//	get reset to the default width.
//--------------------------------------------------------------------
void guiDialogTabbedMgr::BuildForm(	const char* i_DialogName, 
				const char* i_TabName,
				const prtyPropertyUIInfoContainer& i_PropertyContainer,
				bool i_bShowCategory,
				bool i_bAutoCollapse)
{
	DBG_ASSERT(sm_pImplementation, "guiDialogTabbedMgr: No implementation");
	if (sm_pImplementation)
	{
		sm_pImplementation->BuildForm(i_DialogName, i_TabName, 
			i_PropertyContainer, i_bShowCategory, i_bAutoCollapse);
	}
}
//---------------------------------------------------------------------------
// Stop the dialog from updating
//---------------------------------------------------------------------------
void guiDialogTabbedMgr::Freeze(const char* i_DialogName)
{
	sm_pImplementation->Freeze(i_DialogName);
}

//---------------------------------------------------------------------------
// Allow dialog to update
//---------------------------------------------------------------------------
void guiDialogTabbedMgr::Thaw(const char* i_DialogName)
{
	sm_pImplementation->Thaw(i_DialogName);
}


