/********************************************************************************************\
**  tmlnDriverColorFlickerParser.cpp
**
**		see .hpp
**
**  Extra Large Technology
**  Copyright(C) 2005 - All Rights Reserved
\********************************************************************************************/

#include "Drivers/ColorFlicker/tmlnDriverColorFlickerParser.hpp"

#include "Support/tmln/tmlnDriverInfoParser.hpp"
#include "Drivers/ColorFlicker/tmlnDriverColorFlickerInfo.hpp"

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
	const chDefs::Name c_CLRF = chDefs::MakeName('C', 'L', 'R', 'F');


	//========================================================================
	//   ReadColorFlickerInfo
	//========================================================================
	void ReadColorFlickerInfo(	chReader& i_Reader,
								chDefs::Version i_Version,
								chDefs::Size i_Size,
								tmlnDriverColorFlickerInfo &o_Driver )
	{
		chChunkParserUtil::Read(i_Reader, o_Driver.m_StartColor1);
		chChunkParserUtil::Read(i_Reader, o_Driver.m_StartColor2);
		chChunkParserUtil::Read(i_Reader, o_Driver.m_EndColor1);
		chChunkParserUtil::Read(i_Reader, o_Driver.m_EndColor2);
		chChunkParserUtil::Read(i_Reader, o_Driver.m_fFrequency);
	}

	//========================================================================
	//   WriteColorFlickerInfo
	//========================================================================
	void WriteColorFlickerInfo(	chWriter& o_Writer,
								const tmlnDriverColorFlickerInfo &i_Driver )
	{
		const int c_CLRF_VERSION = 0;
		o_Writer.WriteChunkHeader( c_CLRF, c_CLRF_VERSION, false );

		chChunkParserUtil::Write(o_Writer, i_Driver.m_StartColor1);
		chChunkParserUtil::Write(o_Writer, i_Driver.m_StartColor2);
		chChunkParserUtil::Write(o_Writer, i_Driver.m_EndColor1);
		chChunkParserUtil::Write(o_Writer, i_Driver.m_EndColor2);
		chChunkParserUtil::Write(o_Writer, i_Driver.m_fFrequency);

		o_Writer.FinishChunk();
	}

}	// local namespace


//--------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------
tmlnDriverColorFlickerParser::tmlnDriverColorFlickerParser()
: m_ChunkName(c_CLRF)
{

}

//--------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------
tmlnDriverColorFlickerParser::tmlnDriverColorFlickerParser(chDefs::Name i_ChunkName)
: m_ChunkName(i_ChunkName)
{

}

//--------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------
tmlnDriverColorFlickerParser::~tmlnDriverColorFlickerParser()
{

}

//--------------------------------------------------------------------
// Returns chunk name used by this parser
//--------------------------------------------------------------------
chDefs::Name tmlnDriverColorFlickerParser::GetChunkName()
{
	return m_ChunkName;
}

//--------------------------------------------------------------------
//	Read is called by a parser utility when the chunk name has been read
//  from the data file.  It will parse from the chunk into the given
//  parsable object.
//--------------------------------------------------------------------
void tmlnDriverColorFlickerParser::Read(	chReader& i_Reader,
											chDefs::Version i_Version,
											chDefs::Size i_Size,
											tmlnDriverInfo& o_Driver ) const
{
	tmlnDriverColorFlickerInfo &driver = dynamic_cast<tmlnDriverColorFlickerInfo &>(o_Driver);

	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;

	while( i_Reader.ReadChunkHeader(name, version, size) )
	{
		if ( name == tmlnDriverInfoParser::GetChunkName() )
		{
			tmlnDriverInfoParser::ReadDriverInfo(i_Reader, version, size, driver);
		}
		else if ( name == c_CLRF )
		{
			ReadColorFlickerInfo(i_Reader, version, size, driver);
		}
		i_Reader.FinishChunk();
	}
}

//--------------------------------------------------------------------
//	Write is called to parse data from the given object to the given
//	chWriter.  i_Driver is the object being written.
//--------------------------------------------------------------------
void tmlnDriverColorFlickerParser::Write(	chWriter& o_Writer,
											const tmlnDriverInfo& i_Driver ) const
{
	const tmlnDriverColorFlickerInfo &driver = dynamic_cast<const tmlnDriverColorFlickerInfo &>(i_Driver);

	o_Writer.WriteChunkHeader( m_ChunkName, 0, true );
	tmlnDriverInfoParser::WriteDriverInfo(o_Writer, driver);

	WriteColorFlickerInfo(o_Writer, driver);
	o_Writer.FinishChunk();
}

//--------------------------------------------------------------------
// Create should be overridden to create the user specific parsable object.
// Users of parsers should call create to create the object that is then
// passed to Read.
//--------------------------------------------------------------------
tmlnDriverInfo* tmlnDriverColorFlickerParser::Create() const
{
	return new tmlnDriverColorFlickerInfo(m_ChunkName);
}


