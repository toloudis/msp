/*****************************************************************************
**	trfnDriverCreator.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Systems/Transforms/Timeline/trfnDriverCreator.hpp"
#include "Systems/Transforms/Data/trfnTransformsDataParser.hpp"
#include "Systems/Transforms/Object/trfnObjectMgr.hpp"

#include "Drivers/Attach/tmlnDriverAttachOrient.hpp"
#include "Drivers/Attach/tmlnDriverAttachOrientInfo.hpp"
#include "Drivers/Attach/tmlnDriverAttachOrientParser.hpp"
#include "Drivers/Spline/tmlnDriverSplineInfo.hpp"
#include "Drivers/Spline/tmlnDriverSplineOriented.hpp"
#include "Drivers/Spline/tmlnDriverSplineParser.hpp"
#include "Support/spln/splnSpline.hpp"
#include "Support/tmln/tmlnChannelBoolean.hpp"
#include "Support/tmln/tmlnChannelFloat.hpp"
#include "Support/tmln/tmlnChannelOrientation.hpp"
#include "Support/tmln/tmlnChannelPosition.hpp"
#include "Systems/Common/DriverCreator/cmmDriverCreatorBoolean.hpp"
#include "Systems/Common/DriverCreator/cmmDriverCreatorFloat.hpp"
#include "Systems/Common/DriverCreator/cmmDriverCreatorOrientation.hpp"
#include "Systems/Common/DriverCreator/cmmDriverCreatorPosition.hpp"


//============================================================================
//============================================================================
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

	const char* TRFNDRIVERNAME_SPLINEORI = "Spline Oriented";
	const char* TRFNDRIVERNAME_ATTACHORIENT = "Attach Orient";

	const chDefs::Name c_TATO = chDefs::MakeName('T', 'A', 'T', 'O');  // Prop Attach Orient
	const chDefs::Name c_TOSP = chDefs::MakeName('T', 'O', 'S', 'P');  // Transform Orientation SPline

}

//--------------------------------------------------------------------
// Set directory to look for sound files within
//--------------------------------------------------------------------
trfnDriverCreator::trfnDriverCreator()
{
}

//--------------------------------------------------------------------
// Create parsers for driver types created by this creator
//--------------------------------------------------------------------
//static
void trfnDriverCreator::CreateParsers()
{
	chDefs::Name sys_code = trfnTransformsDataParser::GetChunkName();

	// Creation of driver parsers per channel
	cmmDriverCreatorPosition::CreateParsers(sys_code, c_PositionChannelCode); 
	cmmDriverCreatorBoolean::CreateParsers(sys_code, c_VisibleChannelCode); 
	cmmDriverCreatorOrientation::CreateParsers(sys_code, c_OrientationChannelCode); 
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_ScaleChannelCode); 

	// System specific drivers
	tmlnParser::AddDriverParser(sys_code, c_TOSP, new tmlnDriverSplineParser(c_TOSP));
	tmlnParser::AddDriverParser(sys_code, c_TATO, new tmlnDriverAttachOrientParser(c_TATO));
}

//--------------------------------------------------------------------
// Gather the possible types of drivers that can be created for
//	for this object.  Driver names are added through the
//	DriverNameList's API
//--------------------------------------------------------------------
void trfnDriverCreator::GatherPossibleDrivers( const tmlnScriptObject* i_pObject,
											   tmlnDriverNameList& io_Drivers )
{
	if (dynamic_cast<const trfnScriptObject*>(i_pObject))
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

		io_Drivers.AddDriver(TRFNDRIVERNAME_SPLINEORI, "Motion", this);
		io_Drivers.AddDriver(TRFNDRIVERNAME_ATTACHORIENT, "Motion", this);
	}
}

//--------------------------------------------------------------------
// Create driver for this object based on the name used in
//	GatherPossibleDrivers and attach it to the channels
//	on this script object.
//--------------------------------------------------------------------
tmlnDriver* trfnDriverCreator::CreateDriverByName( const char* i_DriverName,
													tmlnScriptObject* i_pObject )
{
	trfnScriptObject* trfn_obj = dynamic_cast<trfnScriptObject*>(i_pObject);
	if (!trfn_obj) return NULL;

	// Creation of drivers per channel
	tmlnDriver* pDriver = NULL;

	// Position Channel
	pDriver = cmmDriverCreatorPosition::CreateDriverByName(i_DriverName, 
		trfn_obj->ChannelPosition(), c_PositionChannelName, c_PositionChannelCode, 
		trfnObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// Orientation Channel
	pDriver = cmmDriverCreatorOrientation::CreateDriverByName(i_DriverName, 
		trfn_obj->ChannelOrientation(), c_OrientationChannelName, c_OrientationChannelCode, 
		trfnObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// Scale Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		trfn_obj->ChannelScale(), c_ScaleChannelName, c_ScaleChannelCode, 
		trfnObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// Visible Channel
	pDriver = cmmDriverCreatorBoolean::CreateDriverByName(i_DriverName, 
		trfn_obj->ChannelVisible(), c_VisibleChannelName, c_VisibleChannelCode, 
		trfnObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	if (!::_stricmp(i_DriverName,TRFNDRIVERNAME_SPLINEORI))
	{
		tmlnDriverSplineOriented * pDriver = new tmlnDriverSplineOriented( trfn_obj->ChannelPosition(), 
			trfn_obj->ChannelOrientation(), c_TOSP );
		pDriver->SetName(TRFNDRIVERNAME_SPLINEORI);
		pDriver->SetInitialTime();
		pDriver->ShowIcons( trfnObjectMgr::IconsVisible() );

		tmlnChannel& Ochannel = trfn_obj->ChannelOrientation();
		tmlnChannelPosition& Pchannel = trfn_obj->ChannelPosition();
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
	else if (!::_stricmp(i_DriverName, TRFNDRIVERNAME_ATTACHORIENT))
	{
		tmlnDriverAttachOrient * pDriver = new tmlnDriverAttachOrient( trfn_obj->ChannelPosition(), trfn_obj->ChannelOrientation(), c_TATO );
		pDriver->SetName(TRFNDRIVERNAME_ATTACHORIENT);
		pDriver->SetInitialTime();
		pDriver->ShowIcons( trfnObjectMgr::IconsVisible() );

		tmlnChannel& Pchannel = trfn_obj->ChannelPosition();
		tmlnChannel& Ochannel = trfn_obj->ChannelOrientation();

		// Attach driver to the appropriate channels
		Pchannel.AddDriver( pDriver );
		Ochannel.AddDriver( pDriver );
		return pDriver;
	}

	return 0;
}

//--------------------------------------------------------------------
// Create static key driver for the given channel
//--------------------------------------------------------------------
tmlnDriver* trfnDriverCreator::CreateKeyForChannel( tmlnScriptObject* io_pObject,
												    tmlnChannel* i_pChannel)
{
	trfnScriptObject* trfn_obj = dynamic_cast<trfnScriptObject*>(io_pObject);
	if (!trfn_obj) return NULL;

	if (i_pChannel == &trfn_obj->ChannelPosition())
	{
		// Position Channel
		return cmmDriverCreatorPosition::CreateKeyForChannel(
			trfn_obj->ChannelPosition(), c_PositionChannelName, c_PositionChannelCode, 
			trfnObjectMgr::IconsVisible() );
	}
	else if (i_pChannel == &trfn_obj->ChannelOrientation())
	{
		// Orientation Channel
		return cmmDriverCreatorOrientation::CreateKeyForChannel(
			trfn_obj->ChannelOrientation(), c_OrientationChannelName, c_OrientationChannelCode, 
			trfnObjectMgr::IconsVisible() );
	}
	else if (i_pChannel == &trfn_obj->ChannelScale())
	{
		// Scale Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			trfn_obj->ChannelScale(), c_ScaleChannelName, c_ScaleChannelCode, 
			trfnObjectMgr::IconsVisible() );
	}
	else if (i_pChannel == &trfn_obj->ChannelVisible())
	{
		// Visible Channel
		return cmmDriverCreatorBoolean::CreateKeyForChannel(
			trfn_obj->ChannelVisible(), c_VisibleChannelName, c_VisibleChannelCode, 
			trfnObjectMgr::IconsVisible() );
	}

	return NULL;
}

//--------------------------------------------------------------------
// Create driver for this object based on the the info structure
//--------------------------------------------------------------------
tmlnDriver* trfnDriverCreator::CreateDriverFromInfo( tmlnScriptObject* io_pObject,
					const tmlnDriverInfo& i_Info)
{
	trfnScriptObject* trfn_obj = dynamic_cast<trfnScriptObject*>(io_pObject);
	if (!trfn_obj) return NULL;

	chDefs::Name name = i_Info.GetBaseChunkName();
	if (name == c_TOSP)	// character Orientation spline
	{
		const tmlnDriverSplineInfo& driver_info = dynamic_cast<const tmlnDriverSplineInfo&>(i_Info);
		tmlnDriverSplineOriented * pDriver = new tmlnDriverSplineOriented( trfn_obj->ChannelPosition(), 
			trfn_obj->ChannelOrientation(), c_TOSP );
		pDriver->SetDriverInfo(driver_info);

		tmlnChannel& Pchannel = trfn_obj->ChannelPosition();
		tmlnChannel& Ochannel = trfn_obj->ChannelOrientation();
		// Attach driver to the appropriate channels
		Pchannel.AddDriver( pDriver );
		Ochannel.AddDriver( pDriver );
		return pDriver;
	}
	else if (name == c_TATO)	// attach orient
	{
		const tmlnDriverAttachOrientInfo& driver_info = dynamic_cast<const tmlnDriverAttachOrientInfo&>(i_Info);
		tmlnDriverAttachOrient * pDriver = new tmlnDriverAttachOrient( trfn_obj->ChannelPosition(), trfn_obj->ChannelOrientation(), c_TATO );
		pDriver->SetDriverInfo(driver_info);

		// Attach driver to the appropriate channels
		tmlnChannel& Pchannel = trfn_obj->ChannelPosition();
		tmlnChannel& Ochannel = trfn_obj->ChannelOrientation();
		Pchannel.AddDriver( pDriver );
		Ochannel.AddDriver( pDriver );
		return pDriver;
	}

	// Creation of drivers per channel
	tmlnDriver* pDriver = NULL;

	// Position channel
	pDriver = cmmDriverCreatorPosition::CreateDriverFromInfo(
		trfn_obj->ChannelPosition(), c_PositionChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// Orientation channel
	pDriver = cmmDriverCreatorOrientation::CreateDriverFromInfo(
		trfn_obj->ChannelOrientation(), c_OrientationChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// Scale channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		trfn_obj->ChannelScale(), c_ScaleChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// Visible channel
	pDriver = cmmDriverCreatorBoolean::CreateDriverFromInfo(
		trfn_obj->ChannelVisible(), c_VisibleChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	return NULL;
}

