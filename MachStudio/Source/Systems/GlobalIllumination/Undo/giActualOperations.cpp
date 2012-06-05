/*****************************************************************************
**	giActualOperations.cpp
**
**	Everything outside of the "undo" folder should call a function
**	in the "Operations" namespace to make any change to the data.
**	This namespace is for internal use of the classes in the undo folder.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "Systems/GlobalIllumination/Undo/giActualOperations.hpp"

#include "Systems/GlobalIllumination/GUI/giDialogDataUtil.hpp"
#include "Systems/GlobalIllumination/GUI/giDialogUtil.hpp"
#include "Systems/GlobalIllumination/Data/giDocumentChunk.hpp"

//============================================================================
//============================================================================
//--------------------------------------------------------------------
//  Add new light set
//--------------------------------------------------------------------
int  giActualOperations::AddObject(const giScriptData& i_Data)
{
	int index = giObjectMgr::AddObject(i_Data);
	giDialogDataUtil::UpdateListDialog();
	giDocumentChunk::ActiveDataChanged();

	// not sure if this should be here or in giOperations
	giObjectMgr::SelectObject();
	return 0;
}

//--------------------------------------------------------------------
//  Delete light set with given index
//--------------------------------------------------------------------
void  giActualOperations::DeleteObject()
{
	giObjectMgr::DeleteObject();
	giDialogDataUtil::UpdateListDialog();
	giDocumentChunk::ActiveDataChanged();
}

//--------------------------------------------------------------------
// Update individual driver properties
//--------------------------------------------------------------------
void giActualOperations::ChangeDriverData(const giScriptData& i_Data)
{
	// no need to notify giObjectMgr if there is no undo, 
	// this message is coming from the object directly

	giDocumentChunk::ActiveDataChanged();
}

