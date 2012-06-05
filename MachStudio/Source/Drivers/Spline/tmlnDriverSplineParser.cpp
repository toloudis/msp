/********************************************************************************************\
**  tmlnDriverSplineParser.cpp
**
**		see .hpp
**
**  StudioGPU
**  Copyright(C) 2003 - All Rights Reserved
\********************************************************************************************/

#include "Drivers/Spline/tmlnDriverSplineParser.hpp"

#include "Support/tmln/tmlnDriverInfoParser.hpp"
#include "Drivers/Spline/tmlnDriverSplineInfo.hpp"
#include "Support/spln/splnSpline.hpp"

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
	const chDefs::Name c_SPLN = chDefs::MakeName('S', 'P', 'L', 'N');


	//========================================================================
	//   ReadSplineInfo
	//========================================================================
	void ReadSplineInfo(	chReader& i_Reader,
							chDefs::Version i_Version,
							chDefs::Size i_Size,
							std::vector<maPoint3d> &o_Points )
	{

		int num_points = 0;
		chChunkParserUtil::Read(i_Reader, num_points);
		o_Points.resize(num_points);
		for (int i=0; i<num_points; i++)
			chChunkParserUtil::Read(i_Reader, o_Points[i]);
	}

	//========================================================================
	//   WriteSplineInfo
	//========================================================================
	void WriteSplineInfo(	chWriter& o_Writer,
							const std::vector<maPoint3d> &i_Points )
	{

		o_Writer.WriteChunkHeader( c_SPLN, 0, false );
		int num_points = i_Points.size();
		chChunkParserUtil::Write(o_Writer, num_points);
		for (int i=0; i<num_points; i++)
			chChunkParserUtil::Write(o_Writer, i_Points[i]);
		o_Writer.FinishChunk();
	}

}	// local namespace

//--------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------
tmlnDriverSplineParser::tmlnDriverSplineParser(chDefs::Name i_ChunkName)
: m_ChunkName(i_ChunkName)
{

}

//--------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------
tmlnDriverSplineParser::~tmlnDriverSplineParser()
{

}

//--------------------------------------------------------------------
// Returns chunk name used by this parser
//--------------------------------------------------------------------
chDefs::Name tmlnDriverSplineParser::GetChunkName()
{
	return m_ChunkName;
}

//--------------------------------------------------------------------
//	Read is called by a parser utility when the chunk name has been read
//  from the data file.  It will parse from the chunk into the given
//  parsable object.
//--------------------------------------------------------------------
void tmlnDriverSplineParser::Read(	chReader& i_Reader,
									chDefs::Version i_Version,
									chDefs::Size i_Size,
									tmlnDriverInfo& o_Driver ) const
{
	tmlnDriverSplineInfo &driver = dynamic_cast<tmlnDriverSplineInfo &>(o_Driver);

	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;

	while( i_Reader.ReadChunkHeader(name, version, size) )
	{
		if ( name == tmlnDriverInfoParser::GetChunkName() )
		{
			tmlnDriverInfoParser::ReadDriverInfo(i_Reader, version, size, driver);
		}
		else if ( name == c_SPLN )
		{
			ReadSplineInfo(i_Reader, version, size, driver.m_SplineInfo.m_Points);
		}
		i_Reader.FinishChunk();
	}
}

//--------------------------------------------------------------------
//	Write is called to parse data from the given object to the given
//	chWriter.  i_Driver is the object being written.
//--------------------------------------------------------------------
void tmlnDriverSplineParser::Write( chWriter& o_Writer,
									const tmlnDriverInfo& i_Driver ) const
{
	const tmlnDriverSplineInfo &driver = dynamic_cast<const tmlnDriverSplineInfo &>(i_Driver);

	o_Writer.WriteChunkHeader( m_ChunkName, 0, true );

	tmlnDriverInfoParser::WriteDriverInfo(o_Writer, driver);
	WriteSplineInfo(o_Writer, driver.m_SplineInfo.m_Points);

	o_Writer.FinishChunk();
}

//--------------------------------------------------------------------
// Create should be overridden to create the user specific parsable object.
// Users of parsers should call create to create the object that is then
// passed to Read.
//--------------------------------------------------------------------
tmlnDriverInfo* tmlnDriverSplineParser::Create() const
{
	return new tmlnDriverSplineInfo(m_ChunkName);
}


