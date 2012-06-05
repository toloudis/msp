/********************************************************************************************\
**  cmraDriverFramedMedLongParser.cpp
**
**		see .hpp
**
**  Extra Large Technology
**  Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/

#include "Systems/Cameras/Drivers/cmraDriverFramedMedLongParser.hpp"

#include "Support/tmln/tmlnDriverInfoParser.hpp"
#include "Systems/Cameras/Drivers/cmraDriverFramedBaseInfo.hpp"

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
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	const chDefs::Name c_CDML = chDefs::MakeName('C', 'D', 'M', 'L');	// main chunk
	const chDefs::Name c_MELI = chDefs::MakeName('M', 'E', 'L', 'I');	// info chunk

	const int c_CDML_version	= 0;
	const int c_MELI_version	= 0;

	//------------------------------------------------------------------------
	//   ReadFramedMedLongInfo
	//------------------------------------------------------------------------
	void ReadFramedMedLongInfo(	chReader& i_Reader,
								chDefs::Version i_Version,
								chDefs::Size i_Size,
								cmraDriverFramedBaseInfo &o_Driver )
	{
		//chChunkParserUtil::Read(i_Reader, o_Driver.m_bPositionRestrict);
	}

	//------------------------------------------------------------------------
	//   WriteFramedMedLongInfo
	//------------------------------------------------------------------------
	void WriteFramedMedLongInfo(	chWriter& o_Writer,
									const cmraDriverFramedBaseInfo &i_Driver )
	{
		o_Writer.WriteChunkHeader( c_MELI, c_MELI_version, false );

		//chChunkParserUtil::Write(o_Writer, i_Driver.m_bPositionRestrict);

		o_Writer.FinishChunk();
	}

}	// local namespace

//--------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------
cmraDriverFramedMedLongParser::cmraDriverFramedMedLongParser()
{

}

//--------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------
cmraDriverFramedMedLongParser::~cmraDriverFramedMedLongParser()
{

}

//--------------------------------------------------------------------
// Returns chunk name used by this parser
//--------------------------------------------------------------------
//static
chDefs::Name cmraDriverFramedMedLongParser::GetChunkName()
{
	return c_CDML;
}

//--------------------------------------------------------------------
//	Read is called by a parser utility when the chunk name has been read
//  from the data file.  It will parse from the chunk into the given
//  parsable object.
//--------------------------------------------------------------------
void cmraDriverFramedMedLongParser::Read(	chReader& i_Reader,
											chDefs::Version i_Version,
											chDefs::Size i_Size,
											tmlnDriverInfo& o_Driver ) const
{
	cmraDriverFramedBaseInfo &driver = dynamic_cast<cmraDriverFramedBaseInfo &>(o_Driver);

	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;

	while( i_Reader.ReadChunkHeader(name, version, size) )
	{
		if ( cmraDriverFramedBaseParser::ReadChunk( i_Reader, name, version, size, o_Driver ) )
		{
			//	chunk successfull read in
		}
		else if ( name == c_MELI )
		{
			ReadFramedMedLongInfo( i_Reader, version, size, driver );
		}

		i_Reader.FinishChunk();
	}
}

//--------------------------------------------------------------------
//	Write is called to parse data from the given object to the given
//	chWriter.  i_Driver is the object being written.
//--------------------------------------------------------------------
void cmraDriverFramedMedLongParser::Write( chWriter& o_Writer,
									 const tmlnDriverInfo& i_Driver ) const
{
	const cmraDriverFramedBaseInfo &driver = dynamic_cast<const cmraDriverFramedBaseInfo &>(i_Driver);

	o_Writer.WriteChunkHeader( c_CDML, c_CDML_version, true );

	cmraDriverFramedBaseParser::WriteChunks( o_Writer, i_Driver );

	WriteFramedMedLongInfo(o_Writer, driver);

	o_Writer.FinishChunk();
}

//--------------------------------------------------------------------
// Create should be overridden to create the user specific parsable object.
// Users of parsers should call create to create the object that is then
// passed to Read.
//--------------------------------------------------------------------
tmlnDriverInfo* cmraDriverFramedMedLongParser::Create() const
{
	return new cmraDriverFramedBaseInfo(c_CDML);
}


