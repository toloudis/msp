/*****************************************************************************
**	chnlNotesOperations.hpp
**
**	Operations related to notes
**
**	Extra Large Technology
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

//============================================================================
//============================================================================
namespace chnlNotesOperations
{
	//--------------------------------------------------------------------
	// Create a new note for the the channels timeline
	//--------------------------------------------------------------------
	void AddNote();
	void AddNote(	float i_Time, 
					int &i_Status, 
					const std::string &i_Note);

	//--------------------------------------------------------------------
	// Delete a note by time
	//--------------------------------------------------------------------
	void DeleteNote();
	void DeleteNote(float i_Time);

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
	void ShowProperties(float i_Time);

}	// end of namespace
