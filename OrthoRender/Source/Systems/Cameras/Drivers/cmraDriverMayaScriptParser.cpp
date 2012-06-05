/********************************************************************************************\
**  cmraDriverMayaScriptParser.cpp
**
**		see .hpp
**
**  Extra Large Technology
**  Copyright(C) 2005 - All Rights Reserved
\********************************************************************************************/

#include "Systems/Cameras/Drivers/cmraDriverMayaScriptParser.hpp"

#include "Support/tmln/tmlnDriverInfoParser.hpp"
#include "Systems/Cameras/Drivers/cmraDriverMayaScriptInfo.hpp"

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
	const chDefs::Name c_CMAY = chDefs::MakeName('C', 'M', 'A', 'Y');
	const chDefs::Name c_CMYI = chDefs::MakeName('C', 'M', 'Y', 'I');

	const int l_CMAY_VERSION = 0;

	//------------------------------------------------------------------------
	//   ReadScriptInfo
	//------------------------------------------------------------------------
	void ReadScriptInfo( chReader& i_Reader,
							chDefs::Version i_Version,
							chDefs::Size i_Size,
							cmraDriverMayaScriptInfo &o_Info )
	{
		chChunkParserUtil::Read(i_Reader, o_Info.m_AnimFilename);
		chChunkParserUtil::Read(i_Reader, o_Info.m_FrameRate);
		chChunkParserUtil::Read(i_Reader, o_Info.m_StartFrame );
		chChunkParserUtil::Read(i_Reader, o_Info.m_EndFrame );
		chChunkParserUtil::Read(i_Reader, o_Info.m_bLooping );
		chChunkParserUtil::Read(i_Reader, o_Info.m_bDriverToAnimLength );

		if (i_Version >= 1)
		{
			chChunkParserUtil::Read(i_Reader, o_Info.m_Offset );
		}
		if (i_Version >= 2)
			chChunkParserUtil::Read(i_Reader, o_Info.m_CutThreshold );
		else
			o_Info.m_CutThreshold = 1.0f; // previous threshold was hardcoded at 1.0 in code

		if (i_Version >= 3)
		{
			chChunkParserUtil::Read(i_Reader, o_Info.m_bUseAnimStart );
		}
	}

	//------------------------------------------------------------------------
	//   WriteScriptInfo
	//------------------------------------------------------------------------
	void WriteScriptInfo(chWriter& o_Writer,
							const cmraDriverMayaScriptInfo &i_Info )
	{
		const int l_CMYI_VERSION = 3;
		o_Writer.WriteChunkHeader( c_CMYI, l_CMYI_VERSION, false );

		chChunkParserUtil::Write(o_Writer, i_Info.m_AnimFilename);
		chChunkParserUtil::Write(o_Writer, i_Info.m_FrameRate);
		chChunkParserUtil::Write(o_Writer, i_Info.m_StartFrame );
		chChunkParserUtil::Write(o_Writer, i_Info.m_EndFrame );
		chChunkParserUtil::Write(o_Writer, i_Info.m_bLooping );
		chChunkParserUtil::Write(o_Writer, i_Info.m_bDriverToAnimLength );

		// version 1 adds offset position
		chChunkParserUtil::Write(o_Writer, i_Info.m_Offset );

		// version 2 adds cut threshold
		chChunkParserUtil::Write(o_Writer, i_Info.m_CutThreshold );

		// version 3 adds m_bUseAnimStart
		chChunkParserUtil::Write(o_Writer, i_Info.m_bUseAnimStart );

		o_Writer.FinishChunk();
	}

}	// local namespace


//----------------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------------
cmraDriverMayaScriptParser::cmraDriverMayaScriptParser()
{
}

//----------------------------------------------------------------------------
// Destructor
//----------------------------------------------------------------------------
cmraDriverMayaScriptParser::~cmraDriverMayaScriptParser()
{
}

//----------------------------------------------------------------------------
// Returns chunk name used by this parser
//----------------------------------------------------------------------------
//static
chDefs::Name cmraDriverMayaScriptParser::GetChunkName()
{
	return c_CMAY;
}

//----------------------------------------------------------------------------
//	Read is called by a parser utility when the chunk name has been read
//  from the data file.  It will parse from the chunk into the given
//  parsable object.
//----------------------------------------------------------------------------
void cmraDriverMayaScriptParser::Read(	chReader& i_Reader,
										chDefs::Version i_Version,
										chDefs::Size i_Size,
										tmlnDriverInfo& o_Driver ) const
{
	cmraDriverMayaScriptInfo &driver = dynamic_cast<cmraDriverMayaScriptInfo &>(o_Driver);

	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;

	while( i_Reader.ReadChunkHeader(name, version, size) )
	{
		if ( name == tmlnDriverInfoParser::GetChunkName() )
		{
			tmlnDriverInfoParser::ReadDriverInfo(i_Reader, version, size, driver);
		}
		else if ( name == c_CMYI )
		{
			ReadScriptInfo(i_Reader, version, size, driver);
		}
		i_Reader.FinishChunk();
	}
}

//----------------------------------------------------------------------------
//	Write is called to parse data from the given object to the given
//	chWriter.  i_Driver is the object being written.
//----------------------------------------------------------------------------
void cmraDriverMayaScriptParser::Write( chWriter& o_Writer,
										const tmlnDriverInfo& i_Driver ) const
{
	const cmraDriverMayaScriptInfo &driver = dynamic_cast<const cmraDriverMayaScriptInfo &>(i_Driver);

	o_Writer.WriteChunkHeader( c_CMAY, l_CMAY_VERSION, true );
	tmlnDriverInfoParser::WriteDriverInfo(o_Writer, driver);

	WriteScriptInfo(o_Writer, driver);
	o_Writer.FinishChunk();
}

//----------------------------------------------------------------------------
// Create should be overridden to create the user specific parsable object.
// Users of parsers should call create to create the object that is then
// passed to Read.
//----------------------------------------------------------------------------
tmlnDriverInfo* cmraDriverMayaScriptParser::Create() const
{
	return new cmraDriverMayaScriptInfo(c_CMAY);
}


