/*****************************************************************************
**	cmmDriverCreatorFloat.cpp
**
**	Creates common drivers and parsers for float channels
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/Common/DriverCreator/cmmDriverCreatorFloat.hpp"

#include "Support/tmln/tmlnChannelFloat.hpp"
#include "Support/tmln/tmlnCreator.hpp"
#include "Drivers/Float/tmlnDriverFloat.hpp"
#include "Drivers/Float/tmlnDriverFloatInfo.hpp"
#include "Drivers/Float/tmlnDriverFloatParser.hpp"
#include "Drivers/ConnectChannel/tmlnDriverConnectChannelFloat.hpp"
#include "Drivers/ConnectChannel/tmlnDriverConnectChannelParser.hpp"


namespace
{
	//
	//	Float Key (FK)
	//
	const chDefs::Name make_key_chunk_name(const char i_Code[2])
	{
		return chDefs::MakeName(i_Code[0], i_Code[1], 'F', 'K'); 
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
}

//--------------------------------------------------------------------
// Create parsers for driver types created by this creator
//--------------------------------------------------------------------
void cmmDriverCreatorFloat::CreateParsers(chDefs::Name i_SystemCode,
						const char i_Code[2])
{
	const chDefs::Name c_XXFK = make_key_chunk_name( i_Code );
	tmlnParser::AddDriverParser(i_SystemCode, c_XXFK,	new tmlnDriverFloatParser(c_XXFK));

	const chDefs::Name c_XXCZ = make_connectchannel_chunk_name( i_Code );
	tmlnParser::AddDriverParser(i_SystemCode, c_XXCZ,	new tmlnDriverConnectChannelParser(c_XXCZ));
}

//--------------------------------------------------------------------
// Helper function for finding what the chunk name will be
//--------------------------------------------------------------------
chDefs::Name cmmDriverCreatorFloat::GetKeyChunkName(const char i_Code[2])
{
	return make_key_chunk_name(i_Code);
}

chDefs::Name cmmDriverCreatorFloat::GetConnectChannelChunkName(const char i_Code[2])
{
	return make_connectchannel_chunk_name(i_Code);
}

//--------------------------------------------------------------------
// Gather the possible types of drivers that can be created for
//	for this channel.  Driver names are added through the
//	DriverNameList's API
//--------------------------------------------------------------------
void cmmDriverCreatorFloat::GatherPossibleDrivers(const char* i_ChannelName,
											      tmlnDriverNameList& io_Drivers,
												  const char *i_DefaultCategory,
												  tmlnDriverCreator* i_Creator)
{
	std::string key_name = get_key_name(i_ChannelName);
	io_Drivers.AddDriver(key_name.c_str(), i_DefaultCategory, i_Creator);

	std::string connectchannel_name = get_connectchannel_name(i_ChannelName);
	io_Drivers.AddDriver(connectchannel_name.c_str(), i_DefaultCategory, i_Creator);
}

//--------------------------------------------------------------------
// Create static key driver for the given channel
//--------------------------------------------------------------------
tmlnDriver* cmmDriverCreatorFloat::CreateKeyForChannel( tmlnChannelFloat& io_Channel,
													   const char* i_ChannelName,
													   const char i_Code[2],
													   bool i_bShowIcons)
{
	const chDefs::Name c_XXFK = make_key_chunk_name( i_Code );
	tmlnDriverFloat * pDriver = new tmlnDriverFloat( io_Channel, c_XXFK );
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
tmlnDriver* cmmDriverCreatorFloat::CreateDriverByName( const char* i_DriverName,
													   tmlnChannelFloat& io_Channel,
													   const char* i_ChannelName,
													   const char i_Code[2],
													   bool i_bShowIcons)
{
	std::string key_name = get_key_name(i_ChannelName);
	std::string connectchannel_name = get_connectchannel_name(i_ChannelName);

	if (!::_stricmp(i_DriverName, key_name.c_str()))
	{
		const chDefs::Name c_XXFK = make_key_chunk_name( i_Code );
		tmlnDriverFloat * pDriver = new tmlnDriverFloat( io_Channel, c_XXFK );
		pDriver->SetName(i_ChannelName);
		pDriver->SetInitialTime( 0.0f ); // short key
		pDriver->ShowIcons( i_bShowIcons );
		io_Channel.AddDriver( pDriver );
		return pDriver;
	}
	else if (!::_stricmp(i_DriverName, connectchannel_name.c_str()))
	{
		const chDefs::Name c_XXCZ = make_connectchannel_chunk_name( i_Code );
		tmlnDriverConnectChannelFloat * pDriver = new tmlnDriverConnectChannelFloat( io_Channel, c_XXCZ );

		std::string sname = get_connectchannel_name(io_Channel.GetName().c_str());
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
tmlnDriver* cmmDriverCreatorFloat::CreateDriverFromInfo( tmlnChannelFloat& io_Channel,
														 const char i_Code[2],
													     const tmlnDriverInfo& i_Info,
														 chDefs::Name i_ChunkName)
{
	const chDefs::Name c_XXFK = make_key_chunk_name( i_Code );
	const chDefs::Name c_XXCZ = make_connectchannel_chunk_name( i_Code );

	if (i_ChunkName == c_XXFK)
	{
		const tmlnDriverFloatInfo& driver_info = dynamic_cast<const tmlnDriverFloatInfo&>(i_Info);
		tmlnDriverFloat * pDriver = new tmlnDriverFloat( io_Channel, c_XXFK );
		pDriver->SetDriverInfo(driver_info);
		io_Channel.AddDriver( pDriver );
		return pDriver;
	}
	else if (i_ChunkName == c_XXCZ)
	{
		const tmlnDriverConnectChannelInfo& driver_info = dynamic_cast<const tmlnDriverConnectChannelInfo&>(i_Info);
		tmlnDriverConnectChannelFloat * pDriver = new tmlnDriverConnectChannelFloat( io_Channel, c_XXCZ );
		pDriver->SetDriverInfo(driver_info);
		io_Channel.AddDriver( pDriver );
		return pDriver;
	}

	return NULL;
}
