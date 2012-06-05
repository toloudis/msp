/********************************************************************************************\
**  tmlnChannelInfoParser.cpp
**
**		see .hpp
**
**  Extra Large Technology
**  Copyright(C) 2005 - All Rights Reserved
\********************************************************************************************/
#include "Support/tmln/tmlnChannelInfoParser.hpp"

#include "Support/tmln/tmlnChannelInfo.hpp"

#include "Core/ch/chExceptionX.hpp"
#include "Core/ch/chReader.hpp"
#include "Core/ch/chWriter.hpp"
#include "Core/dbg/dbgLog.hpp"
#include "Core/ch/chChunkParserUtil.hpp"


//============================================================================
//============================================================================
namespace tmlnChannelInfoParser
{

//============================================================================
//============================================================================
namespace
{
	const chDefs::Name c_CHNI = chDefs::MakeName('C', 'H', 'N', 'I');
	const chDefs::Name c_CHMI = chDefs::MakeName('C', 'H', 'M', 'I');

	//------------------------------------------------------------------------
	//   ReadOldChannelInfo - old format was just boolean flags
	//	written in order of channels in script object
	//------------------------------------------------------------------------
	void ReadOldChannelInfo(chReader& i_Reader,
							chDefs::Version i_Version,
							tmlnChannelInfo &o_ChannelInfo )
	{
		DBG_ASSERT1(i_Version < 1, "This function should only be used for version 0 info structs, read version %d", i_Version);
		
		// old version, just had boolean flag, no channel name
		chChunkParserUtil::Read(i_Reader, o_ChannelInfo.m_bLocked);
	}
	//------------------------------------------------------------------------
	//   ReadChannelInfo
	//------------------------------------------------------------------------
	void ReadChannelInfo(	chReader& i_Reader,
							chDefs::Version i_Version,
							tmlnChannelInfo &o_ChannelInfo,
							std::string &o_ChannelName )
	{
		DBG_ASSERT1(i_Version >= 1, "This function should not be used for version 0 info structs, read version %d", i_Version);
		
		// new version writes name and then info
		chChunkParserUtil::Read(i_Reader, o_ChannelName);
		chChunkParserUtil::Read(i_Reader, o_ChannelInfo.m_bLocked);
	}

	//------------------------------------------------------------------------
	//   WriteChannelInfo
	//------------------------------------------------------------------------
	void WriteChannelInfo(	chWriter& o_Writer,
							const std::string &i_ChannelName,
							const tmlnChannelInfo& i_ChannelInfo )
	{
		const int l_cCHMI_VERSION = 1;
		o_Writer.WriteChunkHeader( c_CHMI, l_cCHMI_VERSION, false );

		// version 1 writes channel name and then flag in order to
		// match up info to channel by name instead of index.
		DBG_LOG2("Writing channel %s info locked(%s)", i_ChannelName.c_str(), (i_ChannelInfo.m_bLocked?"true":"false"));
		chChunkParserUtil::Write(o_Writer, i_ChannelName);
		chChunkParserUtil::Write(o_Writer, i_ChannelInfo.m_bLocked);

		o_Writer.FinishChunk();
	}

}	// local namespace

//------------------------------------------------------------------------
//------------------------------------------------------------------------
chDefs::Name GetChunkName()
{
	return c_CHNI;
}

//------------------------------------------------------------------------
//   ReadChannels 
//------------------------------------------------------------------------
void ReadChannels(	chReader& i_Reader,
					std::map<std::string, tmlnChannelInfo> &o_Channels,
					const char* i_IndexedStringNames[],
					const int i_NumIndexedStringNames )
{
	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;

	int index = 0;
	while( i_Reader.ReadChunkHeader(name, version, size) )
	{
		if (name == c_CHMI)
		{
			// New version, writes sparse list of named infos, uses name to match info to channel
			tmlnChannelInfo info;
			std::string name;
			ReadChannelInfo(i_Reader, version, info, name);
			o_Channels[name] = info;
		}
		else if (name == c_CHNI)
		{
			// Old version, used indexing to match channels to info
			tmlnChannelInfo info;
			ReadOldChannelInfo(i_Reader, version, info);
			if (index < i_NumIndexedStringNames)
				o_Channels[i_IndexedStringNames[index++]] = info;
		}

		i_Reader.FinishChunk();
	}
}

//------------------------------------------------------------------------
//   WriteChannels
//------------------------------------------------------------------------
void WriteChannels(	chWriter& o_Writer,
					const std::map<std::string, tmlnChannelInfo> &i_Channels )
{
	std::map<std::string, tmlnChannelInfo>::const_iterator it;
	for (it = i_Channels.begin(); it != i_Channels.end(); ++it)
	{
		DBG_LOG1("Writing channel info #%s", it->first.c_str());

		// Write Channel info
		WriteChannelInfo( o_Writer, it->first, it->second );
	}
}

}	// end of namespace
