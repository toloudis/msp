/********************************************************************************************\
**  cmraDriverDataAttachNodeParser.hpp
**
**		the parser for cmra AttachNode
**
**  Extra Large Technology
**  Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/

#ifdef CMRA_DRIVERTARGETPARSER_HPP
#error cmraDriverDataAttachNodeParser.hpp multiply included
#endif
#define CMRA_DRIVERTARGETPARSER_HPP

#ifndef CH_DEFS_HPP
#include "Core/Ch/chDefs.hpp"
#endif

#ifndef CMRA_DRIVERDATAATTACHNODEINFO_HPP
#include "Systems/Cameras/Drivers/cmraDriverDataAttachNodeInfo.hpp"
#endif


//============================================================================
//============================================================================
class tmlnDriverInfo;
class chReader;
class chWriter;
class fsLocator;


//============================================================================
//============================================================================
class cmraDriverDataAttachNodeParser
{
public:
	//--------------------------------------------------------------------
	// Constructor
	//--------------------------------------------------------------------
	cmraDriverDataAttachNodeParser();

	//--------------------------------------------------------------------
	// Destructor
	//--------------------------------------------------------------------
	virtual ~cmraDriverDataAttachNodeParser();

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
						cmraDriverDataAttachNodeInfo& o_Driver ) const;

	//--------------------------------------------------------------------
	//	Write is called to parse data from the given object to the given
	//	chWriter.  i_Driver is the object being written.
	//--------------------------------------------------------------------
	virtual void Write( chWriter& i_Writer,
						const cmraDriverDataAttachNodeInfo& i_Driver ) const;

	//--------------------------------------------------------------------
	// Create should be overridden to create the user specific parsable object.
	// Users of parsers should call create to create the object that is then
	// passed to Read.
	//--------------------------------------------------------------------
	virtual cmraDriverDataAttachNodeInfo* Create() const;
};

