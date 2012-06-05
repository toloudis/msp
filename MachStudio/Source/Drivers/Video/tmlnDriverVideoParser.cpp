/********************************************************************************************\
**  tmlnDriverVideoParser.cpp
**
**		see .hpp
**
**  StudioGPU
**  Copyright(C) 2005 - All Rights Reserved
\********************************************************************************************/
#include "Drivers/Video/tmlnDriverVideoParser.hpp"
#include "Drivers/Video/tmlnDriverVideoInfo.hpp"

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
	const chDefs::Name c_VIDF = chDefs::MakeName('V', 'I', 'D', 'F');

	//------------------------------------------------------------------------
	//   ReadScriptInfo
	//------------------------------------------------------------------------
	void ReadScriptInfo( chReader& i_Reader,
							chDefs::Version i_Version,
							chDefs::Size i_Size,
							tmlnDriverVideoInfo &o_Info )
	{
		// Read in a single itString with path separators and then 
		// split into locator
		itString loadfilename;
		chChunkParserUtil::Read( i_Reader, loadfilename );
		fsFileUtil::UnicodeStringToLocator( loadfilename, o_Info.m_FileName );
//		chChunkParserUtil::Read(i_Reader, o_Info.m_FileName);

		chChunkParserUtil::Read(i_Reader, o_Info.m_FrameRate);
		chChunkParserUtil::Read(i_Reader, o_Info.m_bLooping );
		chChunkParserUtil::Read(i_Reader, o_Info.m_bDriverToAnimLength );
		chChunkParserUtil::Read(i_Reader, o_Info.m_bReversing );
	}

	//------------------------------------------------------------------------
	//   WriteScriptInfo
	//------------------------------------------------------------------------
	void WriteScriptInfo(chWriter& o_Writer,
							const tmlnDriverVideoInfo &i_Info )
	{
		const int l_VIDF_VERSION = 1;
		o_Writer.WriteChunkHeader( c_VIDF, l_VIDF_VERSION, false );

		itString savefilename;
		fsFileUtil::LocatorToUnicodeString( i_Info.m_FileName, savefilename );
		chChunkParserUtil::Write( o_Writer, savefilename );
//		chChunkParserUtil::Write( o_Writer, i_Info.m_FileName );

		chChunkParserUtil::Write(o_Writer, i_Info.m_FrameRate);
		chChunkParserUtil::Write(o_Writer, i_Info.m_bLooping );
		chChunkParserUtil::Write(o_Writer, i_Info.m_bDriverToAnimLength );
		chChunkParserUtil::Write(o_Writer, i_Info.m_bReversing );

		o_Writer.FinishChunk();
	}

}	// local namespace


//----------------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------------
tmlnDriverVideoParser::tmlnDriverVideoParser(chDefs::Name i_ChunkName)
: m_ChunkName(i_ChunkName)
{
}

//----------------------------------------------------------------------------
// Destructor
//----------------------------------------------------------------------------
tmlnDriverVideoParser::~tmlnDriverVideoParser()
{
}

//----------------------------------------------------------------------------
// Returns chunk name used by this parser
//----------------------------------------------------------------------------
//static
chDefs::Name tmlnDriverVideoParser::GetChunkName()
{
	return m_ChunkName;
}

//----------------------------------------------------------------------------
//	Read is called by a parser utility when the chunk name has been read
//  from the data file.  It will parse from the chunk into the given
//  parsable object.
//----------------------------------------------------------------------------
void tmlnDriverVideoParser::Read(	chReader& i_Reader,
										chDefs::Version i_Version,
										chDefs::Size i_Size,
										tmlnDriverInfo& o_Driver ) const
{
	tmlnDriverVideoInfo &driver = dynamic_cast<tmlnDriverVideoInfo &>(o_Driver);

	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;

	while( i_Reader.ReadChunkHeader(name, version, size) )
	{
		if ( name == tmlnDriverInfoParser::GetChunkName() )
		{
			tmlnDriverInfoParser::ReadDriverInfo(i_Reader, version, size, driver);
		}
		else if ( name == c_VIDF )
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
void tmlnDriverVideoParser::Write( chWriter& o_Writer,
										const tmlnDriverInfo& i_Driver ) const
{
	const tmlnDriverVideoInfo &driver = dynamic_cast<const tmlnDriverVideoInfo &>(i_Driver);

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
tmlnDriverInfo* tmlnDriverVideoParser::Create() const
{
	return new tmlnDriverVideoInfo(m_ChunkName);
}


