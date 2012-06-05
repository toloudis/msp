/*****************************************************************************
**	todDialogUtil.cpp
**
**	API for opening dialogs for system
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/


#include "todDialogUtil.hpp"

#include "todTimeOfDayDataForm.h"
#include "todDataMgr.hpp"

#include "tmaDialogTabbedMgr.hpp"


//============================================================================
//============================================================================
using namespace GeneratedForms;



//
namespace todDialogUtil
{
	namespace
	{
		// Callback when any data in todDataMgr changes
		class MyDataChanged : public todDataMgr::DataChangedCallback
		{
		public:
			virtual void DataChanged()
			{
				if (todTimeOfDayDataForm::FormInstance /*&&
					todTimeOfDayDataForm::FormInstance->Visible*/)
				{
					todTimeOfDayDataForm::FormInstance->Update(todDataMgr::GetData());
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
		todDataMgr::AddDataChangedCallback(&l_DataChangedObj);

		CreateTimeOfDayTabPage();
	}

	//--------------------------------------------------------------------
	//  Clean up dialogs
	//--------------------------------------------------------------------
	void  CleanUp()
	{
		todTimeOfDayDataForm::FormInstance = 0;
		todDataMgr::RemoveDataChangedCallback(&l_DataChangedObj);

	}

	//--------------------------------------------------------------------
	//  Create tab page for tod properties
	//--------------------------------------------------------------------
	void  CreateTimeOfDayTabPage()
	{
		if (!todTimeOfDayDataForm::FormInstance)
		{
			todTimeOfDayDataForm::FormInstance = new todTimeOfDayDataForm();
			tmaDialogTabbedMgr::AddTabPage( "Scene", todTimeOfDayDataForm::FormInstance->GetTabPage(0) );
			todTimeOfDayDataForm::FormInstance->Update(todDataMgr::GetData());
		}
	}

	//--------------------------------------------------------------------
	//  Show dialog to edit tod
	//--------------------------------------------------------------------
	void  ShowTimeOfDayDialog()
	{
		// Use class to keep track of instance if already visible
		if (!todTimeOfDayDataForm::FormInstance)
		{
			todTimeOfDayDataForm::FormInstance = new todTimeOfDayDataForm();
			tmaDialogTabbedMgr::AddTabPage( "Scene", todTimeOfDayDataForm::FormInstance->GetTabPage(0) );
		}

		todOperations::StartNewOp();

		todTimeOfDayDataForm::FormInstance->Update(todDataMgr::GetData());

		// FIX: (?) - should this happen on init?  right now it can't since it's created on the fly
		//
		//	register the tab page
		//
		//tmaDialogTabbedMgr::Show("Scene");

		//todTimeOfDayDataForm::FormInstance->Show();
	}

}	// end of namespace
