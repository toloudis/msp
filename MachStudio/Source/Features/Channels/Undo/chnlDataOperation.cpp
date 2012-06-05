/*****************************************************************************
**	chnlDataOperation.cpp
**
**		see .hpp
**	
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#include "Features/Channels/Undo/chnlDataOperation.hpp"

#include "Features/Channels/Markers/chnlMarkerMgr.hpp"
#include "Features/Channels/Notes/chnlNotesMgr.hpp"
#include "Features/Channels/chnlDialogUtil.hpp"
#include "Features/Channels/Data/chnlTimeDocumentChunk.hpp"

//--------------------------------------------------------------------
// constructor takes old to be restored if undone
//--------------------------------------------------------------------
chnlMarkerDataOperation::chnlMarkerDataOperation(  int i_Index,
							const chnlMarkerDataItem& i_OldData,
							const chnlMarkerDataItem& i_NewData,
							const char* i_DisplayName)
:	m_Index( i_Index ),
	m_DataOld( i_OldData ),
	m_DataNew( i_NewData ),
	m_DisplayName( i_DisplayName )
{
}

//--------------------------------------------------------------------
//  Get Name for the operation
//--------------------------------------------------------------------
std::string chnlMarkerDataOperation::GetDisplayName()
{
	return m_DisplayName.c_str();
}

//--------------------------------------------------------------------
//  Get memory usage for this operation (in KB). This can be
// accurate or approximate.
//--------------------------------------------------------------------
float chnlMarkerDataOperation::GetMemoryUsage()
{
	return (sizeof(chnlMarkerDataItem) / 1000.0f); // convert to KB
}

//--------------------------------------------------------------------
//  Undo is called on an operation when the user chooses
// Edit->Undo from the menu.
//--------------------------------------------------------------------
void chnlMarkerDataOperation::Undo()
{
	//reload the previous data
	chnlMarkerMgr::SetMarkerData(m_Index, m_DataOld);
	chnlTimeDocumentChunk::ActiveDataChanged();
	chnlDialogUtil::UpdateMarkersAndNotes();
}

//--------------------------------------------------------------------
// Redo is called on an operation when the user chooses
// Edit->Redo from the menu and this operation is the next in
// line to be redone.
//--------------------------------------------------------------------
void chnlMarkerDataOperation::Redo()
{
	//reload the new data
	chnlMarkerMgr::SetMarkerData(m_Index, m_DataNew);
	chnlTimeDocumentChunk::ActiveDataChanged();
	chnlDialogUtil::UpdateMarkersAndNotes();
}

//--------------------------------------------------------------------
//  Commit is called on an operation when it is no longer
// possible for the user to undo this operation.  The
// destructor will soon be called.
//--------------------------------------------------------------------
void chnlMarkerDataOperation::Commit()
{
	// nothing needed
}

//--------------------------------------------------------------------
//  Destroy is called on an operation when it has been undone
// and it can no longer be redone. This may happen after the
// history gets long enough or a new operation is made when
// its current state is "undone". The destructor will soon be
// called.
//--------------------------------------------------------------------
void chnlMarkerDataOperation::Destroy()
{
	// nothing needed
}

//********************************************************************
// Note operations
//********************************************************************

//--------------------------------------------------------------------
// constructor takes old to be restored if undone
//--------------------------------------------------------------------
chnlNoteDataOperation::chnlNoteDataOperation(  int i_Index,
							const chnlNoteDataItem& i_OldData,
							const chnlNoteDataItem& i_NewData,
							const char* i_DisplayName)
:	m_Index( i_Index ),
	m_DataOld( i_OldData ),
	m_DataNew( i_NewData ),
	m_DisplayName( i_DisplayName )
{
}

//--------------------------------------------------------------------
//  Get Name for the operation
//--------------------------------------------------------------------
std::string chnlNoteDataOperation::GetDisplayName()
{
	return m_DisplayName.c_str();
}

//--------------------------------------------------------------------
//  Get memory usage for this operation (in KB). This can be
// accurate or approximate.
//--------------------------------------------------------------------
float chnlNoteDataOperation::GetMemoryUsage()
{
	return (sizeof(chnlNoteDataItem) / 1000.0f); // convert to KB
}

//--------------------------------------------------------------------
//  Undo is called on an operation when the user chooses
// Edit->Undo from the menu.
//--------------------------------------------------------------------
void chnlNoteDataOperation::Undo()
{
	//reload the previous data
	chnlNotesMgr::SetNoteData(m_Index, m_DataOld);
	chnlTimeDocumentChunk::ActiveDataChanged();
	chnlDialogUtil::UpdateMarkersAndNotes();
}

//--------------------------------------------------------------------
// Redo is called on an operation when the user chooses
// Edit->Redo from the menu and this operation is the next in
// line to be redone.
//--------------------------------------------------------------------
void chnlNoteDataOperation::Redo()
{
	//reload the new data
	chnlNotesMgr::SetNoteData(m_Index, m_DataNew);
	chnlTimeDocumentChunk::ActiveDataChanged();
	chnlDialogUtil::UpdateMarkersAndNotes();
}

//--------------------------------------------------------------------
//  Commit is called on an operation when it is no longer
// possible for the user to undo this operation.  The
// destructor will soon be called.
//--------------------------------------------------------------------
void chnlNoteDataOperation::Commit()
{
	// nothing needed
}

//--------------------------------------------------------------------
//  Destroy is called on an operation when it has been undone
// and it can no longer be redone. This may happen after the
// history gets long enough or a new operation is made when
// its current state is "undone". The destructor will soon be
// called.
//--------------------------------------------------------------------
void chnlNoteDataOperation::Destroy()
{
	// nothing needed
}