/*****************************************************************************
**	skyDialogUtil.cpp
**
**	API for opening dialogs for system
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/


#include "skyDialogUtil.hpp"

#include "skySkyDataForm.h"

#include "tmaDialogTabbedMgr.hpp"


//============================================================================
//============================================================================
using namespace GeneratedForms;



//
namespace skyDialogUtil
{
	namespace
	{
		// Callback when any data in skyDataMgr changes
		class MyDataChanged : public skyDataMgr::DataChangedCallback
		{
		public:
			virtual void DataChanged()
			{
				if (skySkyDataForm::FormInstance /*&&
					skySkyDataForm::FormInstance->Visible*/)
				{
					skySkyDataForm::FormInstance->Update(daySkyMgr::GetListData());
				}

			}
		};

		MyDataChanged l_DataChangedObj;

	}	// end of namespace

	//--------------------------------------------------------------------
	// Init
	//--------------------------------------------------------------------
	void  Init()
	{
		skyDataMgr::AddDataChangedCallback(&l_DataChangedObj);

		CreateSkyTabPage();
	}

	//--------------------------------------------------------------------
	//  Clean up dialogs
	//--------------------------------------------------------------------
	void  CleanUp()
	{
		skySkyDataForm::FormInstance = 0;
		skyDataMgr::RemoveDataChangedCallback(&l_DataChangedObj);

	}

	//--------------------------------------------------------------------
	//  Create tab page for sky properties
	//--------------------------------------------------------------------
	void  CreateSkyTabPage()
	{
		if (!skySkyDataForm::FormInstance)
		{
			skySkyDataForm::FormInstance = new skySkyDataForm();
			tmaDialogTabbedMgr::AddTabPage( "Scene", skySkyDataForm::FormInstance->GetTabPage(0) );
			skySkyDataForm::FormInstance->Update(daySkyMgr::GetListData());
		}
	}

	//--------------------------------------------------------------------
	//  Show dialog to edit sky
	//--------------------------------------------------------------------
	void  ShowSkyDialog()
	{
		// Use class to keep track of instance if already visible
		if (!skySkyDataForm::FormInstance)
		{
			skySkyDataForm::FormInstance = new skySkyDataForm();
			tmaDialogTabbedMgr::AddTabPage( "Scene", skySkyDataForm::FormInstance->GetTabPage(0) );
		}

		skyOperations::StartNewOp();

		skySkyDataForm::FormInstance->Update(daySkyMgr::GetListData());

		// FIX: (?) - should this happen on init?  right now it can't since it's created on the fly
		//
		//	register the tab page
		//
		//tmaDialogTabbedMgr::Show("Scene");

		//skySkyDataForm::FormInstance->Show();
	}

	//--------------------------------------------------------------------
	// Update sky dialog
	//--------------------------------------------------------------------
	void UpdateDialog()
	{
		if (skySkyDataForm::FormInstance )
		{
			skySkyDataForm::FormInstance->Update( daySkyMgr::GetListData() );
		}
	}

}	// end of namespace
