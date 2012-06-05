/********************************************************************************************\
**  cmraDriverFocusAttachParser.cpp
**
**		see .hpp
**
**  Extra Large Technology
**  Copyright(C) 2005 - All Rights Reserved
\********************************************************************************************/

#include "Systems/Cameras/Drivers/cmraDriverFocusAttachParser.hpp"

#include "Support/tmln/tmlnDriverInfoParser.hpp"
#include "Systems/Cameras/Drivers/cmraDriverFocusAttachInfo.hpp"

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
	const int l_ATCI_VERSION = 2;

	//========================================================================
	//   ReadAttachInfo
	//========================================================================
	void ReadAttachInfo(	chReader& i_Reader,
							chDefs::Version i_Version,
							chDefs::Size i_Size,
							cmraDriverFocusAttachInfo &o_Driver )
	{
		std::string name;
		chChunkParserUtil::Read(i_Reader, name);
		o_Driver.m_ObjectName.SetString( name );

		nameUID uid;
		chChunkParserUtil::Read(i_Reader, uid);
		o_Driver.m_ObjectName.SetUID( uid );

		// inserted before float data in version 2
		if (i_Version >= 2)
		{
			chChunkParserUtil::Read(i_Reader, o_Driver.m_AttachName);
		}

		// prior to version 1, some data was stored as vector
		if (i_Version < 1)
		{
			maPoint3d worldSpaceOffset;
			chChunkParserUtil::Read(i_Reader, worldSpaceOffset);
			o_Driver.m_NearFocusOffset = -worldSpaceOffset.GetX();
			o_Driver.m_FarFocusOffset = worldSpaceOffset.GetY();
			chChunkParserUtil::Read(i_Reader, o_Driver.m_NearFalloffDist);
			chChunkParserUtil::Read(i_Reader, o_Driver.m_FarFalloffDist);
		}
		else
		{
			// version 1 and up, data is 4 separate floats 
			chChunkParserUtil::Read(i_Reader, o_Driver.m_NearFocusOffset);
			chChunkParserUtil::Read(i_Reader, o_Driver.m_FarFocusOffset);
			chChunkParserUtil::Read(i_Reader, o_Driver.m_NearFalloffDist);
			chChunkParserUtil::Read(i_Reader, o_Driver.m_FarFalloffDist);
		}
	}

	//========================================================================
	//   WriteAttachInfo
	//========================================================================
	void WriteAttachInfo(	chWriter& o_Writer,
					const cmraDriverFocusAttachInfo &i_Driver )
	{
		o_Writer.WriteChunkHeader( c_ATCI, l_ATCI_VERSION, false );

		chChunkParserUtil::Write(o_Writer, i_Driver.m_ObjectName.GetString());
		chChunkParserUtil::Write(o_Writer, i_Driver.m_ObjectName.GetUID());
		chChunkParserUtil::Write(o_Writer, i_Driver.m_AttachName);
		chChunkParserUtil::Write(o_Writer, i_Driver.m_NearFocusOffset);
		chChunkParserUtil::Write(o_Writer, i_Driver.m_FarFocusOffset);
		chChunkParserUtil::Write(o_Writer, i_Driver.m_NearFalloffDist);
		chChunkParserUtil::Write(o_Writer, i_Driver.m_FarFalloffDist);

		o_Writer.FinishChunk();
	}

}	// local namespace


//--------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------
cmraDriverFocusAttachParser::cmraDriverFocusAttachParser(chDefs::Name i_ChunkName)
: m_ChunkName(i_ChunkName)
{

}

//--------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------
cmraDriverFocusAttachParser::~cmraDriverFocusAttachParser()
{

}

//--------------------------------------------------------------------
// Returns chunk name used by this parser
//--------------------------------------------------------------------
//static
chDefs::Name cmraDriverFocusAttachParser::GetChunkName()
{
	return m_ChunkName;
}

//--------------------------------------------------------------------
//	Read is called by a parser utility when the chunk name has been read
//  from the data file.  It will parse from the chunk into the given
//  parsable object.
//--------------------------------------------------------------------
void cmraDriverFocusAttachParser::Read(	chReader& i_Reader,
									chDefs::Version i_Version,
									chDefs::Size i_Size,
									tmlnDriverInfo& o_Driver ) const
{
	cmraDriverFocusAttachInfo &driver = dynamic_cast<cmraDriverFocusAttachInfo &>(o_Driver);

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
void cmraDriverFocusAttachParser::Write( chWriter& o_Writer,
									const tmlnDriverInfo& i_Driver ) const
{
	const cmraDriverFocusAttachInfo &driver = dynamic_cast<const cmraDriverFocusAttachInfo &>(i_Driver);

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
tmlnDriverInfo* cmraDriverFocusAttachParser::Create() const
{
	return new cmraDriverFocusAttachInfo(m_ChunkName);
}


