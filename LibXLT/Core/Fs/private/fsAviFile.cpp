/****************************************************************************\
**  fsAviFile.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2002 - All Rights Reserved
\****************************************************************************/
#include "Core/fs/fsAviFile.hpp"

#include "Core/fs/private/fsAviFilePAC.hpp"


//============================================================================
//============================================================================
namespace fsAviFile
{

Handle Create(const fsLocator& i_Filename, int i_FramePeriod)
{
	return fsAviFilePAC::Create(i_Filename, i_FramePeriod);
}

Handle Open(const fsLocator& i_Filename)
{
	return fsAviFilePAC::Open(i_Filename);
}

bool Close(Handle i_Avi)
{
	return fsAviFilePAC::Close(i_Avi);
}

bool SetVideoCompression(Handle i_Avi, const char* i_FourCC,
						 bool i_ShowDialog, void* i_ParentWindow,
						 int i_Width, int i_Height, int i_BitsPerPixel,
						 int i_ImageSize)
{
	return fsAviFilePAC::SetVideoCompression(i_Avi, i_FourCC, i_ShowDialog, i_ParentWindow,
											 i_Width, i_Height, i_BitsPerPixel, i_ImageSize);
}

bool AddFrame(Handle i_Avi, const uint8_t* i_Pixels, int i_ImageSize,
			  int i_Width, int i_Height, int i_BitsPerPixel)
{
	return fsAviFilePAC::AddFrame(i_Avi, i_Pixels, i_ImageSize, i_Width, i_Height, i_BitsPerPixel);
}

bool AddWav(Handle i_Avi, const fsLocator& i_WavFile)
{
	return fsAviFilePAC::AddWav(i_Avi, i_WavFile);
}

bool Append(Handle i_First, Handle i_Second, const fsLocator& i_SavePath)
{
	return fsAviFilePAC::Append(i_First, i_Second, i_SavePath);
}

float GetLength(Handle i_Avi)
{
	return fsAviFilePAC::GetLength(i_Avi);
}

void GetDimensions(Handle i_Avi, int& o_Width, int& o_Height)
{
	fsAviFilePAC::GetDimensions(i_Avi, o_Width, o_Height);
}

bool GetFrame(Handle i_Avi, int i_Frame, uint8_t*& o_Data, int& o_Size)
{
	return fsAviFilePAC::GetFrame(i_Avi, i_Frame, o_Data, o_Size);
}

std::string GetLastErrorMessage()
{
	return fsAviFilePAC::GetLastErrorMessage();
}

}
