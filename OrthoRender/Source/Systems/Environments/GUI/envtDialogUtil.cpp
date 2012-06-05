/*****************************************************************************
**	envtDialogUtil.cpp
**
**	API for opening dialogs for system
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Systems/Environments/GUI/envtDialogUtil.hpp"

#include "Systems/Environments/GUI/envtDialogDataUtil.hpp"
#include "Systems/Environments/Data/envtDocumentChunk.hpp"
#include "Systems/Environments/GUI/envtEnvironmentObjectsForm.h"
#include "Systems/Environments/GUI/wxGUI/envtEnvironmentObjectsPage.hpp"
#include "Support/evmt/evmtEnvironmentInterest.hpp"

#include "Systems/Common/GUI/wxGUI/cmmObjectDialog.hpp"
#include "Systems/Common/GUI/cmmObjectDialogUtil.hpp"
#include "Systems/Common/GUI/cmmSystemDialogInterest.hpp"
#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"

#include "ToolUIManaged/tma/tmaSystem.hpp"


//============================================================================
//============================================================================
#ifdef _MANAGED
using namespace SystemEnvironments;
#endif

//--------------------------------------------------------------------
//--------------------------------------------------------------------
namespace
{
	envtScriptData l_CurData;

	//--------------------------------------------------------------------
	// Callback when any data in evmtEnvironmentMgr changes
	//--------------------------------------------------------------------
	class MyDataChanged : public evmtEnvironmentInterest
	{
	public:
		//--------------------------------------------------------------------
		//	ObjectAdded - an object has been 
		//		added or removed to the system
		//--------------------------------------------------------------------
		virtual void ObjectAdded()
		{
		}

		//--------------------------------------------------------------------
		//	ObjectAdded - an object has been 
		//		added or removed to the system
		//--------------------------------------------------------------------
		virtual void ObjectRemoved(const nameString& i_Name)
		{
			envtOperations::RemoveByName(i_Name);
		}

		//--------------------------------------------------------------------
		//	ObjectRenamed - a object has been renamed
		//--------------------------------------------------------------------
		virtual void ObjectRenamed()
		{
			envtOperations::RefreshNames();
		}

		//--------------------------------------------------------------------
		//	DataChanged - objects have been added/removed from 
		//		environments
		//--------------------------------------------------------------------
		virtual void DataChanged()
		{
		}
	};
	
	MyDataChanged l_DataChangedObj;
}	// end of namespace

//============================================================================
//============================================================================



//--------------------------------------------------------------------
// Init
//--------------------------------------------------------------------
void  envtDialogUtil::Init()
{
	evmtEnvironmentMgr::RegisterEnvironmentInterest(&l_DataChangedObj);

//	create_dialog();
#ifdef _MANAGED
	// Create tab page dialog
	if (!envtEnvironmentObjectsForm::FormInstance)
	{
		envtEnvironmentObjectsForm::FormInstance = gcnew envtEnvironmentObjectsForm();
	}
#endif // _MANAGED
}

//--------------------------------------------------------------------
//  Clean up dialogs
//--------------------------------------------------------------------
void  envtDialogUtil::CleanUp()
{
	evmtEnvironmentMgr::UnRegisterEnvironmentInterest(&l_DataChangedObj);

	RemoveDataPage();

#ifdef _MANAGED
	envtEnvironmentObjectsForm::FormInstance = nullptr;
#endif // _MANAGED
}

//--------------------------------------------------------------------
//	Update the dialog list
//--------------------------------------------------------------------
//static 
void envtDialogUtil::UpdateListDialog()
{
	envtDialogDataUtil::UpdateListDialog();
}

//--------------------------------------------------------------------
// Update individual environment properties dialog
//--------------------------------------------------------------------
void envtDialogUtil::UpdateEnvironmentData(int i_Index, const envtData& i_Data)
{
	l_CurData.m_BaseData = i_Data;

	envtOperations::SetSelectedIndex(i_Index);
}

//--------------------------------------------------------------------
// Update individual environment properties dialog
//--------------------------------------------------------------------
void envtDialogUtil::UpdateEnvironmentData(int i_Index, const envtScriptData& i_Data)
{
	l_CurData = i_Data;

	envtOperations::SetSelectedIndex(i_Index);
}

//--------------------------------------------------------------------
// Update common data tab page
//--------------------------------------------------------------------
void envtDialogUtil::UpdateDialog(envtScriptObject *i_pObject)
{
#ifdef _MANAGED
	if (envtEnvironmentObjectsForm::FormInstance )
	{
		envtEnvironmentObjectsForm::FormInstance->Update(i_pObject);
	}
#endif // _MANAGED
#ifdef USE_WXWIDGETS
	if (envtEnvironmentObjectsPage::Instance)
	{
		envtEnvironmentObjectsPage::Instance->Update(i_pObject);
	}
#endif // USE_WXWIDGETS
}

//--------------------------------------------------------------------
//  Add/Remove tab page from selected object dialog
//--------------------------------------------------------------------
void  envtDialogUtil::AddDataPage()
{
#ifdef _MANAGED
	if (!cmmObjectDialogUtil::HasTabPage(envtEnvironmentObjectsForm::FormInstance->GetTabPage(0)))
		cmmObjectDialogUtil::AddTabPage( envtEnvironmentObjectsForm::FormInstance->GetTabPage(0) );
#endif // _MANAGED

#ifdef USE_WXWIDGETS
	if (cmmObjectDialog::FormInstance)
	{
//WXGUI
/*
		if (!envtEnvironmentObjectsPage::Instance)
			envtEnvironmentObjectsPage::Instance = new envtEnvironmentObjectsPage(cmmObjectDialog::FormInstance->GetNotebook());
		if (!cmmObjectDialogUtil::HasTabPage(envtEnvironmentObjectsPage::Instance))
		{
			cmmObjectDialogUtil::AddTabPage( envtEnvironmentObjectsPage::Instance, "Objects" );
		}
*/
	}
#endif
}

void  envtDialogUtil::RemoveDataPage()
{
#ifdef _MANAGED
	if (cmmObjectDialogUtil::HasTabPage(envtEnvironmentObjectsForm::FormInstance->GetTabPage(0)))
		cmmObjectDialogUtil::RemoveTabPage( envtEnvironmentObjectsForm::FormInstance->GetTabPage(0) );
#endif // _MANAGED
#ifdef USE_WXWIDGETS
	if (envtEnvironmentObjectsPage::Instance)
	{
		if (cmmObjectDialogUtil::HasTabPage(envtEnvironmentObjectsPage::Instance))
		{
			cmmObjectDialogUtil::RemoveTabPage( envtEnvironmentObjectsPage::Instance );
			envtEnvironmentObjectsPage::Instance->Show(false);
		}
	}
#endif
}
