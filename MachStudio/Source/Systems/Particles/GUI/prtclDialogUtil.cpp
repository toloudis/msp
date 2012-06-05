/*****************************************************************************
**	prtclDialogUtil.cpp
**
**	 API for opening dialogs for system
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Systems/Particles/GUI/prtclDialogUtil.hpp"

#include "Systems/Particles/GUI/prtclDialogDataUtil.hpp"
#include "Systems/Particles/Object/prtclObjectMgr.hpp"
#include "Systems/Particles/Undo/prtclOperations.hpp"


//============================================================================
//============================================================================
namespace
{
	prtclData l_CurData;
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
void prtclDialogUtil::Init()
{
}

//--------------------------------------------------------------------
// Clean up dialogs
//--------------------------------------------------------------------
void prtclDialogUtil::CleanUp()
{
}

//--------------------------------------------------------------------
// Update individual Particle properties dialog
//--------------------------------------------------------------------
void prtclDialogUtil::UpdateParticleData(int i_Index, const prtclData& i_Data)
{
	l_CurData = i_Data;
	//DBG_LOG3( "update prtcl pos (%6.3f, %6.3f, %6.3f)", i_Data.m_Position.GetX(), i_Data.m_Position.GetY(), i_Data.m_Position.GetZ() );

	prtclOperations::SetSelectedIndex(i_Index);
}

//--------------------------------------------------------------------
// Update individual Particle properties dialog
//--------------------------------------------------------------------
void prtclDialogUtil::UpdateParticleData(int i_Index, const prtclScriptData& i_Data)
{
	l_CurData = i_Data.m_BaseData;
	//DBG_LOG3( "update prtcl pos (%6.3f, %6.3f, %6.3f)", i_Data.m_Position.GetX(), i_Data.m_Position.GetY(), i_Data.m_Position.GetZ() );

	prtclOperations::SetSelectedIndex(i_Index);
}


//--------------------------------------------------------------------
//	Update the dialog list
//--------------------------------------------------------------------
//static 
void prtclDialogUtil::UpdateListDialog()
{
	prtclDialogDataUtil::UpdateListDialog();
}
