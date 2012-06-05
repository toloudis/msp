/*****************************************************************************
**	tmlnParser.hpp
**
**		tmlnParser handles the creation of channels and drivers for
**	different types of objects.
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#ifdef TMLN_PARSER_HPP
#error tmlnParser.hpp multiply included
#endif
#define TMLN_PARSER_HPP

#ifndef CH_DEFS_HPP
#include "Core/ch/chDefs.hpp"
#endif

#include <vector>

//============================================================================
//	forward references
//============================================================================
class tmlnDriverInfo;
class tmlnScriptObject;
class chReader;
class chWriter;

//============================================================================
//============================================================================
class tmlnDriverParser
{
public:
	//--------------------------------------------------------------------
	// Destructor
	//--------------------------------------------------------------------
	virtual ~tmlnDriverParser() {}

	//--------------------------------------------------------------------
	//	Read is called by a parser utility when the chunk name has been read
	//  from the data file.  It will parse from the chunk into the given
	//  parsable object.
	//--------------------------------------------------------------------
	virtual void Read(	chReader& i_Reader,
						chDefs::Version i_Version,
						chDefs::Size i_Size,
						tmlnDriverInfo& o_Driver ) const = 0;

	//--------------------------------------------------------------------
	//	Write is called to parse data from the given object to the given
	//	chWriter.  i_Driver is the object being written.
	//--------------------------------------------------------------------
	virtual void Write( chWriter& i_Writer,
						const tmlnDriverInfo& i_Driver ) const = 0;

	//--------------------------------------------------------------------
	// Create should be overridden to create the user specific parsable object.
	// Users of parsers should call create to create the object that is then
	// passed to Read.
	//--------------------------------------------------------------------
	//virtual tmlnDriverInfo* Create(tmlnScriptObject& io_Object) const = 0;
	virtual tmlnDriverInfo* Create() const=0;
};

//============================================================================
//============================================================================
namespace tmlnParser
{
	//--------------------------------------------------------------------
	//	Deinitialize/Initialize
	//--------------------------------------------------------------------
	void Initialize();
	void DeInitialize();

	//--------------------------------------------------------------------
	// Adds a method for creating parsers for a given driver type.
	// The system code (the chunk name for the document) can be used to
	// limit the scope of the chunk name in order to prevent overlaps
	// in the chunk names. If not included, the chunk name will be
	// put in a general list and will be considered for all systems.
	//--------------------------------------------------------------------
	void AddDriverParser(chDefs::Name i_Name, 
						 tmlnDriverParser* i_pParser);	
	void AddDriverParser(chDefs::Name i_SystemCode, 
						 chDefs::Name i_Name, 
						 tmlnDriverParser* i_pParser);

	//--------------------------------------------------------------------
	//   ReadDrivers - read drivers into vector
	//--------------------------------------------------------------------
	void ReadDrivers(	chReader& i_Reader,
						chDefs::Name i_SystemCode, 
						 std::vector<tmlnDriverInfo*> &o_Drivers );

	//--------------------------------------------------------------------
	//   WriteDrivers - write drivers from vector into file
	//--------------------------------------------------------------------
	void WriteDrivers( chWriter& o_Writer,
					   chDefs::Name i_SystemCode, 
					   const std::vector<tmlnDriverInfo*> &i_Drivers );

};
