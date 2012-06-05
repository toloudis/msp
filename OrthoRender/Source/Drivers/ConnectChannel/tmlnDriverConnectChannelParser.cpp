/********************************************************************************************\
**  tmlnDriverConnectChannelParser.cpp
**
**
**  Extra Large Technology
**  Copyright(C) 2006 - All Rights Reserved
\********************************************************************************************/
#include "Drivers/ConnectChannel/tmlnDriverConnectChannelParser.hpp"

#include "Support/tmln/tmlnDriverInfoParser.hpp"
#include "Drivers/ConnectChannel/tmlnDriverConnectChannelInfo.hpp"

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
	const chDefs::Name c_CNCH = chDefs::MakeName('C', 'N', 'C', 'H');

	//========================================================================
	//   ReadConnectInfo
	//========================================================================
	void ReadConnectInfo(	chReader& i_Reader,
						chDefs::Version i_Version,
						chDefs::Size i_Size,
						tmlnDriverConnectChannelInfo &o_Driver )
	{
		std::string name;
		chChunkParserUtil::Read(i_Reader, name);
		o_Driver.m_Value.SetString( name );
		nameUID uid;
		chChunkParserUtil::Read(i_Reader, uid);
		o_Driver.m_Value.SetUID( uid );
	}

	//========================================================================
	//   WriteConnectInfo
	//========================================================================
	void WriteConnectInfo(chWriter& o_Writer,
						const tmlnDriverConnectChannelInfo &i_Driver )
	{
		const int lc_CNCH_VERSION = 0;
		o_Writer.WriteChunkHeader( c_CNCH, lc_CNCH_VERSION, false );
		chChunkParserUtil::Write(o_Writer, i_Driver.m_Value.GetString());
		chChunkParserUtil::Write(o_Writer, i_Driver.m_Value.GetUID());
		o_Writer.FinishChunk();
	}

}	// local namespace


//--------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------
tmlnDriverConnectChannelParser::tmlnDriverConnectChannelParser(chDefs::Name i_ChunkName)
: m_ChunkName(i_ChunkName)
{

}

//--------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------
tmlnDriverConnectChannelParser::~tmlnDriverConnectChannelParser()
{

}

//--------------------------------------------------------------------
// Returns chunk name used by this parser
//--------------------------------------------------------------------
chDefs::Name tmlnDriverConnectChannelParser::GetChunkName()
{
	return m_ChunkName;
}

//--------------------------------------------------------------------
//	Read is called by a parser utility when the chunk name has been read
//  from the data file.  It will parse from the chunk into the given
//  parsable object.
//--------------------------------------------------------------------
void tmlnDriverConnectChannelParser::Read(	chReader& i_Reader,
											chDefs::Version i_Version,
											chDefs::Size i_Size,
											tmlnDriverInfo& o_Driver ) const
{
	tmlnDriverConnectChannelInfo &driver = dynamic_cast<tmlnDriverConnectChannelInfo &>(o_Driver);

	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;

	while( i_Reader.ReadChunkHeader(name, version, size) )
	{
		if ( name == tmlnDriverInfoParser::GetChunkName() )
		{
			tmlnDriverInfoParser::ReadDriverInfo(i_Reader, version, size, driver);
		}
		else if ( name == c_CNCH )
		{
			ReadConnectInfo(i_Reader, version, size, driver);
		}
		i_Reader.FinishChunk();
	}
}

//--------------------------------------------------------------------
//	Write is called to parse data from the given object to the given
//	chWriter.  i_Driver is the object being written.
//--------------------------------------------------------------------
void tmlnDriverConnectChannelParser::Write(	chWriter& o_Writer,
											const tmlnDriverInfo& i_Driver ) const
{
	const tmlnDriverConnectChannelInfo &driver = dynamic_cast<const tmlnDriverConnectChannelInfo &>(i_Driver);

	const int lc_CONNECTCHUNK_VERSION = 0;
	o_Writer.WriteChunkHeader( m_ChunkName, lc_CONNECTCHUNK_VERSION, true );

	//	write the general driver info
	tmlnDriverInfoParser::WriteDriverInfo(o_Writer, driver);

	//	write the connect driver info
	WriteConnectInfo(o_Writer, driver);

	o_Writer.FinishChunk();
}

//--------------------------------------------------------------------
// Create should be overridden to create the user specific parsable object.
// Users of parsers should call create to create the object that is then
// passed to Read.
//--------------------------------------------------------------------
tmlnDriverInfo* tmlnDriverConnectChannelParser::Create() const
{
	return new tmlnDriverConnectChannelInfo(m_ChunkName);
}
