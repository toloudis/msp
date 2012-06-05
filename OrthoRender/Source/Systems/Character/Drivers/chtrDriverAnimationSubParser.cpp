/********************************************************************************************\
**  chtrDriverAnimationSubParser.cpp
**
**
**  Extra Large Technology
**  Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/

#include "Systems/Character/Drivers/chtrDriverAnimationSubParser.hpp"

#include "Systems/Character/Drivers/chtrDriverAnimationSubInfo.hpp"

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
	const chDefs::Name c_CDAS = chDefs::MakeName('C', 'D', 'A', 'S');	//prop drive anim sub
	const chDefs::Name c_CDAD = chDefs::MakeName('C', 'D', 'A', 'D');	//prop driver anim sub data

	//========================================================================
	//   ReadAnimSubInfo
	//========================================================================
	void ReadAnimSubInfo(	chReader& i_Reader,
							chDefs::Version i_Version,
							chDefs::Size i_Size,
							chtrAnimationSubInfo& o_Info )
	{
		chChunkParserUtil::Read(i_Reader, o_Info.m_AnimFilename );
		chChunkParserUtil::Read(i_Reader, o_Info.m_bDriverToAnimLength );

		chChunkParserUtil::Read(i_Reader, o_Info.m_AnimName );
		chChunkParserUtil::Read(i_Reader, o_Info.m_FrameRate );
		chChunkParserUtil::Read(i_Reader, o_Info.m_StartFrame );
		chChunkParserUtil::Read(i_Reader, o_Info.m_EndFrame );
		chChunkParserUtil::Read(i_Reader, o_Info.m_bLooping );
	}

	//========================================================================
	//   WriteAnimSubInfo
	//========================================================================
	void WriteAnimSubInfo(	chWriter& o_Writer,
							const chtrAnimationSubInfo& i_Info )
	{
		o_Writer.WriteChunkHeader( c_CDAD, 0, false );

		chChunkParserUtil::Write(o_Writer, i_Info.m_AnimFilename );
		chChunkParserUtil::Write(o_Writer, i_Info.m_bDriverToAnimLength );

		chChunkParserUtil::Write(o_Writer, i_Info.m_AnimName );
		chChunkParserUtil::Write(o_Writer, i_Info.m_FrameRate );
		chChunkParserUtil::Write(o_Writer, i_Info.m_StartFrame );
		chChunkParserUtil::Write(o_Writer, i_Info.m_EndFrame );
		chChunkParserUtil::Write(o_Writer, i_Info.m_bLooping );

		o_Writer.FinishChunk();
	}

}	// local namespace

//--------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------
chtrDriverAnimationSubParser::chtrDriverAnimationSubParser()
{

}

//--------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------
chtrDriverAnimationSubParser::~chtrDriverAnimationSubParser()
{

}

//--------------------------------------------------------------------
// Returns chunk name used by this parser
//--------------------------------------------------------------------
//static
chDefs::Name chtrDriverAnimationSubParser::GetChunkName()
{
	return c_CDAS;
}

//--------------------------------------------------------------------
//	Read is called by a parser utility when the chunk name has been read
//  from the data file.  It will parse from the chunk into the given
//  parsable object.
//--------------------------------------------------------------------
void chtrDriverAnimationSubParser::Read(	chReader& i_Reader,
											chDefs::Version i_Version,
											chDefs::Size i_Size,
											tmlnDriverInfo& o_Driver ) const
{
	chtrDriverAnimationSubInfo &driver = dynamic_cast<chtrDriverAnimationSubInfo &>(o_Driver);

	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;

	while( i_Reader.ReadChunkHeader(name, version, size) )
	{
		if ( name == tmlnDriverInfoParser::GetChunkName() )
		{
			tmlnDriverInfoParser::ReadDriverInfo(i_Reader, version, size, driver);
		}
		else if ( name == c_CDAD )
		{
			ReadAnimSubInfo(i_Reader, version, size, driver.m_Info );
		}
		i_Reader.FinishChunk();
	}
}

//--------------------------------------------------------------------
//	Write is called to parse data from the given object to the given
//	chWriter.  i_Driver is the object being written.
//--------------------------------------------------------------------
void chtrDriverAnimationSubParser::Write( chWriter& o_Writer,
					const tmlnDriverInfo& i_Driver ) const
{
	const chtrDriverAnimationSubInfo &driver = dynamic_cast<const chtrDriverAnimationSubInfo &>(i_Driver);

	o_Writer.WriteChunkHeader( c_CDAS, 0, true );
	tmlnDriverInfoParser::WriteDriverInfo(o_Writer, driver);

	WriteAnimSubInfo( o_Writer, driver.m_Info );

	o_Writer.FinishChunk();
}

//--------------------------------------------------------------------
// Create should be overridden to create the user specific parsable object.
// Users of parsers should call create to create the object that is then
// passed to Read.
//--------------------------------------------------------------------
tmlnDriverInfo* chtrDriverAnimationSubParser::Create() const
{
	return new chtrDriverAnimationSubInfo(c_CDAS);
}


