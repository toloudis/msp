/*****************************************************************************
**	cmmDriverCreatorTexture.cpp
**
**	Creates common drivers and parsers for color channels
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/Common/DriverCreator/cmmDriverCreatorTexture.hpp"

#include "Support/tmln/tmlnChannelFileName.hpp"
#include "Support/tmln/tmlnCreator.hpp"
#include "Drivers/FileName/tmlnDriverAnimatedFileName.hpp"
#include "Drivers/FileName/tmlnDriverAnimatedFileNameInfo.hpp"
#include "Drivers/FileName/tmlnDriverAnimatedFileNameParser.hpp"
#include "Drivers/FileName/tmlnDriverFileName.hpp"
#include "Drivers/FileName/tmlnDriverFileNameInfo.hpp"
#include "Drivers/FileName/tmlnDriverFileNameParser.hpp"
////#include "Drivers/ConnectChannel/tmlnDriverConnectChannelFileName.hpp"
#include "Drivers/ConnectChannel/tmlnDriverConnectChannelParser.hpp"


namespace
{
	//
	//	teXture filename Key (XK)
	//
	const chDefs::Name make_key_chunk_name(const char i_Code[2])
	{
		return chDefs::MakeName(i_Code[0], i_Code[1], 'X', 'K'); 
	}
	std::string get_key_name(const char* i_ChannelName)
	{
		return i_ChannelName; // Just name from channel, since simple key. Could maybe add "Key"
	}
	//
	//	Animated Texture Driver (AX)
	//
	const chDefs::Name make_texlist_chunk_name(const char i_Code[2])
	{
		return chDefs::MakeName(i_Code[0], i_Code[1], 'A', 'X'); 
	}
	std::string get_texlist_name(const char* i_ChannelName)
	{
		std::string txt;
		//txt = i_ChannelName;
		//txt += " : ";
		txt += "Texture List";
		return txt;
	}
}

//--------------------------------------------------------------------
// Create parsers for driver types created by this creator
//--------------------------------------------------------------------
void cmmDriverCreatorTexture::CreateParsers(chDefs::Name i_SystemCode,
										  const char i_Code[2])
{
	const chDefs::Name c_XXXK = make_key_chunk_name( i_Code );
	tmlnParser::AddDriverParser(i_SystemCode, c_XXXK,	new tmlnDriverFileNameParser(c_XXXK));

	const chDefs::Name c_XXAX = make_texlist_chunk_name( i_Code );
	tmlnParser::AddDriverParser(i_SystemCode, c_XXAX,	new tmlnDriverAnimatedFileNameParser(c_XXAX));
}

//--------------------------------------------------------------------
// Helper function for finding what the chunk name will be
//--------------------------------------------------------------------
chDefs::Name cmmDriverCreatorTexture::GetKeyChunkName(const char i_Code[2])
{
	return make_key_chunk_name(i_Code);
}
chDefs::Name cmmDriverCreatorTexture::GetTextureListChunkName(const char i_Code[2])
{
	return make_texlist_chunk_name(i_Code);
}

//--------------------------------------------------------------------
// Gather the possible types of drivers that can be created for
//	for this channel.  Driver names are added through the
//	DriverNameList's API
//--------------------------------------------------------------------
void cmmDriverCreatorTexture::GatherPossibleDrivers(const char* i_ChannelName,
											      tmlnDriverNameList& io_Drivers,
												  const char *i_DefaultCategory,
												  tmlnDriverCreator* i_Creator)
{
	std::string key_name = get_key_name(i_ChannelName);
	io_Drivers.AddDriver(key_name.c_str(), i_DefaultCategory, i_Creator);

	std::string texlist_name = get_texlist_name(i_ChannelName);
	io_Drivers.AddDriver(texlist_name.c_str(), i_DefaultCategory, i_Creator);

	//std::string connectchannel_name = get_connectchannel_name(i_ChannelName);
	//io_Drivers.AddDriver(connectchannel_name.c_str(), i_DefaultCategory, i_Creator);
}

//--------------------------------------------------------------------
// Create static key driver for the given channel
//--------------------------------------------------------------------
tmlnDriver* cmmDriverCreatorTexture::CreateKeyForChannel( tmlnChannelFileName& io_Channel,
													   const char* i_ChannelName,
													   const char i_Code[2],
													   bool i_bShowIcons)
{
	const chDefs::Name c_XXXK = make_key_chunk_name( i_Code );
	tmlnDriverFileName * pDriver = new tmlnDriverFileName( io_Channel, c_XXXK );
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
tmlnDriver* cmmDriverCreatorTexture::CreateDriverByName( const char* i_DriverName,
													   tmlnChannelFileName& io_Channel,
													   const char* i_ChannelName,
													   const char i_Code[2],
													   bool i_bShowIcons)
{
	std::string key_name = get_key_name(i_ChannelName);
	std::string texlist_name = get_texlist_name(i_ChannelName);
//	std::string connectchannel_name = get_connectchannel_name(i_ChannelName);

	if (!::_stricmp(i_DriverName, key_name.c_str()))
	{
		const chDefs::Name c_XXXK = make_key_chunk_name( i_Code );
		tmlnDriverFileName * pDriver = new tmlnDriverFileName( io_Channel, c_XXXK );
		pDriver->SetName(i_ChannelName);
		pDriver->SetInitialTime( 0.0f ); // short key
		pDriver->ShowIcons( i_bShowIcons );
		io_Channel.AddDriver( pDriver );
		return pDriver;
	}
	else if (!::_stricmp(i_DriverName, texlist_name.c_str()))
	{
		const chDefs::Name c_XXAX = make_texlist_chunk_name( i_Code );
		tmlnDriverAnimatedFileName * pDriver = new tmlnDriverAnimatedFileName( io_Channel, c_XXAX );
		pDriver->SetName("Texture List");
		pDriver->SetInitialTime(0.0f); // has no length until animation is chosen
		pDriver->ShowIcons( i_bShowIcons );
		io_Channel.AddDriver( pDriver );
		return pDriver;
	}
	//else if (!::_stricmp(i_DriverName, connectchannel_name.c_str()))
	//{
	//	const chDefs::Name c_XXCZ = make_connectchannel_chunk_name( i_Code );
	//	tmlnDriverConnectChannelFileName * pDriver = new tmlnDriverConnectChannelFileName( io_Channel, c_XXCZ );

	//	std::string sname = get_connectchannel_name(io_Channel.GetName().c_str());
	//	pDriver->SetName( sname.c_str() );

	//	pDriver->SetInitialTime( -1.0f );
	//	pDriver->ShowIcons( i_bShowIcons );
	//	io_Channel.AddDriver( pDriver );
	//	return pDriver;
	//}
	return 0;
}


//--------------------------------------------------------------------
// Create driver for this channel based on the the info structure
// Note, the chunk name normally comes from the i_Info, but needs
// to be passed in here in order to support older file formats.
//--------------------------------------------------------------------
tmlnDriver* cmmDriverCreatorTexture::CreateDriverFromInfo( tmlnChannelFileName& io_Channel,
														 const char i_Code[2],
													     const tmlnDriverInfo& i_Info,
														 chDefs::Name i_ChunkName)
{
	const chDefs::Name c_XXXK = make_key_chunk_name( i_Code );
	const chDefs::Name c_XXAX = make_texlist_chunk_name( i_Code );
	//const chDefs::Name c_XXCZ = make_connectchannel_chunk_name( i_Code );
	if (i_ChunkName == c_XXXK)
	{
		const tmlnDriverFileNameInfo& driver_info = dynamic_cast<const tmlnDriverFileNameInfo&>(i_Info);
		tmlnDriverFileName * pDriver = new tmlnDriverFileName( io_Channel, c_XXXK );
		pDriver->SetDriverInfo(driver_info);
		io_Channel.AddDriver( pDriver );
		return pDriver;
	}
	else if (i_ChunkName == c_XXAX)
	{
		const tmlnDriverAnimatedFileNameInfo& driver_info = dynamic_cast<const tmlnDriverAnimatedFileNameInfo&>(i_Info);
		tmlnDriverAnimatedFileName * pDriver = new tmlnDriverAnimatedFileName( io_Channel, c_XXAX );
		pDriver->SetDriverInfo(driver_info);
		io_Channel.AddDriver( pDriver );
		return pDriver;
	}
	//else if (i_ChunkName == c_XXCZ)
	//{
	//	const tmlnDriverConnectChannelInfo& driver_info = dynamic_cast<const tmlnDriverConnectChannelInfo&>(i_Info);
	//	tmlnDriverConnectChannelFileName * pDriver = new tmlnDriverConnectChannelFileName( io_Channel, c_XXCZ );
	//	pDriver->SetDriverInfo(driver_info);
	//	io_Channel.AddDriver( pDriver );
	//	return pDriver;
	//}

	return NULL;
}
