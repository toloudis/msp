/********************************************************************************************\
**  cmraDriverFramedBaseParser.cpp
**
**		see .hpp
**
**  StudioGPU
**  Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/

#include "Systems/Cameras/Drivers/cmraDriverFramedBaseParser.hpp"

#include "Support/tmln/tmlnDriverInfoParser.hpp"
#include "Systems/Cameras/Drivers/cmraDriverFramedBaseInfo.hpp"
#include "Systems/Cameras/Drivers/cmraDriverDataPositionParser.hpp"
#include "Systems/Cameras/Drivers/cmraDriverDataSubjectParser.hpp"
#include "Systems/Cameras/Drivers/cmraDriverDataViewTypeParser.hpp"

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
	const chDefs::Name c_CDFR = chDefs::MakeName('C', 'D', 'F', 'R');	// main chunk
	const chDefs::Name c_FRMI = chDefs::MakeName('F', 'R', 'M', 'I');	// info chunk

	const int c_CDFR_version	= 0;
	const int c_FRMI_version	= 0;

	//------------------------------------------------------------------------
	//   ReadFramedInfo
	//------------------------------------------------------------------------
	void ReadFramedInfo(	chReader& i_Reader,
							chDefs::Version i_Version,
							chDefs::Size i_Size,
							cmraDriverFramedBaseInfo &o_Driver )
	{
		chChunkParserUtil::Read(i_Reader, o_Driver.m_bPositionRestrict);
		chChunkParserUtil::Read(i_Reader, o_Driver.m_PositionTolerance);
		chChunkParserUtil::Read(i_Reader, o_Driver.m_bAngleRestrict);
		chChunkParserUtil::Read(i_Reader, o_Driver.m_AngleTolerance);
		chChunkParserUtil::Read(i_Reader, o_Driver.m_bDistanceRestrict);
		chChunkParserUtil::Read(i_Reader, o_Driver.m_DistanceTolerance);
		chChunkParserUtil::Read(i_Reader, o_Driver.m_bDirectionRestrict);
		chChunkParserUtil::Read(i_Reader, o_Driver.m_fDirectionTolerance);
	}

	//------------------------------------------------------------------------
	//   WriteFramedInfo
	//------------------------------------------------------------------------
	void WriteFramedInfo(	chWriter& o_Writer,
							const cmraDriverFramedBaseInfo &i_Driver )
	{

		o_Writer.WriteChunkHeader( c_FRMI, c_FRMI_version, false );

		chChunkParserUtil::Write(o_Writer, i_Driver.m_bPositionRestrict);
		chChunkParserUtil::Write(o_Writer, i_Driver.m_PositionTolerance);
		chChunkParserUtil::Write(o_Writer, i_Driver.m_bAngleRestrict);
		chChunkParserUtil::Write(o_Writer, i_Driver.m_AngleTolerance);
		chChunkParserUtil::Write(o_Writer, i_Driver.m_bDistanceRestrict);
		chChunkParserUtil::Write(o_Writer, i_Driver.m_DistanceTolerance);
		chChunkParserUtil::Write(o_Writer, i_Driver.m_bDirectionRestrict);
		chChunkParserUtil::Write(o_Writer, i_Driver.m_fDirectionTolerance);

		o_Writer.FinishChunk();
	}

}	// local namespace

//--------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------
cmraDriverFramedBaseParser::cmraDriverFramedBaseParser()
{

}

//--------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------
cmraDriverFramedBaseParser::~cmraDriverFramedBaseParser()
{

}

//--------------------------------------------------------------------
// Returns chunk name used by this parser
//--------------------------------------------------------------------
//static
chDefs::Name cmraDriverFramedBaseParser::GetChunkName()
{
	return c_CDFR;
}

//--------------------------------------------------------------------
//	Read is called by a parser utility when the chunk name has been read
//  from the data file.  It will parse from the chunk into the given
//  parsable object.
//--------------------------------------------------------------------
void cmraDriverFramedBaseParser::Read(	chReader& i_Reader,
										chDefs::Version i_Version,
										chDefs::Size i_Size,
										tmlnDriverInfo& o_Driver ) const
{
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
bool cmraDriverFramedBaseParser::ReadChunk(	chReader& i_Reader,
										    chDefs::Name i_Name, 
											chDefs::Version i_Version,
											chDefs::Size i_Size,
											tmlnDriverInfo& o_Driver ) const
{
	cmraDriverFramedBaseInfo &driver = dynamic_cast<cmraDriverFramedBaseInfo &>(o_Driver);

	if ( i_Name == tmlnDriverInfoParser::GetChunkName() )
	{
		tmlnDriverInfoParser::ReadDriverInfo(i_Reader, i_Version, i_Size, driver);
		return true;
	}
	else if ( i_Name == cmraDriverDataPositionParser::GetChunkName() )
	{
		cmraDriverDataPositionParser::Read(i_Reader, i_Version, i_Size, driver.m_StartPositionData );
		return true;
	}
	else if ( i_Name == cmraDriverDataSubjectParser::GetChunkName() )
	{
		cmraDriverDataSubjectParser::Read(i_Reader, i_Version, i_Size, driver.m_SubjectData );
		return true;
	}
	else if ( i_Name == cmraDriverDataViewTypeParser::GetChunkName() )
	{
		cmraDriverDataViewTypeParser::Read(i_Reader, i_Version, i_Size, driver.m_ViewTypeData );
		return true;
	}
	else if ( i_Name == c_FRMI )
	{
		ReadFramedInfo(i_Reader, i_Version, i_Size, driver);
		return true;
	}

	return false;
}


//--------------------------------------------------------------------
//	Write is called to parse data from the given object to the given
//	chWriter.  i_Driver is the object being written.
//--------------------------------------------------------------------
void cmraDriverFramedBaseParser::Write( chWriter& o_Writer,
										const tmlnDriverInfo& i_Driver ) const
{
	o_Writer.WriteChunkHeader( c_CDFR, c_CDFR_version, true );

	WriteChunks( o_Writer, i_Driver );

	o_Writer.FinishChunk();
}

//--------------------------------------------------------------------
//	Write out the sub-chunks for this parser.
//
//	Note: this function does finish each sub-chunk.
//--------------------------------------------------------------------
//virtual 
void cmraDriverFramedBaseParser::WriteChunks(  chWriter& o_Writer,
											   const tmlnDriverInfo& i_Driver ) const
{
	const cmraDriverFramedBaseInfo &driver = dynamic_cast<const cmraDriverFramedBaseInfo &>(i_Driver);

	tmlnDriverInfoParser::WriteDriverInfo(o_Writer, driver);
	cmraDriverDataPositionParser::Write( o_Writer, driver.m_StartPositionData );
	cmraDriverDataSubjectParser::Write( o_Writer, driver.m_SubjectData );
	cmraDriverDataViewTypeParser::Write( o_Writer, driver.m_ViewTypeData );
	WriteFramedInfo(o_Writer, driver);
}

//--------------------------------------------------------------------
// Create should be overridden to create the user specific parsable object.
// Users of parsers should call create to create the object that is then
// passed to Read.
//--------------------------------------------------------------------
tmlnDriverInfo* cmraDriverFramedBaseParser::Create() const
{
	return new cmraDriverFramedBaseInfo(c_CDFR);
}


