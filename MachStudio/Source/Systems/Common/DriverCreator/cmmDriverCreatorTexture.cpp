/*****************************************************************************
**	cmmDriverCreatorTexture.cpp
**
**	Creates common drivers and parsers for color channels
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/Common/DriverCreator/cmmDriverCreatorTexture.hpp"

#include "Support/tmln/tmlnChannelFilePath.hpp"
#include "Support/tmln/tmlnChannelTextureFileName.hpp"
#include "Support/tmln/tmlnCreator.hpp"
#include "Drivers/FilePath/tmlnDriverAnimatedFilePath.hpp"
#include "Drivers/FilePath/tmlnDriverAnimatedFilePathInfo.hpp"
#include "Drivers/FilePath/tmlnDriverAnimatedFilePathParser.hpp"
#include "Drivers/FilePath/tmlnDriverFilePath.hpp"
#include "Drivers/FilePath/tmlnDriverFilePathInfo.hpp"
#include "Drivers/FilePath/tmlnDriverFilePathParser.hpp"
#include "Drivers/TextureFileName/tmlnDriverAnimatedTextureFileName.hpp"
#include "Drivers/TextureFileName/tmlnDriverAnimatedTextureFileNameInfo.hpp"
#include "Drivers/TextureFileName/tmlnDriverAnimatedTextureFileNameParser.hpp"
#include "Drivers/TextureFileName/tmlnDriverTextureFileName.hpp"
#include "Drivers/TextureFileName/tmlnDriverTextureFileNameInfo.hpp"
#include "Drivers/TextureFileName/tmlnDriverTextureFileNameParser.hpp"
////#include "Drivers/ConnectChannel/tmlnDriverConnectChannelFilePath.hpp"
#include "Drivers/ConnectChannel/tmlnDriverConnectChannelParser.hpp"

#include "Core/Fs/fsAbsolutePathMgr.hpp"


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
	
	//
	//	teXture filename Key (FK)
	//
	const chDefs::Name make_tex_key_chunk_name(const char i_Code[2])
	{
		return chDefs::MakeName(i_Code[0], i_Code[1], 'F', 'K'); 
	}
	std::string get_tex_key_name(const char* i_ChannelName)
	{
		return i_ChannelName; // Just name from channel, since simple key. Could maybe add "Key"
	}

	//
	//	Animated Texture Filename Key (AK)
	//
	const chDefs::Name make_texname_texlist_chunk_name(const char i_Code[2])
	{
		return chDefs::MakeName(i_Code[0], i_Code[1], 'A', 'K'); 
	}
	std::string get_texname_texlist_name(const char* i_ChannelName)
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
	tmlnParser::AddDriverParser(i_SystemCode, c_XXXK,	new tmlnDriverFilePathParser(c_XXXK));

	const chDefs::Name c_XXAX = make_texlist_chunk_name( i_Code );
	tmlnParser::AddDriverParser(i_SystemCode, c_XXAX,	new tmlnDriverAnimatedFilePathParser(c_XXAX));

	const chDefs::Name c_XXFK = make_tex_key_chunk_name( i_Code );
	tmlnParser::AddDriverParser(i_SystemCode, c_XXFK,	new tmlnDriverTextureFileNameParser(c_XXFK));

	const chDefs::Name c_XXAK = make_texname_texlist_chunk_name( i_Code );
	tmlnParser::AddDriverParser(i_SystemCode, c_XXAK,	new tmlnDriverAnimatedTextureFileNameParser(c_XXAK));
}

//--------------------------------------------------------------------
// Create parsers for driver types created by this creator
// Use this for TextureFileName types
//--------------------------------------------------------------------
void cmmDriverCreatorTexture::CreateParsersTexture(chDefs::Name i_SystemCode,
												   const char i_Code[2])
{
	const chDefs::Name c_XXXK = make_key_chunk_name( i_Code );
	tmlnParser::AddDriverParser(i_SystemCode, c_XXXK,	new tmlnDriverTextureFileNameParser(c_XXXK));

	const chDefs::Name c_XXAX = make_texlist_chunk_name( i_Code );
	tmlnParser::AddDriverParser(i_SystemCode, c_XXAX,	new tmlnDriverAnimatedFilePathParser(c_XXAX));

	const chDefs::Name c_XXFK = make_tex_key_chunk_name( i_Code );
	tmlnParser::AddDriverParser(i_SystemCode, c_XXFK,	new tmlnDriverTextureFileNameParser(c_XXFK));

	const chDefs::Name c_XXAK = make_texname_texlist_chunk_name( i_Code );
	tmlnParser::AddDriverParser(i_SystemCode, c_XXAK,	new tmlnDriverAnimatedTextureFileNameParser(c_XXAK));
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
chDefs::Name cmmDriverCreatorTexture::GetTextureKeyChunkName(const char i_Code[2])
{
	return make_tex_key_chunk_name(i_Code);
}
chDefs::Name cmmDriverCreatorTexture::GetTextureNameListChunkName(const char i_Code[2])
{
	return make_texname_texlist_chunk_name(i_Code);
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
	// With property key buttons, we don't need to add this driver creation anymore
	//std::string key_name = get_key_name(i_ChannelName);
	//io_Drivers.AddDriver(key_name.c_str(), i_DefaultCategory, i_Creator);

	std::string texlist_name = get_texlist_name(i_ChannelName);
	io_Drivers.AddDriver(texlist_name.c_str(), i_DefaultCategory, i_Creator);

	//std::string connectchannel_name = get_connectchannel_name(i_ChannelName);
	//io_Drivers.AddDriver(connectchannel_name.c_str(), i_DefaultCategory, i_Creator);
}

//--------------------------------------------------------------------
// Create static key driver for the given channel
//--------------------------------------------------------------------
tmlnDriver* cmmDriverCreatorTexture::CreateKeyForChannel( tmlnChannelFilePath& io_Channel,
													   const char* i_ChannelName,
													   const char i_Code[2],
													   bool i_bShowIcons)
{
	//const chDefs::Name c_XXXK = make_key_chunk_name( i_Code );
	//tmlnDriverFilePath * pDriver = new tmlnDriverFilePath( io_Channel, c_XXXK );
	const chDefs::Name c_XXAX = make_texlist_chunk_name( i_Code );
	tmlnDriverAnimatedFilePath * pDriver = new tmlnDriverAnimatedFilePath( io_Channel, c_XXAX );
	pDriver->SetName(i_ChannelName);
	pDriver->SetInitialTime( 0.0f ); // short key
	pDriver->ShowIcons( i_bShowIcons );
	io_Channel.AddDriver( pDriver );
	return pDriver;
}

//--------------------------------------------------------------------
// Create static key driver for the given channel
//--------------------------------------------------------------------
tmlnDriver* cmmDriverCreatorTexture::CreateKeyForChannel( tmlnChannelTextureFileName& io_Channel,
													   const char* i_ChannelName,
													   const char i_Code[2],
													   bool i_bShowIcons)
{
	//const chDefs::Name c_XXXK = make_key_chunk_name( i_Code );
	//tmlnDriverFilePath * pDriver = new tmlnDriverFilePath( io_Channel, c_XXXK );
	const chDefs::Name c_XXAK = make_texname_texlist_chunk_name( i_Code );
	tmlnDriverAnimatedTextureFileName * pDriver = new tmlnDriverAnimatedTextureFileName( io_Channel, c_XXAK );
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
													   tmlnChannelFilePath& io_Channel,
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
		tmlnDriverFilePath * pDriver = new tmlnDriverFilePath( io_Channel, c_XXXK );
		pDriver->SetName(i_ChannelName);
		pDriver->SetInitialTime( 0.0f ); // short key
		pDriver->ShowIcons( i_bShowIcons );
		io_Channel.AddDriver( pDriver );
		return pDriver;
	}
	else if (!::_stricmp(i_DriverName, texlist_name.c_str()))
	{
		const chDefs::Name c_XXAX = make_texlist_chunk_name( i_Code );
		tmlnDriverAnimatedFilePath * pDriver = new tmlnDriverAnimatedFilePath( io_Channel, c_XXAX );
		pDriver->SetName("Texture List");
		pDriver->SetInitialTime(0.0f); // has no length until animation is chosen
		pDriver->ShowIcons( i_bShowIcons );
		io_Channel.AddDriver( pDriver );
		return pDriver;
	}
	//else if (!::_stricmp(i_DriverName, connectchannel_name.c_str()))
	//{
	//	const chDefs::Name c_XXCZ = make_connectchannel_chunk_name( i_Code );
	//	tmlnDriverConnectChannelFilePath * pDriver = new tmlnDriverConnectChannelFilePath( io_Channel, c_XXCZ );

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
// Create driver for this channel based on the name used in
//	GatherPossibleDrivers and attach it to the channel.
//--------------------------------------------------------------------
tmlnDriver* cmmDriverCreatorTexture::CreateDriverByName( const char* i_DriverName,
													   tmlnChannelTextureFileName& io_Channel,
													   const char* i_ChannelName,
													   const char i_Code[2],
													   bool i_bShowIcons)
{
	std::string key_name = get_tex_key_name(i_ChannelName);
	std::string texlist_name = get_texname_texlist_name(i_ChannelName);
//	std::string connectchannel_name = get_connectchannel_name(i_ChannelName);

	if (!::_stricmp(i_DriverName, key_name.c_str()))
	{
		const chDefs::Name c_XXFK = make_tex_key_chunk_name( i_Code );
		tmlnDriverTextureFileName * pDriver = new tmlnDriverTextureFileName( io_Channel, c_XXFK );
		pDriver->SetName(i_ChannelName);
		pDriver->SetInitialTime( 0.0f ); // short key
		pDriver->ShowIcons( i_bShowIcons );
		io_Channel.AddDriver( pDriver );
		return pDriver;
	}
	else if (!::_stricmp(i_DriverName, texlist_name.c_str()))
	{
		const chDefs::Name c_XXAK = make_texname_texlist_chunk_name( i_Code );
		tmlnDriverAnimatedTextureFileName * pDriver = new tmlnDriverAnimatedTextureFileName( io_Channel, c_XXAK );
		pDriver->SetName("Texture List");
		pDriver->SetInitialTime(0.0f); // has no length until animation is chosen
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
tmlnDriver* cmmDriverCreatorTexture::CreateDriverFromInfo( tmlnChannelFilePath& io_Channel,
														 const char i_Code[2],
													     const tmlnDriverInfo& i_Info,
														 chDefs::Name i_ChunkName)
{
	const chDefs::Name c_XXXK = make_key_chunk_name( i_Code );
	const chDefs::Name c_XXAX = make_texlist_chunk_name( i_Code );
	//const chDefs::Name c_XXCZ = make_connectchannel_chunk_name( i_Code );
	if (i_ChunkName == c_XXXK)
	{
		tmlnDriverFilePathInfo driver_info = dynamic_cast<const tmlnDriverFilePathInfo&>(i_Info);
		if (driver_info.m_Value.GetNumNames() > 1)
			fsAbsolutePathMgr::ResolvePath(driver_info.m_Value, "Textures");
		tmlnDriverFilePath * pDriver = new tmlnDriverFilePath( io_Channel, c_XXXK );
		pDriver->SetDriverInfo(driver_info);
		io_Channel.AddDriver( pDriver );
		return pDriver;
	}
	else if (i_ChunkName == c_XXAX)
	{
		tmlnDriverAnimatedFilePathInfo driver_info = dynamic_cast<const tmlnDriverAnimatedFilePathInfo&>(i_Info);
		if (driver_info.m_FirstFilePath.GetNumNames() > 1)
			fsAbsolutePathMgr::ResolvePath(driver_info.m_FirstFilePath, "Textures");
		tmlnDriverAnimatedFilePath * pDriver = new tmlnDriverAnimatedFilePath( io_Channel, c_XXAX );
		pDriver->SetDriverInfo(driver_info);
		io_Channel.AddDriver( pDriver );
		return pDriver;
	}
	//else if (i_ChunkName == c_XXCZ)
	//{
	//	const tmlnDriverConnectChannelInfo& driver_info = dynamic_cast<const tmlnDriverConnectChannelInfo&>(i_Info);
	//	tmlnDriverConnectChannelFilePath * pDriver = new tmlnDriverConnectChannelFilePath( io_Channel, c_XXCZ );
	//	pDriver->SetDriverInfo(driver_info);
	//	io_Channel.AddDriver( pDriver );
	//	return pDriver;
	//}

	return NULL;
}

tmlnDriver* cmmDriverCreatorTexture::CreateDriverFromInfo( tmlnChannelTextureFileName& io_Channel,
														 const char i_Code[2],
													     const tmlnDriverInfo& i_Info,
														 chDefs::Name i_ChunkName)
{
	const chDefs::Name c_XXFK = make_tex_key_chunk_name( i_Code );
	const chDefs::Name c_XXAK = make_texname_texlist_chunk_name( i_Code );
	//const chDefs::Name c_XXCZ = make_connectchannel_chunk_name( i_Code );
	if (i_ChunkName == c_XXFK)
	{
		tmlnDriverTextureFileNameInfo driver_info = dynamic_cast<const tmlnDriverTextureFileNameInfo&>(i_Info);
		if (driver_info.m_Value.GetValue().GetNumNames() > 1)
		{
			prtyTextureFileData val = driver_info.m_Value.GetFullValue();
			fsAbsolutePathMgr::ResolvePath(val.m_TextureLocator, "Textures");
		}
		tmlnDriverTextureFileName * pDriver = new tmlnDriverTextureFileName( io_Channel, c_XXFK );
		pDriver->SetDriverInfo(driver_info);
		io_Channel.AddDriver( pDriver );
		return pDriver;
	}
	else if (i_ChunkName == c_XXAK)
	{
		tmlnDriverAnimatedTextureFileNameInfo driver_info = dynamic_cast<const tmlnDriverAnimatedTextureFileNameInfo&>(i_Info);
		if (driver_info.m_FirstTextureFileName.GetValue().GetNumNames() > 1)
		{
			prtyTextureFileData val = driver_info.m_FirstTextureFileName.GetValue();
			fsAbsolutePathMgr::ResolvePath(val.m_TextureLocator, "Textures");
		}
		tmlnDriverAnimatedTextureFileName * pDriver = new tmlnDriverAnimatedTextureFileName( io_Channel, c_XXAK );
		pDriver->SetDriverInfo(driver_info);
		io_Channel.AddDriver( pDriver );
		return pDriver;
	}

	return NULL;
}
