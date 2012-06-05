/********************************************************************************************\
**  cmraDriverKeyTargetParser.cpp
**
**		see .hpp
**
**  StudioGPU
**  Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/

#include "Systems/Cameras/Drivers/cmraDriverKeyTargetParser.hpp"

#include "Support/tmln/tmlnDriverInfoParser.hpp"
#include "Systems/Cameras/Drivers/cmraDriverKeyTargetInfo.hpp"

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
	const chDefs::Name c_CKEY = chDefs::MakeName('C', 'K', 'Y', 'T');
	const chDefs::Name c_CKYI = chDefs::MakeName('C', 'K', 'Y', 'I');

	const int l_CKEY_VERSION = 0;
	const int l_CKYI_VERSION = 0;

	//------------------------------------------------------------------------
	//   ReadCameraKeyInfo
	//------------------------------------------------------------------------
	void ReadCameraKeyInfo( chReader& i_Reader,
							chDefs::Version i_Version,
							chDefs::Size i_Size,
							cmraDriverKeyTargetInfo &o_Driver )
	{
		chChunkParserUtil::Read(i_Reader, o_Driver.m_CameraKeyTarget);
	}

	//------------------------------------------------------------------------
	//   WriteCameraKeyInfo
	//------------------------------------------------------------------------
	void WriteCameraKeyInfo(chWriter& o_Writer,
							const cmraDriverKeyTargetInfo &i_Driver )
	{
		o_Writer.WriteChunkHeader( c_CKYI, l_CKYI_VERSION, false );

		chChunkParserUtil::Write(o_Writer, i_Driver.m_CameraKeyTarget);

		o_Writer.FinishChunk();
	}

}	// local namespace


//----------------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------------
cmraDriverKeyTargetParser::cmraDriverKeyTargetParser()
{
}

//----------------------------------------------------------------------------
// Destructor
//----------------------------------------------------------------------------
cmraDriverKeyTargetParser::~cmraDriverKeyTargetParser()
{
}

//----------------------------------------------------------------------------
// Returns chunk name used by this parser
//----------------------------------------------------------------------------
//static
chDefs::Name cmraDriverKeyTargetParser::GetChunkName()
{
	return c_CKEY;
}

//----------------------------------------------------------------------------
//	Read is called by a parser utility when the chunk name has been read
//  from the data file.  It will parse from the chunk into the given
//  parsable object.
//----------------------------------------------------------------------------
void cmraDriverKeyTargetParser::Read(	chReader& i_Reader,
										chDefs::Version i_Version,
										chDefs::Size i_Size,
										tmlnDriverInfo& o_Driver ) const
{
	cmraDriverKeyTargetInfo &driver = dynamic_cast<cmraDriverKeyTargetInfo &>(o_Driver);

	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;

	while( i_Reader.ReadChunkHeader(name, version, size) )
	{
		if ( name == tmlnDriverInfoParser::GetChunkName() )
		{
			tmlnDriverInfoParser::ReadDriverInfo(i_Reader, version, size, driver);
		}
		else if ( name == c_CKYI )
		{
			ReadCameraKeyInfo(i_Reader, version, size, driver);
		}
		i_Reader.FinishChunk();
	}
}

//----------------------------------------------------------------------------
//	Write is called to parse data from the given object to the given
//	chWriter.  i_Driver is the object being written.
//----------------------------------------------------------------------------
void cmraDriverKeyTargetParser::Write( chWriter& o_Writer,
										const tmlnDriverInfo& i_Driver ) const
{
	const cmraDriverKeyTargetInfo &driver = dynamic_cast<const cmraDriverKeyTargetInfo &>(i_Driver);

	o_Writer.WriteChunkHeader( c_CKEY, l_CKEY_VERSION, true );
	tmlnDriverInfoParser::WriteDriverInfo(o_Writer, driver);

	WriteCameraKeyInfo(o_Writer, driver);
	o_Writer.FinishChunk();
}

//----------------------------------------------------------------------------
// Create should be overridden to create the user specific parsable object.
// Users of parsers should call create to create the object that is then
// passed to Read.
//----------------------------------------------------------------------------
tmlnDriverInfo* cmraDriverKeyTargetParser::Create() const
{
	return new cmraDriverKeyTargetInfo(c_CKEY);
}


