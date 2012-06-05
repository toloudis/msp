/********************************************************************************************\
**  dcutDriverCaptureParser.cpp
**
**
**  Extra Large Technology
**  Copyright(C) 2005 - All Rights Reserved
\********************************************************************************************/
#include "Systems/DirectorsCut/Drivers/dcutDriverCaptureParser.hpp"

#include "Systems/DirectorsCut/Drivers/dcutDriverCaptureInfo.hpp"

#include "Support/tmln/tmlnDriverInfoParser.hpp"

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
	//========================================================================
	//========================================================================
	const chDefs::Name c_DCUT = chDefs::MakeName('D', 'C', 'U', 'T');	//dcut capture driver

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
	//	const int c_DCUT_version = 0;
	//	o_Writer.WriteChunkHeader( c_PDED, c_PDED_version, false );

	//	chChunkParserUtil::Write(o_Writer, 0 );

	//	o_Writer.FinishChunk();
	//}

}	// local namespace

//--------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------
dcutDriverCaptureParser::dcutDriverCaptureParser()
{
}

//--------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------
dcutDriverCaptureParser::~dcutDriverCaptureParser()
{
}

//--------------------------------------------------------------------
// Returns chunk name used by this parser
//--------------------------------------------------------------------
//static
chDefs::Name dcutDriverCaptureParser::GetChunkName()
{
	return c_DCUT;
}

//--------------------------------------------------------------------
//	Read is called by a parser utility when the chunk name has been read
//  from the data file.  It will parse from the chunk into the given
//  parsable object.
//--------------------------------------------------------------------
void dcutDriverCaptureParser::Read(	chReader& i_Reader,
									chDefs::Version i_Version,
									chDefs::Size i_Size,
									tmlnDriverInfo& o_Driver ) const
{
	dcutDriverCaptureInfo &driver = dynamic_cast<dcutDriverCaptureInfo &>(o_Driver);

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
void dcutDriverCaptureParser::Write( chWriter& o_Writer,
									 const tmlnDriverInfo& i_Driver ) const
{
	const dcutDriverCaptureInfo &driver = dynamic_cast<const dcutDriverCaptureInfo &>(i_Driver);

	const int c_DCUT_VERSION = 0;
	o_Writer.WriteChunkHeader( c_DCUT, c_DCUT_VERSION, true );

	tmlnDriverInfoParser::WriteDriverInfo( o_Writer, driver );

	//WriteCaptureInfo( o_Writer );

	o_Writer.FinishChunk();
}

//--------------------------------------------------------------------
// Create should be overridden to create the user specific parsable object.
// Users of parsers should call create to create the object that is then
// passed to Read.
//--------------------------------------------------------------------
tmlnDriverInfo* dcutDriverCaptureParser::Create() const
{
	return new dcutDriverCaptureInfo(c_DCUT);
}


