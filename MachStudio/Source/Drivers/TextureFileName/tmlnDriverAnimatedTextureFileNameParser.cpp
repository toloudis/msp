/********************************************************************************************\
**  tmlnDriverAnimatedTextureFileNameParser.cpp
**
**		see .hpp
**
**  StudioGPU
**  Copyright(C) 2005 - All Rights Reserved
\********************************************************************************************/
#include "Drivers/TextureFileName/tmlnDriverAnimatedTextureFileNameParser.hpp"
#include "Drivers/TextureFileName/tmlnDriverAnimatedTextureFileNameInfo.hpp"

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
	const chDefs::Name c_ATXF = chDefs::MakeName('A', 'T', 'X', 'F');

	//const int l_ATEX_VERSION = 0;

	//------------------------------------------------------------------------
	//   ReadScriptInfo
	//------------------------------------------------------------------------
	void ReadScriptInfo( chReader& i_Reader,
							chDefs::Version i_Version,
							chDefs::Size i_Size,
							tmlnDriverAnimatedTextureFileNameInfo &o_Info )
	{
		//chChunkParserUtil::Read(i_Reader, o_Info.m_FirstTextureFileName);
			
		// Read in a single itString with path separators and then 
		// split into locator
		itString savefilename;
		fsLocator texture_locator;
		chChunkParserUtil::Read( i_Reader, savefilename );
		fsFileUtil::UnicodeStringToLocator( savefilename, texture_locator );

		prtyTextureFileData val;
		val = o_Info.m_FirstTextureFileName.GetFullValue();
		val.m_TextureLocator = texture_locator;
		o_Info.m_FirstTextureFileName.SetValue(val);

		chChunkParserUtil::Read(i_Reader, o_Info.m_NumberOfFrames );
		chChunkParserUtil::Read(i_Reader, o_Info.m_FrameRate);
		chChunkParserUtil::Read(i_Reader, o_Info.m_bLooping );
		chChunkParserUtil::Read(i_Reader, o_Info.m_bDriverToAnimLength );
		chChunkParserUtil::Read(i_Reader, o_Info.m_bReversing );
		
	}

	//------------------------------------------------------------------------
	//   WriteScriptInfo
	//------------------------------------------------------------------------
	void WriteScriptInfo(chWriter& o_Writer,
							const tmlnDriverAnimatedTextureFileNameInfo &i_Info )
	{
		const int l_ATXF_VERSION = 1;
		o_Writer.WriteChunkHeader( c_ATXF, l_ATXF_VERSION, false );

		// Version 1 adds full path as a single itString
		//chChunkParserUtil::Write(o_Writer, i_Info.m_FirstTextureFileName);
		// Convert array of itStrings to a single itString with
		// path separators in order to write to file.
		itString savefilename;
		fsFileUtil::LocatorToUnicodeString( i_Info.m_FirstTextureFileName.GetValue(), savefilename );
		chChunkParserUtil::Write( o_Writer, savefilename );

		chChunkParserUtil::Write(o_Writer, i_Info.m_NumberOfFrames );
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
tmlnDriverAnimatedTextureFileNameParser::tmlnDriverAnimatedTextureFileNameParser(chDefs::Name i_ChunkName)
: m_ChunkName(i_ChunkName)
{
}

//----------------------------------------------------------------------------
// Destructor
//----------------------------------------------------------------------------
tmlnDriverAnimatedTextureFileNameParser::~tmlnDriverAnimatedTextureFileNameParser()
{
}

//----------------------------------------------------------------------------
// Returns chunk name used by this parser
//----------------------------------------------------------------------------
//static
chDefs::Name tmlnDriverAnimatedTextureFileNameParser::GetChunkName()
{
	return m_ChunkName;
}

//----------------------------------------------------------------------------
//	Read is called by a parser utility when the chunk name has been read
//  from the data file.  It will parse from the chunk into the given
//  parsable object.
//----------------------------------------------------------------------------
void tmlnDriverAnimatedTextureFileNameParser::Read(	chReader& i_Reader,
										chDefs::Version i_Version,
										chDefs::Size i_Size,
										tmlnDriverInfo& o_Driver ) const
{
	tmlnDriverAnimatedTextureFileNameInfo &driver = dynamic_cast<tmlnDriverAnimatedTextureFileNameInfo &>(o_Driver);

	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;

	while( i_Reader.ReadChunkHeader(name, version, size) )
	{
		if ( name == tmlnDriverInfoParser::GetChunkName() )
		{
			tmlnDriverInfoParser::ReadDriverInfo(i_Reader, version, size, driver);
		}
		else if ( name == c_ATXF )
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
void tmlnDriverAnimatedTextureFileNameParser::Write( chWriter& o_Writer,
										const tmlnDriverInfo& i_Driver ) const
{
	const tmlnDriverAnimatedTextureFileNameInfo &driver = dynamic_cast<const tmlnDriverAnimatedTextureFileNameInfo &>(i_Driver);

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
tmlnDriverInfo* tmlnDriverAnimatedTextureFileNameParser::Create() const
{
	return new tmlnDriverAnimatedTextureFileNameInfo(m_ChunkName);
}


