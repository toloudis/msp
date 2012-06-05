/*****************************************************************************
**	sbrdDialogUtil.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/Storyboards/GUI/sbrdDialogUtil.hpp"

#include "Systems/Storyboards/GUI/sbrdDialogDataUtil.hpp"
#include "Systems/Storyboards/Object/sbrdObjectMgr.hpp"
#include "Systems/Storyboards/Undo/sbrdOperations.hpp"
#include "Systems/Storyboards/GUI/sbrdStoryboardsForm.h"

#include "Systems/Common/GUI/cmmSystemDialogInterest.hpp"
#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"


//============================================================================
//============================================================================
namespace
{
	sbrdListData l_CurData;


	// Callback when any system dialog requests data
	//
	class MySystemDialogInterest : public cmmSystemDialogInterest
	{
	public:
		//--------------------------------------------------------------------
		//	SceneDialogOpen - perform tasks (like adding tabs) relating
		//	to the scene/system dialog opening.  These tasks happen each time
		//	the scene dialog is launched.
		//--------------------------------------------------------------------
		//virtual 
		void SceneDialogOpen()
		{
#ifdef _MANAGED
			//	create the dialog if needed
			//
			if (SystemStoryboards::sbrdStoryboardsForm::FormInstance == nullptr)
				sbrdDialogUtil::SetupDialog();

			//	add the tab page(s) and update the form (if necessary)
			//
			cmmSystemDialogUtil::AddTabPage(SystemStoryboards::sbrdStoryboardsForm::FormInstance->GetTabPage());
			SystemStoryboards::sbrdStoryboardsForm::FormInstance->Update();
#endif
		}

		//--------------------------------------------------------------------
		//	SceneDialogClose - perform tasks (like adding tabs) relating
		//	to the scene/system dialog closing.  These tasks happen each time
		//	the scene dialog is closed.
		//--------------------------------------------------------------------
		//virtual 
		void SceneDialogClose()
		{
#ifdef _MANAGED
			if (SystemStoryboards::sbrdStoryboardsForm::FormInstance != nullptr)
			{
				//	remove the system tab page
				//
				cmmSystemDialogUtil::RemoveTabPage(SystemStoryboards::sbrdStoryboardsForm::FormInstance->GetTabPage());

				//	free up for form pointer and create a new one if needed
				//
				//tmaSystem::g_pMainForm->RemoveOwnedForm(SystemStoryboards::sbrdStoryboardsForm::FormInstance);
				SystemStoryboards::sbrdStoryboardsForm::FormInstance = nullptr;
			}
#endif
		}
	};
	
	MySystemDialogInterest	l_SystemDialogInterest;
}	// end of namespace

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void sbrdDialogUtil::Init()
{
	cmmSystemDialogUtil::RegisterInterest(&l_SystemDialogInterest);
}

//--------------------------------------------------------------------
// Clean up dialogs
//--------------------------------------------------------------------
void sbrdDialogUtil::CleanUp()
{
	cmmSystemDialogUtil::UnRegisterInterest(&l_SystemDialogInterest);
}

//--------------------------------------------------------------------
//	create + setup the dialog
//--------------------------------------------------------------------
//static 
void sbrdDialogUtil::SetupDialog()
{
#ifdef _MANAGED
	if (SystemStoryboards::sbrdStoryboardsForm::FormInstance == nullptr)
	{
		SystemStoryboards::sbrdStoryboardsForm::FormInstance = gcnew SystemStoryboards::sbrdStoryboardsForm();
		//SystemStoryboards::sbrdStoryboardsForm::FormInstance->Update();
	}
#endif
}

//--------------------------------------------------------------------
// Update individual Billboard properties dialog
//--------------------------------------------------------------------
void sbrdDialogUtil::UpdateStoryboardData(int i_Index, const sbrdListData& i_Data)
{
	l_CurData = i_Data;
	//DBG_LOG3( "update sbrd pos (%6.3f, %6.3f, %6.3f)", i_Data.m_Position.GetX(), i_Data.m_Position.GetY(), i_Data.m_Position.GetZ() );

	sbrdOperations::SetSelectedIndex(i_Index);

//OUT	if (sbrdBillboardListForm::FormInstance)
//OUT	{
//OUT		sbrdBillboardListForm::FormInstance->Select(i_Index);
//OUT	}
}


//--------------------------------------------------------------------
//	Update the dialog list
//--------------------------------------------------------------------
//static 
void sbrdDialogUtil::UpdateListDialog()
{
	sbrdDialogDataUtil::UpdateListDialog();
}

