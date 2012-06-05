/********************************************************************************************\
**  cmraDriverFramedBaseParser.hpp
**
**		Parser for the cmra FramedBase.
**
**  Extra Large Technology
**  Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/

#ifdef CMRA_DRIVERFRAMEDBASEPARSER_HPP
#error cmraDriverFramedBaseParser.hpp multiply included
#endif
#define CMRA_DRIVERFRAMEDBASEPARSER_HPP

#ifndef TMLN_PARSER_HPP
#include "Support/tmln/tmlnParser.hpp"
#endif


class cmraDriverFramedBaseParser : public tmlnDriverParser
{
public:
	//--------------------------------------------------------------------
	// Constructor
	//--------------------------------------------------------------------
	cmraDriverFramedBaseParser();

	//--------------------------------------------------------------------
	// Destructor
	//--------------------------------------------------------------------
	virtual ~cmraDriverFramedBaseParser();

	//--------------------------------------------------------------------
	// Returns chunk name used by this parser
	//--------------------------------------------------------------------
	static chDefs::Name GetChunkName();

	//--------------------------------------------------------------------
	//	Read is called by a parser utility when the chunk name has been read
	//  from the data file.  It will parse from the chunk into the given
	//  parsable object.
	//--------------------------------------------------------------------
	virtual void Read(	chReader& i_Reader,
						chDefs::Version i_Version,
						chDefs::Size i_Size,
						tmlnDriverInfo& o_Driver ) const;

	//--------------------------------------------------------------------
	//	Check the passed in name and see if it is a valid chunk.  If it
	//	is then read in the information and return true.
	//
	//	Note: this function does NOT finish the chunk.
	//--------------------------------------------------------------------
	virtual bool ReadChunk(	chReader& i_Reader,
							chDefs::Name i_Name, 
							chDefs::Version i_Version,
							chDefs::Size i_Size,
							tmlnDriverInfo& o_Driver ) const;

	//--------------------------------------------------------------------
	//	Write is called to parse data from the given object to the given
	//	chWriter.  i_Driver is the object being written.
	//--------------------------------------------------------------------
	virtual void Write( chWriter& i_Writer,
						const tmlnDriverInfo& i_Driver ) const;

	//--------------------------------------------------------------------
	//	Write out the sub-chunks for this parser.
	//
	//	Note: this function does finish each sub-chunk.
	//--------------------------------------------------------------------
	virtual void WriteChunks(  chWriter& o_Writer,
							   const tmlnDriverInfo& i_Driver ) const;

	//--------------------------------------------------------------------
	// Create should be overridden to create the user specific parsable object.
	// Users of parsers should call create to create the object that is then
	// passed to Read.
	//--------------------------------------------------------------------
	virtual tmlnDriverInfo* Create() const;
};

