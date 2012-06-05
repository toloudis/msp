/****************************************************************************\
**  cptrWriteUtil.hpp
**
**      Supplies routines for writing the BMP format
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#ifdef CPTR_WRITEUTIL_HPP
#error cptrWriteUtil.hpp multiply included
#endif
#define CPTR_WRITEUTIL_HPP

#ifndef ENV_TYPE_HPP
#include "Core/Env/envType.hpp"
#endif 

#include <windows.h> // for BITMAPINFOHEADER structs

//============================================================================
//	forward references
//============================================================================
class fsLocator;
class fsFileStream;
class g2dImage;
class chBinWriter;
class g2dWindow;

//============================================================================
//	FileStream - encapsulates fsFileStream or chBinWriter.
//
//  If the utility functions were switched over to write into a
//  large buffer instead of writing line by line to the file,
//  This class wouldn't be necessary.
//============================================================================
class FileStream
{
public:
	FileStream(chBinWriter &i_Writer);

	FileStream(fsFileStream &i_Stream);

	void Write(envType::Int64 i_NumBytes, const void* i_Data);

private:
	FileStream();

	chBinWriter*	m_pBinWriter;
	fsFileStream*	m_pFileStream;
};


// TODO [dmt] try to refactor so that the code in cptrWriteTGA can be merged
//  with this code. Also think about how to get the d3d specific stuff
//  shoved back down to the library level?

// shared utility functions
namespace cptrWriteUtil
{
	void fill_bmp_header(BITMAPINFOHEADER &i_Header, int i_Width, int i_Height,
					  int i_DestBitsPerPixel, int i_DestImageSize);

	void write_bmp_header(FileStream &i_File, int i_Width, int i_Height,
					  int i_DestBitsPerPixel, int i_PaletteSize,
					  int i_DestImageSize);

	void write_bmp_header(	FileStream &i_File,
						g2dImage* i_pSurface,
						bool	i_bGreyscale = false,
						int		i_Sampling = 1,
						RECT*	i_pRect = NULL );

	void write_bmp_data(FileStream &i_File,
					g2dImage* i_pSurface,
					bool	i_bGreyscale = false,
					int		i_Sampling = 1,
					RECT *i_pRect = NULL);

}
