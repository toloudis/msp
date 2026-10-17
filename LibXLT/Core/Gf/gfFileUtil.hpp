/*****************************************************************************
**  gfFileUtil.hpp
**
**      gfFileUtil is a namespace containing several functions for reading and
**		writing simple data types such as uint16_t
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef GF_FILEUTIL_HPP
#error gfFileUtil.hpp multiply included
#endif
#define GF_FILEUTIL_HPP

#ifndef ENV_PLATFORM_HPP
#include "Core/env/envPlatform.hpp"
#endif

#ifndef GF_FILEBIN_HPP
#include "Core/Gf/gfFileBin.hpp"
#endif


//============================================================================
//============================================================================
namespace gfFileUtil
{
	//--------------------------------------------------------------------------------
	//	Read is a template function that receives a simple primtive value such as
	//	uint16_t and reads it from the file.  Files are little endian, like the
	//	platforms we build for, so no byte swapping is needed.
	//--------------------------------------------------------------------------------
	template <class T>
	inline void Read(gfFile& i_File, T& i_Val)
	{
		i_File.Read(sizeof(i_Val), &i_Val);
	}

	//--------------------------------------------------------------------------------
	//	Write is a template function that receives a simple primtive value such as
	//	uint16_t and writes it to the file (little endian, see Read).
	//--------------------------------------------------------------------------------
	template <class T>
	inline void Write(gfFile& i_File, T i_Val)
	{
		i_File.Write(sizeof(i_Val), &i_Val);
	}

}
