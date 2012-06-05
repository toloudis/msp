/*****************************************************************************
**	sbrdDialogUtil.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/Storyboards/GUI/sbrdDialogUtil.hpp"

#include "Systems/Storyboards/GUI/sbrdDialogDataUtil.hpp"
#include "Systems/Storyboards/Object/sbrdObjectMgr.hpp"
#include "Systems/Storyboards/Undo/sbrdOperations.hpp"

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

		}

		//--------------------------------------------------------------------
		//	SceneDialogClose - perform tasks (like adding tabs) relating
		//	to the scene/system dialog closing.  These tasks happen each time
		//	the scene dialog is closed.
		//--------------------------------------------------------------------
		//virtual 
		void SceneDialogClose()
		{

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

