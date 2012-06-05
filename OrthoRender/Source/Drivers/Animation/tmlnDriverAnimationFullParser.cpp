/********************************************************************************************\
**  tmlnDriverAnimationFullParser.cpp
**
**
**  Extra Large Technology
**  Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/

#include "Drivers/Animation/tmlnDriverAnimationFullParser.hpp"

#include "Drivers/Animation/tmlnDriverAnimationFullInfo.hpp"

#include "Support/tmln/tmlnDriverInfoParser.hpp"

#include "Core/ch/chReader.hpp"
#include "Core/ch/chWriter.hpp"
#include "Core/ch/chExceptionX.hpp"
#include "Core/dbg/dbgLog.hpp"
#include "Core/ch/chChunkParserUtil.hpp"


namespace
{
	//========================================================================
	//========================================================================
	const chDefs::Name c_TDAD = chDefs::MakeName('T', 'D', 'A', 'D');	// new common driver anim full data chunk
	// Old formats from character and prop specific versions:
	const chDefs::Name c_CDAD = chDefs::MakeName('C', 'D', 'A', 'D');	// character old chunk
	const chDefs::Name c_PDAD = chDefs::MakeName('P', 'D', 'A', 'D');	// prop old chunk

	//========================================================================
	//   ReadAnimFullInfo
	//========================================================================
	void ReadAnimFullInfo(	chReader& i_Reader,
							chDefs::Version i_Version,
							chDefs::Size i_Size,
							tmlnAnimationFullInfo& o_Info )
	{
		chChunkParserUtil::Read(i_Reader, o_Info.m_AnimFilename );
		chChunkParserUtil::Read(i_Reader, o_Info.m_bDriverToAnimLength );

		chChunkParserUtil::Read(i_Reader, o_Info.m_AnimName );
		chChunkParserUtil::Read(i_Reader, o_Info.m_FrameRate );
		chChunkParserUtil::Read(i_Reader, o_Info.m_StartFrame );
		chChunkParserUtil::Read(i_Reader, o_Info.m_EndFrame );
		chChunkParserUtil::Read(i_Reader, o_Info.m_bLooping );

		if (i_Version >= 1)
		{
			chChunkParserUtil::Read(i_Reader, o_Info.m_bUseAnimStart );
		}
	}

	//========================================================================
	//   WriteAnimFullInfo
	//========================================================================
	void WriteAnimFullInfo(	chWriter& o_Writer,
							const tmlnAnimationFullInfo& i_Info )
	{
		o_Writer.WriteChunkHeader( c_TDAD, 1, false );

		chChunkParserUtil::Write(o_Writer, i_Info.m_AnimFilename );
		chChunkParserUtil::Write(o_Writer, i_Info.m_bDriverToAnimLength );

		chChunkParserUtil::Write(o_Writer, i_Info.m_AnimName );
		chChunkParserUtil::Write(o_Writer, i_Info.m_FrameRate );
		chChunkParserUtil::Write(o_Writer, i_Info.m_StartFrame );
		chChunkParserUtil::Write(o_Writer, i_Info.m_EndFrame );
		chChunkParserUtil::Write(o_Writer, i_Info.m_bLooping );

		// Version 1 adds m_bUseAnimStart
		chChunkParserUtil::Write(o_Writer, i_Info.m_bUseAnimStart );

		o_Writer.FinishChunk();
	}

}	// local namespace

//--------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------
tmlnDriverAnimationFullParser::tmlnDriverAnimationFullParser(chDefs::Name i_ChunkName)
: m_ChunkName(i_ChunkName)
{

}

//--------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------
tmlnDriverAnimationFullParser::~tmlnDriverAnimationFullParser()
{

}

//--------------------------------------------------------------------
// Returns chunk name used by this parser
//--------------------------------------------------------------------
//static
chDefs::Name tmlnDriverAnimationFullParser::GetChunkName()
{
	return m_ChunkName;
}

//--------------------------------------------------------------------
//	Read is called by a parser utility when the chunk name has been read
//  from the data file.  It will parse from the chunk into the given
//  parsable object.
//--------------------------------------------------------------------
void tmlnDriverAnimationFullParser::Read(	chReader& i_Reader,
											chDefs::Version i_Version,
											chDefs::Size i_Size,
											tmlnDriverInfo& o_Driver ) const
{
	tmlnDriverAnimationFullInfo &driver = dynamic_cast<tmlnDriverAnimationFullInfo &>(o_Driver);

	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;

	while( i_Reader.ReadChunkHeader(name, version, size) )
	{
		if ( name == tmlnDriverInfoParser::GetChunkName() )
		{
			tmlnDriverInfoParser::ReadDriverInfo(i_Reader, version, size, driver);
		}
		else if ( name == c_TDAD || name == c_CDAD || name == c_PDAD )
		{
			ReadAnimFullInfo(i_Reader, version, size, driver.m_Info );
		}
		i_Reader.FinishChunk();
	}
}

//--------------------------------------------------------------------
//	Write is called to parse data from the given object to the given
//	chWriter.  i_Driver is the object being written.
//--------------------------------------------------------------------
void tmlnDriverAnimationFullParser::Write( chWriter& o_Writer,
					const tmlnDriverInfo& i_Driver ) const
{
	const tmlnDriverAnimationFullInfo &driver = dynamic_cast<const tmlnDriverAnimationFullInfo &>(i_Driver);

	o_Writer.WriteChunkHeader( m_ChunkName, 0, true );
	tmlnDriverInfoParser::WriteDriverInfo(o_Writer, driver);

	WriteAnimFullInfo( o_Writer, driver.m_Info );
	o_Writer.FinishChunk();
}

//--------------------------------------------------------------------
// Create should be overridden to create the user specific parsable object.
// Users of parsers should call create to create the object that is then
// passed to Read.
//--------------------------------------------------------------------
tmlnDriverInfo* tmlnDriverAnimationFullParser::Create() const
{
	return new tmlnDriverAnimationFullInfo(m_ChunkName);
}


