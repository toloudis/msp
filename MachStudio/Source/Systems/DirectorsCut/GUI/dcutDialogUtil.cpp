/*****************************************************************************
**	dcutDialogUtil.cpp
**
**	API for opening dialogs for system
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/DirectorsCut/GUI/dcutDialogUtil.hpp"

#include "Systems/DirectorsCut/GUI/dcutDialogDataUtil.hpp"
#include "Systems/DirectorsCut/Object/dcutObjectMgr.hpp"
#include "Systems/DirectorsCut/Undo/dcutOperations.hpp"

#include "Support/cams/camsFollowUtil.hpp"
#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"


//============================================================================
//============================================================================
//using namespace StudioFramework;


//============================================================================
//============================================================================
namespace
{
	dcutCueData l_CurData;

	bool l_bSystemPageAdded		= false;

	//// Callback when any system dialog requests data
	////
	//class MySystemDialogInterest : public cmmSystemDialogInterest
	//{
	//public:
	//	//--------------------------------------------------------------------
	//	//	SceneDialogOpen - perform tasks (like adding tabs) relating
	//	//	to the scene/system dialog opening.  These tasks happen each time
	//	//	the scene dialog is launched.
	//	//--------------------------------------------------------------------
	//	//virtual 
	//	void SceneDialogOpen()
	//	{
	//		//	create the dialog if needed
	//		//
	//		if (lyerLayersDataForm::FormInstance == nullptr)
	//			create_dialog();

	//		//	add the tab page(s) and update the form (if necessary)
	//		//
	//		cmmSystemDialogUtil::AddTabPage(lyerLayersDataForm::FormInstance->GetTabPage());
	//		lyerLayersDataForm::FormInstance->Update();
	//	}

	//	//--------------------------------------------------------------------
	//	//	SceneDialogClose - perform tasks (like adding tabs) relating
	//	//	to the scene/system dialog closing.  These tasks happen each time
	//	//	the scene dialog is closed.
	//	//--------------------------------------------------------------------
	//	//virtual 
	//	void SceneDialogClose()
	//	{
	//		if (lyerLayersDataForm::FormInstance != nullptr)
	//		{
	//			//	remove the system tab page
	//			//
	//			cmmSystemDialogUtil::RemoveTabPage(lyerLayersDataForm::FormInstance->GetTabPage());

	//			//	free up for form pointer and create a new one if needed
	//			//
	//			//tmaSystem::g_pMainForm->RemoveOwnedForm(lyerLayersDataForm::FormInstance);
	//			lyerLayersDataForm::FormInstance = nullptr;
	//		}
	//	}
	//};
	//
	//MySystemDialogInterest	l_SystemDialogInterest;

}	// end of namespace


//--------------------------------------------------------------------
// Init
//--------------------------------------------------------------------
void  dcutDialogUtil::Init()
{
}

//--------------------------------------------------------------------
//  Clean up dialogs
//--------------------------------------------------------------------
void  dcutDialogUtil::CleanUp()
{
}

//--------------------------------------------------------------------
// Update individual camera properties dialog
//--------------------------------------------------------------------
void dcutDialogUtil::UpdateCameraData(int i_Index, const dcutCueData& i_Data)
{
	l_CurData = i_Data;

	if ( i_Index >= 0 )
	{
		dcutOperations::SetSelectedIndex(i_Index);
	}
}

//--------------------------------------------------------------------
// Update individual camera properties dialog
//--------------------------------------------------------------------
void dcutDialogUtil::UpdateCameraData(int i_Index, const dcutScriptData& i_Data)
{
	l_CurData = i_Data.m_BaseData;

	if ( i_Index >= 0 )
	{
		dcutOperations::SetSelectedIndex(i_Index);
	}
}

//--------------------------------------------------------------------
//	Select no camera
//--------------------------------------------------------------------
void dcutDialogUtil::DeselectCamera()
{
	dcutOperations::SetSelectedIndex( -1 );
	camsFollowUtil::SetEditorCamera();
}

//--------------------------------------------------------------------
//	Update the dialog list
//--------------------------------------------------------------------
//static 
void dcutDialogUtil::UpdateListDialog()
{
	dcutDialogDataUtil::UpdateListDialog();
}
