/********************************************************************************************\
**  sbrdDriverAnimatedTextureParser.cpp
**
**		see .hpp
**
**  Extra Large Technology
**  Copyright(C) 2006 - All Rights Reserved
\********************************************************************************************/

#include "Systems/Storyboards/Timeline/sbrdDriverAnimatedTextureParser.hpp"

#include "Support/tmln/tmlnDriverInfoParser.hpp"
#include "Systems/Storyboards/Timeline/sbrdDriverAnimatedTextureInfo.hpp"

#include "Core/Ch/chReader.hpp"
#include "Core/Ch/chWriter.hpp"
#include "Core/Ch/chExceptionX.hpp"
#include "Core/Dbg/dbgLog.hpp"
#include "Core/Fs/fsFileUtil.hpp"
#include "Core/Fs/fsFileX.hpp"
#include "Core/Gf/gfFileBin.hpp"
#include "Core/Ch/chChunkParserUtil.hpp"


namespace
{
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	const chDefs::Name c_ATEX = chDefs::MakeName('A', 'T', 'E', 'X');
	const chDefs::Name c_ATXI = chDefs::MakeName('A', 'T', 'X', 'I');

	const int l_ATEX_VERSION = 0;
	const int l_ATXI_VERSION = 0;

	//------------------------------------------------------------------------
	//   ReadScriptInfo
	//------------------------------------------------------------------------
	void ReadScriptInfo( chReader& i_Reader,
							chDefs::Version i_Version,
							chDefs::Size i_Size,
							sbrdDriverAnimatedTextureInfo &o_Info )
	{
		chChunkParserUtil::Read(i_Reader, o_Info.m_FirstTextureFilename);
		chChunkParserUtil::Read(i_Reader, o_Info.m_NumberOfFrames );
		chChunkParserUtil::Read(i_Reader, o_Info.m_FrameRate);
		chChunkParserUtil::Read(i_Reader, o_Info.m_bLooping );
		chChunkParserUtil::Read(i_Reader, o_Info.m_bDriverResizeByAnimLength );
	}

	//------------------------------------------------------------------------
	//   WriteScriptInfo
	//------------------------------------------------------------------------
	void WriteScriptInfo(chWriter& o_Writer,
							const sbrdDriverAnimatedTextureInfo &i_Info )
	{
		o_Writer.WriteChunkHeader( c_ATXI, l_ATXI_VERSION, false );

		chChunkParserUtil::Write(o_Writer, i_Info.m_FirstTextureFilename);
		chChunkParserUtil::Write(o_Writer, i_Info.m_NumberOfFrames );
		chChunkParserUtil::Write(o_Writer, i_Info.m_FrameRate);
		chChunkParserUtil::Write(o_Writer, i_Info.m_bLooping );
		chChunkParserUtil::Write(o_Writer, i_Info.m_bDriverResizeByAnimLength );

		o_Writer.FinishChunk();
	}

}	// local namespace


//----------------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------------
sbrdDriverAnimatedTextureParser::sbrdDriverAnimatedTextureParser()
{
}

//----------------------------------------------------------------------------
// Destructor
//----------------------------------------------------------------------------
sbrdDriverAnimatedTextureParser::~sbrdDriverAnimatedTextureParser()
{
}

//----------------------------------------------------------------------------
// Returns chunk name used by this parser
//----------------------------------------------------------------------------
//static
chDefs::Name sbrdDriverAnimatedTextureParser::GetChunkName()
{
	return c_ATEX;
}

//----------------------------------------------------------------------------
//	Read is called by a parser utility when the chunk name has been read
//  from the data file.  It will parse from the chunk into the given
//  parsable object.
//----------------------------------------------------------------------------
void sbrdDriverAnimatedTextureParser::Read(	chReader& i_Reader,
										chDefs::Version i_Version,
										chDefs::Size i_Size,
										tmlnDriverInfo& o_Driver ) const
{
	sbrdDriverAnimatedTextureInfo &driver = dynamic_cast<sbrdDriverAnimatedTextureInfo &>(o_Driver);

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
void sbrdDriverAnimatedTextureParser::Write( chWriter& o_Writer,
										const tmlnDriverInfo& i_Driver ) const
{
	const sbrdDriverAnimatedTextureInfo &driver = dynamic_cast<const sbrdDriverAnimatedTextureInfo &>(i_Driver);

	o_Writer.WriteChunkHeader( c_ATEX, l_ATEX_VERSION, true );
	tmlnDriverInfoParser::WriteDriverInfo(o_Writer, driver);

	WriteScriptInfo(o_Writer, driver);
	o_Writer.FinishChunk();
}

//----------------------------------------------------------------------------
// Create should be overridden to create the user specific parsable object.
// Users of parsers should call create to create the object that is then
// passed to Read.
//----------------------------------------------------------------------------
tmlnDriverInfo* sbrdDriverAnimatedTextureParser::Create() const
{
	return new sbrdDriverAnimatedTextureInfo(c_ATEX);
}


