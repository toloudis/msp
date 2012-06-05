/********************************************************************************************\
**  tmlnDriverAnimatedFileNameParser.cpp
**
**		see .hpp
**
**  StudioGPU
**  Copyright(C) 2005 - All Rights Reserved
\********************************************************************************************/
#include "Drivers/FileName/tmlnDriverAnimatedFileNameParser.hpp"
#include "Drivers/FileName/tmlnDriverAnimatedFileNameInfo.hpp"

#include "Support/tmln/tmlnDriverInfoParser.hpp"

#include "Core/ch/chReader.hpp"
#include "Core/ch/chWriter.hpp"
#include "Core/ch/chExceptionX.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/gf/gfFileBin.hpp"
#include "Core/ch/chChunkParserUtil.hpp"


namespace
{
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	//const chDefs::Name c_ATEX = chDefs::MakeName('A', 'T', 'E', 'X');
	const chDefs::Name c_ATXI = chDefs::MakeName('A', 'T', 'X', 'I');

	//const int l_ATEX_VERSION = 0;
	const int l_ATXI_VERSION = 0;

	//------------------------------------------------------------------------
	//   ReadScriptInfo
	//------------------------------------------------------------------------
	void ReadScriptInfo( chReader& i_Reader,
							chDefs::Version i_Version,
							chDefs::Size i_Size,
							tmlnDriverAnimatedFileNameInfo &o_Info )
	{
		chChunkParserUtil::Read(i_Reader, o_Info.m_FirstFileName);
		chChunkParserUtil::Read(i_Reader, o_Info.m_NumberOfFrames );
		chChunkParserUtil::Read(i_Reader, o_Info.m_FrameRate);
		chChunkParserUtil::Read(i_Reader, o_Info.m_bLooping );
		chChunkParserUtil::Read(i_Reader, o_Info.m_bDriverToAnimLength );
	}

	//------------------------------------------------------------------------
	//   WriteScriptInfo
	//------------------------------------------------------------------------
	void WriteScriptInfo(chWriter& o_Writer,
							const tmlnDriverAnimatedFileNameInfo &i_Info )
	{
		o_Writer.WriteChunkHeader( c_ATXI, l_ATXI_VERSION, false );

		chChunkParserUtil::Write(o_Writer, i_Info.m_FirstFileName);
		chChunkParserUtil::Write(o_Writer, i_Info.m_NumberOfFrames );
		chChunkParserUtil::Write(o_Writer, i_Info.m_FrameRate);
		chChunkParserUtil::Write(o_Writer, i_Info.m_bLooping );
		chChunkParserUtil::Write(o_Writer, i_Info.m_bDriverToAnimLength );

		o_Writer.FinishChunk();
	}

}	// local namespace


//----------------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------------
tmlnDriverAnimatedFileNameParser::tmlnDriverAnimatedFileNameParser(chDefs::Name i_ChunkName)
: m_ChunkName(i_ChunkName)
{
}

//----------------------------------------------------------------------------
// Destructor
//----------------------------------------------------------------------------
tmlnDriverAnimatedFileNameParser::~tmlnDriverAnimatedFileNameParser()
{
}

//----------------------------------------------------------------------------
// Returns chunk name used by this parser
//----------------------------------------------------------------------------
//static
chDefs::Name tmlnDriverAnimatedFileNameParser::GetChunkName()
{
	return m_ChunkName;
}

//----------------------------------------------------------------------------
//	Read is called by a parser utility when the chunk name has been read
//  from the data file.  It will parse from the chunk into the given
//  parsable object.
//----------------------------------------------------------------------------
void tmlnDriverAnimatedFileNameParser::Read(	chReader& i_Reader,
										chDefs::Version i_Version,
										chDefs::Size i_Size,
										tmlnDriverInfo& o_Driver ) const
{
	tmlnDriverAnimatedFileNameInfo &driver = dynamic_cast<tmlnDriverAnimatedFileNameInfo &>(o_Driver);

	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;

	while( i_Reader.ReadChunkHeader(name, version, size) )
	{
		if ( name == tmlnDriverInfoParser::GetChunkName() )
		{
			tmlnDriverInfoParser::ReadDriverInfo(i_Reader, version, size, driver);
		}
		else if ( name == c_ATXI )
		{
			ReadScriptInfo(i_Reader, version, size, driver);
		}
		i_Reader.FinishChunk();
	}
}

//----------------------------------------------------------------------------
//	Write is called to parse data from the given object to the given
//	chWriter.  i_Driver is the object being written.
//----------------------------------------------------------------------------
void tmlnDriverAnimatedFileNameParser::Write( chWriter& o_Writer,
										const tmlnDriverInfo& i_Driver ) const
{
	const tmlnDriverAnimatedFileNameInfo &driver = dynamic_cast<const tmlnDriverAnimatedFileNameInfo &>(i_Driver);

	o_Writer.WriteChunkHeader( m_ChunkName, 0, true );
	tmlnDriverInfoParser::WriteDriverInfo(o_Writer, driver);

	WriteScriptInfo(o_Writer, driver);
	o_Writer.FinishChunk();
}

//----------------------------------------------------------------------------
// Create should be overridden to create the user specific parsable object.
// Users of parsers should call create to create the object that is then
// passed to Read.
//----------------------------------------------------------------------------
tmlnDriverInfo* tmlnDriverAnimatedFileNameParser::Create() const
{
	return new tmlnDriverAnimatedFileNameInfo(m_ChunkName);
}


