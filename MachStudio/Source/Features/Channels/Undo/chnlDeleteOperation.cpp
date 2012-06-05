/*****************************************************************************
**	chnlDeleteOperation.cpp
**
**		see .hpp
**	
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#include "Features/Channels/Undo/chnlDeleteOperation.hpp"

#include "Features/Channels/Markers/chnlMarkerMgr.hpp"
#include "Features/Channels/Notes/chnlNotesMgr.hpp"
#include "Features/Channels/chnlDialogUtil.hpp"
#include "Features/Channels/Data/chnlTimeDocumentChunk.hpp"

//--------------------------------------------------------------------
// constructor takes old to be restored if undone
//--------------------------------------------------------------------
chnlMarkerDeleteOperation::chnlMarkerDeleteOperation( const maTime& i_Time,
													  int i_Type,
													  std::string i_Note,
													  const char* i_DisplayName)
:	m_DisplayName( i_DisplayName )
{
	m_DataBackup.m_Time.SetValue(i_Time);
	m_DataBackup.m_TimeMarkerType.SetValue(i_Type);
	m_DataBackup.m_Note.SetValue(i_Note);
}

//--------------------------------------------------------------------
//  Get Name for the operation
//--------------------------------------------------------------------
std::string chnlMarkerDeleteOperation::GetDisplayName()
{
	return m_DisplayName.c_str();
}

//--------------------------------------------------------------------
//  Get memory usage for this operation (in KB). This can be
// accurate or approximate.
//--------------------------------------------------------------------
float chnlMarkerDeleteOperation::GetMemoryUsage()
{
	return (sizeof(chnlMarkerDataItem) / 1000.0f); // convert to KB
}

//--------------------------------------------------------------------
//  Undo is called on an operation when the user chooses
// Edit->Undo from the menu.
//--------------------------------------------------------------------
void chnlMarkerDeleteOperation::Undo()
{
	maTime time = m_DataBackup.m_Time.GetValue();
	int type = m_DataBackup.m_TimeMarkerType.GetValue();
	std::string note = m_DataBackup.m_Note.GetValue();

	chnlMarkerMgr::AddMarker(time, type, note);
	chnlTimeDocumentChunk::ActiveDataChanged();
	chnlDialogUtil::UpdateMarkersAndNotes();
}

//--------------------------------------------------------------------
// Redo is called on an operation when the user chooses
// Edit->Redo from the menu and this operation is the next in
// line to be redone.
//--------------------------------------------------------------------
void chnlMarkerDeleteOperation::Redo()
{
	maTime time = m_DataBackup.m_Time.GetValue();
	chnlMarkerMgr::DeleteMarker(time);
	chnlTimeDocumentChunk::ActiveDataChanged();
	chnlDialogUtil::UpdateMarkersAndNotes();
}

//--------------------------------------------------------------------
//  Commit is called on an operation when it is no longer
// possible for the user to undo this operation.  The
// destructor will soon be called.
//--------------------------------------------------------------------
void chnlMarkerDeleteOperation::Commit()
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
void chnlMarkerDeleteOperation::Destroy()
{
	// nothing needed
}

//********************************************************************
// Note operations
//********************************************************************

//--------------------------------------------------------------------
// constructor takes old to be restored if undone
//--------------------------------------------------------------------
chnlNoteDeleteOperation::chnlNoteDeleteOperation( const maTime& i_Time,
												  int i_Status,
												  std::string i_Note,
												  const char* i_DisplayName)
:	m_DisplayName( i_DisplayName )
{
	m_DataBackup.m_Time.SetValue(i_Time);
	m_DataBackup.m_Status.SetValue(i_Status);
	m_DataBackup.m_Note.SetValue(i_Note);
}

//--------------------------------------------------------------------
//  Get Name for the operation
//--------------------------------------------------------------------
std::string chnlNoteDeleteOperation::GetDisplayName()
{
	return m_DisplayName.c_str();
}

//--------------------------------------------------------------------
//  Get memory usage for this operation (in KB). This can be
// accurate or approximate.
//--------------------------------------------------------------------
float chnlNoteDeleteOperation::GetMemoryUsage()
{
	return (sizeof(chnlNoteDataItem) / 1000.0f); // convert to KB
}

//--------------------------------------------------------------------
//  Undo is called on an operation when the user chooses
// Edit->Undo from the menu.
//--------------------------------------------------------------------
void chnlNoteDeleteOperation::Undo()
{
	maTime time = m_DataBackup.m_Time.GetValue();
	int status = m_DataBackup.m_Status.GetValue();
	std::string note = m_DataBackup.m_Note.GetValue();

	chnlNotesMgr::AddNote(time, status, note);
	chnlTimeDocumentChunk::ActiveDataChanged();
	chnlDialogUtil::UpdateMarkersAndNotes();
}

//--------------------------------------------------------------------
// Redo is called on an operation when the user chooses
// Edit->Redo from the menu and this operation is the next in
// line to be redone.
//--------------------------------------------------------------------
void chnlNoteDeleteOperation::Redo()
{
	maTime time = m_DataBackup.m_Time.GetValue();
	chnlNotesMgr::DeleteNote(time);
	chnlTimeDocumentChunk::ActiveDataChanged();
	chnlDialogUtil::UpdateMarkersAndNotes();
}

//--------------------------------------------------------------------
//  Commit is called on an operation when it is no longer
// possible for the user to undo this operation.  The
// destructor will soon be called.
//--------------------------------------------------------------------
void chnlNoteDeleteOperation::Commit()
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
void chnlNoteDeleteOperation::Destroy()
{
	// nothing needed
}
