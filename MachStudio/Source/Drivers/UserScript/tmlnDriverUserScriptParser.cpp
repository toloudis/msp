/********************************************************************************************\
**  tmlnDriverUserScriptParser.cpp
**
**
**  StudioGPU
**  Copyright(C) 2006 - All Rights Reserved
\********************************************************************************************/
#include "Drivers/UserScript/tmlnDriverUserScriptParser.hpp"

#include "Support/tmln/tmlnDriverInfoParser.hpp"
#include "Drivers/UserScript/tmlnDriverUserScriptInfo.hpp"

#include "Core/ch/chReader.hpp"
#include "Core/ch/chWriter.hpp"
#include "Core/ch/chExceptionX.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/gf/gfFileBin.hpp"
#include "Core/ch/chChunkParserUtil.hpp"


namespace
{
	//========================================================================
	//========================================================================
	const chDefs::Name c_USCR = chDefs::MakeName('U', 'S', 'C', 'R');

	//========================================================================
	//   ReadUserScriptInfo
	//========================================================================
	void ReadUserScriptInfo(	chReader& i_Reader,
						chDefs::Version i_Version,
						chDefs::Size i_Size,
						tmlnDriverUserScriptInfo &o_Driver )
	{
		chChunkParserUtil::Read(i_Reader, o_Driver.m_Value);
	}

	//========================================================================
	//   WriteUserScriptInfo
	//========================================================================
	void WriteUserScriptInfo(chWriter& o_Writer,
						const tmlnDriverUserScriptInfo &i_Driver )
	{
		const int lc_USCR_VERSION = 0;
		o_Writer.WriteChunkHeader( c_USCR, lc_USCR_VERSION, false );
		chChunkParserUtil::Write(o_Writer, i_Driver.m_Value);
		o_Writer.FinishChunk();
	}

}	// local namespace


//--------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------
tmlnDriverUserScriptParser::tmlnDriverUserScriptParser(chDefs::Name i_ChunkName)
: m_ChunkName(i_ChunkName)
{

}

//--------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------
tmlnDriverUserScriptParser::~tmlnDriverUserScriptParser()
{

}

//--------------------------------------------------------------------
// Returns chunk name used by this parser
//--------------------------------------------------------------------
chDefs::Name tmlnDriverUserScriptParser::GetChunkName()
{
	return m_ChunkName;
}

//--------------------------------------------------------------------
//	Read is called by a parser utility when the chunk name has been read
//  from the data file.  It will parse from the chunk into the given
//  parsable object.
//--------------------------------------------------------------------
void tmlnDriverUserScriptParser::Read(	chReader& i_Reader,
											chDefs::Version i_Version,
											chDefs::Size i_Size,
											tmlnDriverInfo& o_Driver ) const
{
	tmlnDriverUserScriptInfo &driver = dynamic_cast<tmlnDriverUserScriptInfo &>(o_Driver);

	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;

	while( i_Reader.ReadChunkHeader(name, version, size) )
	{
		if ( name == tmlnDriverInfoParser::GetChunkName() )
		{
			tmlnDriverInfoParser::ReadDriverInfo(i_Reader, version, size, driver);
		}
		else if ( name == c_USCR )
		{
			ReadUserScriptInfo(i_Reader, version, size, driver);
		}
		i_Reader.FinishChunk();
	}
}

//--------------------------------------------------------------------
//	Write is called to parse data from the given object to the given
//	chWriter.  i_Driver is the object being written.
//--------------------------------------------------------------------
void tmlnDriverUserScriptParser::Write(	chWriter& o_Writer,
									const tmlnDriverInfo& i_Driver ) const
{
	const tmlnDriverUserScriptInfo &driver = dynamic_cast<const tmlnDriverUserScriptInfo &>(i_Driver);

	const int lc_USERSCRIPTCHUNK_VERSION = 0;
	o_Writer.WriteChunkHeader( m_ChunkName, lc_USERSCRIPTCHUNK_VERSION, true );

	//	write the general driver info
	tmlnDriverInfoParser::WriteDriverInfo(o_Writer, driver);

	//	write the connect driver info
	WriteUserScriptInfo(o_Writer, driver);

	o_Writer.FinishChunk();
}

//--------------------------------------------------------------------
// Create should be overridden to create the user specific parsable object.
// Users of parsers should call create to create the object that is then
// passed to Read.
//--------------------------------------------------------------------
tmlnDriverInfo* tmlnDriverUserScriptParser::Create() const
{
	return new tmlnDriverUserScriptInfo(m_ChunkName);
}
