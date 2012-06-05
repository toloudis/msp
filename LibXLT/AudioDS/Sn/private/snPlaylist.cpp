/****************************************************************************\
**	snPlaylist.cpp
**
**	Generic song playlist.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "AudioDS/sn/snPlaylist.hpp"

#include "Core/fs/fsLocator.hpp"

#include <algorithm>
#include <functional>

// return an integral random number in the range 0 - (n - 1)
int Rand(int n)
{
    return rand() % n;
}

//====================================================================
// Constructor
//====================================================================
snPlaylist::snPlaylist()
{
	m_CurrentSong = 0;
	m_Volume = 1;
	m_bShuffled = false;
}

//============================================================================
// Destructor
//============================================================================
snPlaylist::~snPlaylist()
{
	Clear();
}

//============================================================================
// Add a song to a playlist
//============================================================================
void snPlaylist::Add(fsLocator& i_Song)
{
	m_Playlist.push_back(i_Song);
	SetShuffle(m_bShuffled);
}

//============================================================================
// Clear the playlist
//============================================================================
void snPlaylist::Clear()
{
	m_CurrentSong = 0;
	m_Playlist.clear();
	m_ShuffledPlaylist.clear();
}

//====================================================================
//	Get whether the playlist is shuffled
//====================================================================
bool snPlaylist::GetShuffle()
{
	return m_bShuffled;
}

//====================================================================
//	Set whether the playlist is shuffled
//====================================================================
void snPlaylist::SetShuffle( bool i_bShuffle )
{
	m_bShuffled = i_bShuffle;

	if (m_bShuffled)
	{
		m_ShuffledPlaylist.clear();

		int i, Size = m_Playlist.size();
		m_ShuffledPlaylist.resize(Size);
		for (i = 0; i < Size; ++i)
		{
			m_ShuffledPlaylist[i] = i;
		}

		//mix it up a few times
		std::random_shuffle(m_ShuffledPlaylist.begin(),
							m_ShuffledPlaylist.end(),
							std::pointer_to_unary_function<int, int>(Rand));
		std::random_shuffle(m_ShuffledPlaylist.begin(),
							m_ShuffledPlaylist.end());
		std::random_shuffle(m_ShuffledPlaylist.begin(),
							m_ShuffledPlaylist.end(),
							std::pointer_to_unary_function<int, int>(Rand));
	}
	else
	{
		m_ShuffledPlaylist.clear();
	}
}

//====================================================================
//	Get the playlist current volume
//====================================================================
float snPlaylist::GetVolume()
{
	return m_Volume;
}

//====================================================================
//	Set the playlist current volume
//====================================================================
void snPlaylist::SetVolume( float i_Volume )
{
	m_Volume = i_Volume;
}

//============================================================================
// NumSongs()
// return the number of songs in the list.
//============================================================================
int snPlaylist::NumSongs() const
{
	return m_Playlist.size();
}

//============================================================================
// GetSong()
// Get a specific song by #
//============================================================================
const fsLocator& snPlaylist::GetSong(int i_SongNum) const
{
	DBG_ASSERT( ((i_SongNum >= 0) && (i_SongNum < m_Playlist.size())), "Invalid song number - " << i_SongNum );
	if (m_bShuffled)
	{
		return m_Playlist[ m_ShuffledPlaylist[i_SongNum] ];
	}
	else
	{
		return m_Playlist[ i_SongNum ];
	}
}

//====================================================================
// Get the current song by locator
//====================================================================
const fsLocator& snPlaylist::GetCurrentSong() const
{
	DBG_ASSERT( ((m_CurrentSong >= 0) && (m_CurrentSong < m_Playlist.size())), "Invalid song number - " << m_CurrentSong );
	if (m_bShuffled)
	{
		return m_Playlist[ m_ShuffledPlaylist[m_CurrentSong] ];
	}
	else
	{
		return m_Playlist[ m_CurrentSong ];
	}
}

//====================================================================
//	Set the playlist current song
//====================================================================
void snPlaylist::SetSong( int i_SongNum )
{
	m_CurrentSong	= i_SongNum;
}


//============================================================================
//	Go the next song
//============================================================================
bool snPlaylist::NextSong()
{
	if ( m_CurrentSong < (NumSongs() - 1) )
	{
		m_CurrentSong++;
		return true;
	}

	return false;
}

//============================================================================
//	Go the previous song
//============================================================================
bool snPlaylist::PrevSong()
{
	if ( m_CurrentSong > 0 )
	{
		m_CurrentSong--;
		return true;
	}

	return false;
}

