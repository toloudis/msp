/********************************************************************************************\
**  dynDriverTransformKeyParser.cpp
**
**		see .hpp
**
**  StudioGPU
**  Copyright(C) 2005 - All Rights Reserved
\********************************************************************************************/

#include "Support/dyn/Drivers/dynDriverTransformKeyParser.hpp"

#include "Support/tmln/tmlnDriverInfoParser.hpp"
#include "Support/dyn/Drivers/dynDriverTransformKeyInfo.hpp"

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
	const chDefs::Name c_XFKI = chDefs::MakeName('X', 'F', 'K', 'I');


	//========================================================================
	//   ReadOrientationInfo
	//========================================================================
	void ReadOrientationInfo(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					dynDriverTransformKeyInfo &o_Driver )
	{
		chChunkParserUtil::Read(i_Reader, o_Driver.m_Rotation);
		chChunkParserUtil::Read(i_Reader, o_Driver.m_Translation);
	}

	//========================================================================
	//   WriteOrientationInfo
	//========================================================================
	void WriteOrientationInfo(	chWriter& o_Writer,
					const dynDriverTransformKeyInfo &i_Driver )
	{

		o_Writer.WriteChunkHeader( c_XFKI, 0, false );
		chChunkParserUtil::Write(o_Writer, i_Driver.m_Rotation);
		chChunkParserUtil::Write(o_Writer, i_Driver.m_Translation);
		o_Writer.FinishChunk();
	}

}	// local namespace

//--------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------
dynDriverTransformKeyParser::dynDriverTransformKeyParser(chDefs::Name i_ChunkName)
: m_ChunkName(i_ChunkName)
{

}

//--------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------
dynDriverTransformKeyParser::~dynDriverTransformKeyParser()
{

}

//--------------------------------------------------------------------
// Returns chunk name used by this parser
//--------------------------------------------------------------------
chDefs::Name dynDriverTransformKeyParser::GetChunkName()
{
	return m_ChunkName;
}

//--------------------------------------------------------------------
//	Read is called by a parser utility when the chunk name has been read
//  from the data file.  It will parse from the chunk into the given
//  parsable object.
//--------------------------------------------------------------------
void dynDriverTransformKeyParser::Read(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					tmlnDriverInfo& o_Driver ) const
{
	dynDriverTransformKeyInfo &driver = dynamic_cast<dynDriverTransformKeyInfo &>(o_Driver);

	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;

	while( i_Reader.ReadChunkHeader(name, version, size) )
	{
		if ( name == tmlnDriverInfoParser::GetChunkName() )
		{
			tmlnDriverInfoParser::ReadDriverInfo(i_Reader, version, size, driver);
		}
		else if ( name == c_XFKI )
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
void dynDriverTransformKeyParser::Write( chWriter& o_Writer,
					const tmlnDriverInfo& i_Driver ) const
{
	const dynDriverTransformKeyInfo &driver = dynamic_cast<const dynDriverTransformKeyInfo &>(i_Driver);

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
tmlnDriverInfo* dynDriverTransformKeyParser::Create() const
{
	return new dynDriverTransformKeyInfo(m_ChunkName);
}


