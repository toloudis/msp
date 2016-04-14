/*****************************************************************************
**	vtxZLibCompress.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Graphics/vtx/private/vtxZLibCompress.hpp"

#include "Core/ch/chReader.hpp"
#include "Core/ch/chWriter.hpp"
#include "Graphics/vtx/vtxExceptionX.hpp"

// Problems with zlib in VS2005 Debug configurations...
//#define NO_ZLIB

#ifndef NO_ZLIB
// Zip compression of delta buffers
#include <png/zlib-1.2.5/zlib.h>

// Need the zlib library for this compression algorithm
#pragma comment(lib,"zdll.lib")
#endif


//============================================================================
//============================================================================
namespace
{
	const int c_ChunkSize = 16384; //16KB output buffer
}


//--------------------------------------------------------------------
// Write encoded deltas as a large buffer of memory to
// a file using zlib compression.
//--------------------------------------------------------------------
void vtxZLibCompress::WriteCompressed(chWriter &o_Writer, const void* i_Data, int i_NumBytes)
{
#ifndef NO_ZLIB
	// Use Zlib to compress the deltas buffer
    z_stream strm;
    unsigned char chunk[c_ChunkSize];
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    int ret = deflateInit(&strm, Z_DEFAULT_COMPRESSION);
    if (ret != Z_OK)
	{
		DBG_ERROR("Error initializing compression code.");
        throw vtxCompressErrorX(o_Writer.GetLocator());
	}

	// Setup the input buffer 
    strm.avail_in = i_NumBytes;
    strm.next_in = (Bytef*)(i_Data);

    // run deflate() on input until output buffer not full, finish
    //   compression if all of source has been read in 
    do {
		// Setup the output buffer
        strm.avail_out = c_ChunkSize;
        strm.next_out = chunk;
        ret = deflate(&strm, Z_FINISH);    // no bad return value 

		// state not clobbered 
		if (ret == Z_STREAM_ERROR)
		{
			(void)deflateEnd(&strm);
			DBG_ERROR("Stream error in compression code.");
			throw vtxCompressErrorX(o_Writer.GetLocator());
		}

        int have = c_ChunkSize - strm.avail_out;
		o_Writer.Write(chunk, have);
    } while (strm.avail_out == 0);

    (void)deflateEnd(&strm);
#else
	DBG_ERROR("ZLib diabled, cannot write compressed stream.");
	throw vtxCompressErrorX(o_Writer.GetLocator());
#endif
}

//--------------------------------------------------------------------
// Read encoded deltas as a large buffer of memory from
// a file using zlib compression. Returns amount of data 
// put into the output buffer
//--------------------------------------------------------------------
int vtxZLibCompress::ReadCompressed( chReader &i_Reader, 
								 chDefs::Size i_Size,
								 void* o_OutputData, 
								 int i_OutputBufferSize )
{
#ifndef NO_ZLIB
	// Read whole chunk at once from file, could do this in smaller chunks later also
	unsigned char *pBuffer = new unsigned char[i_Size];
	i_Reader.Read(pBuffer, i_Size);

	// Use Zlib to inflate the deltas buffer
    z_stream strm;
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;
    int ret = inflateInit(&strm);
    if (ret != Z_OK)
	{
		delete [] pBuffer;
		DBG_ERROR("Error initializing decompression code.");
        throw vtxDecompressErrorX(i_Reader.GetLocator());
	}

	// Setup the input buffer 
    strm.avail_in = i_Size;
    strm.next_in = pBuffer;

    // call inflate once, if the stream didn't finish, then
	// there was an error.
    strm.avail_out = i_OutputBufferSize;
    strm.next_out = (Bytef*)o_OutputData;
    ret = inflate(&strm, Z_NO_FLUSH); // Z_FINISH?
	delete [] pBuffer;

	if (ret != Z_STREAM_END)
	{
		(void)inflateEnd(&strm);
		DBG_ERROR("Decompressed data did not fit expected size.");
		throw vtxDecompressErrorX(i_Reader.GetLocator());
	//	return 0;
	}

    // clean up and return 
    (void)inflateEnd(&strm);
	return (i_OutputBufferSize - strm.avail_out);
#else
	DBG_ERROR("ZLib diabled, cannot read compressed stream.");
	throw vtxDecompressErrorX(i_Reader.GetLocator());
#endif
}

