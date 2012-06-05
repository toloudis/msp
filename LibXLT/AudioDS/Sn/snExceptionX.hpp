/****************************************************************************\
**  snExceptionX.hpp
**
**      snExceptionX.hpp defines the exceptions that can be thrown from the
**	sn package.  (The sn package's package error index is 8).
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef sn_EXCEPTIONX_HPP
#error snExceptionX.hpp multiply included
#endif
#define sn_EXCEPTIONX_HPP

#ifndef FS_FILEX_HPP
#include "Core/Fs/fsFileX.hpp"
#endif 


//============================================================================
//	snSoundSystemCreateFailedX is thrown when the sound system creation
//	fails.
//============================================================================
class snSoundSystemCreateFailedX : public envExceptionX
{
	public:
		snSoundSystemCreateFailedX();

 		//====================================================================
		// Returns error message string
		//====================================================================
		virtual std::string GetErrorMessage() const;
};


//============================================================================
//	snSoundSystemBufferCreateFailedX is thrown when the sound system creation
//	fails.
//============================================================================
class snSoundSystemBufferCreateFailedX : public envExceptionX
{
	public:
		snSoundSystemBufferCreateFailedX();

 		//====================================================================
		// Returns error message string
		//====================================================================
		virtual std::string GetErrorMessage() const;
};


//============================================================================
//	snUnsupportedSoundFileTypeX is thrown when a function encounters a Audio
//	type that is not supported.
//============================================================================
class snUnsupportedSoundFileTypeX : public fsFileExceptionX
{
	public:
		snUnsupportedSoundFileTypeX( const fsLocator& i_Locator );

 		//====================================================================
		// Returns error message string
		//====================================================================
		virtual std::string GetErrorMessage() const;


};


//============================================================================
//	snSoundCreateFailedX is thrown when a sound was unable to be created.
//============================================================================
class snSoundCreateFailedX : public fsFileExceptionX
{
	public:
		snSoundCreateFailedX( const fsLocator& i_Locator );

 		//====================================================================
		// Returns error message string
		//====================================================================
		virtual std::string GetErrorMessage() const;

};


//============================================================================
//	snCorruptedFileX is thrown when invalid data was read in from a 
//	valid sound file.
//============================================================================
class snCorruptedFileX : public fsFileExceptionX
{
	public:
		snCorruptedFileX( const fsLocator& i_Locator );

 		//====================================================================
		// Returns error message string
		//====================================================================
		virtual std::string GetErrorMessage() const;

};


//============================================================================
//	snUnsupportedPlaylistTypeX is thrown when a function encounters a
//	playlist type that is not supported.
//============================================================================
class snUnsupportedPlaylistTypeX : public fsFileExceptionX
{
	public:
		snUnsupportedPlaylistTypeX( const fsLocator& i_Locator );

 		//====================================================================
		// Returns error message string
		//====================================================================
		virtual std::string GetErrorMessage() const;

};


