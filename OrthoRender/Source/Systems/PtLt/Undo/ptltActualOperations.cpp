/*****************************************************************************
**	ptltActualOperations.cpp
**
**	Everything outside of the "undo" folder should call a function
**	in the "Operations" namespace to make any change to the data.
**	This namespace is for internal use of the classes in the undo folder.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Systems/PtLt/Undo/ptltActualOperations.hpp"

#include "Systems/PtLt/GUI/ptltDialogDataUtil.hpp"
#include "Systems/PtLt/GUI/ptltDialogUtil.hpp"
#include "Systems/PtLt/Data/ptltDocumentChunk.hpp"

#include "Support/cmps/cmpsCompassMgr.hpp"


//============================================================================
//============================================================================
//--------------------------------------------------------------------
//  Add new point light to world
//--------------------------------------------------------------------
int  ptltActualOperations::AddObject(const ptltScriptData& i_Data)
{
	int index = ptltObjectMgr::AddObject(i_Data);
	ptltDialogDataUtil::UpdateListDialog();
	ptltDocumentChunk::ActiveDataChanged();

	// not sure if this should be here or in ptltOperations
	ptltObjectMgr::SelectObject(index);
	return index;
}

//--------------------------------------------------------------------
//  Delete point light with given index
//--------------------------------------------------------------------
void  ptltActualOperations::DeleteObject(int i_Index)
{
	ptltObjectMgr::DeleteObject(i_Index);
	ptltDialogDataUtil::UpdateListDialog();
	ptltDocumentChunk::ActiveDataChanged();
}

//--------------------------------------------------------------------
// Update individual driver properties
//--------------------------------------------------------------------
void ptltActualOperations::ChangeDriverData(int i_Index, const ptltScriptData& i_Data)
{
	// no need to notify ptltObjectMgr if there is no undo, 
	// this message is coming from the object directly

	ptltDialogUtil::UpdateLightData(i_Index, i_Data);
	ptltDocumentChunk::ActiveDataChanged();;
}
