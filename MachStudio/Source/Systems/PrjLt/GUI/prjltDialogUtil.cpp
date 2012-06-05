/*****************************************************************************
**	prjltDialogUtil.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Systems/PrjLt/GUI/prjltDialogUtil.hpp"

#include "Systems/PrjLt/GUI/prjltDialogDataUtil.hpp"
#include "Systems/PrjLt/Object/prjltObjectMgr.hpp"
#include "Systems/PrjLt/Undo/prjltOperations.hpp"


//============================================================================
//============================================================================
//using namespace SystemProjectedLights;


//============================================================================
//============================================================================
namespace
{
	prjltData l_CurData;
}	// end of namespace

//--------------------------------------------------------------------
// Init
//--------------------------------------------------------------------
void  prjltDialogUtil::Init()
{
}

//--------------------------------------------------------------------
//  Clean up dialogs
//--------------------------------------------------------------------
void  prjltDialogUtil::CleanUp()
{
}

//--------------------------------------------------------------------
// Update individual properties dialog
//--------------------------------------------------------------------
void prjltDialogUtil::UpdateData(int i_Index, const prjltData& i_Data)
{
	l_CurData = i_Data;

	prjltOperations::SetSelectedIndex(i_Index);
}

//--------------------------------------------------------------------
// Update individual projected light properties dialog
//--------------------------------------------------------------------
void prjltDialogUtil::UpdateLightData(int i_Index, const prjltScriptData& i_Data)
{
	l_CurData = i_Data.m_BaseData;

	prjltOperations::SetSelectedIndex(i_Index);
}


//--------------------------------------------------------------------
//	Update the dialog list
//--------------------------------------------------------------------
//static 
void prjltDialogUtil::UpdateListDialog()
{
	prjltDialogDataUtil::UpdateListDialog();
}
