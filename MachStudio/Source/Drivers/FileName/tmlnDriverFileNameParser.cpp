/********************************************************************************************\
**  tmlnDriverFileNameParser.cpp
**
**
**  StudioGPU
**  Copyright(C) 2003 - All Rights Reserved
\********************************************************************************************/

#include "Drivers/FileName/tmlnDriverFileNameParser.hpp"

#include "Support/tmln/tmlnDriverInfoParser.hpp"
#include "Drivers/FileName/tmlnDriverFileNameInfo.hpp"

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
	const chDefs::Name c_FNME = chDefs::MakeName('F', 'N', 'M', 'E');


	//========================================================================
	//   ReadFileNameInfo
	//========================================================================
	void ReadFileNameInfo(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					tmlnDriverFileNameInfo &o_Driver )
	{
		chChunkParserUtil::Read(i_Reader, o_Driver.m_Value);
	}

	//========================================================================
	//   WriteFileNameInfo
	//========================================================================
	void WriteFileNameInfo(	chWriter& o_Writer,
					const tmlnDriverFileNameInfo &i_Driver )
	{

		o_Writer.WriteChunkHeader( c_FNME, 0, false );
		chChunkParserUtil::Write(o_Writer, i_Driver.m_Value);
		o_Writer.FinishChunk();
	}

}	// local namespace

//--------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------
tmlnDriverFileNameParser::tmlnDriverFileNameParser(chDefs::Name i_ChunkName)
: m_ChunkName(i_ChunkName)
{

}

//--------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------
tmlnDriverFileNameParser::~tmlnDriverFileNameParser()
{

}

//--------------------------------------------------------------------
// Returns chunk name used by this parser
//--------------------------------------------------------------------
chDefs::Name tmlnDriverFileNameParser::GetChunkName()
{
	return m_ChunkName;
}

//--------------------------------------------------------------------
//	Read is called by a parser utility when the chunk name has been read
//  from the data file.  It will parse from the chunk into the given
//  parsable object.
//--------------------------------------------------------------------
void tmlnDriverFileNameParser::Read(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					tmlnDriverInfo& o_Driver ) const
{
	tmlnDriverFileNameInfo &driver = dynamic_cast<tmlnDriverFileNameInfo &>(o_Driver);

	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;

	while( i_Reader.ReadChunkHeader(name, version, size) )
	{
		if ( name == tmlnDriverInfoParser::GetChunkName() )
		{
			tmlnDriverInfoParser::ReadDriverInfo(i_Reader, version, size, driver);
		}
		else if ( name == c_FNME )
		{
			ReadFileNameInfo(i_Reader, version, size, driver);
		}
		i_Reader.FinishChunk();
	}
}

//--------------------------------------------------------------------
//	Write is called to parse data from the given object to the given
//	chWriter.  i_Driver is the object being written.
//--------------------------------------------------------------------
void tmlnDriverFileNameParser::Write( chWriter& o_Writer,
					const tmlnDriverInfo& i_Driver ) const
{
	const tmlnDriverFileNameInfo &driver = dynamic_cast<const tmlnDriverFileNameInfo &>(i_Driver);

	o_Writer.WriteChunkHeader( m_ChunkName, 0, true );
	tmlnDriverInfoParser::WriteDriverInfo(o_Writer, driver);

	WriteFileNameInfo(o_Writer, driver);
	o_Writer.FinishChunk();
}

//--------------------------------------------------------------------
// Create should be overridden to create the user specific parsable object.
// Users of parsers should call create to create the object that is then
// passed to Read.
//--------------------------------------------------------------------
tmlnDriverInfo* tmlnDriverFileNameParser::Create() const
{
	return new tmlnDriverFileNameInfo(m_ChunkName);
}


