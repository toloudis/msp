/*****************************************************************************
**	vtxZLibCompress.hpp
**
**		vtxZLibCompress provides functions for writing  and reading 
**	block of memory to a file in a compressed form.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef VTX_ZLIBCOMPRESS_HPP
#error vtxZLibCompress.hpp multiply included
#endif
#define VTX_ZLIBCOMPRESS_HPP

#ifndef CH_DEFS_HPP
#include "Core/Ch/chDefs.hpp"
#endif 


//============================================================================
//============================================================================
class chReader;
class chWriter;


//============================================================================
//============================================================================
namespace vtxZLibCompress
{
	//--------------------------------------------------------------------
	// Write encoded deltas as a large buffer of memory to
	// a file using zlib compression.
	//--------------------------------------------------------------------
	void WriteCompressed(chWriter &o_Writer, const void* i_Data, int i_NumBytes);

	//--------------------------------------------------------------------
	// Read encoded deltas as a large buffer of memory from
	// a file using zlib compression.  Returns amount of data 
	// put into the output buffer
	//--------------------------------------------------------------------
	int ReadCompressed( chReader &i_Reader, 
						chDefs::Size i_Size,
						void* o_OutputData, 
						int i_OutputBufferSize );
};

