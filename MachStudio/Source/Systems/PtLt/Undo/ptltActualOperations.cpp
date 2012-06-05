/*****************************************************************************
**	ptltActualOperations.cpp
**
**	Everything outside of the "undo" folder should call a function
**	in the "Operations" namespace to make any change to the data.
**	This namespace is for internal use of the classes in the undo folder.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Systems/PtLt/Undo/ptltActualOperations.hpp"

#include "Systems/PtLt/Data/ptltDocumentChunk.hpp"

#include "Support/cmps/cmpsCompassMgr.hpp"
#include "Tool/gpx/gpxRenderControl.hpp"


//============================================================================
//============================================================================
//--------------------------------------------------------------------
//  Add new point light to world
//--------------------------------------------------------------------
int  ptltActualOperations::AddObject(const ptltScriptData& i_Data)
{
	// stop any render threads
	gpxRenderControl::ConfirmSingleThread();

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
	// stop any render threads
	gpxRenderControl::ConfirmSingleThread();

	ptltObjectMgr::DeleteObject(i_Index);
	ptltDialogDataUtil::UpdateListDialog();
	ptltDocumentChunk::ActiveDataChanged();
}
