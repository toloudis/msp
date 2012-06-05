/********************************************************************************************\
**  propDriverAnimationSubParser.cpp
**
**
**  Extra Large Technology
**  Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/

#include "Systems/Props/Drivers/propDriverAnimationSubParser.hpp"

#include "Systems/Props/Drivers/propDriverAnimationSubInfo.hpp"

#include "Support/tmln/tmlnDriverInfoParser.hpp"

#include "Core/Ch/chReader.hpp"
#include "Core/Ch/chWriter.hpp"
#include "Core/Ch/chExceptionX.hpp"
#include "Core/Dbg/dbgLog.hpp"
#include "Core/Fs/fsFileUtil.hpp"
#include "Core/Fs/fsFileX.hpp"
#include "Core/Gf/gfFileBin.hpp"
#include "Core/Ch/chChunkParserUtil.hpp"


namespace
{
	//========================================================================
	//========================================================================
	const chDefs::Name c_PDAS = chDefs::MakeName('P', 'D', 'A', 'S');	//prop drive anim sub
	const chDefs::Name c_PDAD = chDefs::MakeName('P', 'D', 'A', 'D');	//prop driver anim sub data

	//========================================================================
	//   ReadAnimFullInfo
	//========================================================================
	void ReadAnimFullInfo(	chReader& i_Reader,
							chDefs::Version i_Version,
							chDefs::Size i_Size,
							int& o_AnimIndex,
							itString& o_AnimName,
							bool o_bDriverResizeByAnimLength )
	{
		int anim_index = 0;
		chChunkParserUtil::Read(i_Reader, anim_index);
		o_AnimIndex = anim_index;

		chChunkParserUtil::Read(i_Reader, o_AnimName);
		//o_AnimName = anim_name.c_str();

		int driver_resize = 0;
		chChunkParserUtil::Read(i_Reader, driver_resize);
		o_bDriverResizeByAnimLength = ( (driver_resize > 0) ? true : false);
	}

	//========================================================================
	//   WriteAnimFullInfo
	//========================================================================
	void WriteAnimFullInfo(	chWriter& o_Writer,
							const int i_AnimIndex,
							const itString& i_AnimName,
							const bool i_bDriverResizeByAnimLength )
	{
		o_Writer.WriteChunkHeader( c_PDAD, 0, false );

		chChunkParserUtil::Write(o_Writer, i_AnimIndex );
		chChunkParserUtil::Write(o_Writer, i_AnimName );
		chChunkParserUtil::Write(o_Writer, i_bDriverResizeByAnimLength );

		o_Writer.FinishChunk();
	}

}	// local namespace

//--------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------
propDriverAnimationSubParser::propDriverAnimationSubParser()
{

}

//--------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------
propDriverAnimationSubParser::~propDriverAnimationSubParser()
{

}

//--------------------------------------------------------------------
// Returns chunk name used by this parser
//--------------------------------------------------------------------
//static
chDefs::Name propDriverAnimationSubParser::GetChunkName()
{
	return c_PDAS;
}

//--------------------------------------------------------------------
//	Read is called by a parser utility when the chunk name has been read
//  from the data file.  It will parse from the chunk into the given
//  parsable object.
//--------------------------------------------------------------------
void propDriverAnimationSubParser::Read(	chReader& i_Reader,
											chDefs::Version i_Version,
											chDefs::Size i_Size,
											tmlnDriverInfo& o_Driver ) const
{
	propDriverAnimationSubInfo &driver = dynamic_cast<propDriverAnimationSubInfo &>(o_Driver);

	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;

	while( i_Reader.ReadChunkHeader(name, version, size) )
	{
		if ( name == tmlnDriverInfoParser::GetChunkName() )
		{
			tmlnDriverInfoParser::ReadDriverInfo(i_Reader, version, size, driver);
		}
		else if ( name == c_PDAD )
		{
			ReadAnimFullInfo(i_Reader, version, size, driver.m_Info.m_AnimIndex, driver.m_Info.m_AnimName, driver.m_Info.m_bDriverResizeByAnimLength );
		}
		i_Reader.FinishChunk();
	}
}

//--------------------------------------------------------------------
//	Write is called to parse data from the given object to the given
//	chWriter.  i_Driver is the object being written.
//--------------------------------------------------------------------
void propDriverAnimationSubParser::Write( chWriter& o_Writer,
					const tmlnDriverInfo& i_Driver ) const
{
	const propDriverAnimationSubInfo &driver = dynamic_cast<const propDriverAnimationSubInfo &>(i_Driver);

	o_Writer.WriteChunkHeader( c_PDAS, 0, true );
	tmlnDriverInfoParser::WriteDriverInfo(o_Writer, driver);

	WriteAnimFullInfo( o_Writer, driver.m_Info.m_AnimIndex, driver.m_Info.m_AnimName, driver.m_Info.m_bDriverResizeByAnimLength );
	o_Writer.FinishChunk();
}

//--------------------------------------------------------------------
// Create should be overridden to create the user specific parsable object.
// Users of parsers should call create to create the object that is then
// passed to Read.
//--------------------------------------------------------------------
tmlnDriverInfo* propDriverAnimationSubParser::Create() const
{
	return new propDriverAnimationSubInfo(c_PDAS);
}


