/*****************************************************************************
**	chnlNotesOperations.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Features/Channels/Notes/chnlNotesOperations.hpp"

#include "Features/Channels/chnlDialogUtil.hpp"
#include "Features/Channels/Notes/chnlNotesMgr.hpp"
#include "Features/Channels/Data/chnlTimeDocumentChunk.hpp"

#include "Support/tmln/tmlnTimeLine.hpp"

#include "Tool/gui/guiPropertyDialog.hpp"
#include "Core/prty/prtyComboBoxUIInfo.hpp"
#include "Core/prty/prtyEnum.hpp"
#include "Core/prty/prtyTextBoxUIInfo.hpp"

namespace chnlNotesOperations
{
	//--------------------------------------------------------------------
	// Create a new note for the the channels timeline
	//--------------------------------------------------------------------
	void AddNote()
	{
		//TODO: Needs undo operation
		chnlNotesMgr::AddNote(tmlnTimeLine::GetValue());
		chnlTimeDocumentChunk::ActiveDataChanged();
		chnlDialogUtil::UpdateMarkersAndNotes();
	}
	void AddNote(float i_Time, 
				   int &i_Status, 
				   const std::string &i_Note)
	{
		//TODO: Needs undo operation
		chnlNotesMgr::AddNote(i_Time, i_Status, i_Note);
		chnlTimeDocumentChunk::ActiveDataChanged();
		chnlDialogUtil::UpdateMarkersAndNotes();
	}

	//--------------------------------------------------------------------
	// Delete a note by time
	//--------------------------------------------------------------------
	void DeleteNote()
	{
		chnlNotesMgr::DeleteNote(tmlnTimeLine::GetValue());
		chnlTimeDocumentChunk::ActiveDataChanged();
		chnlDialogUtil::UpdateMarkersAndNotes();
	}
	void DeleteNote(float i_Time)
	{
		//TODO: Needs undo operation
		chnlNotesMgr::DeleteNote(i_Time);
		chnlTimeDocumentChunk::ActiveDataChanged();
		chnlDialogUtil::UpdateMarkersAndNotes();
	}

	//--------------------------------------------------------------------
	// Move current time to next or previous note
	//--------------------------------------------------------------------
	void MoveToNextNote()
	{
		float mtime = chnlNotesMgr::GetNextNoteTime(tmlnTimeLine::GetValue());
		if (mtime < 0)
			tmlnTimeLine::SetValue(tmlnTimeLine::GetMaximum());
		else
			tmlnTimeLine::SetValue(mtime);
	}
	void MoveToPrevNote()
	{
		float mtime = chnlNotesMgr::GetPrevNoteTime(tmlnTimeLine::GetValue());
		if (mtime < 0)
			tmlnTimeLine::SetValue(tmlnTimeLine::GetMinimum());
		else
			tmlnTimeLine::SetValue(mtime);
	}


	//--------------------------------------------------------------------
	// Set data for given note by index
	//--------------------------------------------------------------------
	void SetNoteData(int i_Index, const chnlNoteDataItem &i_Data)
	{
		//TODO: Needs undo operation
		chnlNotesMgr::SetNoteData(i_Index, i_Data);
		chnlTimeDocumentChunk::ActiveDataChanged();
		chnlDialogUtil::UpdateMarkersAndNotes();
	}


	//--------------------------------------------------------------------
	// Show Dialog to edit properties of note at given time
	//--------------------------------------------------------------------
	void ShowProperties(float i_Time)
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
			if (guiPropertyDialog::ShowModal("Note Properties", noteInfo.GetList())
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

