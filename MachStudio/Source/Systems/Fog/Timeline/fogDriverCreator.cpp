/*****************************************************************************
**	fogDriverCreator.cpp
**
**	Creates timeline channels for point lights
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Systems/Fog/Timeline/fogDriverCreator.hpp"

#include "Systems/Fog/Data/fogFogDataParser.hpp"
#include "Systems/Fog/Object/fogScriptObject.hpp"
#include "Systems/Fog/Object/fogObjectMgr.hpp"

#include "Systems/Common/DriverCreator/cmmDriverCreatorColor.hpp"
#include "Systems/Common/DriverCreator/cmmDriverCreatorFloat.hpp"
#include "Systems/Common/DriverCreator/cmmDriverCreatorBoolean.hpp"

#include "Support/tmln/tmlnChannelColor.hpp"
#include "Support/tmln/tmlnChannelFloat.hpp"
#include "Support/tmln/tmlnChannelBoolean.hpp"

namespace
{
	//========================================================================
	//========================================================================
	const char *c_bEnableChannelName	= "Fog Enable";
	const char c_bEnableChannelCode[2]	= { 'F', 'E' };  // Fog Enable code
	const char *c_bWorldOrientationChannelName	= "World Orientation";
	const char c_bWorldOrientationChannelCode[2]	= { 'W', 'O' };  // Fog Enable code
	//const char *c_ModeChannelName		= "Mode";
	//const char c_ModeChannelCode[2]		= { 'M', 'D' };  // Mode channel code
	const char *c_ColorChannelName		= "Color";
	const char c_ColorChannelCode[2]	= { 'C', 'L' };  // Color channel code
	const char *c_DensityChannelName	= "Density";
	const char c_DensityChannelCode[2]	= { 'D', 'N' };  // Density channel code
	const char *c_StartChannelName		= "Start";
	const char c_StartChannelCode[2]	= { 'S', 'T' };  // Start channel code
	const char *c_EndChannelName		= "End";
	const char c_EndChannelCode[2]		= { 'E', 'N' };  // End channel code
	const char *c_AltitudeStartChannelName		= "Altitude Start";
	const char c_AltitudeStartChannelCode[2]	= { 'A', 'S' };  // Altitude Start channel code
	const char *c_AltitudeEndChannelName		= "Altitude End";
	const char c_AltitudeEndChannelCode[2]		= { 'A', 'E' };  // Altitude End code
	const char *c_AltitudeDensityChannelName	= "Altitude Density";
	const char c_AltitudeDensityChannelCode[2]	= { 'A', 'D' };  // Altitude Density code
}

//--------------------------------------------------------------------
// Create parsers for driver types created by this creator
//--------------------------------------------------------------------
//static
void fogDriverCreator::CreateParsers()
{
	chDefs::Name sys_code = fogFogDataParser::GetChunkName();

	// Creation of driver parsers per channel
	cmmDriverCreatorBoolean::CreateParsers(sys_code, c_bEnableChannelCode); 
	cmmDriverCreatorBoolean::CreateParsers(sys_code, c_bWorldOrientationChannelCode); 
	//cmmDriverCreator::CreateParsers(sys_code, c_ModeChannelCode); 
	cmmDriverCreatorColor::CreateParsers(sys_code, c_ColorChannelCode); 
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_DensityChannelCode); 
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_StartChannelCode); 
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_EndChannelCode); 
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_AltitudeStartChannelCode); 
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_AltitudeEndChannelCode); 
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_AltitudeDensityChannelCode); 
}

//--------------------------------------------------------------------
// Gather the possible types of drivers that can be created for
//	for this object.  Driver names are added through the
//	DriverNameList's API
//--------------------------------------------------------------------
void fogDriverCreator::GatherPossibleDrivers(	const tmlnScriptObject* i_pObject,
												tmlnDriverNameList& io_Drivers)
{
	if (dynamic_cast<const fogScriptObject*>(i_pObject))
	{
		cmmDriverCreatorColor::GatherPossibleDrivers(c_bEnableChannelName,
			io_Drivers, "Parameters", this);
		cmmDriverCreatorColor::GatherPossibleDrivers(c_bWorldOrientationChannelName,
			io_Drivers, "Parameters", this);
/*		cmmDriverCreatorFloat::GatherPossibleDrivers(c_ModeChannelName,
			io_Drivers, "Parameters", this);*/ 
		cmmDriverCreatorColor::GatherPossibleDrivers(c_ColorChannelName,
			io_Drivers, "Parameters", this);
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_DensityChannelName,
			io_Drivers, "Parameters", this); 
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_StartChannelName,
			io_Drivers, "Parameters", this); 
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_EndChannelName,
			io_Drivers, "Parameters", this); 
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_AltitudeStartChannelName,
			io_Drivers, "Parameters", this); 
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_AltitudeEndChannelName,
			io_Drivers, "Parameters", this);  
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_AltitudeDensityChannelName,
			io_Drivers, "Parameters", this); 
	}
}

//--------------------------------------------------------------------
// Create driver for this object based on the name used in
//	GatherPossibleDrivers and attach it to the channels
//	on this script object.
//--------------------------------------------------------------------
tmlnDriver* fogDriverCreator::CreateDriverByName(	const char* i_DriverName,
													tmlnScriptObject* i_pObject )
{
	fogScriptObject* fog_obj = dynamic_cast<fogScriptObject*>(i_pObject);
	if (!fog_obj) return NULL;

	// Creation of drivers per channel
	tmlnDriver* pDriver = NULL;
	
	// bEnable Channel	
	pDriver = cmmDriverCreatorBoolean::CreateDriverByName(i_DriverName, 
		fog_obj->bEnableChannel(), c_bEnableChannelName, c_bEnableChannelCode, 
		false );
	if (pDriver != NULL) 
		return pDriver;

	// bWorldOrientation Channel	
	pDriver = cmmDriverCreatorBoolean::CreateDriverByName(i_DriverName, 
		fog_obj->bWorldOrientationChannel(), c_bWorldOrientationChannelName, c_bWorldOrientationChannelCode, 
		false );
	if (pDriver != NULL) 
		return pDriver;

	//// Mode Channel
	//pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
	//	fog_obj->ModeChannel(), c_ModeChannelName, c_ModeChannelCode, 
	//	false );
	//if (pDriver != NULL) 
	//	return pDriver;

	// Color Channel
	pDriver = cmmDriverCreatorColor::CreateDriverByName(i_DriverName, 
		fog_obj->ColorChannel(), c_ColorChannelName, c_ColorChannelCode, 
		false );
	if (pDriver != NULL) 
		return pDriver;

	// Density Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		fog_obj->DensityChannel(), c_DensityChannelName, c_DensityChannelCode, 
		false );
	if (pDriver != NULL) 
		return pDriver;

	// Start Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		fog_obj->StartChannel(), c_StartChannelName, c_StartChannelCode, 
		false );
	if (pDriver != NULL) 
		return pDriver;

	// End Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		fog_obj->EndChannel(), c_EndChannelName, c_EndChannelCode, 
		false );
	if (pDriver != NULL) 
		return pDriver;

	// AltitudeStart Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		fog_obj->AltitudeStartChannel(), c_AltitudeStartChannelName, c_AltitudeStartChannelCode, 
		false );
	if (pDriver != NULL) 
		return pDriver;

	// AltitudeEnd Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		fog_obj->AltitudeEndChannel(), c_AltitudeEndChannelName, c_AltitudeEndChannelCode, 
		false );
	if (pDriver != NULL) 
		return pDriver;

	// AltitudeDensity Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		fog_obj->AltitudeDensityChannel(), c_AltitudeDensityChannelName, c_AltitudeDensityChannelCode, 
		false );
	if (pDriver != NULL) 
		return pDriver;

	return NULL;
}

//--------------------------------------------------------------------
// Create static key driver for the given channel
//--------------------------------------------------------------------
tmlnDriver* fogDriverCreator::CreateKeyForChannel( tmlnScriptObject* io_pObject,
												    tmlnChannel* i_pChannel)
{
	fogScriptObject* fog_obj = dynamic_cast<fogScriptObject*>(io_pObject);
	if (!fog_obj) return NULL;

	if (i_pChannel == &fog_obj->bEnableChannel())
	{
		// bEnable Channel
		return cmmDriverCreatorBoolean::CreateKeyForChannel(
			fog_obj->bEnableChannel(), c_bEnableChannelName, c_bEnableChannelCode, 
			false );
	}

	if (i_pChannel == &fog_obj->bWorldOrientationChannel())
	{
		// bWorldOrientation Channel
		return cmmDriverCreatorBoolean::CreateKeyForChannel(
			fog_obj->bWorldOrientationChannel(), c_bWorldOrientationChannelName, c_bWorldOrientationChannelCode, 
			false );
	}

	//if (i_pChannel == &fog_obj->ModeChannel())
	//{
	//	// Mode Channel
	//	return cmmDriverCreatorFloat::CreateKeyForChannel(
	//		fog_obj->ModeChannel(), c_ModeChannelName, c_ModeChannelCode, 
	//		false );
	//}
	//else 
	else if (i_pChannel == &fog_obj->ColorChannel())
	{
		// Color Channel
		return cmmDriverCreatorColor::CreateKeyForChannel(
			fog_obj->ColorChannel(), c_ColorChannelName, c_ColorChannelCode, 
			false );
	}
	else if (i_pChannel == &fog_obj->DensityChannel())
	{
		// Density Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			fog_obj->DensityChannel(), c_DensityChannelName, c_DensityChannelCode, 
			false );
	}
	else if (i_pChannel == &fog_obj->StartChannel())
	{
		// Start Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			fog_obj->StartChannel(), c_StartChannelName, c_StartChannelCode, 
			false );
	}
	else if (i_pChannel == &fog_obj->EndChannel())
	{
		// End Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			fog_obj->EndChannel(), c_EndChannelName, c_EndChannelCode, 
			false );
	}
	else if (i_pChannel == &fog_obj->AltitudeStartChannel())
	{
		// AltitudeStart Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			fog_obj->AltitudeStartChannel(), c_AltitudeStartChannelName, c_AltitudeStartChannelCode, 
			false );
	}
	else if (i_pChannel == &fog_obj->AltitudeEndChannel())
	{
		// AltitudeEnd Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			fog_obj->AltitudeEndChannel(), c_AltitudeEndChannelName, c_AltitudeEndChannelCode, 
			false );
	}
	else if (i_pChannel == &fog_obj->AltitudeDensityChannel())
	{
		// AltitudeDensity Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			fog_obj->AltitudeDensityChannel(), c_AltitudeDensityChannelName, c_AltitudeDensityChannelCode, 
			false );
	}

	return NULL;
}

//--------------------------------------------------------------------
// Create driver for this object based on the the info structure
//--------------------------------------------------------------------
tmlnDriver* fogDriverCreator::CreateDriverFromInfo( tmlnScriptObject* io_pObject,
													 const tmlnDriverInfo& i_Info)
{
	fogScriptObject* fog_obj = dynamic_cast<fogScriptObject*>(io_pObject);
	if (!fog_obj) return NULL;

	chDefs::Name name = i_Info.GetBaseChunkName();

	// Creation of drivers per channel
	tmlnDriver* pDriver = NULL;

	// Color channel
	pDriver = cmmDriverCreatorBoolean::CreateDriverFromInfo(
		fog_obj->bEnableChannel(), c_bEnableChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// WorldOrientation channel
	pDriver = cmmDriverCreatorBoolean::CreateDriverFromInfo(
		fog_obj->bWorldOrientationChannel(), c_bWorldOrientationChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	//// Mode channel
	//pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
	//	fog_obj->ModeChannel(), c_ModeChannelCode, i_Info, name );
	//if (pDriver != NULL) 
	//	return pDriver;

	// Color channel
	pDriver = cmmDriverCreatorColor::CreateDriverFromInfo(
		fog_obj->ColorChannel(), c_ColorChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// Density channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		fog_obj->DensityChannel(), c_DensityChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// Start channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		fog_obj->StartChannel(), c_StartChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// End channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		fog_obj->EndChannel(), c_EndChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// AltitudeStart channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		fog_obj->AltitudeStartChannel(), c_AltitudeStartChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// AltitudeEnd channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		fog_obj->AltitudeEndChannel(), c_AltitudeEndChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// AltitudeDensity channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		fog_obj->AltitudeDensityChannel(), c_AltitudeDensityChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	return NULL;
}
