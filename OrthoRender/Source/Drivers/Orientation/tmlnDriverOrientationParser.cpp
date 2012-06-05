/********************************************************************************************\
**  tmlnDriverOrientationParser.cpp
**
**		see .hpp
**
**  Extra Large Technology
**  Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/

#include "Drivers/Orientation/tmlnDriverOrientationParser.hpp"

#include "Support/tmln/tmlnDriverInfoParser.hpp"
#include "Drivers/Orientation/tmlnDriverOrientationInfo.hpp"

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
	const chDefs::Name c_ORII = chDefs::MakeName('O', 'R', 'I', 'I');


	//========================================================================
	//   ReadOrientationInfo
	//========================================================================
	void ReadOrientationInfo(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					tmlnDriverOrientationInfo &o_Driver )
	{
		if (i_Version >= 1)
		{
			// Read euler angles
			i_Reader.Read( o_Driver.m_X );
			i_Reader.Read( o_Driver.m_Y );
			i_Reader.Read( o_Driver.m_Z );
			// Read whether to do euler interpolation
			i_Reader.Read( o_Driver.m_bEulerInterpolation );
		}
		else
		{
			// Old version wrote quaterion (actually axis, angle?)
			maRotation rot;
			chChunkParserUtil::Read(i_Reader, rot);
			rot.GetEuler( o_Driver.m_X, o_Driver.m_Y, o_Driver.m_Z );
		}
	}

	//========================================================================
	//   WriteOrientationInfo
	//========================================================================
	void WriteOrientationInfo(	chWriter& o_Writer,
					const tmlnDriverOrientationInfo &i_Driver )
	{

		// Version 1 switched to euler angles from quaternion
		//  and added boolean for euler interpolation
		o_Writer.WriteChunkHeader( c_ORII, 1, false );
		o_Writer.Write( i_Driver.m_X );
		o_Writer.Write( i_Driver.m_Y );
		o_Writer.Write( i_Driver.m_Z );
		o_Writer.Write( i_Driver.m_bEulerInterpolation );
		o_Writer.FinishChunk();
	}

}	// local namespace

//--------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------
tmlnDriverOrientationParser::tmlnDriverOrientationParser(chDefs::Name i_ChunkName)
: m_ChunkName(i_ChunkName)
{

}

//--------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------
tmlnDriverOrientationParser::~tmlnDriverOrientationParser()
{

}

//--------------------------------------------------------------------
// Returns chunk name used by this parser
//--------------------------------------------------------------------
chDefs::Name tmlnDriverOrientationParser::GetChunkName()
{
	return m_ChunkName;
}

//--------------------------------------------------------------------
//	Read is called by a parser utility when the chunk name has been read
//  from the data file.  It will parse from the chunk into the given
//  parsable object.
//--------------------------------------------------------------------
void tmlnDriverOrientationParser::Read(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					tmlnDriverInfo& o_Driver ) const
{
	tmlnDriverOrientationInfo &driver = dynamic_cast<tmlnDriverOrientationInfo &>(o_Driver);

	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;

	while( i_Reader.ReadChunkHeader(name, version, size) )
	{
		if ( name == tmlnDriverInfoParser::GetChunkName() )
		{
			tmlnDriverInfoParser::ReadDriverInfo(i_Reader, version, size, driver);
		}
		else if ( name == c_ORII )
		{
			ReadOrientationInfo(i_Reader, version, size, driver);
		}
		i_Reader.FinishChunk();
	}
}

//--------------------------------------------------------------------
//	Write is called to parse data from the given object to the given
//	chWriter.  i_Driver is the object being written.
//--------------------------------------------------------------------
void tmlnDriverOrientationParser::Write( chWriter& o_Writer,
					const tmlnDriverInfo& i_Driver ) const
{
	const tmlnDriverOrientationInfo &driver = dynamic_cast<const tmlnDriverOrientationInfo &>(i_Driver);

	o_Writer.WriteChunkHeader( m_ChunkName, 0, true );
	tmlnDriverInfoParser::WriteDriverInfo(o_Writer, driver);

	WriteOrientationInfo(o_Writer, driver);
	o_Writer.FinishChunk();
}

//--------------------------------------------------------------------
// Create should be overridden to create the user specific parsable object.
// Users of parsers should call create to create the object that is then
// passed to Read.
//--------------------------------------------------------------------
tmlnDriverInfo* tmlnDriverOrientationParser::Create() const
{
	return new tmlnDriverOrientationInfo(m_ChunkName);
}


