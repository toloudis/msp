/*****************************************************************************
**	dirltDialogUtil.cpp
**
**	API for opening dialogs for system
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "dirltDialogUtil.hpp"

#include "dirltListForm.h"
#include "dirltObjectMgr.hpp"
#include "dirltDirLightDataForm.h"

// tool library
#include "tmaDialogTabbedMgr.hpp"

//
using namespace GeneratedForms;


//
namespace
{
	dirltScriptData l_CurData;
	bool l_bDataPageAdded = false;

	void show_object_properties()
	{
		tmaDialogTabbedMgr::Show("Object");
	}

}	// end of namespace

//--------------------------------------------------------------------
// Init
//--------------------------------------------------------------------
void  dirltDialogUtil::Init()
{
	// edit shows "Object" dialog now
	dirltListForm::DoEditCallback = show_object_properties;

	// Create list tab page dialog
	if (!dirltListForm::FormInstance)
	{
		dirltListForm::FormInstance = new dirltListForm();
		dirltListForm::FormInstance->Update(dirltObjectMgr::GetData());
		tmaDialogTabbedMgr::AddTabPage( "Scene", dirltListForm::FormInstance->GetTabPage(0) );
	}

	// Create tab page dialog
	if (!dirltDirLightDataForm::FormInstance)
	{
		dirltDirLightDataForm::FormInstance = new dirltDirLightDataForm(l_CurData);
	}
}

//--------------------------------------------------------------------
//  Clean up dialogs
//--------------------------------------------------------------------
void  dirltDialogUtil::CleanUp()
{
	if (l_bDataPageAdded)
		RemoveDataPage();

	if (dirltListForm::FormInstance)
		tmaDialogTabbedMgr::RemoveTabPage( "Scene", dirltListForm::FormInstance->GetTabPage(0) );

	dirltListForm::FormInstance = 0;
	dirltDirLightDataForm::FormInstance = 0;
}

//--------------------------------------------------------------------
//  Show dialog to add/delete dir lights
//--------------------------------------------------------------------
//	void  dirltDialogUtil::ShowDirLightsDialog()
//	{
//		// Use class to keep track of instance if already visible
//		if (!dirltListForm::FormInstance)
//		{
//			dirltListForm::FormInstance = new dirltListForm();
//		}
//
//		dirltOperations::StartNewOp();
//		dirltListForm::FormInstance->Update(dirltObjectMgr::GetData());
//		dirltListForm::FormInstance->Show();
//	}

//--------------------------------------------------------------------
//  Show dialog to set individual properties of a dir light
//--------------------------------------------------------------------
//	void  dirltDialogUtil::ShowDirLightDataDialog()
//	{
//		// Use class to keep track of instance if already visible
//		if (!dirltDirLightDataForm::FormInstance)
//		{
//			dirltDirLightDataForm::FormInstance = new dirltDirLightDataForm(l_CurData);
//		}
//
//		dirltOperations::StartNewOp();
//
//		// FIX: (?) - should this happen on init?  right now it can't since it's created on the fly
//		//
//		//	register the tab page
//		//
//		//tmaDialogTabbedMgr::l_pDialog->AddTabPage( dirltDirLightDataForm::FormInstance->GetTabPage(0) );
//		//tmaDialogTabbedMgr::l_pDialog->Show();
//	}

//--------------------------------------------------------------------
// Update main list dialog
//--------------------------------------------------------------------
void dirltDialogUtil::UpdateListDialog()
{
	if (dirltListForm::FormInstance )
	{
		dirltListForm::FormInstance->Update(dirltObjectMgr::GetData());
	}
}

//--------------------------------------------------------------------
// Update individual dir light properties dialog
//--------------------------------------------------------------------
void dirltDialogUtil::UpdateLightData(int i_Index, const dirltData& i_Data)
{
	l_CurData.m_BaseData = i_Data;

	dirltOperations::SetSelectedIndex(i_Index);

	if (dirltListForm::FormInstance)
		dirltListForm::FormInstance->Select(i_Index);

	if (dirltDirLightDataForm::FormInstance)
	{
		dirltDirLightDataForm::FormInstance->Enable(true);
		dirltDirLightDataForm::FormInstance->Update();
	}

}

//--------------------------------------------------------------------
// Update individual dir light properties dialog
//--------------------------------------------------------------------
void dirltDialogUtil::UpdateLightData(int i_Index, const dirltScriptData& i_Data)
{
	l_CurData = i_Data;

	dirltOperations::SetSelectedIndex(i_Index);

	if (dirltListForm::FormInstance)
		dirltListForm::FormInstance->Select(i_Index);

	if (dirltDirLightDataForm::FormInstance)
	{
		dirltDirLightDataForm::FormInstance->Enable(true);
		dirltDirLightDataForm::FormInstance->Update();
	}

}

//--------------------------------------------------------------------
//  Add/Remove tab page from selected object dialog
//--------------------------------------------------------------------
void  dirltDialogUtil::AddDataPage()
{
	if (!l_bDataPageAdded)
		tmaDialogTabbedMgr::AddTabPage( "Object", dirltDirLightDataForm::FormInstance->GetTabPage(0) );
	l_bDataPageAdded = true;
}
void  dirltDialogUtil::RemoveDataPage()
{
	if (l_bDataPageAdded)
		tmaDialogTabbedMgr::RemoveTabPage( "Object", dirltDirLightDataForm::FormInstance->GetTabPage(0) );
	l_bDataPageAdded = false;
}

//--------------------------------------------------------------------
// Light Data can only be displayed when a light is selected.
//	When a light is not selected, call this function to disable
//	the interface.
//--------------------------------------------------------------------
//	void dirltDialogUtil::DisableLightDataDialog()
//	{
//		if (dirltDirLightDataForm::FormInstance)
//			dirltDirLightDataForm::FormInstance->Enable(false);
//	}
