/*****************************************************************************
**	chnlNotesMgr.cpp
**
**	Handles time-associated notes data
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Features/Channels/Notes/chnlNotesMgr.hpp"

#include "Features/Channels/chnlSnapUtil.hpp"
#include "Features/Channels/Data/chnlTimeData.hpp"

#include "Core/Dbg/dbgMsg.hpp"
#include "Support/tmln/tmlnTimeUtil.hpp"

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
			time_match(const maTime& i_Time) : m_Time(i_Time) {}
			bool operator()(const chnlMarkerDataItem &i_Item) 
			{  
				//if (fabs(m_Time - i_Item.m_Time.GetValue()) < 0.0005)
				//TIME - can do exact compare with time units?
				if (m_Time == i_Item.m_Time.GetValue())
					return true;
				//return i_Item.m_Time == m_Time; 
				return false;
			}
			bool operator()(const chnlNoteDataItem &i_Item) 
			{  
				//if (fabs(m_Time - i_Item.m_Time.GetValue()) < 0.0005)
				//TIME - can do exact compare with time units?
				if (m_Time == i_Item.m_Time.GetValue())
					return true;
				//return i_Item.m_Time == m_Time; 
				return false;
			}
			maTime m_Time;
		};

	}	// end of namespace

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	bool AddNote(const maTime& i_Time, int i_Status, const std::string& i_Note)
	{
		if (!IsNoteExisted(i_Time))
		{
			chnlNoteDataItem item;
			item.m_Time = i_Time;
			item.m_Status = i_Status;
			item.m_Note = i_Note;
			l_Notes.push_back( item );
		}
		else
		{
			return false;
		}

		return true;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void DeleteNote(const maTime& i_Time)
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
	// Check if there is a note existed at a given time
	//------------------------------------------------------------------------
	bool IsNoteExisted(const maTime& i_Time)
	{
		std::vector<chnlNoteDataItem> &notes = l_Notes;
		std::vector<chnlNoteDataItem>::iterator it =
			std::find_if(notes.begin(), notes.end(), time_match(i_Time));
		if (it != notes.end())
		{
			return true;
		}
		else
		{
			return false;
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
		DBG_ASSERT( (i_Index >= 0) && (i_Index < l_Notes.size()), "Index out of range to GetNoteData call" );

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
	// Get Note index at the given time
	//------------------------------------------------------------------------
	int GetNoteIndexAtTime(const maTime& i_Time)
	{
		for (int i = 0; i < l_Notes.size(); ++i)
		{
			if( l_Notes[i].m_Time.GetValue() == i_Time )
				return i;
		}
		return -1;
	}

	//------------------------------------------------------------------------
	// Get Note array at the given frame
	// There might be multiple notes found
	//------------------------------------------------------------------------
	void GetNotesAtFrame(int i_Frame, std::vector<chnlNoteDataItem>& o_Notes)
	{
		for (int i = 0; i < l_Notes.size(); i++)
		{
			int frame;
			tmlnTimeUtil::GetTimeInFrames(l_Notes[i].m_Time.GetValue(), frame);
			if (i_Frame == frame)
			{
				o_Notes.push_back(l_Notes[i]);
			}

		}
	}
	
	//------------------------------------------------------------------------
	// SnapTimeToNotes - see if given time is within snap threshold
	//	of the a note. If so, return true and the time for the note.
	//------------------------------------------------------------------------
	bool SnapTimeToNotes(const maTime& i_Time, int& o_NoteIndex)
	{
		bool bFound = false;
		maTime mtime, closest;
		for (int i = 0; i < l_Notes.size(); ++i)
		{
			mtime = l_Notes[i].m_Time.GetValue();
			if (chnlSnapUtil::TimesMatch(i_Time, mtime))
			{
				// Lost for closest match
				maTime diff = (mtime - i_Time).Abs();
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
	bool SnapTimeToNotes(const maTime& i_Time, maTime& o_NoteTime)
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
	maTime GetNextNoteTime( const maTime& i_Time )
	{
		if ( l_Notes.empty() ) return maTime::FromSeconds(-1);	//TIME - magic number

		maTime best_time = i_Time;
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
			return maTime::FromSeconds(-1);	//TIME - magic number
	}
	maTime GetPrevNoteTime( const maTime& i_Time )
	{
		if ( l_Notes.empty() ) return maTime::FromSeconds(-1);	//TIME - magic number

		maTime best_time = i_Time;
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
			return maTime::FromSeconds(-1);	//TIME - magic number
	}
}	// end of namespace
