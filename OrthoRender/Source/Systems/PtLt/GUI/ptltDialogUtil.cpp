/*****************************************************************************
**	ptltDialogUtil.cpp
**
**	API for opening dialogs for system
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Systems/PtLt/GUI/ptltDialogUtil.hpp"

#include "Systems/PtLt/GUI/ptltDialogDataUtil.hpp"
#include "Systems/PtLt/Object/ptltObjectMgr.hpp"
#include "Systems/PtLt/Undo/ptltOperations.hpp"


//============================================================================
//============================================================================
namespace
{
	ptltScriptData l_CurData;
}	// end of namespace


//--------------------------------------------------------------------
// Init
//--------------------------------------------------------------------
void  ptltDialogUtil::Init()
{
}

//--------------------------------------------------------------------
//  Clean up dialogs
//--------------------------------------------------------------------
void  ptltDialogUtil::CleanUp()
{
}

//--------------------------------------------------------------------
// Update individual point light properties dialog
//--------------------------------------------------------------------
void ptltDialogUtil::UpdateLightData(int i_Index, const ptltData& i_Data)
{
	l_CurData.m_BaseData = i_Data;

	ptltOperations::SetSelectedIndex(i_Index);
}

//--------------------------------------------------------------------
// Update individual point light properties dialog
//--------------------------------------------------------------------
void ptltDialogUtil::UpdateLightData(int i_Index, const ptltScriptData& i_Data)
{
	l_CurData = i_Data;

	ptltOperations::SetSelectedIndex(i_Index);
}


//--------------------------------------------------------------------
//	Update the dialog list
//--------------------------------------------------------------------
//static 
void ptltDialogUtil::UpdateListDialog()
{
	ptltDialogDataUtil::UpdateListDialog();
}
