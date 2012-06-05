/********************************************************************************************\
**  cmraDriverFramedCloseUpParser.cpp
**
**		see .hpp
**
**  StudioGPU
**  Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/

#include "Systems/Cameras/Drivers/cmraDriverFramedCloseUpParser.hpp"

#include "Support/tmln/tmlnDriverInfoParser.hpp"
#include "Systems/Cameras/Drivers/cmraDriverFramedBaseInfo.hpp"

#include "Core/ch/chReader.hpp"
#include "Core/ch/chWriter.hpp"
#include "Core/ch/chExceptionX.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/gf/gfFileBin.hpp"
#include "Core/ch/chChunkParserUtil.hpp"


//============================================================================
//============================================================================
namespace
{
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	const chDefs::Name c_CDCU = chDefs::MakeName('C', 'D', 'C', 'U');	// main chunk
	const chDefs::Name c_CUPI = chDefs::MakeName('C', 'U', 'P', 'I');	// info chunk

	const int c_CDCU_version	= 0;
	const int c_CUPI_version	= 0;

	//------------------------------------------------------------------------
	//   ReadFramedCloseUpInfo
	//------------------------------------------------------------------------
	void ReadFramedCloseUpInfo(	chReader& i_Reader,
								chDefs::Version i_Version,
								chDefs::Size i_Size,
								cmraDriverFramedBaseInfo &o_Driver )
	{
		//chChunkParserUtil::Read(i_Reader, o_Driver.m_bPositionRestrict);
	}

	//------------------------------------------------------------------------
	//   WriteFramedCloseUpInfo
	//------------------------------------------------------------------------
	void WriteFramedCloseUpInfo(	chWriter& o_Writer,
									const cmraDriverFramedBaseInfo &i_Driver )
	{
		o_Writer.WriteChunkHeader( c_CUPI, c_CUPI_version, false );

		//chChunkParserUtil::Write(o_Writer, i_Driver.m_bPositionRestrict);

		o_Writer.FinishChunk();
	}

}	// local namespace

//--------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------
cmraDriverFramedCloseUpParser::cmraDriverFramedCloseUpParser()
{

}

//--------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------
cmraDriverFramedCloseUpParser::~cmraDriverFramedCloseUpParser()
{

}

//--------------------------------------------------------------------
// Returns chunk name used by this parser
//--------------------------------------------------------------------
//static
chDefs::Name cmraDriverFramedCloseUpParser::GetChunkName()
{
	return c_CDCU;
}

//--------------------------------------------------------------------
//	Read is called by a parser utility when the chunk name has been read
//  from the data file.  It will parse from the chunk into the given
//  parsable object.
//--------------------------------------------------------------------
void cmraDriverFramedCloseUpParser::Read(	chReader& i_Reader,
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
		else if ( name == c_CUPI )
		{
			ReadFramedCloseUpInfo( i_Reader, version, size, driver );
		}

		i_Reader.FinishChunk();
	}
}

//--------------------------------------------------------------------
//	Write is called to parse data from the given object to the given
//	chWriter.  i_Driver is the object being written.
//--------------------------------------------------------------------
void cmraDriverFramedCloseUpParser::Write( chWriter& o_Writer,
									 const tmlnDriverInfo& i_Driver ) const
{
	const cmraDriverFramedBaseInfo &driver = dynamic_cast<const cmraDriverFramedBaseInfo &>(i_Driver);

	o_Writer.WriteChunkHeader( c_CDCU, c_CDCU_version, true );

	cmraDriverFramedBaseParser::WriteChunks( o_Writer, i_Driver );

	WriteFramedCloseUpInfo(o_Writer, driver);

	o_Writer.FinishChunk();
}

//--------------------------------------------------------------------
// Create should be overridden to create the user specific parsable object.
// Users of parsers should call create to create the object that is then
// passed to Read.
//--------------------------------------------------------------------
tmlnDriverInfo* cmraDriverFramedCloseUpParser::Create() const
{
	return new cmraDriverFramedBaseInfo(c_CDCU);
}


