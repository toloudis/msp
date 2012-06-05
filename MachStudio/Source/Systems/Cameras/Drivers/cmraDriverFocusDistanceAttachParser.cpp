/********************************************************************************************\
**  cmraDriverFocusDistanceAttachParser.cpp
**
**		see .hpp
**
**  StudioGPU
**  Copyright(C) 2005 - All Rights Reserved
\********************************************************************************************/

#include "Systems/Cameras/Drivers/cmraDriverFocusDistanceAttachParser.hpp"

#include "Support/tmln/tmlnDriverInfoParser.hpp"
#include "Systems/Cameras/Drivers/cmraDriverFocusDistanceAttachInfo.hpp"

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
	const chDefs::Name c_ATCF = chDefs::MakeName('A', 'T', 'C', 'F');

	//========================================================================
	//   ReadAttachInfo
	//========================================================================
	void ReadAttachInfo(	chReader& i_Reader,
							chDefs::Version i_Version,
							chDefs::Size i_Size,
							cmraDriverFocusDistanceAttachInfo &o_Driver )
	{
		std::string name;
		chChunkParserUtil::Read(i_Reader, name);
		o_Driver.m_ObjectName.SetString( name );

		nameUID uid;
		chChunkParserUtil::Read(i_Reader, uid);
		o_Driver.m_ObjectName.SetUID( uid );

		chChunkParserUtil::Read(i_Reader, o_Driver.m_AttachName);
	}
	

	//========================================================================
	//   WriteAttachInfo
	//========================================================================
	void WriteAttachInfo(	chWriter& o_Writer,
					const cmraDriverFocusDistanceAttachInfo &i_Driver )
	{
		const int l_ATCF_VERSION = 0;
		o_Writer.WriteChunkHeader( c_ATCF, l_ATCF_VERSION, false );

		chChunkParserUtil::Write(o_Writer, i_Driver.m_ObjectName.GetString());
		chChunkParserUtil::Write(o_Writer, i_Driver.m_ObjectName.GetUID());
		chChunkParserUtil::Write(o_Writer, i_Driver.m_AttachName);

		o_Writer.FinishChunk();
	}

}	// local namespace


//--------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------
cmraDriverFocusDistanceAttachParser::cmraDriverFocusDistanceAttachParser(chDefs::Name i_ChunkName)
: m_ChunkName(i_ChunkName)
{

}

//--------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------
cmraDriverFocusDistanceAttachParser::~cmraDriverFocusDistanceAttachParser()
{

}

//--------------------------------------------------------------------
// Returns chunk name used by this parser
//--------------------------------------------------------------------
//static
chDefs::Name cmraDriverFocusDistanceAttachParser::GetChunkName()
{
	return m_ChunkName;
}

//--------------------------------------------------------------------
//	Read is called by a parser utility when the chunk name has been read
//  from the data file.  It will parse from the chunk into the given
//  parsable object.
//--------------------------------------------------------------------
void cmraDriverFocusDistanceAttachParser::Read(	chReader& i_Reader,
									chDefs::Version i_Version,
									chDefs::Size i_Size,
									tmlnDriverInfo& o_Driver ) const
{
	cmraDriverFocusDistanceAttachInfo &driver = dynamic_cast<cmraDriverFocusDistanceAttachInfo &>(o_Driver);

	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;

	while( i_Reader.ReadChunkHeader(name, version, size) )
	{
		if ( name == tmlnDriverInfoParser::GetChunkName() )
		{
			tmlnDriverInfoParser::ReadDriverInfo(i_Reader, version, size, driver);
		}
		else if ( name == c_ATCF )
		{
			ReadAttachInfo(i_Reader, version, size, driver);
		}
		i_Reader.FinishChunk();
	}
}

//--------------------------------------------------------------------
//	Write is called to parse data from the given object to the given
//	chWriter.  i_Driver is the object being written.
//--------------------------------------------------------------------
void cmraDriverFocusDistanceAttachParser::Write( chWriter& o_Writer,
									const tmlnDriverInfo& i_Driver ) const
{
	const cmraDriverFocusDistanceAttachInfo &driver = dynamic_cast<const cmraDriverFocusDistanceAttachInfo &>(i_Driver);

	o_Writer.WriteChunkHeader( m_ChunkName, 0, true );
	tmlnDriverInfoParser::WriteDriverInfo(o_Writer, driver);

	WriteAttachInfo(o_Writer, driver);
	o_Writer.FinishChunk();
}

//--------------------------------------------------------------------
// Create should be overridden to create the user specific parsable object.
// Users of parsers should call create to create the object that is then
// passed to Read.
//--------------------------------------------------------------------
tmlnDriverInfo* cmraDriverFocusDistanceAttachParser::Create() const
{
	return new cmraDriverFocusDistanceAttachInfo(m_ChunkName);
}


