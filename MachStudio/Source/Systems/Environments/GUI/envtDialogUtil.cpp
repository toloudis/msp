/*****************************************************************************
**	envtDialogUtil.cpp
**
**	API for opening dialogs for system
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Systems/Environments/GUI/envtDialogUtil.hpp"

#include "Systems/Environments/GUI/envtDialogDataUtil.hpp"
#include "Systems/Environments/Data/envtDocumentChunk.hpp"
#include "Systems/Environments/GUI/wxGUI/envtEnvironmentObjectsPage.hpp"
#include "Systems/Environments/GUI/wxGUI/envtEnvironmentSwlPage.hpp"
#include "Support/evmt/evmtEnvironmentInterest.hpp"

#include "Systems/Common/GUI/wxGUI/cmmObjectDialog.hpp"
#include "Systems/Common/GUI/cmmObjectDialogUtil.hpp"
#include "Systems/Common/GUI/cmmSystemDialogInterest.hpp"
#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"

// Includes from old managed file
#ifndef EVMT_ENVIRONMENTMGR_HPP
#include "Support/evmt/evmtEnvironmentMgr.hpp"
#endif
#ifndef ENVT_OPERATIONS_HPP
#include "Systems/Environments/Undo/envtOperations.hpp"
#endif
#ifndef ENVT_SCRIPTOBJECT_HPP
#include "Systems/Environments/Object/envtScriptObject.hpp"
#endif
#ifndef NAME_STRING_HPP
#include "Core/name/nameString.hpp"
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
			// must ensure that state is consistent between evmt and envt.
//			envtOperations::RefreshNames();
		}

		//--------------------------------------------------------------------
		//	ObjectAdded - an object has been 
		//		added or removed to the system
		//--------------------------------------------------------------------
		virtual void ObjectRemoved(const nameString& i_Name)
		{
//			envtOperations::RemoveByName(i_Name);
		}

		//--------------------------------------------------------------------
		//	ObjectRenamed - a object has been renamed
		//--------------------------------------------------------------------
		virtual void ObjectRenamed()
		{
//			envtOperations::RefreshNames();
		}

		//--------------------------------------------------------------------
		//	DataChanged - objects have been added/removed from 
		//		environments
		//--------------------------------------------------------------------
		virtual void DataChanged()
		{
//			envtOperations::RefreshNames();
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

}

//--------------------------------------------------------------------
//  Clean up dialogs
//--------------------------------------------------------------------
void  envtDialogUtil::CleanUp()
{
	evmtEnvironmentMgr::UnRegisterEnvironmentInterest(&l_DataChangedObj);

	RemoveDataPage();

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
void envtDialogUtil::ClearDialog()
{
#ifdef USE_WXWIDGETS
	if (envtEnvironmentObjectsPage::Instance)
		envtEnvironmentObjectsPage::Instance->Clear();
	if (envtEnvironmentObjectsPage::Instance)
		envtEnvironmentObjectsPage::Instance->Clear();
#ifdef USE_SWL_UI
	if (envtEnvironmentSwlPage::Instance)
		envtEnvironmentSwlPage::Instance->Clear();
#endif
#endif // USE_WXWIDGETS
}
void envtDialogUtil::UpdateDialog(const nameString& i_EnvironmentName)
{

#ifdef USE_WXWIDGETS
	if (envtEnvironmentObjectsPage::Instance)
	{
		envtEnvironmentObjectsPage::Instance->Update(i_EnvironmentName);
	}
#ifdef USE_SWL_UI
	if (envtEnvironmentSwlPage::Instance)
	{
		envtEnvironmentSwlPage::Instance->Update(i_EnvironmentName);
	}
#endif
#endif // USE_WXWIDGETS
}

//--------------------------------------------------------------------
// Update specific pages when membership changed from something
// outside GUI checkboxes
//--------------------------------------------------------------------
void envtDialogUtil::UpdateObjectsPage()
{
#ifdef USE_WXWIDGETS
	if (envtEnvironmentObjectsPage::Instance)
	{
		if (cmmObjectDialogUtil::HasTabPage(envtEnvironmentObjectsPage::Instance))
		{
			envtEnvironmentObjectsPage::Instance->ReUpdate();
		}
	}
#ifdef USE_SWL_UI
	if (envtEnvironmentSwlPage::Instance)
	{
		if (cmmObjectDialogUtil::HasTabPage(envtEnvironmentSwlPage::Instance))
		{
			envtEnvironmentSwlPage::Instance->ReUpdate();
		}
	}
#endif
#endif // USE_WXWIDGETS
}

//--------------------------------------------------------------------
//  Add/Remove tab page from selected object dialog
//--------------------------------------------------------------------
void  envtDialogUtil::AddDataPage()
{
#ifdef USE_WXWIDGETS
	if (cmmObjectDialog::FormInstance)
	{
#ifdef USE_SWL_UI
		if (!envtEnvironmentSwlPage::Instance)
			envtEnvironmentSwlPage::Instance = new envtEnvironmentSwlPage(cmmObjectDialog::FormInstance->GetNotebook());
		if (!cmmObjectDialogUtil::HasTabPage(envtEnvironmentSwlPage::Instance))
		{
			cmmObjectDialogUtil::AddTabPage( envtEnvironmentSwlPage::Instance, "Software Lighting" );
		}
#endif

		if (!envtEnvironmentObjectsPage::Instance)
			envtEnvironmentObjectsPage::Instance = new envtEnvironmentObjectsPage(cmmObjectDialog::FormInstance->GetNotebook());
		if (!cmmObjectDialogUtil::HasTabPage(envtEnvironmentObjectsPage::Instance))
		{
			cmmObjectDialogUtil::AddTabPage( envtEnvironmentObjectsPage::Instance, "Objects" );
		}
	}
#endif
}

void  envtDialogUtil::RemoveDataPage()
{
#ifdef USE_WXWIDGETS
	if (envtEnvironmentObjectsPage::Instance)
	{
		if (cmmObjectDialogUtil::HasTabPage(envtEnvironmentObjectsPage::Instance))
		{
			cmmObjectDialogUtil::RemoveTabPage( envtEnvironmentObjectsPage::Instance );
			envtEnvironmentObjectsPage::Instance->Show(false);
		}
	}
#ifdef USE_SWL_UI 
	if (envtEnvironmentSwlPage::Instance)
	{
		if (cmmObjectDialogUtil::HasTabPage(envtEnvironmentSwlPage::Instance))
		{
			cmmObjectDialogUtil::RemoveTabPage( envtEnvironmentSwlPage::Instance );
			envtEnvironmentSwlPage::Instance->Show(false);
		}
	}
#endif
#endif
}
