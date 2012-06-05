/*****************************************************************************
**	sbrdDriverCreator.cpp
**
**	Creates timeline channels for sbrdboards
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/Storyboards/Timeline/sbrdDriverCreator.hpp"

#include "Systems/Storyboards/Timeline/sbrdAdapterGetPosition.hpp"
#include "Systems/Storyboards/Data/sbrdDataParser.hpp"
#include "Systems/Storyboards/GUI/sbrdGeomList.hpp"
#include "Systems/Storyboards/Object/sbrdScriptObject.hpp"
#include "Systems/Storyboards/Object/sbrdObjectMgr.hpp"

#include "Systems/Common/DriverCreator/cmmDriverCreatorBoolean.hpp"
#include "Systems/Common/DriverCreator/cmmDriverCreatorColor.hpp"
#include "Systems/Common/DriverCreator/cmmDriverCreatorFloat.hpp"
#include "Systems/Common/DriverCreator/cmmDriverCreatorOrientation.hpp"
#include "Systems/Common/DriverCreator/cmmDriverCreatorPosition.hpp"
#include "Systems/Common/DriverCreator/cmmDriverCreatorTexture.hpp"

#include "Support/fsys/fsysFileUtil.hpp"
#include "Support/mnm/mnmPaths.hpp"
#include "Support/tmln/tmlnChannelBoolean.hpp"
#include "Support/tmln/tmlnChannelColor.hpp"
#include "Support/tmln/tmlnChannelFileName.hpp"
#include "Support/tmln/tmlnChannelFloat.hpp"
#include "Support/tmln/tmlnChannelOrientation.hpp"
#include "Support/tmln/tmlnChannelPosition.hpp"
#include "Drivers/Attach/tmlnDriverAttachParser.hpp"
#include "Drivers/Color/tmlnDriverColorParser.hpp"
#include "Drivers/FileName/tmlnDriverAnimatedFileNameParser.hpp"
#include "Drivers/Orientation/tmlnDriverOrientation.hpp"
#include "Drivers/Orientation/tmlnDriverOrientationInfo.hpp"
#include "Drivers/Orientation/tmlnDriverOrientationParser.hpp"
#include "Drivers/Position/tmlnDriverPositionParser.hpp"
#include "Drivers/Sound/tmlnDriverSound.hpp"
#include "Drivers/Sound/tmlnDriverSoundInfo.hpp"
#include "Drivers/Sound/tmlnDriverSoundParser.hpp"
#include "Drivers/Spline/tmlnDriverSplineParser.hpp"


namespace
{
	//========================================================================
	//========================================================================
	const char *c_PositionChannelName = "Position";
	const char c_PositionChannelCode[2] = { 'P', 'S' };		// PoSition channel code
	const char *c_ScaleChannelName = "Scale";
	const char c_ScaleChannelCode[2] = { 'S', 'C' };		// SCale channel code
	const char *c_ColorChannelName = "Color";
	const char c_ColorChannelCode[2] = { 'C', 'L' };		// CoLor channel code
	const char *c_OrientationChannelName = "Orientation";
	const char c_OrientationChannelCode[2] = { 'O', 'R' };  // ORientation channel code
	const char *c_VisibleChannelName = "Visible";
	const char c_VisibleChannelCode[2] = { 'V', 'S' };		// ViSible channel code
	const char *c_TextureChannelName = "Texture";
	const char c_TextureChannelCode[2] = { 'T', 'X' };  // TeXture channel code

	//========================================================================
	//========================================================================
	const chDefs::Name c_SPSP = chDefs::MakeName('S', 'P', 'S', 'P');  // Billboard Position SPline
	const chDefs::Name c_SATC = chDefs::MakeName('S', 'A', 'C', 'H');  // Billboard Attachment
	const chDefs::Name c_SPOS = chDefs::MakeName('S', 'P', 'O', 'S');  // Billboard Static position
	const chDefs::Name c_SCLR = chDefs::MakeName('S', 'C', 'L', 'R');  // Billboard Color
	const chDefs::Name c_SORI = chDefs::MakeName('S', 'O', 'R', 'I');  // Billboard static ORIentation
	const chDefs::Name c_ATEX = chDefs::MakeName('A', 'T', 'E', 'X');  // Animated texture driver - old format

	const char* c_SOUND_DRIVERNAME = "Play Sound";
}

//--------------------------------------------------------------------
// Create parsers for driver types created by this creator
//--------------------------------------------------------------------
//static
void sbrdDriverCreator::CreateParsers()
{
	chDefs::Name sys_code = sbrdDataParser::GetChunkName();
	
	// Creation of driver parsers per channel
	cmmDriverCreatorPosition::CreateParsers(sys_code, c_PositionChannelCode); 
	cmmDriverCreatorColor::CreateParsers(sys_code, c_ColorChannelCode); 
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_ScaleChannelCode); 
	cmmDriverCreatorOrientation::CreateParsers(sys_code, c_OrientationChannelCode); 
	cmmDriverCreatorBoolean::CreateParsers(sys_code, c_VisibleChannelCode); 
	cmmDriverCreatorTexture::CreateParsers(sys_code, c_TextureChannelCode); 

	// System specific drivers
	//tmlnParser::AddDriverParser(sys_code, sbrdDriverAnimatedTextureParser::GetChunkName(),
	//	new sbrdDriverAnimatedTextureParser());

	// Because of old file formats, we still have to create these parsers here
	tmlnParser::AddDriverParser(sys_code, c_SORI, new tmlnDriverOrientationParser(c_SORI));
	tmlnParser::AddDriverParser(sys_code, c_SCLR, new tmlnDriverColorParser(c_SCLR));
	tmlnParser::AddDriverParser(sys_code, c_SPSP, new tmlnDriverSplineParser(c_SPSP));
	tmlnParser::AddDriverParser(sys_code, c_SATC, new tmlnDriverAttachParser(c_SATC));
	tmlnParser::AddDriverParser(sys_code, c_SPOS, new tmlnDriverPositionParser(c_SPOS));
	tmlnParser::AddDriverParser(sys_code, tmlnDriverSoundParser::GetChunkName(), new tmlnDriverSoundParser());
	tmlnParser::AddDriverParser(sys_code, c_ATEX, new tmlnDriverAnimatedFileNameParser(c_ATEX));
}

//--------------------------------------------------------------------
// Gather the possible types of drivers that can be created for
//	for this object.  Driver names are added through the
//	DriverNameList's API
//--------------------------------------------------------------------
void sbrdDriverCreator::GatherPossibleDrivers(const tmlnScriptObject* i_pObject,
											   tmlnDriverNameList& io_Drivers)
{
	if (dynamic_cast<const sbrdScriptObject*>(i_pObject))
	{
		// Creation of drivers per channel
		cmmDriverCreatorPosition::GatherPossibleDrivers(c_PositionChannelName,
			io_Drivers, "Motion", this); 
		cmmDriverCreatorColor::GatherPossibleDrivers(c_ColorChannelName,
			io_Drivers, "Parameters", this); 
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_ScaleChannelName,
			io_Drivers, "Parameters", this); 
		cmmDriverCreatorOrientation::GatherPossibleDrivers(c_OrientationChannelName, 
			io_Drivers, "Parameters", this); 
		cmmDriverCreatorBoolean::GatherPossibleDrivers(c_VisibleChannelName,
			io_Drivers, "Other", this); 
		cmmDriverCreatorTexture::GatherPossibleDrivers(c_TextureChannelName, 
			io_Drivers, "Parameters", this); 
		io_Drivers.AddDriver(c_SOUND_DRIVERNAME, "Other", this);

		// System specific drivers
		//io_Drivers.AddDriver("Animated Texture", "Texture Parameters", this);
	}
}

//--------------------------------------------------------------------
// Create driver for this object based on the name used in
//	GatherPossibleDrivers and attach it to the channels
//	on this script object.
//--------------------------------------------------------------------
tmlnDriver* sbrdDriverCreator::CreateDriverByName( const char* i_DriverName,
													tmlnScriptObject* i_pObject )
{
	sbrdScriptObject* sbrd_obj = dynamic_cast<sbrdScriptObject*>(i_pObject);
	if (!sbrd_obj) return NULL;

	// Creation of drivers per channel
	tmlnDriver* pDriver = NULL;

	// Position Channel
	pDriver = cmmDriverCreatorPosition::CreateDriverByName(i_DriverName, 
		sbrd_obj->ChannelPosition(), c_PositionChannelName, c_PositionChannelCode, 
		sbrdObjectMgr::IconsVisible(), sbrd_obj );
	if (pDriver != NULL) 
		return pDriver;

	// Color Channel
	pDriver = cmmDriverCreatorColor::CreateDriverByName(i_DriverName, 
		sbrd_obj->ChannelColor(), c_ColorChannelName, c_ColorChannelCode, 
		sbrdObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// Scale Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		sbrd_obj->ScaleChannel(), c_ScaleChannelName, c_ScaleChannelCode, 
		sbrdObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// Orientation Channel
	pDriver = cmmDriverCreatorOrientation::CreateDriverByName(i_DriverName, 
		sbrd_obj->ChannelOrientation(), c_OrientationChannelName, c_OrientationChannelCode, 
		sbrdObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// Visible Channel
	pDriver = cmmDriverCreatorBoolean::CreateDriverByName(i_DriverName, 
		sbrd_obj->ChannelVisible(), c_VisibleChannelName, c_VisibleChannelCode, 
		sbrdObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// Texture Channel
	pDriver = cmmDriverCreatorTexture::CreateDriverByName(i_DriverName, 
		sbrd_obj->ChannelTexture(), c_TextureChannelName, c_TextureChannelCode, 
		sbrdObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	//
	if (!::_stricmp(i_DriverName, "Static Orientation"))
	{
		tmlnDriverOrientation * pDriver = new tmlnDriverOrientation( sbrd_obj->ChannelOrientation(), c_SORI );
		pDriver->SetName("Orientation");
		pDriver->SetInitialTime( 0.0f ); // short key
		pDriver->ShowIcons( sbrdObjectMgr::IconsVisible() );

		// Attach driver to the appropriate channels
		tmlnChannelOrientation& channel = sbrd_obj->ChannelOrientation();
		channel.AddDriver( pDriver );
		return pDriver;
	}
	//else if ( !::_stricmp(i_DriverName, "Animated Texture") )
	//{
	//	sbrdDriverAnimatedTexture* pDriver = new sbrdDriverAnimatedTexture( sbrd_obj->ChannelTexture() );
	//	pDriver->SetName( "Animated Texture" );
	//	pDriver->SetInitialTime(0.0f);	// short key
	//	pDriver->ShowIcons( sbrdObjectMgr::IconsVisible() );

	//	// Attach driver to the appropriate channels
	//	tmlnChannel& channel = sbrd_obj->ChannelTexture();
	//	channel.AddDriver( pDriver );
	//	return pDriver;
	//}
	else if ( !::_stricmp(i_DriverName, c_SOUND_DRIVERNAME) )
	{
		fsLocator snddir = gfPaths::GetPath(mnmPaths::e_DataScene);
		//snddir.Push( sbrdGeomList::GetSystemDirName() );
		snddir.Push( gfPaths::GetSubPath( gfPaths::e_Sounds ) );
		snddir.Remove( gfPaths::GetAppPath() );		// make it a relative path

		// DEBUG ONLY
		//std::string filename;
		//fsFileUtil::LocatorToANSIFilename(snddir, filename);
		//DBG_LOG1("driver sound path (%s)", filename.c_str());

		tmlnDriverSound* pDriver = new tmlnDriverSound( sbrd_obj->AdapterGetPosition(), snddir );
		pDriver->SetName( c_SOUND_DRIVERNAME );
		pDriver->SetInitialTime(0.0f);
		pDriver->ShowIcons( sbrdObjectMgr::IconsVisible() );

		tmlnChannel& channel = sbrd_obj->ChannelSound();
		// Attach driver to the appropriate channels
		channel.AddDriver( pDriver );
		return pDriver;
	}

	return NULL;
}

//--------------------------------------------------------------------
// Create static key driver for the given channel
//--------------------------------------------------------------------
tmlnDriver* sbrdDriverCreator::CreateKeyForChannel( tmlnScriptObject* io_pObject,
												    tmlnChannel* i_pChannel)
{
	sbrdScriptObject* sbrd_obj = dynamic_cast<sbrdScriptObject*>(io_pObject);
	if (!sbrd_obj) return NULL;

	if (i_pChannel == &sbrd_obj->ChannelPosition())
	{
		// Position Channel
		return cmmDriverCreatorPosition::CreateKeyForChannel(
			sbrd_obj->ChannelPosition(), c_PositionChannelName, c_PositionChannelCode, 
			sbrdObjectMgr::IconsVisible(), sbrd_obj );
	}
	else if (i_pChannel == &sbrd_obj->ChannelColor())
	{
		// Color Channel
		return cmmDriverCreatorColor::CreateKeyForChannel(
			sbrd_obj->ChannelColor(), c_ColorChannelName, c_ColorChannelCode, 
			sbrdObjectMgr::IconsVisible() );
	}
	else if (i_pChannel == &sbrd_obj->ScaleChannel())
	{
		// Scale Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			sbrd_obj->ScaleChannel(), c_ScaleChannelName, c_ScaleChannelCode, 
			sbrdObjectMgr::IconsVisible() );
	}
	else if (i_pChannel == &sbrd_obj->ChannelOrientation())
	{
		// Orientation Channel
		return cmmDriverCreatorOrientation::CreateKeyForChannel(
			sbrd_obj->ChannelOrientation(), c_OrientationChannelName, c_OrientationChannelCode, 
			sbrdObjectMgr::IconsVisible() );
	}
	else if (i_pChannel == &sbrd_obj->ChannelVisible())
	{
		// Visible Channel
		return cmmDriverCreatorBoolean::CreateKeyForChannel(
			sbrd_obj->ChannelVisible(), c_VisibleChannelName, c_VisibleChannelCode, 
			sbrdObjectMgr::IconsVisible() );
	}
	else if (i_pChannel == &sbrd_obj->ChannelTexture())
	{
		// Texture Channel
		return cmmDriverCreatorTexture::CreateKeyForChannel(
			sbrd_obj->ChannelTexture(), c_TextureChannelName, c_TextureChannelCode, 
			sbrdObjectMgr::IconsVisible() );
	}

	return NULL;
}

//--------------------------------------------------------------------
// Create driver for this object based on the the info structure
//--------------------------------------------------------------------
tmlnDriver* sbrdDriverCreator::CreateDriverFromInfo( tmlnScriptObject* io_pObject,
													 const tmlnDriverInfo& i_Info)
{
	sbrdScriptObject* sbrd_obj = dynamic_cast<sbrdScriptObject*>(io_pObject);
	if (!sbrd_obj) return NULL;

	chDefs::Name name = i_Info.GetBaseChunkName();

	if (name == c_SORI)	// static orientation
	{
		const tmlnDriverOrientationInfo& driver_info = dynamic_cast<const tmlnDriverOrientationInfo&>(i_Info);
		tmlnDriverOrientation * pDriver = new tmlnDriverOrientation( sbrd_obj->ChannelOrientation(), c_SORI );
		pDriver->SetDriverInfo(driver_info);

		// Attach driver to the appropriate channels
		tmlnChannel& channel = sbrd_obj->ChannelOrientation();
		channel.AddDriver( pDriver );
		return pDriver;
	}
	//else if (name == sbrdDriverAnimatedTextureParser::GetChunkName())
	//{
	//	const sbrdDriverAnimatedTextureInfo& driver_info = dynamic_cast<const sbrdDriverAnimatedTextureInfo&>(i_Info);
	//	sbrdDriverAnimatedTexture * pDriver = new sbrdDriverAnimatedTexture( sbrd_obj->ChannelTexture() );
	//	pDriver->SetDriverInfo(driver_info);

	//	// Attach driver to the appropriate channels
	//	tmlnChannel& channel = sbrd_obj->ChannelTexture();
	//	channel.AddDriver( pDriver );
	//	return pDriver;
	//}
	else if (name == c_SPSP)
	{
		// Old parser chunk name, convert to new one
		name = cmmDriverCreatorPosition::GetSplineChunkName(c_PositionChannelCode);
	}
	else if (name == c_SATC)
	{
		// Old parser chunk name, convert to new one
		name = cmmDriverCreatorPosition::GetAttachChunkName(c_PositionChannelCode);
	}
	else if (name == c_SPOS)
	{
		// Old parser chunk name, convert to new one
		name = cmmDriverCreatorPosition::GetKeyChunkName(c_PositionChannelCode);
	}
	else if (name == c_SCLR)
	{
		// Old parser chunk name, convert to new one
		name = cmmDriverCreatorColor::GetKeyChunkName(c_ColorChannelCode);
	}
	else if (name == c_ATEX)
	{
		// Old parser chunk name, convert to new one
		name = cmmDriverCreatorTexture::GetTextureListChunkName(c_TextureChannelCode);
	}
	else if (name == tmlnDriverSoundParser::GetChunkName())
	{
		fsLocator snddir;
		//sbrd_obj->GetDirectory(snddir);
		//snddir.Pop();
		//snddir.Push( gfPaths::GetSubPath( gfPaths::e_Sounds ) );
		//snddir.Remove( gfPaths::GetAppPath() );		// make it a relative path

		const tmlnDriverSoundInfo& driver_info = dynamic_cast<const tmlnDriverSoundInfo&>(i_Info);

		snddir = gfPaths::GetPath(mnmPaths::e_DataScene);
		snddir.Remove( gfPaths::GetAppPath() );		// make it a relative path

		itString sdname; //= sbrdGeomList::GetSystemDirName();
		fsysFileUtil::GetFilePath(  sdname,
									itString(gfPaths::GetSubPath( gfPaths::e_Sounds )), 
									(driver_info.m_SoundName), 
									snddir);
		
		//	if the sound isn't found, add the driver, but don't fail loading the scene.
		//
		//DBG_ASSERT1( snddir.GetNumNames() > 0, "Cannot find sound (%s)", itStringUtil::GetStdString(driver_info.m_SoundName).c_str() );
		if (snddir.GetNumNames() > 0)
		{
			snddir.Remove( gfPaths::GetAppPath() );		// make it a relative path
			snddir.Pop();
		}

		tmlnDriverSound * pDriver = new tmlnDriverSound( sbrd_obj->AdapterGetPosition(), snddir );
		pDriver->SetDriverInfo(driver_info);

		tmlnChannel& channel = sbrd_obj->ChannelSound();
		// Attach driver to the appropriate channels
		channel.AddDriver( pDriver );
		return pDriver;
	}

	// Creation of drivers per channel
	tmlnDriver* pDriver = NULL;

	// Position channel
	pDriver = cmmDriverCreatorPosition::CreateDriverFromInfo(
		sbrd_obj->ChannelPosition(), c_PositionChannelCode, i_Info, name, sbrd_obj );
	if (pDriver != NULL) 
		return pDriver;

	// Color channel
	pDriver = cmmDriverCreatorColor::CreateDriverFromInfo(
		sbrd_obj->ChannelColor(), c_ColorChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// Orientation channel
	pDriver = cmmDriverCreatorOrientation::CreateDriverFromInfo(
		sbrd_obj->ChannelOrientation(), c_OrientationChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// Scale channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		sbrd_obj->ScaleChannel(), c_ScaleChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// Visible channel
	pDriver = cmmDriverCreatorBoolean::CreateDriverFromInfo(
		sbrd_obj->ChannelVisible(), c_VisibleChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// Texture channel
	pDriver = cmmDriverCreatorTexture::CreateDriverFromInfo(
		sbrd_obj->ChannelTexture(), c_TextureChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	return NULL;
}
