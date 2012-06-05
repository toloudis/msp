/********************************************************************************************\
**	mnmAppPackageParser.hpp
**
**		Application package chunk.  This chunk defines what application wrote (or last wrote)
**	the file.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\********************************************************************************************/
#ifdef	MNM_APPPACKAGEPARSER_HPP
#error mnmAppPackageParser.hpp multiply included
#endif
#define MNM_APPPACKAGEPARSER_HPP

#ifndef CH_DEFS_HPP
#include "Core/ch/chDefs.hpp"
#endif


//============================================================================
//============================================================================
class chReader;
class chWriter;
struct mnmAppPackageData;


//============================================================================
//============================================================================
namespace mnmAppPackageParser
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
	void Read(	chReader& io_Reader,
				chDefs::Version i_Version,
				chDefs::Size i_Size,
				mnmAppPackageData& o_Data );

	//--------------------------------------------------------------------
	//	Write is called to parse data from the given object to the given
	//	chWriter.  i_Driver is the object being written.
	//--------------------------------------------------------------------
	void Write( chWriter& io_Writer,
				const mnmAppPackageData& i_Data );
};

