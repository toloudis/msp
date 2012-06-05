/*****************************************************************************
**	propDriverCreator.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "Systems/Props/Drivers/propDriverCreator.hpp"

#include "Systems/Props/Timeline/propAdapterGetPosition.hpp"
#include "Systems/Props/Data/propDataParser.hpp"
//#include "Systems/Props/Drivers/propDriverAIMoveBase.hpp"
//#include "Systems/Props/Drivers/propDriverAIMoveBaseInfo.hpp"
//#include "Systems/Props/Drivers/propDriverAIMoveBaseParser.hpp"
#include "Systems/Props/Drivers/propDriverAnimationFull.hpp"
#include "Systems/Props/GUI/propAnimList.hpp"
#include "Systems/Props/GUI/propGeomList.hpp"
#include "Systems/Props/Object/propScriptObject.hpp"
#include "Systems/Props/Object/propObjectMgr.hpp"

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
#include "Drivers/Attach/tmlnDriverAttach.hpp"
#include "Drivers/Attach/tmlnDriverAttachInfo.hpp"
#include "Drivers/Attach/tmlnDriverAttachParser.hpp"
#include "Drivers/Attach/tmlnDriverAttachOrient.hpp"
#include "Drivers/Attach/tmlnDriverAttachOrientInfo.hpp"
#include "Drivers/Attach/tmlnDriverAttachOrientParser.hpp"
#include "Drivers/Enable/tmlnDriverEnableParser.hpp"
#include "Drivers/Orientation/tmlnDriverOrientationParser.hpp"
#include "Drivers/Position/tmlnDriverPositionParser.hpp"
#include "Drivers/Sound/tmlnDriverSound.hpp"
#include "Drivers/Sound/tmlnDriverSoundInfo.hpp"
#include "Drivers/Sound/tmlnDriverSoundParser.hpp"
#include "Drivers/Spline/tmlnDriverSplineInfo.hpp"
#include "Drivers/Spline/tmlnDriverSplineOriented.hpp"
#include "Drivers/Spline/tmlnDriverSplineParser.hpp"

#include "Core/Fs/fsAbsolutePathMgr.hpp"
#include "Core/gf/gfPaths.hpp"
#include "Core/dbg/dbgMsg.hpp"


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

	const char* PROPDRIVERNAME_AIMOVEBASE = "AI Movement";
	const char* PROPDRIVERNAME_ANIMATIONFULL = "Anim-Full";
	const char* PROPDRIVERNAME_SOUND = "Play Sound";
	const char* PROPDRIVERNAME_ATTACHORIENT = "Attach Orient";
	const char* PROPDRIVERNAME_SPLINEORI = "Spline Oriented";

	const chDefs::Name c_PATC = chDefs::MakeName('P', 'A', 'C', 'H');  // Prop Attachment
	const chDefs::Name c_PATO = chDefs::MakeName('P', 'A', 'T', 'O');  // Prop Attach Orient
	const chDefs::Name c_PPOS = chDefs::MakeName('P', 'P', 'O', 'S');  // Prop static POSition
	const chDefs::Name c_PPSP = chDefs::MakeName('P', 'P', 'S', 'P');  // Prop Position SPline
	const chDefs::Name c_PORI = chDefs::MakeName('P', 'O', 'R', 'I');  // Prop static ORIentation
	const chDefs::Name c_POSP = chDefs::MakeName('P', 'O', 'S', 'P');  // Prop Orientation SPline
	const chDefs::Name c_ENBD = chDefs::MakeName('E', 'N', 'B', 'D');  // Enabled
	const chDefs::Name c_PDAF = chDefs::MakeName('P', 'D', 'A', 'F');	// prop drive anim full
	
	//--------------------------------------------------------------------
	// Check absolute path and resolve single filenames into fullpaths
	//--------------------------------------------------------------------
	void resolve_animpath(fsLocator &io_FilePath)
	{
		fsLocator anim_loc = io_FilePath;
		if (anim_loc.GetNumNames() > 0)
		{
			// This comparison is only needed to support old file formats
			// and could be removed in product
			if (anim_loc.GetNumNames() == 1)
			{
				itString filename = anim_loc.GetLastName();
				fsysFileUtil::GetFilePath(propAnimList::GetSystemDirName(),
					itString(gfPaths::GetSubPath(gfPaths::e_Data)), filename, anim_loc);
			}

			if (fsAbsolutePathMgr::ResolvePath(anim_loc, "Animations"))
				io_FilePath = anim_loc;
		}
	}
}

//--------------------------------------------------------------------
// Set directory to look for sound files within
//--------------------------------------------------------------------
propDriverCreator::propDriverCreator(const fsLocator &i_SoundDir)
: m_SoundDir(i_SoundDir)
{

}


//--------------------------------------------------------------------
// Create parsers for driver types created by this creator
//--------------------------------------------------------------------
//static
void propDriverCreator::CreateParsers()
{
	chDefs::Name sys_code = propDataParser::GetChunkName();

	// Creation of driver parsers per channel
	cmmDriverCreatorPosition::CreateParsers(sys_code, c_PositionChannelCode); 
	cmmDriverCreatorBoolean::CreateParsers(sys_code, c_VisibleChannelCode); 
	cmmDriverCreatorOrientation::CreateParsers(sys_code, c_OrientationChannelCode);  
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_ScaleChannelCode); 

	// System specific drivers
//	tmlnParser::AddDriverParser(sys_code, propDriverAIMoveBaseParser::GetChunkName(),
//		new propDriverAIMoveBaseParser());
	tmlnParser::AddDriverParser(sys_code, c_PDAF,new tmlnDriverAnimationFullParser(c_PDAF));
	tmlnParser::AddDriverParser(sys_code, c_PATO, new tmlnDriverAttachOrientParser(c_PATO));
	tmlnParser::AddDriverParser(sys_code, tmlnDriverSoundParser::GetChunkName(),
		new tmlnDriverSoundParser());
	tmlnParser::AddDriverParser(sys_code, c_POSP, new tmlnDriverSplineParser(c_POSP));

	// Because of old file formats, we still have to create these parsers here
	tmlnParser::AddDriverParser(sys_code, c_PPOS, new tmlnDriverPositionParser(c_PPOS));
	tmlnParser::AddDriverParser(sys_code, c_PPSP, new tmlnDriverSplineParser(c_PPSP));
	tmlnParser::AddDriverParser(sys_code, c_PATC, new tmlnDriverAttachParser(c_PATC));
	tmlnParser::AddDriverParser(sys_code, c_ENBD, new tmlnDriverEnableParser(c_ENBD));
	tmlnParser::AddDriverParser(sys_code, c_PORI, new tmlnDriverOrientationParser(c_PORI));
}

//--------------------------------------------------------------------
// Gather the possible types of drivers that can be created for
//	for this object.  Driver names are added through the
//	DriverNameList's API
//--------------------------------------------------------------------
void propDriverCreator::GatherPossibleDrivers( const tmlnScriptObject* i_pObject,
											   tmlnDriverNameList& io_Drivers )
{
	if (dynamic_cast<const propScriptObject*>(i_pObject))
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

		io_Drivers.AddDriver(PROPDRIVERNAME_ANIMATIONFULL, "Animation", this);
		io_Drivers.AddDriver(PROPDRIVERNAME_SOUND, "Parameters", this);
		io_Drivers.AddDriver(PROPDRIVERNAME_SPLINEORI, "Motion", this);
		io_Drivers.AddDriver(PROPDRIVERNAME_ATTACHORIENT, "Motion", this);
		//io_Drivers.AddDriver(PROPDRIVERNAME_AIMOVEBASE, "AI Movement", this);
	}
}

//--------------------------------------------------------------------
// Create driver for this object based on the name used in
//	GatherPossibleDrivers and attach it to the channels
//	on this script object.
//--------------------------------------------------------------------
tmlnDriver* propDriverCreator::CreateDriverByName( const char* i_DriverName,
													tmlnScriptObject* i_pObject )
{
	propScriptObject* prop_obj = dynamic_cast<propScriptObject*>(i_pObject);
	if (!prop_obj) return NULL;

	// Creation of drivers per channel
	tmlnDriver* pDriver = NULL;

	// Position Channel
	pDriver = cmmDriverCreatorPosition::CreateDriverByName(i_DriverName, 
		prop_obj->ChannelPosition(), c_PositionChannelName, c_PositionChannelCode, 
		propObjectMgr::IconsVisible(), prop_obj );
	if (pDriver != NULL) 
		return pDriver;

	// Visible Channel
	pDriver = cmmDriverCreatorBoolean::CreateDriverByName(i_DriverName, 
		prop_obj->ChannelVisible(), c_VisibleChannelName, c_VisibleChannelCode, 
		propObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// Orientation Channel
	pDriver = cmmDriverCreatorOrientation::CreateDriverByName(i_DriverName, 
		prop_obj->ChannelOrientation(), c_OrientationChannelName, c_OrientationChannelCode, 
		propObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// Scale Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		prop_obj->ChannelScale(), c_ScaleChannelName, c_ScaleChannelCode, 
		propObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	//if ( !::_stricmp(i_DriverName,PROPDRIVERNAME_AIMOVEBASE) )
	//{
	//	propDriverAIMoveBase* pDriver = new propDriverAIMoveBase( prop_obj );
	//	pDriver->SetName( PROPDRIVERNAME_AIMOVEBASE );
	//	pDriver->SetInitialTime();
	//	pDriver->ShowIcons( propObjectMgr::IconsVisible() );

	//	// Attach driver to the appropriate channels
	//	tmlnChannel& channelP = prop_obj->ChannelPosition();
	//	tmlnChannel& channelO = prop_obj->ChannelOrientation();
	//	tmlnChannel& channelAF = prop_obj->ChannelAnimationFull();
	//	channelP.AddDriver( pDriver );
	//	channelO.AddDriver( pDriver );
	//	channelAF.AddDriver( pDriver );
	//	return pDriver;
	//}
	//else 
	if ( !::_stricmp(i_DriverName,PROPDRIVERNAME_ANIMATIONFULL) )
	{
		propDriverAnimationFull* pDriver = new propDriverAnimationFull( prop_obj->ChannelAnimationFull(), c_PDAF );
		pDriver->SetName( PROPDRIVERNAME_ANIMATIONFULL );
		pDriver->SetInitialTime(0.0f);
		pDriver->ShowIcons( propObjectMgr::IconsVisible() );

		// Attach driver to the appropriate channels
		tmlnChannel& channel = prop_obj->ChannelAnimationFull();
		channel.AddDriver( pDriver );
		return pDriver;
	}
	else if (!::_stricmp(i_DriverName,PROPDRIVERNAME_SPLINEORI))
	{
		tmlnDriverSplineOriented * pDriver = new tmlnDriverSplineOriented( prop_obj->ChannelPosition(), 
			prop_obj->ChannelOrientation(), c_POSP, prop_obj );
		pDriver->SetName(PROPDRIVERNAME_SPLINEORI);
		pDriver->SetInitialTime();
		pDriver->ShowIcons( propObjectMgr::IconsVisible() );

		tmlnChannelPosition& Pchannel = prop_obj->ChannelPosition();
		tmlnChannel& Ochannel = prop_obj->ChannelOrientation();

		// spline
		splnSpline spline;
		spline.AppendPoint( Pchannel.GetPosition() );
		//spline.AppendPoint( Pchannel.GetPosition() + maPoint3d( 0.0f, 2.5f, 0.0f ) );
		pDriver->SetSpline( spline );

		// Attach driver to the appropriate channels

		Pchannel.AddDriver( pDriver );
		Ochannel.AddDriver( pDriver );
		return pDriver;
	}
	else if (!::_stricmp(i_DriverName, PROPDRIVERNAME_ATTACHORIENT))
	{
		tmlnDriverAttachOrient * pDriver = new tmlnDriverAttachOrient( prop_obj->ChannelPosition(), prop_obj->ChannelOrientation(), c_PATO );
		pDriver->SetName(PROPDRIVERNAME_ATTACHORIENT);
		pDriver->SetInitialTime();
		pDriver->ShowIcons( propObjectMgr::IconsVisible() );

		tmlnChannel& Pchannel = prop_obj->ChannelPosition();
		tmlnChannel& Ochannel = prop_obj->ChannelOrientation();

		// Attach driver to the appropriate channels
		Pchannel.AddDriver( pDriver );
		Ochannel.AddDriver( pDriver );
		return pDriver;
	}
	else if ( !::_stricmp(i_DriverName, PROPDRIVERNAME_SOUND) )
	{
		fsLocator snddir;
		prop_obj->GetDirectory(snddir);
		snddir.Pop();
		snddir.Push( gfPaths::GetSubPath( gfPaths::e_Sounds ) );
		snddir.Remove( gfPaths::GetAppPath() );		// make it a relative path

		// DEBUG ONLY
		//std::string filename;
		//fsFileUtil::LocatorToANSIFilename(snddir, filename);
		//DBG_LOG("driver sound path (" << filename.c_str() << ")" );

		tmlnDriverSound* pDriver = new tmlnDriverSound( prop_obj->AdapterGetPosition(), snddir );
		pDriver->SetName( PROPDRIVERNAME_SOUND );
		pDriver->SetInitialTime(0.0f);
		pDriver->ShowIcons( propObjectMgr::IconsVisible() );

		tmlnChannel& channel = prop_obj->ChannelSound();

		// Attach driver to the appropriate channels
		channel.AddDriver( pDriver );
		return pDriver;
	}
	return 0;
}

//--------------------------------------------------------------------
// Create static key driver for the given channel
//--------------------------------------------------------------------
tmlnDriver* propDriverCreator::CreateKeyForChannel( tmlnScriptObject* io_pObject,
												    tmlnChannel* i_pChannel)
{
	propScriptObject* prop_obj = dynamic_cast<propScriptObject*>(io_pObject);
	if (!prop_obj) return NULL;

	if (i_pChannel == &prop_obj->ChannelPosition())
	{
		// Position Channel
		return cmmDriverCreatorPosition::CreateKeyForChannel(
			prop_obj->ChannelPosition(), c_PositionChannelName, c_PositionChannelCode, 
			propObjectMgr::IconsVisible(), prop_obj );
	}
	else if (i_pChannel == &prop_obj->ChannelOrientation())
	{
		// Orientation Channel
		return cmmDriverCreatorOrientation::CreateKeyForChannel(
			prop_obj->ChannelOrientation(), c_OrientationChannelName, c_OrientationChannelCode, 
			propObjectMgr::IconsVisible() );
	}
	else if (i_pChannel == &prop_obj->ChannelScale())
	{
		// Scale Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			prop_obj->ChannelScale(), c_ScaleChannelName, c_ScaleChannelCode, 
			propObjectMgr::IconsVisible() );
	}
	else if (i_pChannel == &prop_obj->ChannelVisible())
	{
		// Visible Channel
		return cmmDriverCreatorBoolean::CreateKeyForChannel(
			prop_obj->ChannelVisible(), c_VisibleChannelName, c_VisibleChannelCode, 
			propObjectMgr::IconsVisible() );
	}

	return NULL;
}

//--------------------------------------------------------------------
// Create driver for this object based on the the info structure
//--------------------------------------------------------------------
tmlnDriver* propDriverCreator::CreateDriverFromInfo( tmlnScriptObject* io_pObject,
					const tmlnDriverInfo& i_Info)
{
	propScriptObject* prop_obj = dynamic_cast<propScriptObject*>(io_pObject);
	if (!prop_obj) return NULL;

	chDefs::Name name = i_Info.GetBaseChunkName();

	//if (name == propDriverAIMoveBaseParser::GetChunkName())
	//{
	//	const propDriverAIMoveBaseInfo& driver_info = dynamic_cast<const propDriverAIMoveBaseInfo&>(i_Info);
	//	propDriverAIMoveBase * pDriver = new propDriverAIMoveBase( prop_obj );
	//	pDriver->SetDriverInfo(driver_info);

	//	// Attach driver to the appropriate channels
	//	tmlnChannel& Pchannel = prop_obj->ChannelPosition();
	//	tmlnChannel& Ochannel = prop_obj->ChannelOrientation();

	//	Pchannel.AddDriver( pDriver );
	//	Ochannel.AddDriver( pDriver );
	//	return pDriver;
	//}
	//else 
	if (name == c_PDAF)
	{
		//const tmlnDriverAnimationFullInfo& driver_info = dynamic_cast<const tmlnDriverAnimationFullInfo&>(i_Info);
		// Make copy of data in order to resolve fullpaths
		tmlnDriverAnimationFullInfo driver_info( dynamic_cast<const tmlnDriverAnimationFullInfo&>(i_Info) );
		resolve_animpath(driver_info.m_Info.m_AnimFilename);

		propDriverAnimationFull * pDriver = new propDriverAnimationFull( prop_obj->ChannelAnimationFull(), c_PDAF );
		pDriver->SetDriverInfo(driver_info);

		// Attach driver to the appropriate channels
		tmlnChannel& channel = prop_obj->ChannelAnimationFull();

		channel.AddDriver( pDriver );
		return pDriver;
	}
	else if (name == c_POSP)	// prop Orientation spline
	{
		const tmlnDriverSplineInfo& driver_info = dynamic_cast<const tmlnDriverSplineInfo&>(i_Info);
		tmlnDriverSplineOriented * pDriver = new tmlnDriverSplineOriented( prop_obj->ChannelPosition(), 
			prop_obj->ChannelOrientation(), c_POSP, prop_obj );
		pDriver->SetDriverInfo(driver_info);

		// Attach driver to the appropriate channels
		tmlnChannel& channel = prop_obj->ChannelOrientation();
		channel.AddDriver( pDriver );
		return pDriver;
	}
	else if (name == c_PATO)	// prop attach orient
	{
		const tmlnDriverAttachOrientInfo& driver_info = dynamic_cast<const tmlnDriverAttachOrientInfo&>(i_Info);
		tmlnDriverAttachOrient * pDriver = new tmlnDriverAttachOrient( prop_obj->ChannelPosition(), prop_obj->ChannelOrientation(), c_PATO );
		pDriver->SetDriverInfo(driver_info);

		// Attach driver to the appropriate channels
		tmlnChannel& Pchannel = prop_obj->ChannelPosition();
		tmlnChannel& Ochannel = prop_obj->ChannelOrientation();
		Pchannel.AddDriver( pDriver );
		Ochannel.AddDriver( pDriver );
		return pDriver;
	}
	else if (name == tmlnDriverSoundParser::GetChunkName())
	{		
		//const tmlnDriverSoundInfo& driver_info = dynamic_cast<const tmlnDriverSoundInfo&>(i_Info);
		tmlnDriverSoundInfo driver_info = dynamic_cast<const tmlnDriverSoundInfo&>(i_Info);

		fsLocator snddir;
		if (driver_info.m_SoundName.GetNumNames() > 1)
		{
			fsAbsolutePathMgr::ResolvePath(driver_info.m_SoundName, "Sounds");
		}
		else if (driver_info.m_SoundName.GetNumNames() == 1)
		{
			itString sdname = propGeomList::GetSystemDirName();
			fsysFileUtil::GetFilePath(  sdname,
										itString(gfPaths::GetSubPath( gfPaths::e_Sounds )), 
										driver_info.m_SoundName.GetLastName(), 
										snddir);

			//	if the sound isn't found, add the driver, but don't fail loading the scene.
			//
			//DBG_ASSERT1( snddir.GetNumNames() > 0, "Cannot find sound (%s)", itStringUtil::GetStdString(driver_info.m_SoundName).c_str() );
			if (snddir.GetNumNames() > 0)
			{
				snddir.Remove( gfPaths::GetAppPath() );		// make it a relative path
				snddir.Pop();
			}
		}

		tmlnDriverSound * pDriver = new tmlnDriverSound( prop_obj->AdapterGetPosition(), snddir );
		pDriver->SetDriverInfo(driver_info);

		tmlnChannel& channel = prop_obj->ChannelSound();
		// Attach driver to the appropriate channels
		channel.AddDriver( pDriver );
		return pDriver;
	}
	else if (name == c_PPSP)	// prop position spline
	{
		// Old parser chunk name, convert to new one
		name = cmmDriverCreatorPosition::GetSplineChunkName(c_PositionChannelCode);
	}
	else if (name == c_PATC)	// prop attachment
	{
		// Old parser chunk name, convert to new one
		name = cmmDriverCreatorPosition::GetAttachChunkName(c_PositionChannelCode);
	}
	else if (name == c_PPOS)	// prop static position
	{
		// Old parser chunk name, convert to new one
		name = cmmDriverCreatorPosition::GetKeyChunkName(c_PositionChannelCode);
	}
	else if (name == c_ENBD)
	{
		// Old parser chunk name, convert to new one
		name = cmmDriverCreatorBoolean::GetKeyChunkName(c_VisibleChannelCode);
	}
	else if (name == c_PORI)	// prop static orientation
	{
		// Old parser chunk name, convert to new one
		name = cmmDriverCreatorOrientation::GetKeyChunkName(c_OrientationChannelCode);
	}


	// Creation of drivers per channel
	tmlnDriver* pDriver = NULL;

	// Position channel
	pDriver = cmmDriverCreatorPosition::CreateDriverFromInfo(
		prop_obj->ChannelPosition(), c_PositionChannelCode, i_Info, name, prop_obj );
	if (pDriver != NULL) 
		return pDriver;

	// Visible channel
	pDriver = cmmDriverCreatorBoolean::CreateDriverFromInfo(
		prop_obj->ChannelVisible(), c_VisibleChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// Orientation channel
	pDriver = cmmDriverCreatorOrientation::CreateDriverFromInfo(
		prop_obj->ChannelOrientation(), c_OrientationChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// Scale channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		prop_obj->ChannelScale(), c_ScaleChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	return NULL;
}

