/*****************************************************************************
**  snPlaylistParser.cpp
**
**      snPlaylistParser provides a way to read in playlists and return a
**	list of locators.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/


#include "AudioDS/sn/snPlaylistParser.hpp"

#include "Core/dbg/dbgMsg.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/fs/private/fsFileUtilPAC.hpp"
#include "Core/gf/gfFileTranslationMgr.hpp"
#include "Core/gf/gfFileTxt.hpp"
#include "AudioDS/sn/snExceptionX.hpp"
#include "AudioDS/sn/snPlaylist.hpp"


//============================================================================
//============================================================================
namespace snPlaylistParser
{

//========================================================================
//	pick_format()
//
//	determine what format the playlist is in and return the type if
//	supported.
//========================================================================
PlaylistFormatType 
pick_format( const fsLocator& i_PlaylistFilename )
{
	if (   ( i_PlaylistFilename.GetLastName().HasSubString( itString( ".m3u" ) ) )
		|| ( i_PlaylistFilename.GetLastName().HasSubString( itString( ".M3U" ) ) ) )
	{
		return e_PLAYLISTTYPE_M3U;
	}
	else
	{
		return e_PLAYLISTTYPE_UNKNOWN;
	}
}


//============================================================================
//	BuildSongFilePath_M3U()
//
//	Build the absolute path for an M3U file based on a root path.
//============================================================================
void
BuildSongFilePath_M3U( const fsLocator&  i_BasePath, 
					   const std::string i_rawSongPath,
							 fsLocator&  o_SongPath )
{
	o_SongPath.Clear();

	if ( i_rawSongPath.c_str()[0] == '\\' )
	{
		// network path
		fsFileUtil::ANSIFilenameToLocator( i_rawSongPath.c_str(), o_SongPath );
	}
	else
	if ( i_rawSongPath.c_str()[1] == ':' )
	{
		// full path already
		fsFileUtil::ANSIFilenameToLocator( i_rawSongPath.c_str(), o_SongPath );
	}
	else
	{
		// relative path
		o_SongPath = i_BasePath;
		o_SongPath.Push( i_rawSongPath.c_str() );
	}
}


//============================================================================
//	ReadList_M3U()
//
//	read in the file, parse the data, and fill in the playlist.
//============================================================================
void
ReadList_M3U( const fsLocator& i_PlaylistFilename, snPlaylist& o_Songs, bool i_bClearList )
{
	gfFileTxt PlaylistFile( i_PlaylistFilename, fsFileStream::e_ReadOnly );

	//	print out the playlist name
	//
	std::string playlist_path;
	fsFileUtilPAC::LocatorToANSIFilename(i_PlaylistFilename, playlist_path);
	//DBG_LOG( "Playlist " << playlist_path.c_str() );

	//	if the user asked to clear the list, then clear it.
	//
	if ( i_bClearList )
	{
		o_Songs.Clear();
	}

	fsLocator	oneFile;
	std::string aLine;
	fsLocator	basePath( i_PlaylistFilename );

	basePath.Pop();	// remove filename and leave path

	while ( PlaylistFile.ReadLine( aLine ) )
	{
		//	check if the line is one of the tags and discard it.
		//
		//	note: we should probably check for each tag and deal with it, but
		//	since we don't care about any of them now, I will ignore them all.
		//
		if ( aLine.c_str()[0] != '#' )
		{
			//DBG_LOG( "   " << aLine.c_str() );

			fsLocator oneFile;

			BuildSongFilePath_M3U( basePath, aLine, oneFile );
			
			o_Songs.Add( oneFile );
		}
	}

	//DBG_LOG( "size=%d" << o_Songs.size() );
}


//============================================================================
//	ReadList()
//
//	Read the Playlist and return a list of locators.  
//
//	exceptions thrown:
//		fsFileDoesntExistX			- if playlist file doesn't exist
//		snUnsupportedPlaylistTypeX	- if playlist isn't in a supported format
//============================================================================
void 
ReadList( const fsLocator& i_PlaylistFilename, snPlaylist& o_Songs, bool i_bClearList )
{
	//	check if the file exists
	//
	if ( !gfFileTranslationMgr::FileExists( i_PlaylistFilename ) )
	{
		throw fsFileDoesntExistX( i_PlaylistFilename );
	}

	//this ensures no shuffling thrashing as each song is added one at a time
	//we'll set it the way it was after the list is built
	bool bWasShuffled = o_Songs.GetShuffle();
	o_Songs.SetShuffle(false);

	//	find out the type of playlist and read it in
	//
	PlaylistFormatType file_type = pick_format( i_PlaylistFilename );

	switch ( file_type )
	{
		case e_PLAYLISTTYPE_M3U:
			ReadList_M3U( i_PlaylistFilename, o_Songs, i_bClearList );
			break;

		default:
			throw snUnsupportedPlaylistTypeX( i_PlaylistFilename );
			break;
	}

	//now return the list to its former shuffle status
	o_Songs.SetShuffle(bWasShuffled);

	return;
}


}
