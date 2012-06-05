/********************************************************************************************\
**  tmlnDriverColorParser.cpp
**
**
**  Extra Large Technology
**  Copyright(C) 2003 - All Rights Reserved
\********************************************************************************************/

#include "Drivers/Color/tmlnDriverColorParser.hpp"

#include "Support/tmln/tmlnDriverInfoParser.hpp"
#include "Drivers/Color/tmlnDriverColorInfo.hpp"

#include "Core/ch/chReader.hpp"
#include "Core/ch/chWriter.hpp"
#include "Core/ch/chExceptionX.hpp"
#include "Core/dbg/dbgLog.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/gf/gfFileBin.hpp"
#include "Core/ch/chChunkParserUtil.hpp"

namespace
{
	//========================================================================
	//========================================================================
	const chDefs::Name c_COLR = chDefs::MakeName('C', 'O', 'L', 'R');


	//========================================================================
	//   ReadColorInfo
	//========================================================================
	void ReadColorInfo(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					tmlnDriverColorInfo &o_Driver )
	{
		chChunkParserUtil::Read(i_Reader, o_Driver.m_Value);
	}

	//========================================================================
	//   WriteColorInfo
	//========================================================================
	void WriteColorInfo(	chWriter& o_Writer,
					const tmlnDriverColorInfo &i_Driver )
	{

		o_Writer.WriteChunkHeader( c_COLR, 0, false );
		chChunkParserUtil::Write(o_Writer, i_Driver.m_Value);
		o_Writer.FinishChunk();
	}

}	// local namespace

//--------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------
tmlnDriverColorParser::tmlnDriverColorParser(chDefs::Name i_ChunkName)
: m_ChunkName(i_ChunkName)
{

}

//--------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------
tmlnDriverColorParser::~tmlnDriverColorParser()
{

}

//--------------------------------------------------------------------
// Returns chunk name used by this parser
//--------------------------------------------------------------------
chDefs::Name tmlnDriverColorParser::GetChunkName()
{
	return m_ChunkName;
}

//--------------------------------------------------------------------
//	Read is called by a parser utility when the chunk name has been read
//  from the data file.  It will parse from the chunk into the given
//  parsable object.
//--------------------------------------------------------------------
void tmlnDriverColorParser::Read(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					tmlnDriverInfo& o_Driver ) const
{
	tmlnDriverColorInfo &driver = dynamic_cast<tmlnDriverColorInfo &>(o_Driver);

	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;

	while( i_Reader.ReadChunkHeader(name, version, size) )
	{
		if ( name == tmlnDriverInfoParser::GetChunkName() )
		{
			tmlnDriverInfoParser::ReadDriverInfo(i_Reader, version, size, driver);
		}
		else if ( name == c_COLR )
		{
			ReadColorInfo(i_Reader, version, size, driver);
		}
		i_Reader.FinishChunk();
	}
}

//--------------------------------------------------------------------
//	Write is called to parse data from the given object to the given
//	chWriter.  i_Driver is the object being written.
//--------------------------------------------------------------------
void tmlnDriverColorParser::Write( chWriter& o_Writer,
					const tmlnDriverInfo& i_Driver ) const
{
	const tmlnDriverColorInfo &driver = dynamic_cast<const tmlnDriverColorInfo &>(i_Driver);

	const bool c_bXXXX_CONTAINER_CHUNK = true;
	o_Writer.WriteChunkHeader( m_ChunkName, 0, c_bXXXX_CONTAINER_CHUNK );
	tmlnDriverInfoParser::WriteDriverInfo(o_Writer, driver);

	WriteColorInfo(o_Writer, driver);
	o_Writer.FinishChunk();
}

//--------------------------------------------------------------------
// Create should be overridden to create the user specific parsable object.
// Users of parsers should call create to create the object that is then
// passed to Read.
//--------------------------------------------------------------------
tmlnDriverInfo* tmlnDriverColorParser::Create() const
{
	return new tmlnDriverColorInfo(m_ChunkName);
}


