/********************************************************************************************\
**  tmlnDriverAttachParser.cpp
**
**		see .hpp
**
**  Extra Large Technology
**  Copyright(C) 2003 - All Rights Reserved
\********************************************************************************************/

#include "Drivers/Attach/tmlnDriverAttachParser.hpp"

#include "Support/tmln/tmlnDriverInfoParser.hpp"
#include "Drivers/Attach/tmlnDriverAttachInfo.hpp"

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
	const chDefs::Name c_ATCI = chDefs::MakeName('A', 'T', 'C', 'I');
	const int l_ATCI_VERSION = 3;

	//========================================================================
	//   ReadAttachInfo
	//========================================================================
	void ReadAttachInfo(	chReader& i_Reader,
							chDefs::Version i_Version,
							chDefs::Size i_Size,
							tmlnDriverAttachInfo &o_Driver )
	{
		std::string name;
		chChunkParserUtil::Read(i_Reader, name);
		o_Driver.m_ObjectName.SetString( name );

		if ( i_Version >= 2 )
		{
			nameUID uid;
			chChunkParserUtil::Read(i_Reader, uid);
			o_Driver.m_ObjectName.SetUID( uid );
		}

		if (i_Version >= 1)
		{
			chChunkParserUtil::Read(i_Reader, o_Driver.m_AttachName);
			chChunkParserUtil::Read(i_Reader, o_Driver.m_AttachOffset);
		}

		if (i_Version >= 3)
		{
			chChunkParserUtil::Read(i_Reader, o_Driver.m_WorldSpaceOffset);
		}
	}

	//========================================================================
	//   WriteAttachInfo
	//========================================================================
	void WriteAttachInfo(	chWriter& o_Writer,
					const tmlnDriverAttachInfo &i_Driver )
	{
		o_Writer.WriteChunkHeader( c_ATCI, l_ATCI_VERSION, false );

		chChunkParserUtil::Write(o_Writer, i_Driver.m_ObjectName.GetString());
		chChunkParserUtil::Write(o_Writer, i_Driver.m_ObjectName.GetUID());
		chChunkParserUtil::Write(o_Writer, i_Driver.m_AttachName);
		chChunkParserUtil::Write(o_Writer, i_Driver.m_AttachOffset);
		chChunkParserUtil::Write(o_Writer, i_Driver.m_WorldSpaceOffset);

		o_Writer.FinishChunk();
	}

}	// local namespace


//--------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------
tmlnDriverAttachParser::tmlnDriverAttachParser(chDefs::Name i_ChunkName)
: m_ChunkName(i_ChunkName)
{

}

//--------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------
tmlnDriverAttachParser::~tmlnDriverAttachParser()
{

}

//--------------------------------------------------------------------
// Returns chunk name used by this parser
//--------------------------------------------------------------------
//static
chDefs::Name tmlnDriverAttachParser::GetChunkName()
{
	return m_ChunkName;
}

//--------------------------------------------------------------------
//	Read is called by a parser utility when the chunk name has been read
//  from the data file.  It will parse from the chunk into the given
//  parsable object.
//--------------------------------------------------------------------
void tmlnDriverAttachParser::Read(	chReader& i_Reader,
									chDefs::Version i_Version,
									chDefs::Size i_Size,
									tmlnDriverInfo& o_Driver ) const
{
	tmlnDriverAttachInfo &driver = dynamic_cast<tmlnDriverAttachInfo &>(o_Driver);

	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;

	while( i_Reader.ReadChunkHeader(name, version, size) )
	{
		if ( name == tmlnDriverInfoParser::GetChunkName() )
		{
			tmlnDriverInfoParser::ReadDriverInfo(i_Reader, version, size, driver);
		}
		else if ( name == c_ATCI )
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
void tmlnDriverAttachParser::Write( chWriter& o_Writer,
									const tmlnDriverInfo& i_Driver ) const
{
	const tmlnDriverAttachInfo &driver = dynamic_cast<const tmlnDriverAttachInfo &>(i_Driver);

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
tmlnDriverInfo* tmlnDriverAttachParser::Create() const
{
	return new tmlnDriverAttachInfo(m_ChunkName);
}


