/*****************************************************************************
**	cmmDriverCreatorVideo.cpp
**
**	Creates common drivers and parsers for color channels
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/Common/DriverCreator/cmmDriverCreatorVideo.hpp"

//#include "Support/tmln/tmlnChannelFilePath.hpp"
#include "Support/tmln/tmlnChannelVideo.hpp"
//#include "Support/tmln/tmlnChannelTextureFileName.hpp"
#include "Support/tmln/tmlnCreator.hpp"
//#include "Drivers/FilePath/tmlnDriverAnimatedFilePath.hpp"
//#include "Drivers/FilePath/tmlnDriverAnimatedFilePathInfo.hpp"
//#include "Drivers/FilePath/tmlnDriverAnimatedFilePathParser.hpp"
//#include "Drivers/FilePath/tmlnDriverFilePath.hpp"
//#include "Drivers/FilePath/tmlnDriverFilePathInfo.hpp"
//#include "Drivers/FilePath/tmlnDriverFilePathParser.hpp"
//#include "Drivers/TextureFileName/tmlnDriverAnimatedTextureFileName.hpp"
//#include "Drivers/TextureFileName/tmlnDriverAnimatedTextureFileNameInfo.hpp"
//#include "Drivers/TextureFileName/tmlnDriverAnimatedTextureFileNameParser.hpp"
//#include "Drivers/TextureFileName/tmlnDriverTextureFileName.hpp"
//#include "Drivers/TextureFileName/tmlnDriverTextureFileNameInfo.hpp"
//#include "Drivers/TextureFileName/tmlnDriverTextureFileNameParser.hpp"
//#include "Drivers/ConnectChannel/tmlnDriverConnectChannelParser.hpp"
#include "Drivers/Video/tmlnDriverVideo.hpp"
#include "Drivers/Video/tmlnDriverVideoInfo.hpp"
#include "Drivers/Video/tmlnDriverVideoParser.hpp"

#include "Core/Fs/fsAbsolutePathMgr.hpp"


namespace
{
	//
	//	Video filename Key (VK)
	//
	const chDefs::Name make_key_chunk_name(const char i_Code[2])
	{
		return chDefs::MakeName(i_Code[0], i_Code[1], 'V', 'K'); 
	}
	std::string get_key_name(const char* i_ChannelName)
	{
		return i_ChannelName; // Just name from channel, since simple key. Could maybe add "Key"
	}
}

//--------------------------------------------------------------------
// Create parsers for driver types created by this creator
//--------------------------------------------------------------------
void cmmDriverCreatorVideo::CreateParsers(chDefs::Name i_SystemCode,
										  const char i_Code[2])
{
	const chDefs::Name c_XXXK = make_key_chunk_name( i_Code );
	tmlnParser::AddDriverParser(i_SystemCode, c_XXXK,	new tmlnDriverVideoParser(c_XXXK));
}

//--------------------------------------------------------------------
// Helper function for finding what the chunk name will be
//--------------------------------------------------------------------
chDefs::Name cmmDriverCreatorVideo::GetKeyChunkName(const char i_Code[2])
{
	return make_key_chunk_name(i_Code);
}

//--------------------------------------------------------------------
// Gather the possible types of drivers that can be created for
//	for this channel.  Driver names are added through the
//	DriverNameList's API
//--------------------------------------------------------------------
void cmmDriverCreatorVideo::GatherPossibleDrivers(const char* i_ChannelName,
											      tmlnDriverNameList& io_Drivers,
												  const char *i_DefaultCategory,
												  tmlnDriverCreator* i_Creator)
{
	// With property key buttons, we don't need to add this driver creation anymore
	std::string key_name = get_key_name(i_ChannelName);
	io_Drivers.AddDriver(key_name.c_str(), i_DefaultCategory, i_Creator);

	//std::string connectchannel_name = get_connectchannel_name(i_ChannelName);
	//io_Drivers.AddDriver(connectchannel_name.c_str(), i_DefaultCategory, i_Creator);
}

//--------------------------------------------------------------------
// Create static key driver for the given channel
//--------------------------------------------------------------------
tmlnDriver* cmmDriverCreatorVideo::CreateKeyForChannel( tmlnChannelVideo& io_Channel,
													   const char* i_ChannelName,
													   const char i_Code[2],
													   bool i_bShowIcons)
{
	const chDefs::Name c_XXXK = make_key_chunk_name( i_Code );
	tmlnDriverVideo * pDriver = new tmlnDriverVideo( io_Channel, c_XXXK );
	pDriver->SetName(i_ChannelName);
	pDriver->SetInitialTime( 0.0f ); // short key
	pDriver->ShowIcons( i_bShowIcons );
	io_Channel.AddDriver( pDriver );
	return pDriver;
}

//--------------------------------------------------------------------
// Create driver for this channel based on the name used in
//	GatherPossibleDrivers and attach it to the channel.
//--------------------------------------------------------------------

tmlnDriver* cmmDriverCreatorVideo::CreateDriverByName( const char* i_DriverName,
													   tmlnChannelVideo& io_Channel,
													   const char* i_ChannelName,
													   const char i_Code[2],
													   bool i_bShowIcons)
{
	std::string key_name = get_key_name(i_ChannelName);

	if (!::_stricmp(i_DriverName, key_name.c_str()))
	{
		const chDefs::Name c_XXXK = make_key_chunk_name( i_Code );
		tmlnDriverVideo * pDriver = new tmlnDriverVideo( io_Channel, c_XXXK );
		pDriver->SetName(i_ChannelName);
		pDriver->SetInitialTime( 0.0f ); // short key
		pDriver->ShowIcons( i_bShowIcons );
		io_Channel.AddDriver( pDriver );
		return pDriver;
	}
	return 0;
}
//--------------------------------------------------------------------
// Create driver for this channel based on the the info structure
// Note, the chunk name normally comes from the i_Info, but needs
// to be passed in here in order to support older file formats.
//--------------------------------------------------------------------
tmlnDriver* cmmDriverCreatorVideo::CreateDriverFromInfo( tmlnChannelVideo& io_Channel,
														 const char i_Code[2],
													     const tmlnDriverInfo& i_Info,
														 chDefs::Name i_ChunkName)
{
	const chDefs::Name c_XXXK = make_key_chunk_name( i_Code );

	if (i_ChunkName == c_XXXK)
	{
		tmlnDriverVideoInfo driver_info = dynamic_cast<const tmlnDriverVideoInfo&>(i_Info);
//		if (driver_info.m_Value.GetNumNames() > 1)
//			fsAbsolutePathMgr::ResolvePath(driver_info.m_Value, "Videos");
		tmlnDriverVideo* pDriver = new tmlnDriverVideo( io_Channel, c_XXXK );
		pDriver->SetDriverInfo(driver_info);
		io_Channel.AddDriver( pDriver );
		return pDriver;
	}

	return NULL;
}
