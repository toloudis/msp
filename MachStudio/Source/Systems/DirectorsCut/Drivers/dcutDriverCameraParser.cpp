/********************************************************************************************\
**  dcutDriverCameraParser.cpp
**
**
**  StudioGPU
**  Copyright(C) 2005 - All Rights Reserved
\********************************************************************************************/
#include "Systems/DirectorsCut/Drivers/dcutDriverCameraParser.hpp"

#include "Systems/DirectorsCut/Drivers/dcutDriverCameraInfo.hpp"

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
	const chDefs::Name c_CAMR = chDefs::MakeName('C', 'A', 'M', 'R');	//dcut camera cut driver
	const chDefs::Name c_CNAM = chDefs::MakeName('C', 'N', 'A', 'M');	//dcut camera cut driver

	//========================================================================
	//   ReadCameraInfo
	//========================================================================
	void ReadCameraInfo(	chReader& i_Reader,
							chDefs::Version i_Version,
							chDefs::Size i_Size	,
							dcutDriverCameraInfo &o_Driver )
	{
		std::string name;
		chChunkParserUtil::Read(i_Reader, name);
		o_Driver.m_CameraName.SetString( name );

		nameUID uid;
		chChunkParserUtil::Read(i_Reader, uid);
		o_Driver.m_CameraName.SetUID( uid );
		
	}

	//========================================================================
	//   WriteCameraInfo
	//========================================================================
	void WriteCameraInfo( chWriter& o_Writer,
						  const dcutDriverCameraInfo &i_Driver )
	{
		const int c_CNAM_version = 0;
		o_Writer.WriteChunkHeader( c_CNAM, c_CNAM_version, false );

		chChunkParserUtil::Write(o_Writer, i_Driver.m_CameraName.GetString());
		chChunkParserUtil::Write(o_Writer, i_Driver.m_CameraName.GetUID());

		o_Writer.FinishChunk();
	}

}	// local namespace


//--------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------
dcutDriverCameraParser::dcutDriverCameraParser()
{
}

//--------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------
dcutDriverCameraParser::~dcutDriverCameraParser()
{
}

//--------------------------------------------------------------------
// Returns chunk name used by this parser
//--------------------------------------------------------------------
//static
chDefs::Name dcutDriverCameraParser::GetChunkName()
{
	return c_CAMR;
}

//--------------------------------------------------------------------
//	Read is called by a parser utility when the chunk name has been read
//  from the data file.  It will parse from the chunk into the given
//  parsable object.
//--------------------------------------------------------------------
void dcutDriverCameraParser::Read(	chReader& i_Reader,
									chDefs::Version i_Version,
									chDefs::Size i_Size,
									tmlnDriverInfo& o_Driver ) const
{
	dcutDriverCameraInfo &driver = dynamic_cast<dcutDriverCameraInfo &>(o_Driver);

	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;

	while( i_Reader.ReadChunkHeader(name, version, size) )
	{
		if ( name == tmlnDriverInfoParser::GetChunkName() )
		{
			tmlnDriverInfoParser::ReadDriverInfo(i_Reader, version, size, driver);
		}
		else if ( name == c_CNAM )
		{
			ReadCameraInfo(i_Reader, version, size, driver );
		}
		i_Reader.FinishChunk();
	}
}

//--------------------------------------------------------------------
//	Write is called to parse data from the given object to the given
//	chWriter.  i_Driver is the object being written.
//--------------------------------------------------------------------
void dcutDriverCameraParser::Write( chWriter& o_Writer,
									 const tmlnDriverInfo& i_Driver ) const
{
	const dcutDriverCameraInfo &driver = dynamic_cast<const dcutDriverCameraInfo &>(i_Driver);

	const int c_CAMR_VERSION = 0;
	o_Writer.WriteChunkHeader( c_CAMR, c_CAMR_VERSION, true );

	tmlnDriverInfoParser::WriteDriverInfo( o_Writer, driver );

	WriteCameraInfo( o_Writer, driver );

	o_Writer.FinishChunk();
}

//--------------------------------------------------------------------
// Create should be overridden to create the user specific parsable object.
// Users of parsers should call create to create the object that is then
// passed to Read.
//--------------------------------------------------------------------
tmlnDriverInfo* dcutDriverCameraParser::Create() const
{
	return new dcutDriverCameraInfo(c_CAMR);
}


