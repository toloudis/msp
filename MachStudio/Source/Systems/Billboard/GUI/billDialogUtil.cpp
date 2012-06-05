/*****************************************************************************
**	billDialogUtil.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Systems/Billboard/GUI/billDialogUtil.hpp"

#include "Systems/Billboard/GUI/billDialogDataUtil.hpp"
#include "Systems/Billboard/Object/billObjectMgr.hpp"
#include "Systems/Billboard/Undo/billOperations.hpp"


//============================================================================
//============================================================================
namespace
{
	billData l_CurData;
}	// end of namespace

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void billDialogUtil::Init()
{
}

//--------------------------------------------------------------------
// Clean up dialogs
//--------------------------------------------------------------------
void billDialogUtil::CleanUp()
{
}

//--------------------------------------------------------------------
// Update individual Billboard properties dialog
//--------------------------------------------------------------------
void billDialogUtil::UpdateBillboardData(int i_Index, const billData& i_Data)
{
	l_CurData = i_Data;
	//DBG_LOG3( "update bill pos (%6.3f, %6.3f, %6.3f)", i_Data.m_Position.GetX(), i_Data.m_Position.GetY(), i_Data.m_Position.GetZ() );

	billOperations::SetSelectedIndex(i_Index);

//OUT	if (billBillboardListForm::FormInstance)
//OUT	{
//OUT		billBillboardListForm::FormInstance->Select(i_Index);
//OUT	}
}


//--------------------------------------------------------------------
//	Update the dialog list
//--------------------------------------------------------------------
//static 
void billDialogUtil::UpdateListDialog()
{
	billDialogDataUtil::UpdateListDialog();
}

