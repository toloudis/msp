/********************************************************************************************\
**  tmlnDriverFilePathParser.cpp
**
**
**  StudioGPU
**  Copyright(C) 2003 - All Rights Reserved
\********************************************************************************************/

#include "Drivers/FilePath/tmlnDriverFilePathParser.hpp"

#include "Support/tmln/tmlnDriverInfoParser.hpp"
#include "Drivers/FilePath/tmlnDriverFilePathInfo.hpp"

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
	const chDefs::Name c_FNME = chDefs::MakeName('F', 'N', 'M', 'E');


	//========================================================================
	//   ReadFilePathInfo
	//========================================================================
	void ReadFilePathInfo(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					tmlnDriverFilePathInfo &o_Driver )
	{
		//chChunkParserUtil::Read(i_Reader, o_Driver.m_Value);
			
		// Read in a single itString with path separators and then 
		// split into locator
		itString savefilename;
		chChunkParserUtil::Read( i_Reader, savefilename );
		fsFileUtil::UnicodeStringToLocator( savefilename, o_Driver.m_Value );
	}

	//========================================================================
	//   WriteFilePathInfo
	//========================================================================
	void WriteFilePathInfo(	chWriter& o_Writer,
					const tmlnDriverFilePathInfo &i_Driver )
	{
		// Upping to version 1 when using full locator within a single itString
		// although version 0 was also just a single itString for a single filename.
		o_Writer.WriteChunkHeader( c_FNME, 1, false );
		//chChunkParserUtil::Write(o_Writer, i_Driver.m_Value);

		// Convert array of itStrings to a single itString with
		// path separators in order to write to file.
		itString savefilename;
		fsFileUtil::LocatorToUnicodeString( i_Driver.m_Value, savefilename );
		chChunkParserUtil::Write( o_Writer, savefilename );

		o_Writer.FinishChunk();
	}

}	// local namespace

//--------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------
tmlnDriverFilePathParser::tmlnDriverFilePathParser(chDefs::Name i_ChunkName)
: m_ChunkName(i_ChunkName)
{

}

//--------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------
tmlnDriverFilePathParser::~tmlnDriverFilePathParser()
{

}

//--------------------------------------------------------------------
// Returns chunk name used by this parser
//--------------------------------------------------------------------
chDefs::Name tmlnDriverFilePathParser::GetChunkName()
{
	return m_ChunkName;
}

//--------------------------------------------------------------------
//	Read is called by a parser utility when the chunk name has been read
//  from the data file.  It will parse from the chunk into the given
//  parsable object.
//--------------------------------------------------------------------
void tmlnDriverFilePathParser::Read(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					tmlnDriverInfo& o_Driver ) const
{
	tmlnDriverFilePathInfo &driver = dynamic_cast<tmlnDriverFilePathInfo &>(o_Driver);

	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;

	while( i_Reader.ReadChunkHeader(name, version, size) )
	{
		if ( name == tmlnDriverInfoParser::GetChunkName() )
		{
			tmlnDriverInfoParser::ReadDriverInfo(i_Reader, version, size, driver);
		}
		else if ( name == c_FNME )
		{
			ReadFilePathInfo(i_Reader, version, size, driver);
		}
		i_Reader.FinishChunk();
	}
}

//--------------------------------------------------------------------
//	Write is called to parse data from the given object to the given
//	chWriter.  i_Driver is the object being written.
//--------------------------------------------------------------------
void tmlnDriverFilePathParser::Write( chWriter& o_Writer,
					const tmlnDriverInfo& i_Driver ) const
{
	const tmlnDriverFilePathInfo &driver = dynamic_cast<const tmlnDriverFilePathInfo &>(i_Driver);

	o_Writer.WriteChunkHeader( m_ChunkName, 0, true );
	tmlnDriverInfoParser::WriteDriverInfo(o_Writer, driver);

	WriteFilePathInfo(o_Writer, driver);
	o_Writer.FinishChunk();
}

//--------------------------------------------------------------------
// Create should be overridden to create the user specific parsable object.
// Users of parsers should call create to create the object that is then
// passed to Read.
//--------------------------------------------------------------------
tmlnDriverInfo* tmlnDriverFilePathParser::Create() const
{
	return new tmlnDriverFilePathInfo(m_ChunkName);
}


