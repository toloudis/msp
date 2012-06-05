/********************************************************************************************\
**  cmraDriverDataPositionParser.hpp
**
**		the parser for cmra Position
**
**  Extra Large Technology
**  Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/

#ifdef CMRA_DRIVERDATAPOSITIONPARSER_HPP
#error cmraDriverDataPositionParser.hpp multiply included
#endif
#define CMRA_DRIVERDATAPOSITIONPARSER_HPP

#ifndef CH_DEFS_HPP
#include "Core/ch/chDefs.hpp"
#endif

#ifndef CMRA_DRIVERDATAPOSITIONINFO_HPP
#include "Systems/Cameras/Drivers/cmraDriverDataPositionInfo.hpp"
#endif


//============================================================================
//============================================================================
class tmlnDriverInfo;
class chReader;
class chWriter;
class fsLocator;


//============================================================================
//============================================================================
namespace cmraDriverDataPositionParser
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
	void Read(	chReader& i_Reader,
				chDefs::Version i_Version,
				chDefs::Size i_Size,
				cmraDriverDataPositionInfo& o_Driver );

	//--------------------------------------------------------------------
	//	Write is called to parse data from the given object to the given
	//	chWriter.  i_Driver is the object being written.
	//--------------------------------------------------------------------
	void Write( chWriter& i_Writer,
				const cmraDriverDataPositionInfo& i_Driver );

	//--------------------------------------------------------------------
	// Create should be overridden to create the user specific parsable object.
	// Users of parsers should call create to create the object that is then
	// passed to Read.
	//--------------------------------------------------------------------
	cmraDriverDataPositionInfo* Create();
};

