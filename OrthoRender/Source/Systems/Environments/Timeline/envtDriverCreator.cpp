/*****************************************************************************
**	envtDriverCreator.cpp
**
**	Creates timeline channels for point lights
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Systems/Environments/Timeline/envtDriverCreator.hpp"

#include "Systems/Environments/Data/envtEnvironmentsDataParser.hpp"
#include "Systems/Environments/Object/envtScriptObject.hpp"
#include "Systems/Environments/Object/envtObjectMgr.hpp"

#include "Systems/Common/DriverCreator/cmmDriverCreatorFloat.hpp"

#include "Support/tmln/tmlnChannelFloat.hpp"
#include "Support/tmln/tmlnDriverInfo.hpp"


namespace
{
	//========================================================================
	//========================================================================
	const char *c_DiffuseFactorChannelName = "Diffuse Factor";
	const char c_DiffuseFactorChannelCode[2] = { 'D', 'F' };  // Diffuse Factor channel code
	const char *c_DiffuseAngleChannelName = "Diffuse Angle";
	const char c_DiffuseAngleChannelCode[2] = { 'D', 'A' };  // Diffuse Angle channel code
	const char *c_SpecularFactorChannelName = "Specular Factor";
	const char c_SpecularFactorChannelCode[2] = { 'S', 'F' };  // Specular Factor channel code
	const char *c_SpecularAngleChannelName = "Specular Angle";
	const char c_SpecularAngleChannelCode[2] = { 'S', 'A' };  // Specular Angle channel code

}

//--------------------------------------------------------------------
// Create parsers for driver types created by this creator
//--------------------------------------------------------------------
//static
void envtDriverCreator::CreateParsers()
{
	chDefs::Name sys_code = envtEnvironmentsDataParser::GetChunkName();

	// Creation of driver parsers per channel
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_DiffuseFactorChannelCode); 
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_DiffuseAngleChannelCode); 
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_SpecularFactorChannelCode); 
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_SpecularAngleChannelCode); 

}

//--------------------------------------------------------------------
// Gather the possible types of drivers that can be created for
//	for this object.  Driver names are added through the
//	DriverNameList's API
//--------------------------------------------------------------------
void envtDriverCreator::GatherPossibleDrivers(	const tmlnScriptObject* i_pObject,
												tmlnDriverNameList& io_Drivers)
{
	if (dynamic_cast<const envtScriptObject*>(i_pObject))
	{
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_DiffuseFactorChannelName,
			io_Drivers, "Parameters", this); 
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_DiffuseAngleChannelName,
			io_Drivers, "Parameters", this); 
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_SpecularFactorChannelName,
			io_Drivers, "Parameters", this); 
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_SpecularAngleChannelName,
			io_Drivers, "Parameters", this); 
	}
}

//--------------------------------------------------------------------
// Create driver for this object based on the name used in
//	GatherPossibleDrivers and attach it to the channels
//	on this script object.
//--------------------------------------------------------------------
tmlnDriver* envtDriverCreator::CreateDriverByName(	const char* i_DriverName,
													tmlnScriptObject* i_pObject )
{
	envtScriptObject* envt_obj = dynamic_cast<envtScriptObject*>(i_pObject);
	if (!envt_obj) return NULL;

	// Creation of drivers per channel
	tmlnDriver* pDriver = NULL;

	// Diffuse Factor Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		envt_obj->DiffuseFactorChannel(), c_DiffuseFactorChannelName, c_DiffuseFactorChannelCode );
	if (pDriver != NULL) 
		return pDriver;

	// Diffuse Angle Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		envt_obj->DiffuseAngleChannel(), c_DiffuseAngleChannelName, c_DiffuseAngleChannelCode );
	if (pDriver != NULL) 
		return pDriver;

	// Specular Factor Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		envt_obj->SpecularFactorChannel(), c_SpecularFactorChannelName, c_SpecularFactorChannelCode );
	if (pDriver != NULL) 
		return pDriver;

	// Specular Angle Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		envt_obj->SpecularAngleChannel(), c_SpecularAngleChannelName, c_SpecularFactorChannelCode );
	if (pDriver != NULL) 
		return pDriver;

	return NULL;
}

//--------------------------------------------------------------------
// Create static key driver for the given channel
//--------------------------------------------------------------------
tmlnDriver* envtDriverCreator::CreateKeyForChannel( tmlnScriptObject* io_pObject,
												    tmlnChannel* i_pChannel)
{
	envtScriptObject* envt_obj = dynamic_cast<envtScriptObject*>(io_pObject);
	if (!envt_obj) return NULL;

	if (i_pChannel == &envt_obj->DiffuseFactorChannel())
	{
		// Diffuse Factor Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			envt_obj->DiffuseFactorChannel(), c_DiffuseFactorChannelName, c_DiffuseFactorChannelCode );
	}
	else if (i_pChannel == &envt_obj->DiffuseAngleChannel())
	{
		// Diffuse Angle Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			envt_obj->DiffuseAngleChannel(), c_DiffuseAngleChannelName, c_DiffuseAngleChannelCode );
	}
	else if (i_pChannel == &envt_obj->SpecularFactorChannel())
	{
		// Specular Factor Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			envt_obj->SpecularFactorChannel(), c_SpecularFactorChannelName, c_SpecularFactorChannelCode );
	}
	else if (i_pChannel == &envt_obj->SpecularAngleChannel())
	{
		// Specular Angle Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			envt_obj->SpecularAngleChannel(), c_SpecularAngleChannelName, c_SpecularFactorChannelCode );
	}

	return NULL;
}

//--------------------------------------------------------------------
// Create driver for this object based on the the info structure
//--------------------------------------------------------------------
tmlnDriver* envtDriverCreator::CreateDriverFromInfo( tmlnScriptObject* io_pObject,
													 const tmlnDriverInfo& i_Info)
{
	envtScriptObject* envt_obj = dynamic_cast<envtScriptObject*>(io_pObject);
	if (!envt_obj) return NULL;

	chDefs::Name name = i_Info.GetBaseChunkName();

	// Creation of drivers per channel
	tmlnDriver* pDriver = NULL;

	// Diffuse Factor channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		envt_obj->DiffuseFactorChannel(), c_DiffuseFactorChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// Diffuse Angle channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		envt_obj->DiffuseAngleChannel(), c_DiffuseAngleChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// Specular Factor channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		envt_obj->SpecularFactorChannel(), c_SpecularFactorChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// Specular Angle channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		envt_obj->SpecularAngleChannel(), c_SpecularAngleChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	return NULL;
}
