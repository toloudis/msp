/*****************************************************************************
**	rcdFloatDialogUtil.cpp
**
**	API for opening dialogs for system
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Record/Float/rcdFloatDialogUtil.hpp"

#include "Record/Float/rcdDriverFloatForm.h"

#include "ToolUIManaged/tma/tmaDialogTabbedMgr.hpp"
#include "Support/tmln/tmlnTimeInterest.hpp"


//============================================================================
//============================================================================
namespace rcdFloatDialogUtil
{
	namespace
	{
		class rcdFloatTimeInterest : public tmlnTimeInterest
		{
			virtual void TimeChanged(float i_Time)
			{
				rcdFloatDialogUtil::UpdateDialog(i_Time);
			}
			virtual void MaxTimeChanged(float i_Time)
			{
			}
			virtual void TimeFormatChanged(int i_TimeFormat)
			{
			}
		};

		rcdFloatTimeInterest* l_pInterest = NULL;

	}	// end of namespace


	//--------------------------------------------------------------------
	// Init
	//--------------------------------------------------------------------
	void Init()
	{
		l_pInterest = new rcdFloatTimeInterest();
		tmlnTimeLine::AddTimeInterest(l_pInterest);
	}

	//--------------------------------------------------------------------
	// CleanUp
	//--------------------------------------------------------------------
	void CleanUp()
	{
		tmlnTimeLine::RemoveTimeInterest(l_pInterest);
		delete l_pInterest;
	}

	//--------------------------------------------------------------------
	// Show
	//--------------------------------------------------------------------
	void  Show(rcdDriverFloat &i_Driver)
	{
#ifdef _MANAGED
		//	remove the tabs if any exist
		tmaDialogTabbedMgr::RemoveTabPages("Driver");

		//	add the driver specific tab page
		if (!StudioFramework::rcdDriverFloatForm::FormInstance)
		{
			StudioFramework::rcdDriverFloatForm::FormInstance = gcnew StudioFramework::rcdDriverFloatForm(i_Driver);
		}
		
		tmaDialogTabbedMgr::AddTabPage("Driver", StudioFramework::rcdDriverFloatForm::FormInstance->GetTabPage());
		i_Driver.AddBasePropertiesTab();

		//if (this->IsAutoPopUpEditProperties())
			tmaDialogTabbedMgr::Show("Driver");
#endif
	}

	//--------------------------------------------------------------------
	// Update dialog for current timeline time
	//--------------------------------------------------------------------
	void UpdateDialog(float i_Time)
	{
#ifdef _MANAGED
		if (StudioFramework::rcdDriverFloatForm::FormInstance )
		{
			StudioFramework::rcdDriverFloatForm::FormInstance->Update(i_Time);
		}
#endif
	}

}	// end of namespace
