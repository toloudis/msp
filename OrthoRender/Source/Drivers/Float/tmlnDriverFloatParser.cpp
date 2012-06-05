/********************************************************************************************\
**  tmlnDriverFloatParser.cpp
**
**
**  Extra Large Technology
**  Copyright(C) 2003 - All Rights Reserved
\********************************************************************************************/

#include "Drivers/Float/tmlnDriverFloatParser.hpp"

#include "Support/tmln/tmlnDriverInfoParser.hpp"
#include "Drivers/Float/tmlnDriverFloatInfo.hpp"

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
	const chDefs::Name c_FVAL = chDefs::MakeName('F', 'V', 'A', 'L');


	//========================================================================
	//   ReadFloatInfo
	//========================================================================
	void ReadFloatInfo(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					tmlnDriverFloatInfo &o_Driver )
	{
		chChunkParserUtil::Read(i_Reader, o_Driver.m_Value);
	}

	//========================================================================
	//   WriteFloatInfo
	//========================================================================
	void WriteFloatInfo(	chWriter& o_Writer,
					const tmlnDriverFloatInfo &i_Driver )
	{

		o_Writer.WriteChunkHeader( c_FVAL, 0, false );
		chChunkParserUtil::Write(o_Writer, i_Driver.m_Value);
		o_Writer.FinishChunk();
	}

}	// local namespace

//--------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------
tmlnDriverFloatParser::tmlnDriverFloatParser(chDefs::Name i_ChunkName)
: m_ChunkName(i_ChunkName)
{

}

//--------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------
tmlnDriverFloatParser::~tmlnDriverFloatParser()
{

}

//--------------------------------------------------------------------
// Returns chunk name used by this parser
//--------------------------------------------------------------------
chDefs::Name tmlnDriverFloatParser::GetChunkName()
{
	return m_ChunkName;
}

//--------------------------------------------------------------------
//	Read is called by a parser utility when the chunk name has been read
//  from the data file.  It will parse from the chunk into the given
//  parsable object.
//--------------------------------------------------------------------
void tmlnDriverFloatParser::Read(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					tmlnDriverInfo& o_Driver ) const
{
	tmlnDriverFloatInfo &driver = dynamic_cast<tmlnDriverFloatInfo &>(o_Driver);

	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;

	while( i_Reader.ReadChunkHeader(name, version, size) )
	{
		if ( name == tmlnDriverInfoParser::GetChunkName() )
		{
			tmlnDriverInfoParser::ReadDriverInfo(i_Reader, version, size, driver);
		}
		else if ( name == c_FVAL )
		{
			ReadFloatInfo(i_Reader, version, size, driver);
		}
		i_Reader.FinishChunk();
	}
}

//--------------------------------------------------------------------
//	Write is called to parse data from the given object to the given
//	chWriter.  i_Driver is the object being written.
//--------------------------------------------------------------------
void tmlnDriverFloatParser::Write( chWriter& o_Writer,
					const tmlnDriverInfo& i_Driver ) const
{
	const tmlnDriverFloatInfo &driver = dynamic_cast<const tmlnDriverFloatInfo &>(i_Driver);

	o_Writer.WriteChunkHeader( m_ChunkName, 0, true );
	tmlnDriverInfoParser::WriteDriverInfo(o_Writer, driver);

	WriteFloatInfo(o_Writer, driver);
	o_Writer.FinishChunk();
}

//--------------------------------------------------------------------
// Create should be overridden to create the user specific parsable object.
// Users of parsers should call create to create the object that is then
// passed to Read.
//--------------------------------------------------------------------
tmlnDriverInfo* tmlnDriverFloatParser::Create() const
{
	return new tmlnDriverFloatInfo(m_ChunkName);
}


