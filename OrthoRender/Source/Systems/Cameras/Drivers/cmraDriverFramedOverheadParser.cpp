/********************************************************************************************\
**  cmraDriverFramedOverheadParser.cpp
**
**		see .hpp
**
**  Extra Large Technology
**  Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/

#include "Systems/Cameras/Drivers/cmraDriverFramedOverheadParser.hpp"

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
	const chDefs::Name c_CDOH = chDefs::MakeName('C', 'D', 'O', 'H');	// main chunk
	const chDefs::Name c_OVRI = chDefs::MakeName('O', 'V', 'R', 'I');	// info chunk

	const int c_CDOH_version	= 0;
	const int c_OVRI_version	= 0;

	//------------------------------------------------------------------------
	//   ReadFramedOverheadInfo
	//------------------------------------------------------------------------
	void ReadFramedOverheadInfo(	chReader& i_Reader,
								chDefs::Version i_Version,
								chDefs::Size i_Size,
								cmraDriverFramedBaseInfo &o_Driver )
	{
		//chChunkParserUtil::Read(i_Reader, o_Driver.m_bPositionRestrict);
	}

	//------------------------------------------------------------------------
	//   WriteFramedOverheadInfo
	//------------------------------------------------------------------------
	void WriteFramedOverheadInfo(	chWriter& o_Writer,
									const cmraDriverFramedBaseInfo &i_Driver )
	{
		o_Writer.WriteChunkHeader( c_OVRI, c_OVRI_version, false );

		//chChunkParserUtil::Write(o_Writer, i_Driver.m_bPositionRestrict);

		o_Writer.FinishChunk();
	}

}	// local namespace

//--------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------
cmraDriverFramedOverheadParser::cmraDriverFramedOverheadParser()
{

}

//--------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------
cmraDriverFramedOverheadParser::~cmraDriverFramedOverheadParser()
{

}

//--------------------------------------------------------------------
// Returns chunk name used by this parser
//--------------------------------------------------------------------
//static
chDefs::Name cmraDriverFramedOverheadParser::GetChunkName()
{
	return c_CDOH;
}

//--------------------------------------------------------------------
//	Read is called by a parser utility when the chunk name has been read
//  from the data file.  It will parse from the chunk into the given
//  parsable object.
//--------------------------------------------------------------------
void cmraDriverFramedOverheadParser::Read(	chReader& i_Reader,
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
		else if ( name == c_OVRI )
		{
			ReadFramedOverheadInfo( i_Reader, version, size, driver );
		}

		i_Reader.FinishChunk();
	}
}

//--------------------------------------------------------------------
//	Write is called to parse data from the given object to the given
//	chWriter.  i_Driver is the object being written.
//--------------------------------------------------------------------
void cmraDriverFramedOverheadParser::Write( chWriter& o_Writer,
									 const tmlnDriverInfo& i_Driver ) const
{
	const cmraDriverFramedBaseInfo &driver = dynamic_cast<const cmraDriverFramedBaseInfo &>(i_Driver);

	o_Writer.WriteChunkHeader( c_CDOH, c_CDOH_version, true );

	cmraDriverFramedBaseParser::WriteChunks( o_Writer, i_Driver );

	WriteFramedOverheadInfo(o_Writer, driver);

	o_Writer.FinishChunk();
}

//--------------------------------------------------------------------
// Create should be overridden to create the user specific parsable object.
// Users of parsers should call create to create the object that is then
// passed to Read.
//--------------------------------------------------------------------
tmlnDriverInfo* cmraDriverFramedOverheadParser::Create() const
{
	return new cmraDriverFramedBaseInfo(c_CDOH);
}


