/****************************************************************************\
**  snExceptionX.cpp
**
**      snExceptionX.hpp defines the exceptions that can be thrown from the
**	sn package.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "AudioDS/sn/snExceptionX.hpp"


//------------------------------------------------------------------------
//	snSoundSystemCreateFailedX is thrown when the sound system creation
//	fails.
//------------------------------------------------------------------------

//========================================================================
//========================================================================
snSoundSystemCreateFailedX::snSoundSystemCreateFailedX()
{
}

//====================================================================
// Returns error message string
//====================================================================
std::string snSoundSystemCreateFailedX::GetErrorMessage() const
{
	return "No sound card installed!";
}

//------------------------------------------------------------------------
//	snSoundSystemBufferCreateFailedX is thrown when the sound system creation
//	fails.
//------------------------------------------------------------------------

//========================================================================
//========================================================================
snSoundSystemBufferCreateFailedX::snSoundSystemBufferCreateFailedX()
{
}

//====================================================================
// Returns error message string
//====================================================================
std::string snSoundSystemBufferCreateFailedX::GetErrorMessage() const
{
	return "Could not create sound buffer";
}

//------------------------------------------------------------------------
//	snUnsupportedSoundFileTypeX is thrown when a function encounters a Audio
//	type that is not supported.
//------------------------------------------------------------------------

//========================================================================
//========================================================================
snUnsupportedSoundFileTypeX::snUnsupportedSoundFileTypeX( const fsLocator& i_Locator )
: fsFileExceptionX(i_Locator)
{
}

//====================================================================
// Returns error message string
//====================================================================
std::string snUnsupportedSoundFileTypeX::GetErrorMessage() const
{
	return "Unsupported sound file type: " + GetLocatorString();
}

//------------------------------------------------------------------------
//	snSoundCreateFailedX is thrown when a sound buffer was unable to 
//	be created.
//------------------------------------------------------------------------

//========================================================================
//========================================================================
snSoundCreateFailedX::snSoundCreateFailedX( const fsLocator& i_Locator )
: fsFileExceptionX(i_Locator)
{
}

//====================================================================
// Returns error message string
//====================================================================
std::string snSoundCreateFailedX::GetErrorMessage() const
{
	return "Not enough sound memory to load: " + GetLocatorString();
}

//------------------------------------------------------------------------
//	snCorruptedFileX is thrown when invalid data was read in from a 
//	valid sound file.
//------------------------------------------------------------------------

//========================================================================
//========================================================================
snCorruptedFileX::snCorruptedFileX( const fsLocator& i_Locator )
: fsFileExceptionX(i_Locator)
{
}

//====================================================================
// Returns error message string
//====================================================================
std::string snCorruptedFileX::GetErrorMessage() const
{
	return "Corrupted sound file: " + GetLocatorString();
}

//------------------------------------------------------------------------
//	snUnsupportedPlaylistTypeX is thrown when a function encounters a
//	playlist type that is not supported.
//------------------------------------------------------------------------

//========================================================================
//========================================================================
snUnsupportedPlaylistTypeX::snUnsupportedPlaylistTypeX( const fsLocator& i_Locator )
: fsFileExceptionX(i_Locator)
{
}

//====================================================================
// Returns error message string
//====================================================================
std::string snUnsupportedPlaylistTypeX::GetErrorMessage() const
{
	return "Unsupported playlist type: " + GetLocatorString();
}

