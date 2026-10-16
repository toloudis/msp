/****************************************************************************\
**  fsAviFilePACWin.hpp
**
**      fsAviFilePACWin.hpp defines the avi file PAC for windows, which uses
**	Video for Windows.
**
**	StudioGPU
**	Copyright(C) 2002 - All Rights Reserved
\****************************************************************************/

#ifdef FS_AVIFILEPACWIN_HPP
#error fsAviFilePACWin.hpp multiply included
#endif
#define FS_AVIFILEPACWIN_HPP

#ifndef FS_AVIFILE_HPP
#include "Core/fs/fsAviFile.hpp"
#endif

#include <string>


//============================================================================
//============================================================================
namespace fsAviFilePAC
{
	typedef fsAviFile::Handle Handle;

	Handle Create(const fsLocator& i_Filename, int i_FramePeriod);
	Handle Open(const fsLocator& i_Filename);
	bool Close(Handle i_Avi);
	bool SetVideoCompression(Handle i_Avi, const char* i_FourCC,
							 bool i_ShowDialog, void* i_ParentWindow,
							 int i_Width, int i_Height, int i_BitsPerPixel,
							 int i_ImageSize);
	bool AddFrame(Handle i_Avi, const envType::UInt8* i_Pixels, int i_ImageSize,
				  int i_Width, int i_Height, int i_BitsPerPixel);
	bool AddWav(Handle i_Avi, const fsLocator& i_WavFile);
	bool Append(Handle i_First, Handle i_Second, const fsLocator& i_SavePath);
	float GetLength(Handle i_Avi);
	void GetDimensions(Handle i_Avi, int& o_Width, int& o_Height);
	bool GetFrame(Handle i_Avi, int i_Frame, envType::UInt8*& o_Data, int& o_Size);
	std::string GetLastErrorMessage();
}
