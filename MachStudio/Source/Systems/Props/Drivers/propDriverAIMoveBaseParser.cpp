/********************************************************************************************\
**  propDriverAIMoveBaseParser.cpp
**
**		see .hpp
**
**  StudioGPU
**  Copyright(C) 2005 - All Rights Reserved
\********************************************************************************************/

#include "propDriverAIMoveBaseParser.hpp"

#include "propDriverAIMoveBaseInfo.hpp"
#include "propDriverAnimationFullParser.hpp"

#include "chBinReader.hpp"
#include "chBinWriter.hpp"
#include "chExceptionX.hpp"
#include "dbgLog.hpp"
#include "fsFileUtil.hpp"
#include "fsFileX.hpp"
#include "gfFileBin.hpp"
#include "chChunkParserUtil.hpp"
#include "tmlnDriverInfoParser.hpp"
#include "tmlnDriverSplineParser.hpp"

namespace
{
	//========================================================================
	//========================================================================
	const chDefs::Name c_PDAB = chDefs::MakeName('P', 'D', 'A', 'B');	//prop driver AIMoveBase main chunk
	const chDefs::Name c_AIBD = chDefs::MakeName('A', 'I', 'B', 'D');	//prop driver AIMoveBase data chunk
	const chDefs::Name c_AIBS = chDefs::MakeName('A', 'I', 'B', 'S');	//prop driver AIMoveBase spline chunk

	const int c_PDAB_VERSION	= 0;
	const int c_AIBD_VERSION	= 0;

	//========================================================================
	//   ReadAIMoveBaseInfo
	//========================================================================
	void ReadAIMoveBaseInfo(	chReader& i_Reader,
								chDefs::Version i_Version,
								chDefs::Size i_Size,
								propAIMoveBaseInfo& o_AIMoveBaseInfo )
	{
		chChunkParserUtil::Read(i_Reader, o_AIMoveBaseInfo.m_Stub);
	}

	//========================================================================
	//   WriteAIMoveBaseInfo
	//========================================================================
	void WriteAIMoveBaseInfo(	chWriter& o_Writer,
								const propAIMoveBaseInfo& i_AIMoveBaseInfo )
	{
		o_Writer.WriteChunkHeader( c_AIBD, c_AIBD_VERSION, false );

		chChunkParserUtil::Write(o_Writer, i_AIMoveBaseInfo.m_Stub);

		o_Writer.FinishChunk();
	}

}	// local namespace

//--------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------
propDriverAIMoveBaseParser::propDriverAIMoveBaseParser()
{

}

//--------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------
propDriverAIMoveBaseParser::~propDriverAIMoveBaseParser()
{

}

//--------------------------------------------------------------------
// Returns chunk name used by this parser
//--------------------------------------------------------------------
//static
chDefs::Name propDriverAIMoveBaseParser::GetChunkName()
{
	return c_PDAB;
}

//--------------------------------------------------------------------
// FIX: - don't like this at all.  only used to get the chunk name to
//	propDriverAIMoveBaseInfo constructor.
//--------------------------------------------------------------------
//static 
chDefs::Name propDriverAIMoveBaseParser::GetSplineChunkName()
{
	return c_AIBS;
}

//--------------------------------------------------------------------
//	Read is called by a parser utility when the chunk name has been read
//  from the data file.  It will parse from the chunk into the given
//  parsable object.
//--------------------------------------------------------------------
void propDriverAIMoveBaseParser::Read(	chReader& i_Reader,
										chDefs::Version i_Version,
										chDefs::Size i_Size,
										tmlnDriverInfo& o_Driver ) const
{
	propDriverAIMoveBaseInfo &driver = dynamic_cast<propDriverAIMoveBaseInfo &>(o_Driver);

	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;

	while( i_Reader.ReadChunkHeader(name, version, size) )
	{
		ReadChunk( i_Reader, name, version, size, o_Driver );

		i_Reader.FinishChunk();
	}
}


//--------------------------------------------------------------------
//	Check the passed in name and see if it is a valid chunk.  If it
//	is then read in the information and return true.
//
//	Note: this function does NOT finish the chunk.
//--------------------------------------------------------------------
//virtual 
bool propDriverAIMoveBaseParser::ReadChunk(	chReader& i_Reader,
										    chDefs::Name i_Name, 
											chDefs::Version i_Version,
											chDefs::Size i_Size,
											tmlnDriverInfo& o_Driver ) const
{
	propDriverAIMoveBaseInfo &driver = dynamic_cast<propDriverAIMoveBaseInfo &>(o_Driver);

	if ( i_Name == c_AIBS )
	{
		tmlnDriverSplineParser dsparser( c_AIBS );
		dsparser.Read( i_Reader, i_Version, i_Size, *(driver.m_pSplineInfo) );
		return true;
	}
	else if ( i_Name == propDriverAnimationFullParser::GetChunkName() )
	{
		propDriverAnimationFullParser * dafparser = new propDriverAnimationFullParser;
		dafparser->Read( i_Reader, i_Version, i_Size, *(driver.m_pAnimFullInfo) );
		delete dafparser;
		return true;
	}
	else if ( i_Name == c_AIBD )
	{
		ReadAIMoveBaseInfo(i_Reader, i_Version, i_Size, driver.m_AIMoveBaseInfo );
		return true;
	}

	return false;
}

//--------------------------------------------------------------------
//	Write is called to parse data from the given object to the given
//	chWriter.  i_Driver is the object being written.
//--------------------------------------------------------------------
void propDriverAIMoveBaseParser::Write( chWriter& o_Writer,
										const tmlnDriverInfo& i_Driver ) const
{
	const propDriverAIMoveBaseInfo &driver = dynamic_cast<const propDriverAIMoveBaseInfo &>(i_Driver);

	o_Writer.WriteChunkHeader( c_PDAB, c_PDAB_VERSION, true );

	WriteChunks( o_Writer, i_Driver );

	o_Writer.FinishChunk();
}

//--------------------------------------------------------------------
//	Write out the sub-chunks for this parser.
//
//	Note: this function does finish each sub-chunk.
//--------------------------------------------------------------------
//virtual 
void propDriverAIMoveBaseParser::WriteChunks(  chWriter& o_Writer,
											   const tmlnDriverInfo& i_Driver ) const
{
	const propDriverAIMoveBaseInfo &driver = dynamic_cast<const propDriverAIMoveBaseInfo &>(i_Driver);

	tmlnDriverSplineParser dsparser( c_AIBS );
	dsparser.Write( o_Writer, driver );
	propDriverAnimationFullParser dafparser;
	dafparser.Write( o_Writer, driver );

	WriteAIMoveBaseInfo( o_Writer, driver.m_AIMoveBaseInfo );

	//tmlnDriverInfoParser::WriteDriverInfo(o_Writer, driver);
	//cmraDriverDataPositionParser::Write( o_Writer, driver.m_StartPositionData );
	//cmraDriverDataSubjectParser::Write( o_Writer, driver.m_SubjectData );
	//cmraDriverDataViewTypeParser::Write( o_Writer, driver.m_ViewTypeData );
	//WriteFramedInfo(o_Writer, driver);
}

//--------------------------------------------------------------------
// Create should be overridden to create the user specific parsable object.
// Users of parsers should call create to create the object that is then
// passed to Read.
//--------------------------------------------------------------------
tmlnDriverInfo* propDriverAIMoveBaseParser::Create() const
{
	return new propDriverAIMoveBaseInfo(c_PDAB);
}


