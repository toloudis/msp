/********************************************************************************************\
**  cmraDriverCaptureParser.cpp
**
**
**  StudioGPU
**  Copyright(C) 2005 - All Rights Reserved
\********************************************************************************************/
#include "Systems/Cameras/Drivers/cmraDriverCaptureParser.hpp"

#include "Systems/Cameras/Drivers/cmraDriverCaptureInfo.hpp"

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
	//========================================================================
	//========================================================================
	const chDefs::Name c_CAPD = chDefs::MakeName('C', 'A', 'P', 'D');	//cmra capture driver

	////========================================================================
	////   ReadCaptureInfo
	////========================================================================
	//void ReadCaptureInfo(	chReader& i_Reader,
	//						chDefs::Version i_Version,
	//						chDefs::Size i_Size			 )
	//{
	//	int junk = 0;
	//	chChunkParserUtil::Read(i_Reader, junk);
	//}

	////========================================================================
	////   WriteCaptureInfo
	////========================================================================
	//void WriteCaptureInfo(	chWriter& o_Writer )
	//{
	//	const int c_CAPD_version = 0;
	//	o_Writer.WriteChunkHeader( c_PDED, c_PDED_version, false );

	//	chChunkParserUtil::Write(o_Writer, 0 );

	//	o_Writer.FinishChunk();
	//}

}	// local namespace

//--------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------
cmraDriverCaptureParser::cmraDriverCaptureParser()
{
}

//--------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------
cmraDriverCaptureParser::~cmraDriverCaptureParser()
{
}

//--------------------------------------------------------------------
// Returns chunk name used by this parser
//--------------------------------------------------------------------
//static
chDefs::Name cmraDriverCaptureParser::GetChunkName()
{
	return c_CAPD;
}

//--------------------------------------------------------------------
//	Read is called by a parser utility when the chunk name has been read
//  from the data file.  It will parse from the chunk into the given
//  parsable object.
//--------------------------------------------------------------------
void cmraDriverCaptureParser::Read(	chReader& i_Reader,
									chDefs::Version i_Version,
									chDefs::Size i_Size,
									tmlnDriverInfo& o_Driver ) const
{
	cmraDriverCaptureInfo &driver = dynamic_cast<cmraDriverCaptureInfo &>(o_Driver);

	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;

	while( i_Reader.ReadChunkHeader(name, version, size) )
	{
		if ( name == tmlnDriverInfoParser::GetChunkName() )
		{
			tmlnDriverInfoParser::ReadDriverInfo(i_Reader, version, size, driver);
		}
		//else if ( name == c_PDED )
		//{
		//	ReadCaptureInfo(i_Reader, version, size );
		//}
		i_Reader.FinishChunk();
	}
}

//--------------------------------------------------------------------
//	Write is called to parse data from the given object to the given
//	chWriter.  i_Driver is the object being written.
//--------------------------------------------------------------------
void cmraDriverCaptureParser::Write( chWriter& o_Writer,
									 const tmlnDriverInfo& i_Driver ) const
{
	const cmraDriverCaptureInfo &driver = dynamic_cast<const cmraDriverCaptureInfo &>(i_Driver);

	const int c_CAPD_VERSION = 0;
	o_Writer.WriteChunkHeader( c_CAPD, c_CAPD_VERSION, true );

	tmlnDriverInfoParser::WriteDriverInfo( o_Writer, driver );

	//WriteCaptureInfo( o_Writer );

	o_Writer.FinishChunk();
}

//--------------------------------------------------------------------
// Create should be overridden to create the user specific parsable object.
// Users of parsers should call create to create the object that is then
// passed to Read.
//--------------------------------------------------------------------
tmlnDriverInfo* cmraDriverCaptureParser::Create() const
{
	return new cmraDriverCaptureInfo(c_CAPD);
}


