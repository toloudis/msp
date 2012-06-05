/*****************************************************************************
**	chnlNotesOperations.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Features/Channels/Notes/chnlNotesOperations.hpp"

#include "Features/Channels/chnlDialogUtil.hpp"
#include "Features/Channels/Notes/chnlNotesMgr.hpp"
#include "Features/Channels/Data/chnlTimeDocumentChunk.hpp"
#include "Features/Channels/Undo/chnlAddOperation.hpp"
#include "Features/Channels/Undo/chnlDeleteOperation.hpp"
#include "Features/Channels/Undo/chnlDataOperation.hpp"

#include "Support/tmln/tmlnTimeLine.hpp"

#include "Tool/gui/guiPropertyDialog.hpp"
#include "Core/prty/prtyComboBoxUIInfo.hpp"
#include "Core/prty/prtyEnum.hpp"
#include "Core/prty/prtyTextBoxUIInfo.hpp"
#include "Core/undo/undoUndoMgr.hpp"

namespace chnlNotesOperations
{
	namespace
	{
		const char *c_AddOperationDisplayName = "Add Note";
		const char *c_DeleteOperationDisplayName = "Delete Note";
		const char *c_DataOperationDisplayName = "Set Note Data";
	}

	//--------------------------------------------------------------------
	// Create a new note for the the channels timeline
	//--------------------------------------------------------------------
	void AddNote()
	{
		if (chnlNotesMgr::AddNote(tmlnTimeLine::GetValue()))
		{
			chnlTimeDocumentChunk::ActiveDataChanged();
			chnlDialogUtil::UpdateMarkersAndNotes();
			undoUndoMgr::AddOperation(new chnlNoteAddOperation(tmlnTimeLine::GetValue(), 
				0 /*Open*/, 
				"" /*No Note*/, c_AddOperationDisplayName));
		}
	}
	void AddNote(const maTime& i_Time, 
				   int &i_Status, 
				   const std::string &i_Note)
	{
		if (chnlNotesMgr::AddNote(i_Time, i_Status, i_Note))
		{
			chnlTimeDocumentChunk::ActiveDataChanged();
			chnlDialogUtil::UpdateMarkersAndNotes();
			undoUndoMgr::AddOperation(new chnlNoteAddOperation(i_Time, i_Status, i_Note, c_AddOperationDisplayName));
		}
	}

	//--------------------------------------------------------------------
	// Delete a note by time
	//--------------------------------------------------------------------
	void DeleteNote()
	{
		//undo needs to be set before the data is removed from the manager
		int note_index = chnlNotesMgr::GetNoteIndexAtTime(tmlnTimeLine::GetValue());
		if (note_index != -1)
		{
			chnlNoteDataItem note_data = chnlNotesMgr::GetNoteData(note_index);
		
			undoUndoMgr::AddOperation(new chnlNoteDeleteOperation(note_data.m_Time.GetValue(), 
																note_data.m_Status.GetValue(), 
																note_data.m_Note.GetValue(), c_DeleteOperationDisplayName));
		}
		chnlNotesMgr::DeleteNote(tmlnTimeLine::GetValue());
		chnlTimeDocumentChunk::ActiveDataChanged();
		chnlDialogUtil::UpdateMarkersAndNotes();
	}
	void DeleteNote(const maTime& i_Time)
	{
		//undo needs to be set before the data is removed from the manager
		int note_index = chnlNotesMgr::GetNoteIndexAtTime(i_Time);
		if (note_index != -1)
		{
			chnlNoteDataItem note_data = chnlNotesMgr::GetNoteData(note_index);
		
			undoUndoMgr::AddOperation(new chnlNoteDeleteOperation(note_data.m_Time.GetValue(), 
																note_data.m_Status.GetValue(), 
																note_data.m_Note.GetValue(), c_DeleteOperationDisplayName));
		}

		chnlNotesMgr::DeleteNote(i_Time);
		chnlTimeDocumentChunk::ActiveDataChanged();
		chnlDialogUtil::UpdateMarkersAndNotes();
	}

	void DeleteNoteAtFrame(int i_Frame)
	{
		std::vector<chnlNoteDataItem> targetNotes;
		chnlNotesMgr::GetNotesAtFrame(i_Frame, targetNotes);

		for (int i = 0; i < targetNotes.size(); i++)
		{
			DeleteNote(targetNotes[i].m_Time.GetValue());
		}
	}

	//--------------------------------------------------------------------
	// Move current time to next or previous note
	//--------------------------------------------------------------------
	void MoveToNextNote()
	{
		maTime mtime = chnlNotesMgr::GetNextNoteTime(tmlnTimeLine::GetValue());
		if (mtime < maTime::c_ZeroTime)
			tmlnTimeLine::SetValue(tmlnTimeLine::GetMaximum());
		else
			tmlnTimeLine::SetValue(mtime);
		chnlDialogUtil::UpdateChannels();
	}
	void MoveToPrevNote()
	{
		maTime mtime = chnlNotesMgr::GetPrevNoteTime(tmlnTimeLine::GetValue());
		if (mtime < maTime::c_ZeroTime)
			tmlnTimeLine::SetValue(tmlnTimeLine::GetMinimum());
		else
			tmlnTimeLine::SetValue(mtime);
		chnlDialogUtil::UpdateChannels();
	}


	//--------------------------------------------------------------------
	// Set data for given note by index
	//--------------------------------------------------------------------
	void SetNoteData(int i_Index, const chnlNoteDataItem &i_Data)
	{
		chnlNoteDataItem old_data = chnlNotesMgr::GetNoteData(i_Index);
		chnlNotesMgr::SetNoteData(i_Index, i_Data);
		chnlTimeDocumentChunk::ActiveDataChanged();
		chnlDialogUtil::UpdateMarkersAndNotes();
		undoUndoMgr::AddOperation(new chnlNoteDataOperation(i_Index, old_data, i_Data, c_DataOperationDisplayName));
	}


	//--------------------------------------------------------------------
	// Show Dialog to edit properties of note at given time
	//--------------------------------------------------------------------
	void ShowProperties(const maTime& i_Time)
	{
		int note_index = 0;
		if (chnlNotesMgr::SnapTimeToNotes(i_Time, note_index))
		{
			// Get a local copy of the note data, only set the data if the
			// property dialog returns OK
			chnlNoteDataItem note_data = chnlNotesMgr::GetNoteData(note_index);
			prtyPropertyUIInfoContainer noteInfo;

			// Create enum property for purposes of our interface.
			prtyEnum note_status;
			note_status.SetValue( note_data.m_Status.GetValue() );
			note_status.SetEnumTag(e_NoteStatus_Open, "Status Open");
			note_status.SetEnumTag(e_NoteStatus_Pending, "Status Pending");
			note_status.SetEnumTag(e_NoteStatus_Closed, "Status Closed");
			noteInfo.Add(new prtyComboBoxUIInfo(&note_status));

			prtyTextBoxUIInfo *pTBUII = new prtyTextBoxUIInfo(&note_data.m_Note);
			pTBUII->SetMultiline(true);
			noteInfo.Add(pTBUII);
			if (guiPropertyDialog::ShowModal("Note Properties", noteInfo)
									== guiPropertyDialog::e_OK)
			{
				// Set value of enum property into our local data
				note_data.m_Status.SetValue( note_status.GetValue() );

				// Set the new data for the note
				chnlNotesMgr::SetNoteData(note_index, note_data);

				// Update the user interface
				chnlDialogUtil::UpdateMarkersAndNotes();
			}
		}

	}
}	// end of namespace

