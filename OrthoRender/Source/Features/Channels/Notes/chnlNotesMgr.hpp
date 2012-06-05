/*****************************************************************************
**	chnlNotesMgr.hpp
**
**	Handles time-associated notes data
**
**	Extra Large Technology
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
	//------------------------------------------------------------------------
	void AddNote(float i_Time, 
				 int i_Status = e_NoteStatus_Open, 
				 const std::string& i_Note = "");

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void DeleteNote(float i_Time);

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
	// SnapTimeToNotes - see if given time is within snap threshold
	//	of the a note. If so, return true and the time for the note.
	//------------------------------------------------------------------------
	bool SnapTimeToNotes(float i_Time, int& o_NoteIndex);
	bool SnapTimeToNotes(float i_Time, float& o_NoteTime);

	//------------------------------------------------------------------------
	// Get nearest note to given time in either direction.
	// Returns -1 if not found.
	//------------------------------------------------------------------------
	float GetNextNoteTime( float i_Time );
	float GetPrevNoteTime( float i_Time );

}	// end of namespace
