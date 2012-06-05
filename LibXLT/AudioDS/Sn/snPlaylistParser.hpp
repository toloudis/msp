/*****************************************************************************
**  snPlaylistParser.cpp
**
**      snPlaylistParser provides a way to read in playlists and return a
**	list of locators.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef SN_PLAYLISTPARSER_HPP
#error snPlaylistParser.hpp multiply included
#endif
#define SN_PLAYLISTPARSER_HPP

#include <vector>


//----------------------------------------------------------------------------
//	Playlist types
//----------------------------------------------------------------------------
namespace
{
	enum PlaylistFormatType
	{
		e_PLAYLISTTYPE_UNKNOWN	= 0x0,
		e_PLAYLISTTYPE_M3U		= 0x1,
	};
}


//----------------------------------------------------------------------------
//	Forward references
//----------------------------------------------------------------------------
class fsLocator;
class snPlaylist;


//----------------------------------------------------------------------------
//	snPlaylistParser
//----------------------------------------------------------------------------
namespace snPlaylistParser
{
//----------------------------------------------------------------------------
//	Reader
//----------------------------------------------------------------------------

	//============================================================================
	//	ReadList()
	//
	//	Read the Playlist and return a list of locators.  
	//
	//	exceptions thrown:
	//		fsFileDoesntExistX			- if playlist file doesn't exist
	//		snUnsupportedPlaylistTypeX	- if playlist isn't in a supported format
	//============================================================================
	void ReadList( const fsLocator& i_PlaylistFilename, snPlaylist& o_Songs, bool i_bClearList=true );


//----------------------------------------------------------------------------
//	Writer
//----------------------------------------------------------------------------

	//============================================================================
	//	WriteList()
	//
	//	Write the Playlist in specified format.
	//
	//	exceptions thrown:
	//============================================================================
	//void WriteList(  const fsLocator& i_PlaylistFilename, std::vector<fsLocator>& i_SoundFiles, const PlaylistFormatType Format );
}
