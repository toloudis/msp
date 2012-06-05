/********************************************************************************************\
**	fcdcTimelineItemParser.hpp
**
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\********************************************************************************************/
#ifdef	FCDC_TIMELINEITEMPARSER_HPP
#error fcdcTimelineItemParser.hpp multiply included
#endif
#define FCDC_TIMELINEITEMPARSER_HPP

#ifndef CH_DEFS_HPP
#include "Core/ch/chDefs.hpp"
#endif

#ifndef FCDC_TIMELINEITEMDATA_HPP
#include "FCSupport/fcdc/data/fcdcTimelineItemData.hpp"
#endif

//============================================================================
//============================================================================
class chReader;
class chWriter;


//============================================================================
//============================================================================
namespace fcdcTimelineItemParser
{
	//--------------------------------------------------------------------
	// Returns chunk name used by this parser
	//--------------------------------------------------------------------
	chDefs::Name GetChunkName();

	//--------------------------------------------------------------------
	//	Read is called by a parser utility when the chunk name has been read
	//  from the data file.  It will parse from the chunk into the given
	//  parsable object.
	//--------------------------------------------------------------------
	void ReadData(	chReader& io_Reader,
				chDefs::Version i_Version,
				chDefs::Size i_Size,
				fcdcTimelineItemData& o_Data );

	//--------------------------------------------------------------------
	//	Write is called to parse data from the given object to the given
	//	chWriter.  i_Driver is the object being written.
	//--------------------------------------------------------------------
	void WriteData( chWriter& io_Writer,
				const fcdcTimelineItemData& i_Data );
}

