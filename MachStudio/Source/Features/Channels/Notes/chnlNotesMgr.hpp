/*****************************************************************************
**	chnlNotesMgr.hpp
**
**	Handles time-associated notes data
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef CHNL_NOTESMGR_HPP
#error chnlNotesMgr.hpp multiply included
#endif
#define CHNL_NOTESMGR_HPP

#ifndef CHNL_TIMEDATA_HPP
#include "Features/Channels/Data/chnlTimeData.hpp"
#endif


//============================================================================
//============================================================================
namespace chnlNotesMgr
{
	//------------------------------------------------------------------------
	// Add Note on the timeline. return false is no note is added
	//------------------------------------------------------------------------
	bool AddNote(const maTime& i_Time, 
				 int i_Status = e_NoteStatus_Open, 
				 const std::string& i_Note = "");

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void DeleteNote(const maTime& i_Time);

	//------------------------------------------------------------------------
	// Check if there is a note existed at a given time
	//------------------------------------------------------------------------
	bool IsNoteExisted(const maTime& i_Time);

	//------------------------------------------------------------------------
	// Return number of notes
	//------------------------------------------------------------------------
	int GetNumNotes();

	//------------------------------------------------------------------------
	// Accessors to data
	//------------------------------------------------------------------------
	const chnlNoteDataItem& GetNoteData(int i_Index);
	void SetNoteData(int i_Index, const chnlNoteDataItem &i_Data);

	//------------------------------------------------------------------------
	// Get and set note data as a whole
	//------------------------------------------------------------------------
	void GetData(std::vector<chnlNoteDataItem>&	o_Notes);
	void SetData(const std::vector<chnlNoteDataItem>& i_Notes);
	
	//------------------------------------------------------------------------
	// Get Note index at the given time
	//------------------------------------------------------------------------
	int GetNoteIndexAtTime(const maTime& i_Time);

	//------------------------------------------------------------------------
	// Get Note array at the given frame
	// There might be multiple notes found
	//------------------------------------------------------------------------
	void GetNotesAtFrame(int i_Frame, std::vector<chnlNoteDataItem>& o_Notes);

	//------------------------------------------------------------------------
	// SnapTimeToNotes - see if given time is within snap threshold
	//	of the a note. If so, return true and the time for the note.
	//------------------------------------------------------------------------
	bool SnapTimeToNotes(const maTime& i_Time, int& o_NoteIndex);
	bool SnapTimeToNotes(const maTime& i_Time, maTime& o_NoteTime);

	//------------------------------------------------------------------------
	// Get nearest note to given time in either direction.
	// Returns -1 if not found.
	//------------------------------------------------------------------------
	maTime GetNextNoteTime( const maTime& i_Time );
	maTime GetPrevNoteTime( const maTime& i_Time );

}	// end of namespace
