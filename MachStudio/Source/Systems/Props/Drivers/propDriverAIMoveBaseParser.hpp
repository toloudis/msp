/********************************************************************************************\
**  propDriverAIMoveBaseParser.hpp
**
**		Parser for AIMoveBase
**
**	future enhancements:
**		- abstract out the spline driver so other movement types could be put in.
**
**  StudioGPU
**  Copyright(C) 2005 - All Rights Reserved
\********************************************************************************************/

#ifdef PROP_DRIVERAIMOVEBASEPARSER_HPP
#error propDriverAIMoveBaseParser.hpp multiply included
#endif
#define PROP_DRIVERAIMOVEBASEPARSER_HPP

#ifndef TMLN_PARSER_HPP
#include "tmlnParser.hpp"
#endif


class propDriverAIMoveBaseParser : public tmlnDriverParser
{
public:
	//--------------------------------------------------------------------
	// Constructor
	//--------------------------------------------------------------------
	propDriverAIMoveBaseParser();

	//--------------------------------------------------------------------
	// Destructor
	//--------------------------------------------------------------------
	virtual ~propDriverAIMoveBaseParser();

	//--------------------------------------------------------------------
	// Returns chunk name used by this parser
	//--------------------------------------------------------------------
	static chDefs::Name GetChunkName();

	//--------------------------------------------------------------------
	// FIX: - don't like this at all.  only used to get the chunk name to
	//	propDriverAIMoveBaseInfo constructor.
	//--------------------------------------------------------------------
	static chDefs::Name GetSplineChunkName();

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

