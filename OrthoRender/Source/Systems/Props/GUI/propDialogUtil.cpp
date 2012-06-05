/*****************************************************************************
**	propDialogUtil.cpp
**
**	 API for opening dialogs for system
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Systems/Props/GUI/propDialogUtil.hpp"

#include "Systems/Props/GUI/propDialogDataUtil.hpp"
#include "Systems/Props/Object/propObjectMgr.hpp"
#include "Systems/Props/Undo/propOperations.hpp"


//============================================================================
//============================================================================
namespace
{
	propData l_CurData;
	bool l_bDataPageAdded = false;

}	// end of namespace

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void propDialogUtil::Init()
{
}

//--------------------------------------------------------------------
// Clean up dialogs
//--------------------------------------------------------------------
void propDialogUtil::CleanUp()
{
}

//--------------------------------------------------------------------
// Update individual prop properties dialog
//--------------------------------------------------------------------
void propDialogUtil::UpdatePropData(int i_Index, const propData& i_Data)
{
	l_CurData = i_Data;
	//DBG_LOG3( "update prop pos (%6.3f, %6.3f, %6.3f)", i_Data.m_Position.GetX(), i_Data.m_Position.GetY(), i_Data.m_Position.GetZ() );

	propOperations::SetSelectedIndex(i_Index);

	/*if (propDataForm::FormInstance)
	{
		propDataForm::FormInstance->Select(i_Index);
	}*/

	/*if (propDataForm::FormInstance)
	{
		propDataForm::FormInstance->Enable(true);
		propDataForm::FormInstance->Update();
	}*/
}

//--------------------------------------------------------------------
// Update individual Prop properties dialog
//--------------------------------------------------------------------
void propDialogUtil::UpdatePropData(int i_Index, const propScriptData& i_Data)
{
	l_CurData = i_Data.m_BaseData;
	//DBG_LOG3( "update prop pos (%6.3f, %6.3f, %6.3f)", i_Data.m_Position.GetX(), i_Data.m_Position.GetY(), i_Data.m_Position.GetZ() );

	propOperations::SetSelectedIndex(i_Index);

	/*if (propDataForm::FormInstance)
	{
		propDataForm::FormInstance->Select(i_Index);
	}

	if (propDataForm::FormInstance)
	{
		propDataForm::FormInstance->Enable(true);
		propDataForm::FormInstance->Update();
	}*/
}



//--------------------------------------------------------------------
//	Update the dialog list
//--------------------------------------------------------------------
//static 
void propDialogUtil::UpdateListDialog()
{
	propDialogDataUtil::UpdateListDialog();
}
