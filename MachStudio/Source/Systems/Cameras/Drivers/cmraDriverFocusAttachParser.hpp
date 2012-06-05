/********************************************************************************************\
**  cmraDriverFocusAttachParser.hpp
**
**
**  StudioGPU
**  Copyright(C) 2005 - All Rights Reserved
\********************************************************************************************/

#ifdef CMRA_DRIVERFOCUSATTACHPARSER_HPP
#error cmraDriverFocusAttachParser.hpp multiply included
#endif
#define CMRA_DRIVERFOCUSATTACHPARSER_HPP

#ifndef TMLN_PARSER_HPP
#include "Support/tmln/tmlnParser.hpp"
#endif


class cmraDriverFocusAttachParser : public tmlnDriverParser
{
public:
	//--------------------------------------------------------------------
	// Constructor
	//--------------------------------------------------------------------
	cmraDriverFocusAttachParser(chDefs::Name i_ChunkName);

	//--------------------------------------------------------------------
	// Destructor
	//--------------------------------------------------------------------
	virtual ~cmraDriverFocusAttachParser();

	//--------------------------------------------------------------------
	// Returns chunk name used by this parser
	//--------------------------------------------------------------------
	chDefs::Name GetChunkName();

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
	//	Write is called to parse data from the given object to the given
	//	chWriter.  i_Driver is the object being written.
	//--------------------------------------------------------------------
	virtual void Write( chWriter& i_Writer,
						const tmlnDriverInfo& i_Driver ) const;

	//--------------------------------------------------------------------
	// Create should be overridden to create the user specific parsable object.
	// Users of parsers should call create to create the object that is then
	// passed to Read.
	//--------------------------------------------------------------------
	virtual tmlnDriverInfo* Create() const;

private:
	chDefs::Name m_ChunkName;
};

