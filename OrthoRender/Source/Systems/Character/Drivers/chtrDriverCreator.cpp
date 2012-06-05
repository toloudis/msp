/*****************************************************************************
**	chtrDriverCreator.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Systems/Character/Drivers/chtrDriverCreator.hpp"

#include "Systems/Character/Timeline/chtrAdapterGetPosition.hpp"
#include "Systems/Character/Timeline/chtrChannelAnimationSub.hpp"
#include "Systems/Character/Data/chtrDataParser.hpp"
//#include "chtrDriverAIMoveBase.hpp"
//#include "chtrDriverAIMoveBaseInfo.hpp"
//#include "chtrDriverAIMoveBaseParser.hpp"
#include "Systems/Character/Drivers/chtrDriverAnimationFull.hpp"
#include "Systems/Character/Drivers/chtrDriverAnimationSub.hpp"
#include "Systems/Character/Drivers/chtrDriverAnimationSubInfo.hpp"
#include "Systems/Character/Drivers/chtrDriverAnimationSubParser.hpp"
#include "Systems/Character/GUI/chtrGeomList.hpp"
#include "Systems/Character/Object/chtrObjectMgr.hpp"

#include "Systems/Common/DriverCreator/cmmDriverCreatorBoolean.hpp"
#include "Systems/Common/DriverCreator/cmmDriverCreatorFloat.hpp"
#include "Systems/Common/DriverCreator/cmmDriverCreatorOrientation.hpp"
#include "Systems/Common/DriverCreator/cmmDriverCreatorPosition.hpp"
#include "Support/fsys/fsysFileUtil.hpp"
#include "Support/spln/splnSpline.hpp"
#include "Support/tmln/tmlnChannelAnimationFull.hpp"
#include "Support/tmln/tmlnChannelBoolean.hpp"
#include "Support/tmln/tmlnChannelFloat.hpp"
#include "Support/tmln/tmlnChannelOrientation.hpp"
#include "Support/tmln/tmlnChannelPosition.hpp"
#include "Drivers/Animation/tmlnDriverAnimationFullInfo.hpp"
#include "Drivers/Animation/tmlnDriverAnimationFullParser.hpp"
#include "Drivers/Attach/tmlnDriverAttachOrient.hpp"
#include "Drivers/Attach/tmlnDriverAttachOrientInfo.hpp"
#include "Drivers/Attach/tmlnDriverAttachOrientParser.hpp"
#include "Drivers/Attach/tmlnDriverAttachParser.hpp"
#include "Drivers/Enable/tmlnDriverEnableParser.hpp"
#include "Drivers/Orientation/tmlnDriverOrientationParser.hpp"
#include "Drivers/Position/tmlnDriverPositionParser.hpp"
#include "Drivers/Sound/tmlnDriverSound.hpp"
#include "Drivers/Sound/tmlnDriverSoundInfo.hpp"
#include "Drivers/Sound/tmlnDriverSoundParser.hpp"
#include "Drivers/Spline/tmlnDriverSplineInfo.hpp"
#include "Drivers/Spline/tmlnDriverSplineParser.hpp"
#include "Drivers/Spline/tmlnDriverSplineOriented.hpp"

#include "Core/fs/fsFileUtil.hpp"	// for debug



namespace
{
	//========================================================================
	//========================================================================
	const char *c_OrientationChannelName = "Orientation";
	const char c_OrientationChannelCode[2] = { 'O', 'R' };  // ORientation channel code
	const char *c_PositionChannelName = "Position";
	const char c_PositionChannelCode[2] = { 'P', 'S' };  // PoSition channel code
	const char *c_ScaleChannelName = "Scale";
	const char c_ScaleChannelCode[2] = { 'S', 'C' };  // SCale channel code
	const char *c_VisibleChannelName = "Visible";
	const char c_VisibleChannelCode[2] = { 'V', 'S' };  // ViSible channel code

	//const char* CHTRDRIVERNAME_AIMOVEBASE = "AI Movement";
	const char* CHTRDRIVERNAME_ANIMATIONFULL = "Anim-Full";
	const char* CHTRDRIVERNAME_ANIMATIONSUB = "Sub-Anim";
	const char* CHTRDRIVERNAME_SOUND = "Play Sound";
	const char* CHTRDRIVERNAME_SPLINEORI = "Spline Oriented";
	const char* CHTRDRIVERNAME_ATTACHORIENT = "Attach Orient";

	const chDefs::Name c_CATC = chDefs::MakeName('C', 'A', 'C', 'H');  // Character Attachment
	const chDefs::Name c_CATO = chDefs::MakeName('P', 'A', 'T', 'O');  // Prop Attach Orient
	const chDefs::Name c_CPOS = chDefs::MakeName('C', 'P', 'O', 'S');  // Character static POSition
	const chDefs::Name c_CPSP = chDefs::MakeName('C', 'P', 'S', 'P');  // Character Position SPline
	const chDefs::Name c_CORI = chDefs::MakeName('C', 'O', 'R', 'I');  // Character static ORIentation
	const chDefs::Name c_COSP = chDefs::MakeName('C', 'O', 'S', 'P');  // Character Orientation SPline
	const chDefs::Name c_ENBD = chDefs::MakeName('E', 'N', 'B', 'D');  // Enabled
	const chDefs::Name c_CDAF = chDefs::MakeName('C', 'D', 'A', 'F');	// character driver anim full data

}

//--------------------------------------------------------------------
// Set directory to look for sound files within
//--------------------------------------------------------------------
chtrDriverCreator::chtrDriverCreator()
{
}

//--------------------------------------------------------------------
// Create parsers for driver types created by this creator
//--------------------------------------------------------------------
//static
void chtrDriverCreator::CreateParsers()
{
	chDefs::Name sys_code = chtrDataParser::GetChunkName();

	// Creation of driver parsers per channel
	cmmDriverCreatorPosition::CreateParsers(sys_code, c_PositionChannelCode); 
	cmmDriverCreatorBoolean::CreateParsers(sys_code, c_VisibleChannelCode); 
	cmmDriverCreatorOrientation::CreateParsers(sys_code, c_OrientationChannelCode); 
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_ScaleChannelCode); 
 
	// System specific drivers
	//tmlnParser::AddDriverParser(sys_code,
	//	chtrDriverAIMoveBaseParser::GetChunkName(),
	//	new chtrDriverAIMoveBaseParser());
	tmlnParser::AddDriverParser(sys_code, c_CDAF, new tmlnDriverAnimationFullParser(c_CDAF));
	tmlnParser::AddDriverParser(sys_code, chtrDriverAnimationSubParser::GetChunkName(),
		new chtrDriverAnimationSubParser());
	tmlnParser::AddDriverParser(sys_code, c_COSP, new tmlnDriverSplineParser(c_COSP));
	tmlnParser::AddDriverParser(sys_code, tmlnDriverSoundParser::GetChunkName(),
		new tmlnDriverSoundParser());
	tmlnParser::AddDriverParser(sys_code, c_CATO, new tmlnDriverAttachOrientParser(c_CATO));

	// Because of old file formats, we still have to create these parsers here
	tmlnParser::AddDriverParser(sys_code, c_CPOS, new tmlnDriverPositionParser(c_CPOS));
	tmlnParser::AddDriverParser(sys_code, c_CPSP, new tmlnDriverSplineParser(c_CPSP));
	tmlnParser::AddDriverParser(sys_code, c_CATC, new tmlnDriverAttachParser(c_CATC));
	tmlnParser::AddDriverParser(sys_code, c_ENBD, new tmlnDriverEnableParser(c_ENBD));
	tmlnParser::AddDriverParser(sys_code, c_CORI, new tmlnDriverOrientationParser(c_CORI));
}

//--------------------------------------------------------------------
// Gather the possible types of drivers that can be created for
//	for this object.  Driver names are added through the
//	DriverNameList's API
//--------------------------------------------------------------------
void chtrDriverCreator::GatherPossibleDrivers( const tmlnScriptObject* i_pObject,
											   tmlnDriverNameList& io_Drivers )
{
	if (dynamic_cast<const chtrScriptObject*>(i_pObject))
	{
		// Creation of drivers per channel
		cmmDriverCreatorPosition::GatherPossibleDrivers(c_PositionChannelName,
			io_Drivers, "Motion", this); 
		cmmDriverCreatorBoolean::GatherPossibleDrivers(c_VisibleChannelName,
			io_Drivers, "Other", this); 
		cmmDriverCreatorOrientation::GatherPossibleDrivers(c_OrientationChannelName, 
			io_Drivers, "Parameters", this); 
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_ScaleChannelName, 
			io_Drivers, "Parameters", this); 

		//io_Drivers.AddDriver(CHTRDRIVERNAME_AIMOVEBASE, "AI Movement", this);
		io_Drivers.AddDriver(CHTRDRIVERNAME_ANIMATIONFULL, "Animation", this);
		//io_Drivers.AddDriver(CHTRDRIVERNAME_ANIMATIONSUB, "Animation", this);
		io_Drivers.AddDriver(CHTRDRIVERNAME_SOUND, "Other", this);
		io_Drivers.AddDriver(CHTRDRIVERNAME_SPLINEORI, "Motion", this);
		io_Drivers.AddDriver(CHTRDRIVERNAME_ATTACHORIENT, "Motion", this);
	}
}

//--------------------------------------------------------------------
// Create driver for this object based on the name used in
//	GatherPossibleDrivers and attach it to the channels
//	on this script object.
//--------------------------------------------------------------------
tmlnDriver* chtrDriverCreator::CreateDriverByName( const char* i_DriverName,
													tmlnScriptObject* i_pObject )
{
	chtrScriptObject* chtr_obj = dynamic_cast<chtrScriptObject*>(i_pObject);
	if (!chtr_obj) return NULL;

	// Creation of drivers per channel
	tmlnDriver* pDriver = NULL;

	// Position Channel
	pDriver = cmmDriverCreatorPosition::CreateDriverByName(i_DriverName, 
		chtr_obj->ChannelPosition(), c_PositionChannelName, c_PositionChannelCode, 
		chtrObjectMgr::IconsVisible(), chtr_obj );
	if (pDriver != NULL) 
		return pDriver;

	// Orientation Channel
	pDriver = cmmDriverCreatorOrientation::CreateDriverByName(i_DriverName, 
		chtr_obj->ChannelOrientation(), c_OrientationChannelName, c_OrientationChannelCode, 
		chtrObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// Scale Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		chtr_obj->ChannelScale(), c_ScaleChannelName, c_ScaleChannelCode, 
		chtrObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// Visible Channel
	pDriver = cmmDriverCreatorBoolean::CreateDriverByName(i_DriverName, 
		chtr_obj->ChannelVisible(), c_VisibleChannelName, c_VisibleChannelCode, 
		chtrObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	//if ( !::_stricmp(i_DriverName,CHTRDRIVERNAME_AIMOVEBASE) )
	//{
	//	chtrDriverAIMoveBase* pDriver = new chtrDriverAIMoveBase( chtr_obj );
	//	pDriver->SetName( CHTRDRIVERNAME_AIMOVEBASE );
	//	pDriver->SetInitialTime();

	//	// Attach driver to the appropriate channels
	//	tmlnChannel& channelP = chtr_obj->ChannelPosition();
	//	channelP.AddDriver( pDriver );
	//	tmlnChannel& channelO = chtr_obj->ChannelOrientation();
	//	channelO.AddDriver( pDriver );
	//	tmlnChannel& channelAF = chtr_obj->ChannelAnimationFull();
	//	channelAF.AddDriver( pDriver );
	//	return pDriver;
	//}
	if ( !::_stricmp(i_DriverName,CHTRDRIVERNAME_ANIMATIONFULL) )
	{
		chtrDriverAnimationFull* pDriver = new chtrDriverAnimationFull( chtr_obj->ChannelAnimationFull(),
			c_CDAF, chtr_obj->GetDirectory() );
		pDriver->SetName( CHTRDRIVERNAME_ANIMATIONFULL );
		pDriver->SetInitialTime(0.0f);
		pDriver->ShowIcons( chtrObjectMgr::IconsVisible() );

		tmlnChannel& channel = chtr_obj->ChannelAnimationFull();
		// Attach driver to the appropriate channels
		channel.AddDriver( pDriver );
		return pDriver;
	}
	else if ( !::_stricmp(i_DriverName,CHTRDRIVERNAME_ANIMATIONSUB) )
	{
		chtrDriverAnimationSub* pDriver = new chtrDriverAnimationSub( chtr_obj->ChannelAnimationSub(),
			chtr_obj->GetDirectory() );
		pDriver->SetName( CHTRDRIVERNAME_ANIMATIONSUB );
		pDriver->SetInitialTime(0.0f);
		pDriver->ShowIcons( chtrObjectMgr::IconsVisible() );
		tmlnChannel& channel = chtr_obj->ChannelAnimationSub();

		// Attach driver to the appropriate channels
		channel.AddDriver( pDriver );
		return pDriver;
	}
	else if (!::_stricmp(i_DriverName,CHTRDRIVERNAME_SPLINEORI))
	{
		tmlnDriverSplineOriented * pDriver = new tmlnDriverSplineOriented( chtr_obj->ChannelPosition(), 
			chtr_obj->ChannelOrientation(), c_COSP, chtr_obj );
		pDriver->SetName(CHTRDRIVERNAME_SPLINEORI);
		pDriver->SetInitialTime();
		pDriver->ShowIcons( chtrObjectMgr::IconsVisible() );

		tmlnChannel& Ochannel = chtr_obj->ChannelOrientation();
		tmlnChannelPosition& Pchannel = chtr_obj->ChannelPosition();
		maPoint3d pos = Pchannel.GetPosition();

		// spline
		splnSpline spline;
		spline.AppendPoint( pos );
		spline.AppendPoint( pos + maPoint3d( 0.0f, 2.5f, 0.0f ) );
		pDriver->SetSpline( spline );

		// Attach driver to the appropriate channels
		Pchannel.AddDriver( pDriver );
		Ochannel.AddDriver( pDriver );
		return pDriver;
	}
	else if (!::_stricmp(i_DriverName, CHTRDRIVERNAME_ATTACHORIENT))
	{
		tmlnDriverAttachOrient * pDriver = new tmlnDriverAttachOrient( chtr_obj->ChannelPosition(), chtr_obj->ChannelOrientation(), c_CATO );
		pDriver->SetName(CHTRDRIVERNAME_ATTACHORIENT);
		pDriver->SetInitialTime();
		pDriver->ShowIcons( chtrObjectMgr::IconsVisible() );

		tmlnChannel& Pchannel = chtr_obj->ChannelPosition();
		tmlnChannel& Ochannel = chtr_obj->ChannelOrientation();

		// Attach driver to the appropriate channels
		Pchannel.AddDriver( pDriver );
		Ochannel.AddDriver( pDriver );
		return pDriver;
	}
	else if ( !::_stricmp(i_DriverName, CHTRDRIVERNAME_SOUND) )
	{
		fsLocator snddir = chtr_obj->GetDirectory();
		snddir.Pop();
		snddir.Push( gfPaths::GetSubPath( gfPaths::e_Sounds ) );
		snddir.Remove( gfPaths::GetAppPath() );		// make it a relative path

		// DEBUG ONLY
		//std::string filename;
		//fsFileUtil::LocatorToANSIFilename(snddir, filename);
		//DBG_LOG1("driver sound path (%s)", filename.c_str());

		tmlnDriverSound* pDriver = new tmlnDriverSound( chtr_obj->AdapterGetPosition(), snddir );
		pDriver->SetName( CHTRDRIVERNAME_SOUND );
		pDriver->SetInitialTime(0.0f);
		pDriver->ShowIcons( chtrObjectMgr::IconsVisible() );

		tmlnChannel& channel = chtr_obj->ChannelSound();
		// Attach driver to the appropriate channels
		channel.AddDriver( pDriver );
		return pDriver;
	}

	return 0;
}

//--------------------------------------------------------------------
// Create static key driver for the given channel
//--------------------------------------------------------------------
tmlnDriver* chtrDriverCreator::CreateKeyForChannel( tmlnScriptObject* io_pObject,
												    tmlnChannel* i_pChannel)
{
	chtrScriptObject* chtr_obj = dynamic_cast<chtrScriptObject*>(io_pObject);
	if (!chtr_obj) return NULL;

	if (i_pChannel == &chtr_obj->ChannelPosition())
	{
		// Position Channel
		return cmmDriverCreatorPosition::CreateKeyForChannel(
			chtr_obj->ChannelPosition(), c_PositionChannelName, c_PositionChannelCode, 
			chtrObjectMgr::IconsVisible(), chtr_obj );
	}
	else if (i_pChannel == &chtr_obj->ChannelOrientation())
	{
		// Orientation Channel
		return cmmDriverCreatorOrientation::CreateKeyForChannel(
			chtr_obj->ChannelOrientation(), c_OrientationChannelName, c_OrientationChannelCode, 
			chtrObjectMgr::IconsVisible() );
	}
	else if (i_pChannel == &chtr_obj->ChannelScale())
	{
		// Scale Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			chtr_obj->ChannelScale(), c_ScaleChannelName, c_ScaleChannelCode, 
			chtrObjectMgr::IconsVisible() );
	}
	else if (i_pChannel == &chtr_obj->ChannelVisible())
	{
		// Visible Channel
		return cmmDriverCreatorBoolean::CreateKeyForChannel(
			chtr_obj->ChannelVisible(), c_VisibleChannelName, c_VisibleChannelCode, 
			chtrObjectMgr::IconsVisible() );
	}

	return NULL;
}

//--------------------------------------------------------------------
// Create driver for this object based on the the info structure
//--------------------------------------------------------------------
tmlnDriver* chtrDriverCreator::CreateDriverFromInfo( tmlnScriptObject* io_pObject,
					const tmlnDriverInfo& i_Info)
{
	chtrScriptObject* chtr_obj = dynamic_cast<chtrScriptObject*>(io_pObject);
	if (!chtr_obj) return NULL;

	chDefs::Name name = i_Info.GetBaseChunkName();

	//if (name == chtrDriverAIMoveBaseParser::GetChunkName())
	//{
	//	const chtrDriverAIMoveBaseInfo& driver_info = dynamic_cast<const chtrDriverAIMoveBaseInfo&>(i_Info);
	//	chtrDriverAIMoveBase * pDriver = new chtrDriverAIMoveBase( chtr_obj );
	//	pDriver->SetDriverInfo(driver_info);

	//	// Attach driver to the appropriate channels
	//	tmlnChannel& channel = chtr_obj->ChannelPosition();
	//	channel.AddDriver( pDriver );
	//	channel = chtr_obj->ChannelOrientation();
	//	channel.AddDriver( pDriver );
	//	return pDriver;
	//}
	//else 
	if (name == c_CDAF)
	{
		const tmlnDriverAnimationFullInfo& driver_info = dynamic_cast<const tmlnDriverAnimationFullInfo&>(i_Info);
		chtrDriverAnimationFull * pDriver = new chtrDriverAnimationFull( chtr_obj->ChannelAnimationFull(),
			c_CDAF, chtr_obj->GetDirectory() );
		pDriver->SetDriverInfo(driver_info);

		tmlnChannel& channel = chtr_obj->ChannelAnimationFull();
		// Attach driver to the appropriate channels
		channel.AddDriver( pDriver );
		return pDriver;
	}
	else if (name == chtrDriverAnimationSubParser::GetChunkName())
	{
		const chtrDriverAnimationSubInfo& driver_info = dynamic_cast<const chtrDriverAnimationSubInfo&>(i_Info);
		chtrDriverAnimationSub * pDriver = new chtrDriverAnimationSub( chtr_obj->ChannelAnimationSub(),
			chtr_obj->GetDirectory() );
		pDriver->SetDriverInfo(driver_info);
		tmlnChannel& channel = chtr_obj->ChannelAnimationSub();
		// Attach driver to the appropriate channels
		channel.AddDriver( pDriver );
		return pDriver;
	}
	else if (name == c_COSP)	// character Orientation spline
	{
		const tmlnDriverSplineInfo& driver_info = dynamic_cast<const tmlnDriverSplineInfo&>(i_Info);
		tmlnDriverSplineOriented * pDriver = new tmlnDriverSplineOriented( chtr_obj->ChannelPosition(), 
			chtr_obj->ChannelOrientation(), c_COSP, chtr_obj );
		pDriver->SetDriverInfo(driver_info);

		tmlnChannel& Pchannel = chtr_obj->ChannelPosition();
		tmlnChannel& Ochannel = chtr_obj->ChannelOrientation();
		// Attach driver to the appropriate channels
		Pchannel.AddDriver( pDriver );
		Ochannel.AddDriver( pDriver );
		return pDriver;
	}
	else if (name == c_CATO)	// attach orient
	{
		const tmlnDriverAttachOrientInfo& driver_info = dynamic_cast<const tmlnDriverAttachOrientInfo&>(i_Info);
		tmlnDriverAttachOrient * pDriver = new tmlnDriverAttachOrient( chtr_obj->ChannelPosition(), chtr_obj->ChannelOrientation(), c_CATO );
		pDriver->SetDriverInfo(driver_info);

		// Attach driver to the appropriate channels
		tmlnChannel& Pchannel = chtr_obj->ChannelPosition();
		tmlnChannel& Ochannel = chtr_obj->ChannelOrientation();
		Pchannel.AddDriver( pDriver );
		Ochannel.AddDriver( pDriver );
		return pDriver;
	}
	else if (name == tmlnDriverSoundParser::GetChunkName())
	{
		fsLocator snddir;
		//chtr_obj->GetDirectory(snddir);
		//snddir.Pop();
		//snddir.Push( gfPaths::GetSubPath( gfPaths::e_Sounds ) );
		//snddir.Remove( gfPaths::GetAppPath() );		// make it a relative path

		const tmlnDriverSoundInfo& driver_info = dynamic_cast<const tmlnDriverSoundInfo&>(i_Info);

		itString sdname = chtrGeomList::GetSystemDirName();
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

		tmlnDriverSound * pDriver = new tmlnDriverSound( chtr_obj->AdapterGetPosition(), snddir );
		pDriver->SetDriverInfo(driver_info);

		tmlnChannel& channel = chtr_obj->ChannelSound();
		// Attach driver to the appropriate channels
		channel.AddDriver( pDriver );
		return pDriver;
	}
	else if (name == c_CPSP)	// character position spline
	{
		// Old parser chunk name, convert to new one
		name = cmmDriverCreatorPosition::GetSplineChunkName(c_PositionChannelCode);
	}
	else if (name == c_CATC)	// character attachment
	{
		// Old parser chunk name, convert to new one
		name = cmmDriverCreatorPosition::GetAttachChunkName(c_PositionChannelCode);
	}
	else if (name == c_CPOS)	// character static position
	{
		// Old parser chunk name, convert to new one
		name = cmmDriverCreatorPosition::GetKeyChunkName(c_PositionChannelCode);
	}
	else if (name == c_ENBD)
	{
		// Old parser chunk name, convert to new one
		name = cmmDriverCreatorBoolean::GetKeyChunkName(c_VisibleChannelCode);
	}
	else if (name == c_CORI)	// character static orientation
	{
		// Old parser chunk name, convert to new one
		name = cmmDriverCreatorOrientation::GetKeyChunkName(c_OrientationChannelCode);
	}

	// Creation of drivers per channel
	tmlnDriver* pDriver = NULL;

	// Position channel
	pDriver = cmmDriverCreatorPosition::CreateDriverFromInfo(
		chtr_obj->ChannelPosition(), c_PositionChannelCode, i_Info, name, chtr_obj );
	if (pDriver != NULL) 
		return pDriver;

	// Orientation channel
	pDriver = cmmDriverCreatorOrientation::CreateDriverFromInfo(
		chtr_obj->ChannelOrientation(), c_OrientationChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// Scale channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		chtr_obj->ChannelScale(), c_ScaleChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// Visible channel
	pDriver = cmmDriverCreatorBoolean::CreateDriverFromInfo(
		chtr_obj->ChannelVisible(), c_VisibleChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	return NULL;
}

