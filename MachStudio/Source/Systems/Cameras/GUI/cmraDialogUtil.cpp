/*****************************************************************************
**	cmraDialogUtil.cpp
**
**	API for opening dialogs for system
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Systems/Cameras/GUI/cmraDialogUtil.hpp"

#include "Systems/Cameras/GUI/cmraDialogDataUtil.hpp"

//============================================================================
//============================================================================
namespace
{

}	// end of namespace


//--------------------------------------------------------------------
// Init
//--------------------------------------------------------------------
void  cmraDialogUtil::Init()
{
}

//--------------------------------------------------------------------
//  Clean up dialogs
//--------------------------------------------------------------------
void  cmraDialogUtil::CleanUp()
{
}

//--------------------------------------------------------------------
//	Update the dialog list
//--------------------------------------------------------------------
//static 
void cmraDialogUtil::UpdateListDialog()
{
	cmraDialogDataUtil::UpdateListDialog();
}

//--------------------------------------------------------------------
//	Delete
//--------------------------------------------------------------------
//static 
void cmraDialogUtil::DeleteObject(int i_Index)
{
	cmraDialogDataUtil::DeleteObject(i_Index);
}
