/*****************************************************************************
**	chnlNotesMgr.cpp
**
**	Handles time-associated notes data
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Features/Channels/Notes/chnlNotesMgr.hpp"

#include "Features/Channels/chnlSnapUtil.hpp"
#include "Features/Channels/Data/chnlTimeData.hpp"

#include "Core/Dbg/dbgAssert.hpp"

#include <algorithm>
#include <math.h>

//============================================================================
//============================================================================
namespace chnlNotesMgr
{
	namespace
	{
		std::vector<chnlNoteDataItem>	l_Notes;

		//	
		struct time_match
		{
			time_match(float i_Time) : m_Time(i_Time) {}
			bool operator()(const chnlMarkerDataItem &i_Item) 
				{  return i_Item.m_Time == m_Time; }
			bool operator()(const chnlNoteDataItem &i_Item) 
				{  return i_Item.m_Time == m_Time; }
			float m_Time;
		};

	}	// end of namespace

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void AddNote(float i_Time, int i_Status, const std::string& i_Note)
	{
		chnlNoteDataItem item;
		item.m_Time = i_Time;
		item.m_Status = i_Status;
		item.m_Note = i_Note;
		l_Notes.push_back( item );
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void DeleteNote(float i_Time)
	{
		std::vector<chnlNoteDataItem> &notes = l_Notes;
		std::vector<chnlNoteDataItem>::iterator it =
			std::find_if(notes.begin(), notes.end(), time_match(i_Time));
		if (it != notes.end())
		{
			notes.erase( it );
		}	
	}

	//------------------------------------------------------------------------
	// Return number of notes
	//------------------------------------------------------------------------
	int GetNumNotes()
	{
		return l_Notes.size();
	}

	//------------------------------------------------------------------------
	// Accessors to individual data
	//------------------------------------------------------------------------
	const chnlNoteDataItem& GetNoteData(int i_Index)
	{
		DBG_ASSERT0( (i_Index >= 0) && (i_Index < l_Notes.size()), "Index out of range to GetNoteData call" );

		return l_Notes[i_Index];
	}
	void SetNoteData(int i_Index, const chnlNoteDataItem &i_Data)
	{
		l_Notes[i_Index] = i_Data;
	}

	//------------------------------------------------------------------------
	// Get and set note data as a whole
	//------------------------------------------------------------------------
	void GetData(std::vector<chnlNoteDataItem>&	o_Notes)
	{
		o_Notes = l_Notes;
	}
	void SetData(const std::vector<chnlNoteDataItem>& i_Notes)
	{
		l_Notes = i_Notes;
	}
	
	//------------------------------------------------------------------------
	// SnapTimeToNotes - see if given time is within snap threshold
	//	of the a note. If so, return true and the time for the note.
	//------------------------------------------------------------------------
	bool SnapTimeToNotes(float i_Time, int& o_NoteIndex)
	{
		bool bFound = false;
		float mtime, closest;
		for (int i = 0; i < l_Notes.size(); ++i)
		{
			mtime = l_Notes[i].m_Time.GetValue();
			if (chnlSnapUtil::TimesMatch(i_Time, mtime))
			{
				// Lost for closest match
				float diff = ::fabsf(mtime - i_Time);
				if (!bFound || (diff < closest))
				{
					closest = diff;
					o_NoteIndex = i;
					bFound = true;
				}
			}
		}
		return bFound;
	}
	bool SnapTimeToNotes(float i_Time, float& o_NoteTime)
	{
		int note_index = 0;
		if (SnapTimeToNotes(i_Time, note_index))
		{
			o_NoteTime = l_Notes[note_index].m_Time.GetValue();
			return true;
		}
		return false;
	}

	//------------------------------------------------------------------------
	// Get nearest note to given time in either direction.
	// Returns -1 if not found.
	//------------------------------------------------------------------------
	float GetNextNoteTime( float i_Time )
	{
		if ( l_Notes.empty() ) return -1;

		float best_time = i_Time;
		bool bFound = false;
		int i;
		for (i = 0; i < l_Notes.size(); ++i)
		{
			if (i_Time < l_Notes[i].m_Time.GetValue())
			{
				//	if the time is the lesser time then store it.
				if ( !bFound || best_time > l_Notes[i].m_Time.GetValue() )
				{
					bFound = true;
					best_time = l_Notes[i].m_Time.GetValue();
				}
			}
		}
		if (bFound)
			return best_time;
		else
			return -1;
	}
	float GetPrevNoteTime( float i_Time )
	{
		if ( l_Notes.empty() ) return -1;

		float best_time = i_Time;
		bool bFound = false;
		int i;
		for (i = 0; i < l_Notes.size(); ++i)
		{
			if (i_Time > l_Notes[i].m_Time.GetValue())
			{
				//	if the time is the greater time then store it.
				if ( !bFound || best_time < l_Notes[i].m_Time.GetValue() )
				{
					bFound = true;
					best_time = l_Notes[i].m_Time.GetValue();
				}
			}
		}
		if (bFound)
			return best_time;
		else
			return -1;
	}
}	// end of namespace
