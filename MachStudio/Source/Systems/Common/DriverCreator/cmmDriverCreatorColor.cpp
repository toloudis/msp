/*****************************************************************************
**	cmmDriverCreatorColor.cpp
**
**	Creates common drivers and parsers for color channels
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/Common/DriverCreator/cmmDriverCreatorColor.hpp"

#include "Support/tmln/tmlnChannelColor.hpp"
#include "Support/tmln/tmlnCreator.hpp"
#include "Drivers/Color/tmlnDriverColor.hpp"
#include "Drivers/Color/tmlnDriverColorInfo.hpp"
#include "Drivers/Color/tmlnDriverColorParser.hpp"
#include "Drivers/ColorFlicker/tmlnDriverColorFlicker.hpp"
#include "Drivers/ColorFlicker/tmlnDriverColorFlickerInfo.hpp"
#include "Drivers/ColorFlicker/tmlnDriverColorFlickerParser.hpp"
#include "Drivers/ConnectChannel/tmlnDriverConnectChannelColor.hpp"
#include "Drivers/ConnectChannel/tmlnDriverConnectChannelParser.hpp"
#include "Drivers/UserScript/tmlnDriverUserScriptColor.hpp"
#include "Drivers/UserScript/tmlnDriverUserScriptParser.hpp"


namespace
{
	//
	//	Color Key (CK)
	//
	const chDefs::Name make_key_chunk_name(const char i_Code[2])
	{
		return chDefs::MakeName(i_Code[0], i_Code[1], 'C', 'K'); 
	}
	std::string get_key_name(const char* i_ChannelName)
	{
		return i_ChannelName; // Just name from channel, since simple key. Could maybe add "Key"
	}

	//
	//	Color Flicker Key (CF)
	//
	const chDefs::Name make_flicker_chunk_name(const char i_Code[2])
	{
		return chDefs::MakeName(i_Code[0], i_Code[1], 'C', 'F'); 
	}
	std::string get_flicker_name(const char* i_ChannelName)
	{
		std::string txt;
		//txt = i_ChannelName;
		//txt += " : ";
		txt += "Color Flicker";
		return txt;
	}

	//
	//	Connect Channel Color Key (CZ)
	//
	const chDefs::Name make_connectchannel_chunk_name(const char i_Code[2])
	{
		return chDefs::MakeName(i_Code[0], i_Code[1], 'C', 'Z'); 
	}
	std::string get_connectchannel_name(const char* i_ChannelName)
	{
		std::string txt;
		txt = i_ChannelName;
		txt += " : ";
		txt += "Connect Channel";
		return txt;
	}

	//
	//	User Script (US)
	//
	const chDefs::Name make_userscript_chunk_name(const char i_Code[2])
	{
		return chDefs::MakeName(i_Code[0], i_Code[1], 'U', 'S'); 
	}
	std::string get_userscript_name(const char* i_ChannelName)
	{
		std::string txt;
		txt = i_ChannelName;
		txt += " : ";
		txt += "User Script";
		return txt;
	}
}

//--------------------------------------------------------------------
// Create parsers for driver types created by this creator
//--------------------------------------------------------------------
void cmmDriverCreatorColor::CreateParsers(chDefs::Name i_SystemCode,
										  const char i_Code[2])
{
	const chDefs::Name c_XXCK = make_key_chunk_name( i_Code );
	tmlnParser::AddDriverParser(i_SystemCode, c_XXCK,	new tmlnDriverColorParser(c_XXCK));

	const chDefs::Name c_XXCF = make_flicker_chunk_name( i_Code );
	tmlnParser::AddDriverParser(i_SystemCode, c_XXCF,	new tmlnDriverColorFlickerParser(c_XXCF));

	const chDefs::Name c_XXCZ = make_connectchannel_chunk_name( i_Code );
	tmlnParser::AddDriverParser(i_SystemCode, c_XXCZ,	new tmlnDriverConnectChannelParser(c_XXCZ));

	const chDefs::Name c_XXUS = make_userscript_chunk_name( i_Code );
	tmlnParser::AddDriverParser(i_SystemCode, c_XXUS,	new tmlnDriverUserScriptParser(c_XXUS));
}

//--------------------------------------------------------------------
// Helper function for finding what the chunk name will be
//--------------------------------------------------------------------
chDefs::Name cmmDriverCreatorColor::GetKeyChunkName(const char i_Code[2])
{
	return make_key_chunk_name(i_Code);
}
chDefs::Name cmmDriverCreatorColor::GetFlickerChunkName(const char i_Code[2])
{
	return make_flicker_chunk_name(i_Code);
}
chDefs::Name cmmDriverCreatorColor::GetConnectChannelChunkName(const char i_Code[2])
{
	return make_connectchannel_chunk_name(i_Code);
}

//--------------------------------------------------------------------
// Gather the possible types of drivers that can be created for
//	for this channel.  Driver names are added through the
//	DriverNameList's API
//--------------------------------------------------------------------
void cmmDriverCreatorColor::GatherPossibleDrivers(const char* i_ChannelName,
											      tmlnDriverNameList& io_Drivers,
												  const char *i_DefaultCategory,
												  tmlnDriverCreator* i_Creator)
{
	// With property key buttons, we don't need to add this driver creation anymore
	//std::string key_name = get_key_name(i_ChannelName);
	//io_Drivers.AddDriver(key_name.c_str(), i_DefaultCategory, i_Creator);

	std::string flicker_name = get_flicker_name(i_ChannelName);
	io_Drivers.AddDriver(flicker_name.c_str(), i_DefaultCategory, i_Creator);

	std::string connectchannel_name = get_connectchannel_name(i_ChannelName);
	io_Drivers.AddDriver(connectchannel_name.c_str(), i_DefaultCategory, i_Creator);

	std::string userscript_name = get_userscript_name(i_ChannelName);
	io_Drivers.AddDriver(userscript_name.c_str(), "User Scripts", i_Creator);
}

//--------------------------------------------------------------------
// Create static key driver for the given channel
//--------------------------------------------------------------------
tmlnDriver* cmmDriverCreatorColor::CreateKeyForChannel( tmlnChannelColor& io_Channel,
													   const char* i_ChannelName,
													   const char i_Code[2],
													   bool i_bShowIcons)
{
	const chDefs::Name c_XXCK = make_key_chunk_name( i_Code );
	tmlnDriverColor * pDriver = new tmlnDriverColor( io_Channel, c_XXCK );
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
tmlnDriver* cmmDriverCreatorColor::CreateDriverByName( const char* i_DriverName,
													   tmlnChannelColor& io_Channel,
													   const char* i_ChannelName,
													   const char i_Code[2],
													   bool i_bShowIcons)
{
	std::string key_name = get_key_name(i_ChannelName);
	std::string flicker_name = get_flicker_name(i_ChannelName);
	std::string connectchannel_name = get_connectchannel_name(i_ChannelName);
	std::string userscript_name = get_userscript_name(i_ChannelName);

	if (!::_stricmp(i_DriverName, key_name.c_str()))
	{
		const chDefs::Name c_XXCK = make_key_chunk_name( i_Code );
		tmlnDriverColor * pDriver = new tmlnDriverColor( io_Channel, c_XXCK );
		pDriver->SetName(i_ChannelName);
		pDriver->SetInitialTime( 0.0f ); // short key
		pDriver->ShowIcons( i_bShowIcons );
		io_Channel.AddDriver( pDriver );
		return pDriver;
	}
	else if (!::_stricmp(i_DriverName, flicker_name.c_str()))
	{
		const chDefs::Name c_XXCF = make_flicker_chunk_name( i_Code );
		tmlnDriverColorFlicker * pDriver = new tmlnDriverColorFlicker( io_Channel, c_XXCF );
		pDriver->SetName("Color Flicker");
		pDriver->SetInitialTime( 0.0f ); // short key
		pDriver->ShowIcons( i_bShowIcons );
		io_Channel.AddDriver( pDriver );
		return pDriver;
	}
	else if (!::_stricmp(i_DriverName, connectchannel_name.c_str()))
	{
		const chDefs::Name c_XXCZ = make_connectchannel_chunk_name( i_Code );
		tmlnDriverConnectChannelColor * pDriver = new tmlnDriverConnectChannelColor( io_Channel, c_XXCZ );

		std::string sname = get_connectchannel_name(io_Channel.GetName().c_str());
		pDriver->SetName( sname.c_str() );

		pDriver->SetInitialTime( -1.0f );
		pDriver->ShowIcons( i_bShowIcons );
		io_Channel.AddDriver( pDriver );
		return pDriver;
	}	
	else if (!::_stricmp(i_DriverName, userscript_name.c_str()))
	{
		const chDefs::Name c_XXUS = make_userscript_chunk_name( i_Code );
		tmlnDriverUserScriptColor * pDriver = new tmlnDriverUserScriptColor( io_Channel, c_XXUS );

		std::string sname = get_userscript_name(io_Channel.GetName().c_str());
		pDriver->SetName( sname.c_str() );

		pDriver->SetInitialTime( -1.0f );
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
tmlnDriver* cmmDriverCreatorColor::CreateDriverFromInfo( tmlnChannelColor& io_Channel,
														 const char i_Code[2],
													     const tmlnDriverInfo& i_Info,
														 chDefs::Name i_ChunkName)
{
	const chDefs::Name c_XXCK = make_key_chunk_name( i_Code );
	const chDefs::Name c_XXCF = make_flicker_chunk_name( i_Code );
	const chDefs::Name c_XXCZ = make_connectchannel_chunk_name( i_Code );
	const chDefs::Name c_XXUS = make_userscript_chunk_name( i_Code );

	if (i_ChunkName == c_XXCK)
	{
		const tmlnDriverColorInfo& driver_info = dynamic_cast<const tmlnDriverColorInfo&>(i_Info);
		tmlnDriverColor * pDriver = new tmlnDriverColor( io_Channel, c_XXCK );
		pDriver->SetDriverInfo(driver_info);
		io_Channel.AddDriver( pDriver );
		return pDriver;
	}
	else if (i_ChunkName == c_XXCF)
	{
		const tmlnDriverColorFlickerInfo& driver_info = dynamic_cast<const tmlnDriverColorFlickerInfo&>(i_Info);
		tmlnDriverColorFlicker * pDriver = new tmlnDriverColorFlicker( io_Channel, c_XXCF );
		pDriver->SetDriverInfo(driver_info);
		io_Channel.AddDriver( pDriver );
		return pDriver;
	}
	else if (i_ChunkName == c_XXCZ)
	{
		const tmlnDriverConnectChannelInfo& driver_info = dynamic_cast<const tmlnDriverConnectChannelInfo&>(i_Info);
		tmlnDriverConnectChannelColor * pDriver = new tmlnDriverConnectChannelColor( io_Channel, c_XXCZ );
		pDriver->SetDriverInfo(driver_info);
		io_Channel.AddDriver( pDriver );
		return pDriver;
	}
	else if (i_ChunkName == c_XXUS)
	{
		const tmlnDriverUserScriptInfo& driver_info = dynamic_cast<const tmlnDriverUserScriptInfo&>(i_Info);
		tmlnDriverUserScriptColor * pDriver = new tmlnDriverUserScriptColor( io_Channel, c_XXUS );
		pDriver->SetDriverInfo(driver_info);
		io_Channel.AddDriver( pDriver );
		return pDriver;
	}

	return NULL;
}
