/*****************************************************************************
**	cmmDriverCreatorOrientation.cpp
**
**	Creates common drivers and parsers for Orientation channels
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/Common/DriverCreator/cmmDriverCreatorOrientation.hpp"

#include "Support/tmln/tmlnChannelOrientation.hpp"
#include "Support/tmln/tmlnCreator.hpp"
#include "Drivers/Attach/tmlnDriverAttach.hpp"
#include "Drivers/Attach/tmlnDriverAttachInfo.hpp"
#include "Drivers/Attach/tmlnDriverAttachParser.hpp"
#include "Drivers/Orientation/tmlnDriverOrientation.hpp"
#include "Drivers/Orientation/tmlnDriverOrientationInfo.hpp"
#include "Drivers/Orientation/tmlnDriverOrientationParser.hpp"
#include "Drivers/Spline/tmlnDriverSpline.hpp"
#include "Drivers/Spline/tmlnDriverSplineInfo.hpp"
#include "Drivers/Spline/tmlnDriverSplineParser.hpp"
#include "Drivers/ConnectChannel/tmlnDriverConnectChannelOrientation.hpp"
#include "Drivers/ConnectChannel/tmlnDriverConnectChannelParser.hpp"
#include "Support/spln/splnSpline.hpp"


namespace
{
	//
	//	Orientation Key (OK)
	//
	const chDefs::Name make_key_chunk_name(const char i_Code[2])
	{
		return chDefs::MakeName(i_Code[0], i_Code[1], 'O', 'K'); 
	}
	std::string get_key_name(const char* i_ChannelName)
	{
		return std::string("Static ") + i_ChannelName;
	}	
	std::string get_euler_name(const char* i_ChannelName)
	{
		return std::string("Static ") + i_ChannelName + std::string(" Euler");
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
void cmmDriverCreatorOrientation::CreateParsers(chDefs::Name i_SystemCode,
											 const char i_Code[2])
{
	const chDefs::Name c_XXOK = make_key_chunk_name( i_Code );
	tmlnParser::AddDriverParser(i_SystemCode, c_XXOK,	new tmlnDriverOrientationParser(c_XXOK));

	const chDefs::Name c_XXCZ = make_connectchannel_chunk_name( i_Code );
	tmlnParser::AddDriverParser(i_SystemCode, c_XXCZ,	new tmlnDriverConnectChannelParser(c_XXCZ));
}


//--------------------------------------------------------------------
// Helper function for finding what the chunk name will be
//--------------------------------------------------------------------
chDefs::Name cmmDriverCreatorOrientation::GetKeyChunkName(const char i_Code[2])
{
	return make_key_chunk_name(i_Code);
}
chDefs::Name cmmDriverCreatorOrientation::GetConnectChannelChunkName(const char i_Code[2])
{
	return make_connectchannel_chunk_name(i_Code);
}

//--------------------------------------------------------------------
// Gather the possible types of drivers that can be created for
//	for this channel.  Driver names are added through the
//	DriverNameList's API
//--------------------------------------------------------------------
void cmmDriverCreatorOrientation::GatherPossibleDrivers(const char* i_ChannelName,
											      tmlnDriverNameList& io_Drivers,
												  const char *i_DefaultCategory,
												  tmlnDriverCreator* i_Creator)
{
	std::string key_name = get_key_name(i_ChannelName);
	io_Drivers.AddDriver(key_name.c_str(), i_DefaultCategory, i_Creator);

	std::string euler_name = get_euler_name(i_ChannelName);
	io_Drivers.AddDriver(euler_name.c_str(), i_DefaultCategory, i_Creator);

	std::string connectchannel_name = get_connectchannel_name(i_ChannelName);
	io_Drivers.AddDriver(connectchannel_name.c_str(), i_DefaultCategory, i_Creator);
}

//--------------------------------------------------------------------
// Create static key driver for the given channel
//--------------------------------------------------------------------
tmlnDriver* cmmDriverCreatorOrientation::CreateKeyForChannel( tmlnChannelOrientation& io_Channel,
													   const char* i_ChannelName,
													   const char i_Code[2],
													   bool i_bShowIcons)
{
	const chDefs::Name c_XXOK = make_key_chunk_name( i_Code );
	tmlnDriverOrientation * pDriver = new tmlnDriverOrientation( io_Channel, c_XXOK );
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
tmlnDriver* cmmDriverCreatorOrientation::CreateDriverByName( const char* i_DriverName,
															tmlnChannelOrientation& io_Channel,
															const char* i_ChannelName,
															const char i_Code[2],
															bool i_bShowIcons )
{
	std::string key_name = get_key_name(i_ChannelName);
	std::string euler_name = get_euler_name(i_ChannelName);
	std::string connectchannel_name = get_connectchannel_name(i_ChannelName);

	if (!::_stricmp(i_DriverName, key_name.c_str()))
	{
		const chDefs::Name c_XXOK = make_key_chunk_name( i_Code );
		tmlnDriverOrientation * pDriver = new tmlnDriverOrientation( io_Channel, c_XXOK );
		pDriver->SetName(i_ChannelName);
		pDriver->SetInitialTime( 0.0f ); // short key
		pDriver->ShowIcons( i_bShowIcons );
		io_Channel.AddDriver( pDriver );
		return pDriver;
	}
	else if (!::_stricmp(i_DriverName, euler_name.c_str()))
	{
		const chDefs::Name c_XXOK = make_key_chunk_name( i_Code );
		tmlnDriverOrientation * pDriver = new tmlnDriverOrientation( io_Channel, c_XXOK );
		
		// Euler orientation key is same as static interpolation, but
		// it sets the interpolation method to "euler angles" not quaternion.
		pDriver->SetEulerInterpolation( true );

		pDriver->SetName(i_ChannelName);
		pDriver->SetInitialTime( 0.0f ); // short key
		pDriver->ShowIcons( i_bShowIcons );
		io_Channel.AddDriver( pDriver );
		return pDriver;
	}
	else if (!::_stricmp(i_DriverName, connectchannel_name.c_str()))
	{
		const chDefs::Name c_XXCZ = make_connectchannel_chunk_name( i_Code );
		tmlnDriverConnectChannelOrientation * pDriver = new tmlnDriverConnectChannelOrientation( io_Channel, c_XXCZ );

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
tmlnDriver* cmmDriverCreatorOrientation::CreateDriverFromInfo( tmlnChannelOrientation& io_Channel,
																const char i_Code[2],
																const tmlnDriverInfo& i_Info,
																chDefs::Name i_ChunkName )
{
	const chDefs::Name c_XXOK = make_key_chunk_name( i_Code );
	const chDefs::Name c_XXCZ = make_connectchannel_chunk_name( i_Code );

	if (i_ChunkName == c_XXOK)
	{
		const tmlnDriverOrientationInfo& driver_info = dynamic_cast<const tmlnDriverOrientationInfo&>(i_Info);
		tmlnDriverOrientation * pDriver = new tmlnDriverOrientation( io_Channel, c_XXOK );
		pDriver->SetDriverInfo(driver_info);
		io_Channel.AddDriver( pDriver );
		return pDriver;
	}
	else if (i_ChunkName == c_XXCZ)
	{
		const tmlnDriverConnectChannelInfo& driver_info = dynamic_cast<const tmlnDriverConnectChannelInfo&>(i_Info);
		tmlnDriverConnectChannelOrientation * pDriver = new tmlnDriverConnectChannelOrientation( io_Channel, c_XXCZ );
		pDriver->SetDriverInfo(driver_info);
		io_Channel.AddDriver( pDriver );
		return pDriver;
	}

	return NULL;
}
