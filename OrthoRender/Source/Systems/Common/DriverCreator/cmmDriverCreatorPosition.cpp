/*****************************************************************************
**	cmmDriverCreatorPosition.cpp
**
**	Creates common drivers and parsers for position channels
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/Common/DriverCreator/cmmDriverCreatorPosition.hpp"

#include "Support/tmln/tmlnChannelPosition.hpp"
#include "Support/tmln/tmlnCreator.hpp"
#include "Drivers/Attach/tmlnDriverAttach.hpp"
#include "Drivers/Attach/tmlnDriverAttachInfo.hpp"
#include "Drivers/Attach/tmlnDriverAttachParser.hpp"
#include "Drivers/Position/tmlnDriverPosition.hpp"
#include "Drivers/Position/tmlnDriverPositionInfo.hpp"
#include "Drivers/Position/tmlnDriverPositionParser.hpp"
#include "Drivers/Spline/tmlnDriverSpline.hpp"
#include "Drivers/Spline/tmlnDriverSplineInfo.hpp"
#include "Drivers/Spline/tmlnDriverSplineParser.hpp"
#include "Drivers/ConnectChannel/tmlnDriverConnectChannelPosition.hpp"
#include "Drivers/ConnectChannel/tmlnDriverConnectChannelParser.hpp"
#include "Support/spln/splnSpline.hpp"


namespace
{

	// tmlnDriverPosition
	const chDefs::Name make_key_chunk_name(const char i_Code[2])
	{
		// PK - Position Key
		return chDefs::MakeName(i_Code[0], i_Code[1], 'P', 'K'); 
	}
	std::string get_key_name(const char* i_ChannelName)
	{
		return std::string("Static ") + i_ChannelName;
	}	
	
	// tmlnDriverSpline
	const chDefs::Name make_spline_chunk_name(const char i_Code[2])
	{
		// SP - SPline
		return chDefs::MakeName(i_Code[0], i_Code[1], 'S', 'P'); 
	}
	std::string get_spline_name(const char* i_ChannelName)
	{
		return std::string(i_ChannelName) +  " Spline";
	}
	
	// tmlnDriverAttach
	const chDefs::Name make_attach_chunk_name(const char i_Code[2])
	{
		// AT - Attach
		return chDefs::MakeName(i_Code[0], i_Code[1], 'A', 'T'); 
	}
	std::string get_attach_name(const char* i_ChannelName)
	{
		return std::string(i_ChannelName) +  " Attach";
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
void cmmDriverCreatorPosition::CreateParsers(chDefs::Name i_SystemCode,
											 const char i_Code[2])
{
	const chDefs::Name c_XXPK = make_key_chunk_name( i_Code );
	tmlnParser::AddDriverParser(i_SystemCode, c_XXPK,	new tmlnDriverPositionParser(c_XXPK));

	const chDefs::Name c_XXSP = make_spline_chunk_name( i_Code );
	tmlnParser::AddDriverParser(i_SystemCode, c_XXSP,	new tmlnDriverSplineParser(c_XXSP));

	const chDefs::Name c_XXAT = make_attach_chunk_name( i_Code );
	tmlnParser::AddDriverParser(i_SystemCode, c_XXAT,	new tmlnDriverAttachParser(c_XXAT));

	const chDefs::Name c_XXCZ = make_connectchannel_chunk_name( i_Code );
	tmlnParser::AddDriverParser(i_SystemCode, c_XXCZ,	new tmlnDriverConnectChannelParser(c_XXCZ));
}


//--------------------------------------------------------------------
// Helper function for finding what the chunk name will be
//--------------------------------------------------------------------
chDefs::Name cmmDriverCreatorPosition::GetKeyChunkName(const char i_Code[2])
{
	return make_key_chunk_name(i_Code);
}
chDefs::Name cmmDriverCreatorPosition::GetAttachChunkName(const char i_Code[2])
{
	return make_attach_chunk_name(i_Code);
}
chDefs::Name cmmDriverCreatorPosition::GetSplineChunkName(const char i_Code[2])
{
	return make_spline_chunk_name(i_Code);
}
chDefs::Name cmmDriverCreatorPosition::GetConnectChannelChunkName(const char i_Code[2])
{
	return make_connectchannel_chunk_name(i_Code);
}

//--------------------------------------------------------------------
// Gather the possible types of drivers that can be created for
//	for this channel.  Driver names are added through the
//	DriverNameList's API
//--------------------------------------------------------------------
void cmmDriverCreatorPosition::GatherPossibleDrivers(const char* i_ChannelName,
											      tmlnDriverNameList& io_Drivers,
												  const char *i_DefaultCategory,
												  tmlnDriverCreator* i_Creator)
{
	std::string key_name = get_key_name(i_ChannelName);
	io_Drivers.AddDriver(key_name.c_str(), i_DefaultCategory, i_Creator);

	std::string spline_name = get_spline_name(i_ChannelName);
	io_Drivers.AddDriver(spline_name.c_str(), i_DefaultCategory, i_Creator);

	std::string attach_name = get_attach_name(i_ChannelName);
	io_Drivers.AddDriver(attach_name.c_str(), i_DefaultCategory, i_Creator);

	std::string connectchannel_name = get_connectchannel_name(i_ChannelName);
	io_Drivers.AddDriver(connectchannel_name.c_str(), i_DefaultCategory, i_Creator);
}

//--------------------------------------------------------------------
// Create static key driver for the given channel
//--------------------------------------------------------------------
tmlnDriver* cmmDriverCreatorPosition::CreateKeyForChannel( tmlnChannelPosition& io_Channel,
													   const char* i_ChannelName,
													   const char i_Code[2],
													   bool i_bShowIcons, 
													   pick3dPickObject* i_pParent)
{
	const chDefs::Name c_XXPK = make_key_chunk_name( i_Code );
	tmlnDriverPosition * pDriver = new tmlnDriverPosition( io_Channel, c_XXPK, i_pParent );
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
tmlnDriver* cmmDriverCreatorPosition::CreateDriverByName( const char* i_DriverName,
													   tmlnChannelPosition& io_Channel,
													   const char* i_ChannelName,
													   const char i_Code[2],
													   bool i_bShowIcons, 
													   pick3dPickObject* i_pParent)
{
	std::string key_name = get_key_name(i_ChannelName);
	std::string spline_name = get_spline_name(i_ChannelName);
	std::string attach_name = get_attach_name(i_ChannelName);
	std::string connectchannel_name = get_connectchannel_name(i_ChannelName);

	if (!::_stricmp(i_DriverName, key_name.c_str()))
	{
		const chDefs::Name c_XXPK = make_key_chunk_name( i_Code );
		tmlnDriverPosition * pDriver = new tmlnDriverPosition( io_Channel, c_XXPK, i_pParent );
		pDriver->SetName(i_ChannelName);
		pDriver->SetInitialTime( 0.0f ); // short key
		pDriver->ShowIcons( i_bShowIcons );
		io_Channel.AddDriver( pDriver );
		return pDriver;
	}
	else if (!::_stricmp(i_DriverName, spline_name.c_str()))
	{
		const chDefs::Name c_XXSP = make_spline_chunk_name( i_Code );
		tmlnDriverSpline * pDriver = new tmlnDriverSpline( io_Channel, c_XXSP, i_pParent );
		pDriver->SetName("Spline");
		pDriver->SetInitialTime();
		pDriver->ShowIcons( i_bShowIcons );

		// spline
		splnSpline spline;
		spline.AppendPoint( io_Channel.GetPosition() );
		spline.AppendPoint( io_Channel.GetPosition() + maPoint3d( 0.0f, 2.5f, 0.0f ) );
		pDriver->SetSpline( spline );

		// Attach driver to the appropriate channels
		io_Channel.AddDriver( pDriver );
		return pDriver;
	}
	else if (!::_stricmp(i_DriverName, attach_name.c_str()))
	{
		const chDefs::Name c_XXAT = make_attach_chunk_name( i_Code );
		tmlnDriverAttach * pDriver = new tmlnDriverAttach( io_Channel, c_XXAT );
		pDriver->SetName("Attach");
		pDriver->SetInitialTime();
		pDriver->ShowIcons( i_bShowIcons );
		io_Channel.AddDriver( pDriver );
		return pDriver;
	}
	else if (!::_stricmp(i_DriverName, connectchannel_name.c_str()))
	{
		const chDefs::Name c_XXCZ = make_connectchannel_chunk_name( i_Code );
		tmlnDriverConnectChannelPosition * pDriver = new tmlnDriverConnectChannelPosition( io_Channel, c_XXCZ );
		char cname[64];
		sprintf(&cname[0],"%s Connect Channel", io_Channel.GetName().c_str());
		pDriver->SetName(cname);
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
tmlnDriver* cmmDriverCreatorPosition::CreateDriverFromInfo( tmlnChannelPosition& io_Channel,
														 const char i_Code[2],
													     const tmlnDriverInfo& i_Info,
														 chDefs::Name i_ChunkName, 
														 pick3dPickObject* i_pParent)
{
	const chDefs::Name c_XXPK = make_key_chunk_name( i_Code );
	const chDefs::Name c_XXSP = make_spline_chunk_name( i_Code );
	const chDefs::Name c_XXAT = make_attach_chunk_name( i_Code );
	const chDefs::Name c_XXCZ = make_connectchannel_chunk_name( i_Code );

	if (i_ChunkName == c_XXPK)
	{
		const tmlnDriverPositionInfo& driver_info = dynamic_cast<const tmlnDriverPositionInfo&>(i_Info);
		tmlnDriverPosition * pDriver = new tmlnDriverPosition( io_Channel, c_XXPK, i_pParent );
		pDriver->SetDriverInfo(driver_info);
		io_Channel.AddDriver( pDriver );
		return pDriver;
	}
	else if (i_ChunkName == c_XXSP)
	{
		const tmlnDriverSplineInfo& driver_info = dynamic_cast<const tmlnDriverSplineInfo&>(i_Info);
		tmlnDriverSpline * pDriver = new tmlnDriverSpline( io_Channel, c_XXSP, i_pParent );
		pDriver->SetDriverInfo(driver_info);
		io_Channel.AddDriver( pDriver );
		return pDriver;
	}
	else if (i_ChunkName == c_XXAT)
	{
		const tmlnDriverAttachInfo& driver_info = dynamic_cast<const tmlnDriverAttachInfo&>(i_Info);
		tmlnDriverAttach * pDriver = new tmlnDriverAttach( io_Channel, c_XXAT );
		pDriver->SetDriverInfo(driver_info);
		io_Channel.AddDriver( pDriver );
		return pDriver;
	}
	else if (i_ChunkName == c_XXCZ)
	{
		const tmlnDriverConnectChannelInfo& driver_info = dynamic_cast<const tmlnDriverConnectChannelInfo&>(i_Info);
		tmlnDriverConnectChannelPosition * pDriver = new tmlnDriverConnectChannelPosition( io_Channel, c_XXCZ );
		pDriver->SetDriverInfo(driver_info);
		io_Channel.AddDriver( pDriver );
		return pDriver;
	}

	return NULL;
}
