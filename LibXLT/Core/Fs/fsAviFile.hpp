/****************************************************************************\
**  fsAviFile.hpp
**
**      fsAviFile reads and writes AVI movie files.
**
**	Frames are passed as pixel buffers in the layout of an uncompressed,
**	bottom-up bitmap: rows of i_Width pixels of i_BitsPerPixel bits each,
**	each row padded to a multiple of 4 bytes.
**
**	StudioGPU
**	Copyright(C) 2002 - All Rights Reserved
\****************************************************************************/
#ifdef FS_AVIFILE_HPP
#error fsAviFile.hpp multiply included
#endif
#define FS_AVIFILE_HPP

#ifndef ENV_TYPE_HPP
#include "Core/env/envType.hpp"
#endif

#include <string>


//============================================================================
//============================================================================
class fsLocator;


//============================================================================
//============================================================================
namespace fsAviFile
{
	//------------------------------------------------------------------------
	//	A Handle identifies an open avi file.
	//------------------------------------------------------------------------
	struct File;
	typedef File* Handle;

	//------------------------------------------------------------------------
	//	Create starts writing a new avi file.  i_FramePeriod is the number of
	//	milliseconds between frames.  Returns NULL on failure.
	//------------------------------------------------------------------------
	Handle Create(const fsLocator& i_Filename, int i_FramePeriod);

	//------------------------------------------------------------------------
	//	Open opens an existing avi file for reading its frames.  Returns NULL
	//	on failure.
	//------------------------------------------------------------------------
	Handle Open(const fsLocator& i_Filename);

	//------------------------------------------------------------------------
	//	Close finishes and closes the file, and frees the handle.
	//------------------------------------------------------------------------
	bool Close(Handle i_Avi);

	//------------------------------------------------------------------------
	//	SetVideoCompression chooses the video codec by its four character
	//	code (i_FourCC, for example "DIB " or "MJPG").  It must be called
	//	before any frames are added.  The frame format describes the frames
	//	that will be added.  If i_ShowDialog is true, the user picks the
	//	codec in a dialog parented to i_ParentWindow (a native window
	//	handle), starting from i_FourCC.
	//------------------------------------------------------------------------
	bool SetVideoCompression(Handle i_Avi, const char* i_FourCC,
							 bool i_ShowDialog, void* i_ParentWindow,
							 int i_Width, int i_Height, int i_BitsPerPixel,
							 int i_ImageSize);

	//------------------------------------------------------------------------
	//	AddFrame adds a frame to the end of the movie.  i_ImageSize is the
	//	size of i_Pixels in bytes.
	//------------------------------------------------------------------------
	bool AddFrame(Handle i_Avi, const envType::UInt8* i_Pixels, int i_ImageSize,
				  int i_Width, int i_Height, int i_BitsPerPixel);

	//------------------------------------------------------------------------
	//	AddWav adds the audio of an entire wave file to the movie.
	//------------------------------------------------------------------------
	bool AddWav(Handle i_Avi, const fsLocator& i_WavFile);

	//------------------------------------------------------------------------
	//	Append saves the video of i_First followed by the video of i_Second
	//	to a new file.
	//------------------------------------------------------------------------
	bool Append(Handle i_First, Handle i_Second, const fsLocator& i_SavePath);

	//------------------------------------------------------------------------
	//	GetLength returns the number of frames in a file opened with Open.
	//------------------------------------------------------------------------
	float GetLength(Handle i_Avi);

	//------------------------------------------------------------------------
	//	GetDimensions returns the frame size of a file opened with Open.
	//------------------------------------------------------------------------
	void GetDimensions(Handle i_Avi, int& o_Width, int& o_Height);

	//------------------------------------------------------------------------
	//	GetFrame decodes a frame of a file opened with Open into 32-bit
	//	pixels.  o_Data belongs to the handle and stays valid until the next
	//	GetFrame or Close.
	//------------------------------------------------------------------------
	bool GetFrame(Handle i_Avi, int i_Frame, envType::UInt8*& o_Data, int& o_Size);

	//------------------------------------------------------------------------
	//	GetLastErrorMessage describes the result of the last call.
	//------------------------------------------------------------------------
	std::string GetLastErrorMessage();
}
