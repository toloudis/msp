/********************************************************************************************\
**  cmraDriverFramedArcParser.cpp
**
**		see .hpp
**
**  StudioGPU
**  Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/
#include "Systems/Cameras/Drivers/cmraDriverFramedArcParser.hpp"

#include "Support/tmln/tmlnDriverInfoParser.hpp"
#include "Systems/Cameras/Drivers/cmraDriverFramedArcInfo.hpp"

#include "Core/ch/chChunkParserUtil.hpp"
#include "Core/ch/chExceptionX.hpp"
#include "Core/ch/chReader.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/gf/gfFileBin.hpp"


//============================================================================
//============================================================================
namespace
{
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	const chDefs::Name c_CDAR = chDefs::MakeName('C', 'D', 'A', 'R');	// main chunk
	const chDefs::Name c_ARCI = chDefs::MakeName('A', 'R', 'C', 'I');	// info chunk

	const int c_CDAR_version = 0;
	const int c_ARCI_version = 0;

	//------------------------------------------------------------------------
	//   ReadArcInfo
	//------------------------------------------------------------------------
	void ReadArcInfo(	chReader& i_Reader,
						chDefs::Version i_Version,
						chDefs::Size i_Size,
						cmraDriverFramedArcInfo &o_Driver )
	{
		chChunkParserUtil::Read(i_Reader, o_Driver.m_fRevolutions);
		chChunkParserUtil::Read(i_Reader, o_Driver.m_bClockwise);
	}

	//------------------------------------------------------------------------
	//   WriteArcInfo
	//------------------------------------------------------------------------
	void WriteArcInfo(	chWriter& o_Writer,
						const cmraDriverFramedArcInfo &i_Driver )
	{

		o_Writer.WriteChunkHeader( c_ARCI, c_ARCI_version, false );

		chChunkParserUtil::Write(o_Writer, i_Driver.m_fRevolutions);
		chChunkParserUtil::Write(o_Writer, i_Driver.m_bClockwise);

		o_Writer.FinishChunk();
	}

}	// local namespace

//--------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------
cmraDriverFramedArcParser::cmraDriverFramedArcParser()
{

}

//--------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------
cmraDriverFramedArcParser::~cmraDriverFramedArcParser()
{

}

//--------------------------------------------------------------------
// Returns chunk name used by this parser
//--------------------------------------------------------------------
//static
chDefs::Name cmraDriverFramedArcParser::GetChunkName()
{
	return c_CDAR;
}

//--------------------------------------------------------------------
//	Read is called by a parser utility when the chunk name has been read
//  from the data file.  It will parse from the chunk into the given
//  parsable object.
//--------------------------------------------------------------------
void cmraDriverFramedArcParser::Read(	chReader& i_Reader,
								chDefs::Version i_Version,
								chDefs::Size i_Size,
								tmlnDriverInfo& o_Driver ) const
{
	cmraDriverFramedArcInfo &driver = dynamic_cast<cmraDriverFramedArcInfo &>(o_Driver);

	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;

	while( i_Reader.ReadChunkHeader(name, version, size) )
	{
		if ( cmraDriverFramedBaseParser::ReadChunk( i_Reader, name, version, size, o_Driver ) )
		{
			//	chunk successfull read in
		}
		else if ( name == c_ARCI )
		{
			ReadArcInfo(i_Reader, version, size, driver);
		}
		i_Reader.FinishChunk();
	}
}

//--------------------------------------------------------------------
//	Write is called to parse data from the given object to the given
//	chWriter.  i_Driver is the object being written.
//--------------------------------------------------------------------
void cmraDriverFramedArcParser::Write( chWriter& o_Writer,
								 const tmlnDriverInfo& i_Driver ) const
{
	const cmraDriverFramedArcInfo &driver = dynamic_cast<const cmraDriverFramedArcInfo &>(i_Driver);

	o_Writer.WriteChunkHeader( c_CDAR, c_CDAR_version, true );

	cmraDriverFramedBaseParser::WriteChunks( o_Writer, i_Driver );

	WriteArcInfo(o_Writer, driver);

	o_Writer.FinishChunk();
}

//--------------------------------------------------------------------
// Create should be overridden to create the user specific parsable object.
// Users of parsers should call create to create the object that is then
// passed to Read.
//--------------------------------------------------------------------
tmlnDriverInfo* cmraDriverFramedArcParser::Create() const
{
	return new cmraDriverFramedArcInfo(c_CDAR);
}


