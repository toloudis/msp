/********************************************************************************************\
**  cmraDriverCircleParser.cpp
**
**
**  Extra Large Technology
**  Copyright(C) 2007 - All Rights Reserved
\********************************************************************************************/
#include "Systems/Cameras/Drivers/cmraDriverCircleParser.hpp"

#include "Support/tmln/tmlnDriverInfoParser.hpp"
#include "Systems/Cameras/Drivers/cmraDriverCircleInfo.hpp"

#include "Core/ch/chReader.hpp"
#include "Core/ch/chWriter.hpp"
#include "Core/ch/chExceptionX.hpp"
#include "Core/dbg/dbgLog.hpp"
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
	const chDefs::Name c_CRCL = chDefs::MakeName('C', 'R', 'C', 'L');
	const chDefs::Name c_INFO = chDefs::MakeName('I', 'N', 'F', 'O');


	//------------------------------------------------------------------------
	//   ReadInfo
	//------------------------------------------------------------------------
	void ReadInfo(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					cmraDriverCircleInfo &o_Driver )
	{
		chChunkParserUtil::Read(i_Reader, o_Driver.m_Revolutions);
		chChunkParserUtil::Read(i_Reader, o_Driver.m_bClockwise);
		if (i_Version > 0)
		{
			chChunkParserUtil::Read(i_Reader, o_Driver.m_fStartingYaw);
			chChunkParserUtil::Read(i_Reader, o_Driver.m_fStartingPitch);
			chChunkParserUtil::Read(i_Reader, o_Driver.m_fRadius);
		}
	}

	//------------------------------------------------------------------------
	//   WriteInfo
	//------------------------------------------------------------------------
	void WriteInfo(	chWriter& o_Writer,
					const cmraDriverCircleInfo &i_Driver )
	{
		const int lc_INFO_VERSION = 1;
		o_Writer.WriteChunkHeader( c_INFO, lc_INFO_VERSION, false );

		chChunkParserUtil::Write(o_Writer, i_Driver.m_Revolutions);
		chChunkParserUtil::Write(o_Writer, i_Driver.m_bClockwise);
		chChunkParserUtil::Write(o_Writer, i_Driver.m_fStartingYaw);
		chChunkParserUtil::Write(o_Writer, i_Driver.m_fStartingPitch);
		chChunkParserUtil::Write(o_Writer, i_Driver.m_fRadius);

		o_Writer.FinishChunk();
	}

}	// local namespace

//--------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------
cmraDriverCircleParser::cmraDriverCircleParser()
{
}

//--------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------
cmraDriverCircleParser::~cmraDriverCircleParser()
{

}

//--------------------------------------------------------------------
// Returns chunk name used by this parser
//--------------------------------------------------------------------
//static 
chDefs::Name cmraDriverCircleParser::GetChunkName()
{
	return c_CRCL;
}

//--------------------------------------------------------------------
//	Read is called by a parser utility when the chunk name has been read
//  from the data file.  It will parse from the chunk into the given
//  parsable object.
//--------------------------------------------------------------------
void cmraDriverCircleParser::Read(	chReader& i_Reader,
									chDefs::Version i_Version,
									chDefs::Size i_Size,
									tmlnDriverInfo& o_Driver ) const
{
	cmraDriverCircleInfo &driver = dynamic_cast<cmraDriverCircleInfo &>(o_Driver);

	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;

	while( i_Reader.ReadChunkHeader(name, version, size) )
	{
		if ( name == tmlnDriverInfoParser::GetChunkName() )
		{
			tmlnDriverInfoParser::ReadDriverInfo(i_Reader, version, size, driver);
		}
		else if ( name == c_INFO )
		{
			ReadInfo(i_Reader, version, size, driver);
		}
		i_Reader.FinishChunk();
	}
}

//--------------------------------------------------------------------
//	Write is called to parse data from the given object to the given
//	chWriter.  i_Driver is the object being written.
//--------------------------------------------------------------------
void cmraDriverCircleParser::Write( chWriter& o_Writer,
									const tmlnDriverInfo& i_Driver ) const
{
	const cmraDriverCircleInfo &driver = dynamic_cast<const cmraDriverCircleInfo &>(i_Driver);

	const int lc_MAINCHUNK_VERSION = 0;
	o_Writer.WriteChunkHeader( GetChunkName(), lc_MAINCHUNK_VERSION, true );

	tmlnDriverInfoParser::WriteDriverInfo(o_Writer, driver);

	WriteInfo(o_Writer, driver);

	o_Writer.FinishChunk();
}

//--------------------------------------------------------------------
// Create should be overridden to create the user specific parsable object.
// Users of parsers should call create to create the object that is then
// passed to Read.
//--------------------------------------------------------------------
tmlnDriverInfo* cmraDriverCircleParser::Create() const
{
	return new cmraDriverCircleInfo(GetChunkName());
}


