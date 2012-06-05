/*****************************************************************************
**	chtrDialogUtil.cpp
**
**	 API for opening dialogs for system
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Systems/Character/GUI/chtrDialogUtil.hpp"

#include "Systems/Character/GUI/chtrDialogDataUtil.hpp"
#include "Systems/Character/Object/chtrObjectMgr.hpp"
#include "Systems/Character/Undo/chtrOperations.hpp"


//============================================================================
//============================================================================
namespace
{
	chtrData l_CurData;
}	// end of namespace

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void chtrDialogUtil::Init()
{
}

//--------------------------------------------------------------------
// Clean up dialogs
//--------------------------------------------------------------------
void chtrDialogUtil::CleanUp()
{
}


//--------------------------------------------------------------------
// Update individual properties dialog
//--------------------------------------------------------------------
void chtrDialogUtil::UpdateCharacterData(int i_Index, const chtrData& i_Data)
{
	l_CurData = i_Data;
	//DBG_LOG3( "update character pos (%6.3f, %6.3f, %6.3f)", i_Data.m_Position.GetX(), i_Data.m_Position.GetY(), i_Data.m_Position.GetZ() );

	chtrOperations::SetSelectedIndex(i_Index);

	//if (chtrCharacterDataForm::FormInstance)
	//{
	//	chtrCharacterDataForm::FormInstance->Select(i_Index);
	//}

	//if (chtrCharacterDataForm::FormInstance)
	//{
	//	chtrCharacterDataForm::FormInstance->Enable(true);
	//	chtrCharacterDataForm::FormInstance->Update();
	//}
}

//--------------------------------------------------------------------
// Character Data can only be displayed when a Character is selected.
//	When a Character is not selected, call this function to disable
//	the interface.
//--------------------------------------------------------------------
void chtrDialogUtil::DisableCharacterDataDialog()
{
	//if (chtrCharacterDataForm::FormInstance)
	//{
	//	chtrCharacterDataForm::FormInstance->Enable(false);
	//}
}


//--------------------------------------------------------------------
//	Update the dialog list
//--------------------------------------------------------------------
//static 
void chtrDialogUtil::UpdateListDialog()
{
	chtrDialogDataUtil::UpdateListDialog();
}
