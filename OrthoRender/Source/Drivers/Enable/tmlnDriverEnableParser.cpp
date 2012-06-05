/********************************************************************************************\
**  tmlnDriverEnableParser.cpp
**
**
**  Extra Large Technology
**  Copyright(C) 2003 - All Rights Reserved
\********************************************************************************************/

#include "Drivers/Enable/tmlnDriverEnableParser.hpp"

#include "Support/tmln/tmlnDriverInfoParser.hpp"
#include "Drivers/Enable/tmlnDriverEnableInfo.hpp"

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
	const chDefs::Name c_ENBI = chDefs::MakeName('E', 'N', 'B', 'I');


	//========================================================================
	//   ReadEnableInfo
	//========================================================================
	void ReadEnableInfo(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					tmlnDriverEnableInfo &o_Driver )
	{
		chChunkParserUtil::Read(i_Reader, o_Driver.m_Enabled);
	}

	//========================================================================
	//   WriteEnableInfo
	//========================================================================
	void WriteEnableInfo(	chWriter& o_Writer,
					const tmlnDriverEnableInfo &i_Driver )
	{

		o_Writer.WriteChunkHeader( c_ENBI, 0, false );
		chChunkParserUtil::Write(o_Writer, i_Driver.m_Enabled);
		o_Writer.FinishChunk();
	}

}	// local namespace

//--------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------
tmlnDriverEnableParser::tmlnDriverEnableParser(chDefs::Name i_ChunkName)
: m_ChunkName(i_ChunkName)
{

}

//--------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------
tmlnDriverEnableParser::~tmlnDriverEnableParser()
{

}

//--------------------------------------------------------------------
// Returns chunk name used by this parser
//--------------------------------------------------------------------
chDefs::Name tmlnDriverEnableParser::GetChunkName()
{
	return m_ChunkName;
}

//--------------------------------------------------------------------
//	Read is called by a parser utility when the chunk name has been read
//  from the data file.  It will parse from the chunk into the given
//  parsable object.
//--------------------------------------------------------------------
void tmlnDriverEnableParser::Read(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					tmlnDriverInfo& o_Driver ) const
{
	tmlnDriverEnableInfo &driver = dynamic_cast<tmlnDriverEnableInfo &>(o_Driver);

	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;

	while( i_Reader.ReadChunkHeader(name, version, size) )
	{
		if ( name == tmlnDriverInfoParser::GetChunkName() )
		{
			tmlnDriverInfoParser::ReadDriverInfo(i_Reader, version, size, driver);
		}
		else if ( name == c_ENBI )
		{
			ReadEnableInfo(i_Reader, version, size, driver);
		}
		i_Reader.FinishChunk();
	}
}

//--------------------------------------------------------------------
//	Write is called to parse data from the given object to the given
//	chWriter.  i_Driver is the object being written.
//--------------------------------------------------------------------
void tmlnDriverEnableParser::Write( chWriter& o_Writer,
					const tmlnDriverInfo& i_Driver ) const
{
	const tmlnDriverEnableInfo &driver = dynamic_cast<const tmlnDriverEnableInfo &>(i_Driver);

	o_Writer.WriteChunkHeader( m_ChunkName, 0, true );
	tmlnDriverInfoParser::WriteDriverInfo(o_Writer, driver);

	WriteEnableInfo(o_Writer, driver);
	o_Writer.FinishChunk();
}

//--------------------------------------------------------------------
// Create should be overridden to create the user specific parsable object.
// Users of parsers should call create to create the object that is then
// passed to Read.
//--------------------------------------------------------------------
tmlnDriverInfo* tmlnDriverEnableParser::Create() const
{
	return new tmlnDriverEnableInfo(m_ChunkName);
}


