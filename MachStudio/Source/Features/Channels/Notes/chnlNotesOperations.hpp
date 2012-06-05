/*****************************************************************************
**	chnlNotesOperations.hpp
**
**	Operations related to notes
**
**	StudioGPU
**	Copyright(C) 2003-7 - All Rights Reserved
\****************************************************************************/
#ifdef CHNL_NOTESOPERATIONS_HPP
#error chnlNotesOperations.hpp multiply included
#endif
#define CHNL_NOTESOPERATIONS_HPP

#include <string>

//============================================================================
//============================================================================
class chnlNoteDataItem;
class maTime;

//============================================================================
//============================================================================
namespace chnlNotesOperations
{
	//--------------------------------------------------------------------
	// Create a new note for the the channels timeline
	//--------------------------------------------------------------------
	void AddNote();
	void AddNote(	const maTime& i_Time, 
					int &i_Status, 
					const std::string &i_Note);

	//--------------------------------------------------------------------
	// Delete a note by time
	//--------------------------------------------------------------------
	void DeleteNote();
	void DeleteNote(const maTime& i_Time);
	void DeleteNoteAtFrame(int i_Frame);

	//--------------------------------------------------------------------
	// Move current time to next or previous note
	//--------------------------------------------------------------------
	void MoveToNextNote();
	void MoveToPrevNote();

	//--------------------------------------------------------------------
	// Set data for given note by index
	//--------------------------------------------------------------------
	void SetNoteData(int i_Index, const chnlNoteDataItem &i_Data);

	//--------------------------------------------------------------------
	// Show Dialog to edit properties of note at given time
	//--------------------------------------------------------------------
	void ShowProperties(const maTime& i_Time);

}	// end of namespace
