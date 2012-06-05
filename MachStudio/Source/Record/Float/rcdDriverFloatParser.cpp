/********************************************************************************************\
**  rcdDriverFloatParser.cpp
**
**
**  StudioGPU
**  Copyright(C) 2003 - All Rights Reserved
\********************************************************************************************/

#include "Record/Float/rcdDriverFloatParser.hpp"

#include "Support/tmln/tmlnDriverInfoParser.hpp"
#include "Record/Float/rcdDriverFloatInfo.hpp"

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
	const chDefs::Name c_FRCD = chDefs::MakeName('F', 'R', 'C', 'D');


	//========================================================================
	//   ReadFloatInfo
	//========================================================================
	void ReadFloatInfo(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					rcdDriverFloatInfo &o_Driver )
	{
		int num_keys = 0;
		chChunkParserUtil::Read(i_Reader, num_keys);
		float time, val;
		for (int i=0; i<num_keys; i++)
		{
			chChunkParserUtil::Read(i_Reader, time);
			chChunkParserUtil::Read(i_Reader, val);

			if (i==0)
			{
				DBG_ASSERT0(time==0, "Non-zero key");
				o_Driver.m_Keys.Clear(val);
			}
			else
				o_Driver.m_Keys.AddKey(time, val);

		}
	}

	//========================================================================
	//   WriteFloatInfo
	//========================================================================
	void WriteFloatInfo(	chWriter& o_Writer,
					const rcdDriverFloatInfo &i_Driver )
	{

		o_Writer.WriteChunkHeader( c_FRCD, 0, false );
		int num_keys = i_Driver.m_Keys.GetNumKeys();
		chChunkParserUtil::Write(o_Writer, num_keys);
		float time, val;
		for (int i=0; i<num_keys; i++)
		{
			i_Driver.m_Keys.GetKeyData(i, time, val);
			chChunkParserUtil::Write(o_Writer, time);
			chChunkParserUtil::Write(o_Writer, val);
		}
		o_Writer.FinishChunk();
	}

}	// local namespace

//--------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------
rcdDriverFloatParser::rcdDriverFloatParser(chDefs::Name i_ChunkName)
: m_ChunkName(i_ChunkName)
{

}

//--------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------
rcdDriverFloatParser::~rcdDriverFloatParser()
{

}

//--------------------------------------------------------------------
// Returns chunk name used by this parser
//--------------------------------------------------------------------
chDefs::Name rcdDriverFloatParser::GetChunkName()
{
	return m_ChunkName;
}

//--------------------------------------------------------------------
//	Read is called by a parser utility when the chunk name has been read
//  from the data file.  It will parse from the chunk into the given
//  parsable object.
//--------------------------------------------------------------------
void rcdDriverFloatParser::Read(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					tmlnDriverInfo& o_Driver ) const
{
	rcdDriverFloatInfo &driver = dynamic_cast<rcdDriverFloatInfo &>(o_Driver);

	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;

	while( i_Reader.ReadChunkHeader(name, version, size) )
	{
		if ( name == tmlnDriverInfoParser::GetChunkName() )
		{
			tmlnDriverInfoParser::ReadDriverInfo(i_Reader, version, size, driver);
		}
		else if ( name == c_FRCD )
		{
			ReadFloatInfo(i_Reader, version, size, driver);
		}
		i_Reader.FinishChunk();
	}
}

//--------------------------------------------------------------------
//	Write is called to parse data from the given object to the given
//	chWriter.  i_Driver is the object being written.
//--------------------------------------------------------------------
void rcdDriverFloatParser::Write( chWriter& o_Writer,
					const tmlnDriverInfo& i_Driver ) const
{
	const rcdDriverFloatInfo &driver = dynamic_cast<const rcdDriverFloatInfo &>(i_Driver);

	o_Writer.WriteChunkHeader( m_ChunkName, 0, true );
	tmlnDriverInfoParser::WriteDriverInfo(o_Writer, driver);

	WriteFloatInfo(o_Writer, driver);
	o_Writer.FinishChunk();
}

//--------------------------------------------------------------------
// Create should be overridden to create the user specific parsable object.
// Users of parsers should call create to create the object that is then
// passed to Read.
//--------------------------------------------------------------------
tmlnDriverInfo* rcdDriverFloatParser::Create() const
{
	return new rcdDriverFloatInfo(m_ChunkName);
}


