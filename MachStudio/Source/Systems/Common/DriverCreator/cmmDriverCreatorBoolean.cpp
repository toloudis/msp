/*****************************************************************************
**	cmmDriverCreatorBoolean.cpp
**
**	Creates common drivers and parsers for boolean channels
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/Common/DriverCreator/cmmDriverCreatorBoolean.hpp"

#include "Support/tmln/tmlnChannelBoolean.hpp"
#include "Support/tmln/tmlnCreator.hpp"
#include "Drivers/Enable/tmlnDriverEnable.hpp"
#include "Drivers/Enable/tmlnDriverEnableInfo.hpp"
#include "Drivers/Enable/tmlnDriverEnableParser.hpp"
#include "Drivers/ConnectChannel/tmlnDriverConnectChannelBoolean.hpp"
#include "Drivers/ConnectChannel/tmlnDriverConnectChannelParser.hpp"
#include "Drivers/UserScript/tmlnDriverUserScriptBoolean.hpp"
#include "Drivers/UserScript/tmlnDriverUserScriptParser.hpp"


namespace
{
	//
	//	Boolean Key (BK)
	//
	const chDefs::Name make_key_chunk_name(const char i_Code[2])
	{
		return chDefs::MakeName(i_Code[0], i_Code[1], 'B', 'K'); 
	}
	std::string get_key_name(const char* i_ChannelName)
	{
		return i_ChannelName; // Just name from channel, since simple key. Could maybe add "Key"
	}

	//
	//	Connect Channel (CZ)
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
void cmmDriverCreatorBoolean::CreateParsers(chDefs::Name i_SystemCode,
						const char i_Code[2])
{
	const chDefs::Name c_XXBK = make_key_chunk_name( i_Code );
	tmlnParser::AddDriverParser(i_SystemCode, c_XXBK,	new tmlnDriverEnableParser(c_XXBK));

	const chDefs::Name c_XXCZ = make_connectchannel_chunk_name( i_Code );
	tmlnParser::AddDriverParser(i_SystemCode, c_XXCZ,	new tmlnDriverConnectChannelParser(c_XXCZ));

	const chDefs::Name c_XXUS = make_userscript_chunk_name( i_Code );
	tmlnParser::AddDriverParser(i_SystemCode, c_XXUS,	new tmlnDriverUserScriptParser(c_XXUS));
}

//--------------------------------------------------------------------
// Helper function for finding what the chunk name will be
//--------------------------------------------------------------------
chDefs::Name cmmDriverCreatorBoolean::GetKeyChunkName(const char i_Code[2])
{
	return make_key_chunk_name(i_Code);
}

chDefs::Name cmmDriverCreatorBoolean::GetConnectChannelChunkName(const char i_Code[2])
{
	return make_connectchannel_chunk_name(i_Code);
}

//--------------------------------------------------------------------
// Gather the possible types of drivers that can be created for
//	for this channel.  Driver names are added through the
//	DriverNameList's API
//--------------------------------------------------------------------
void cmmDriverCreatorBoolean::GatherPossibleDrivers(const char* i_ChannelName,
											      tmlnDriverNameList& io_Drivers,
												  const char *i_DefaultCategory,
												  tmlnDriverCreator* i_Creator)
{
	// With property key buttons, we don't need to add this driver creation anymore
	//std::string key_name = get_key_name(i_ChannelName);
	//io_Drivers.AddDriver(key_name.c_str(), i_DefaultCategory, i_Creator);

	std::string connectchannel_name = get_connectchannel_name(i_ChannelName);
	io_Drivers.AddDriver(connectchannel_name.c_str(), i_DefaultCategory, i_Creator);

	std::string userscript_name = get_userscript_name(i_ChannelName);
	io_Drivers.AddDriver(userscript_name.c_str(), "User Scripts", i_Creator);
}

//--------------------------------------------------------------------
// Create static key driver for the given channel
//--------------------------------------------------------------------
tmlnDriver* cmmDriverCreatorBoolean::CreateKeyForChannel( tmlnChannelBoolean& io_Channel,
													   const char* i_ChannelName,
													   const char i_Code[2],
													   bool i_bShowIcons)
{
	bool bState = io_Channel.GetState();

	const chDefs::Name c_XXBK = make_key_chunk_name( i_Code );
	tmlnDriverEnable * pDriver = new tmlnDriverEnable( io_Channel, c_XXBK );
	pDriver->SetEnabled(bState);
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
tmlnDriver* cmmDriverCreatorBoolean::CreateDriverByName( const char* i_DriverName,
													   tmlnChannelBoolean& io_Channel,
													   const char* i_ChannelName,
													   const char i_Code[2],
													   bool i_bShowIcons)
{
	std::string key_name = get_key_name(i_ChannelName);
	std::string connectchannel_name = get_connectchannel_name(i_ChannelName);
	std::string userscript_name = get_userscript_name(i_ChannelName);

	if (!::_stricmp(i_DriverName, key_name.c_str()))
	{
		const chDefs::Name c_XXBK = make_key_chunk_name( i_Code );
		tmlnDriverEnable * pDriver = new tmlnDriverEnable( io_Channel, c_XXBK );
		pDriver->SetName(i_ChannelName);
		pDriver->SetInitialTime( 0.0f ); // short key
		pDriver->ShowIcons( i_bShowIcons );
		io_Channel.AddDriver( pDriver );
		return pDriver;
	}
	else if (!::_stricmp(i_DriverName, connectchannel_name.c_str()))
	{
		const chDefs::Name c_XXCZ = make_connectchannel_chunk_name( i_Code );
		tmlnDriverConnectChannelBoolean * pDriver = new tmlnDriverConnectChannelBoolean( io_Channel, c_XXCZ );
		
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
		tmlnDriverUserScriptBoolean * pDriver = new tmlnDriverUserScriptBoolean( io_Channel, c_XXUS );
		
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
tmlnDriver* cmmDriverCreatorBoolean::CreateDriverFromInfo( tmlnChannelBoolean& io_Channel,
														 const char i_Code[2],
													     const tmlnDriverInfo& i_Info,
														 chDefs::Name i_ChunkName)
{
	const chDefs::Name c_XXBK = make_key_chunk_name( i_Code );
	const chDefs::Name c_XXCZ = make_connectchannel_chunk_name( i_Code );
	const chDefs::Name c_XXUS = make_userscript_chunk_name( i_Code );

	if (i_ChunkName == c_XXBK)
	{
		const tmlnDriverEnableInfo& driver_info = dynamic_cast<const tmlnDriverEnableInfo&>(i_Info);
		tmlnDriverEnable * pDriver = new tmlnDriverEnable( io_Channel, c_XXBK );
		pDriver->SetDriverInfo(driver_info);
		io_Channel.AddDriver( pDriver );
		return pDriver;
	}
	else if (i_ChunkName == c_XXCZ)
	{
		const tmlnDriverConnectChannelInfo& driver_info = dynamic_cast<const tmlnDriverConnectChannelInfo&>(i_Info);
		tmlnDriverConnectChannelBoolean * pDriver = new tmlnDriverConnectChannelBoolean( io_Channel, c_XXCZ );
		pDriver->SetDriverInfo(driver_info);
		io_Channel.AddDriver( pDriver );
		return pDriver;
	}
	else if (i_ChunkName == c_XXUS)
	{
		const tmlnDriverUserScriptInfo& driver_info = dynamic_cast<const tmlnDriverUserScriptInfo&>(i_Info);
		tmlnDriverUserScriptBoolean * pDriver = new tmlnDriverUserScriptBoolean( io_Channel, c_XXUS );
		pDriver->SetDriverInfo(driver_info);
		io_Channel.AddDriver( pDriver );
		return pDriver;
	}

	return NULL;
}
