/********************************************************************************************\
**	rstkDataParser.hpp
**
**		Parse the rstk data.
**
**  StudioGPU
**  Copyright(C) 2006 - All Rights Reserved
\********************************************************************************************/
#ifdef RSTKDATAPARSER_HPP
#error rstkDataParser.hpp multiply included
#endif
#define RSTKDATAPARSER_HPP

#ifndef FS_RESOURCETRACKERDATA_HPP
#include "Core/fs/fsResourceTrackerData.hpp"
#endif
#ifndef CH_DEFS_HPP
#include "Core/ch/chDefs.hpp"
#endif


//============================================================================
//============================================================================
class chReader;
class chWriter;
class fsLocator;


//============================================================================
//============================================================================
namespace rstkDataParser
{
	//------------------------------------------------------------------------
	//  returns chunk name for this data type
	//------------------------------------------------------------------------
	chDefs::Name  GetChunkName();

	//------------------------------------------------------------------------
	//   ReadData
	//------------------------------------------------------------------------
	void ReadData(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					fsResourceTrackerData& o_Data );

	//------------------------------------------------------------------------
	//   WriteData
	//------------------------------------------------------------------------
	void WriteData(	chWriter& o_Writer,
					const fsResourceTrackerData& i_Data );

}

