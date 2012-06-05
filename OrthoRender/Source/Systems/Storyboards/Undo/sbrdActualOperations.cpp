/*****************************************************************************
**	sbrdActualOperations.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/Storyboards/Undo/sbrdActualOperations.hpp"

#include "Systems/Storyboards/GUI/sbrdDialogDataUtil.hpp"
#include "Systems/Storyboards/Data/sbrdDocumentChunk.hpp"
#include "Systems/Storyboards/Timeline/sbrdDriverCreator.hpp"

#include "Features/Channels/chnlDialogUtil.hpp"
#include "Support/cmps/cmpsCompassMgr.hpp"
#include "Core/fs/fsResourceTracker.hpp"



//--------------------------------------------------------------------
//	Insert the storyboard
//--------------------------------------------------------------------
void sbrdActualOperations::InsertStoryboard(int i_Index, const itString& i_Filename)
{
	sbrdListData& ldata = sbrdObjectMgr::GetListData();

	//DBG_LOG1("storyboard count-%d", ldata.m_Filenames.size());
	ldata.Insert(i_Index, i_Filename);

	sbrdDialogUtil::UpdateListDialog();

	sbrdDocumentChunk::ActiveDataChanged();
	//sbrdObjectMgr::SelectObject(index);
}

//--------------------------------------------------------------------
//  Add new storyboard to world
//--------------------------------------------------------------------
//static 
int sbrdActualOperations::AddStoryboard(const itString& i_Filename, bool i_bAddTo3DWorld)
{
	int index = sbrdObjectMgr::AddStoryboard(i_Filename, i_bAddTo3DWorld);

	//DBG_LOG1("storyboard count %d", ldata.m_Filenames.size());

	sbrdDialogUtil::UpdateListDialog();

	sbrdDocumentChunk::ActiveDataChanged();

	sbrdObjectMgr::SelectObject(0);
	chnlDialogUtil::ObjectSelected(sbrdObjectMgr::GetObject(0));

	return index;
}

//--------------------------------------------------------------------
//  Delete Billboard with specific data or a given index
//--------------------------------------------------------------------
//static 
void sbrdActualOperations::DeleteStoryboard(int i_Index)
{
	sbrdObjectMgr::DeleteStoryboard(i_Index);

	sbrdDialogUtil::UpdateListDialog();

	sbrdDocumentChunk::ActiveDataChanged();

	//sbrdObjectMgr::SelectObject(0);
	//chnlDialogUtil::ObjectSelected(sbrdObjectMgr::GetObject(0));
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
//static 
void sbrdActualOperations::SwapStoryboards(int i_Index1, int i_Index2)
{
	sbrdObjectMgr::SwapStoryboards(i_Index1, i_Index2);

	sbrdDialogUtil::UpdateListDialog();

	sbrdDocumentChunk::ActiveDataChanged();

	//sbrdObjectMgr::SelectObject(0);
	//chnlDialogUtil::ObjectSelected(sbrdObjectMgr::GetObject(0));
}

//--------------------------------------------------------------------
//  Add new object to world
//--------------------------------------------------------------------
int sbrdActualOperations::AddObject(const sbrdScriptData& i_Data)
{
	int index = sbrdObjectMgr::AddObject( i_Data );

	sbrdDialogDataUtil::UpdateListDialog();
	sbrdDocumentChunk::ActiveDataChanged();

	return index;
}

//--------------------------------------------------------------------
//  Delete object with given index
//--------------------------------------------------------------------
void sbrdActualOperations::DeleteObject(int i_Index)
{
	sbrdObjectMgr::DeleteObject( i_Index );

	sbrdDialogUtil::UpdateListDialog();
	sbrdDocumentChunk::ActiveDataChanged();
}

