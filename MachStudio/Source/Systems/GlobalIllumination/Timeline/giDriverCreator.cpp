/*****************************************************************************
**	giDriverCreator.cpp
**
**	Creates timeline channels for point lights
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "Systems/GlobalIllumination/Timeline/giDriverCreator.hpp"

#include "Systems/GlobalIllumination/Data/giGIDataParser.hpp"
#include "Systems/GlobalIllumination/Object/giScriptObject.hpp"
#include "Systems/GlobalIllumination/Object/giObjectMgr.hpp"

#include "Systems/Common/DriverCreator/cmmDriverCreatorColor.hpp"
#include "Systems/Common/DriverCreator/cmmDriverCreatorFloat.hpp"

#include "Support/tmln/tmlnChannelColor.hpp"
#include "Support/tmln/tmlnChannelFloat.hpp"

namespace
{
	//========================================================================
	//========================================================================
	const char *c_RadiusChannelName = "Scene Scale Near";
	const char c_RadiusChannelCode[2] = { 'S', 'S' };  // Scene Scale channel code
	const char *c_RadiusFarChannelName = "Scene Scale Far";
	const char c_RadiusFarChannelCode[2] = { 'S', 'F' };  // Scene Scale channel code
	const char *c_AngleBiasChannelName		= "Angle Bias";
	const char c_AngleBiasChannelCode[2]	= { 'A', 'B' };  // Angle Bias channel code
	const char *c_AttenuationChannelName = "Attenuation";
	const char c_AttenuationChannelCode[2] = { 'A', 'T' };  // ATtenuation channel code
	const char *c_ContrastChannelName = "Contrast";
	const char c_ContrastChannelCode[2] = { 'C', 'T' };  // ContrasT channel code
	const char *c_BlurWidthChannelName = "Blur Width";
	const char c_BlurWidthChannelCode[2] = { 'B', 'W' };  // Blur Width channel code
	const char *c_BlurSharpnessChannelName = "Blur Sharpness";
	const char c_BlurSharpnessChannelCode[2] = { 'B', 'S' };  // Blur Sharpness channel code
	const char *c_ColorChannelName = "Color";
	const char c_ColorChannelCode[2] = { 'C', 'L' };  // CoLor channel code
}

//--------------------------------------------------------------------
// Create parsers for driver types created by this creator
//--------------------------------------------------------------------
//static
void giDriverCreator::CreateParsers()
{
	chDefs::Name sys_code = giGIDataParser::GetChunkName();

	// Creation of driver parsers per channel
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_RadiusChannelCode); 
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_RadiusFarChannelCode); 
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_AngleBiasChannelCode); 
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_AttenuationChannelCode); 
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_ContrastChannelCode); 
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_BlurWidthChannelCode); 
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_BlurSharpnessChannelCode); 
	cmmDriverCreatorColor::CreateParsers(sys_code, c_ColorChannelCode); 
}

//--------------------------------------------------------------------
// Gather the possible types of drivers that can be created for
//	for this object.  Driver names are added through the
//	DriverNameList's API
//--------------------------------------------------------------------
void giDriverCreator::GatherPossibleDrivers(	const tmlnScriptObject* i_pObject,
												tmlnDriverNameList& io_Drivers)
{
	if (dynamic_cast<const giScriptObject*>(i_pObject))
	{
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_RadiusChannelName,
			io_Drivers, "Parameters", this); 
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_RadiusFarChannelName,
			io_Drivers, "Parameters", this); 
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_AngleBiasChannelName,
			io_Drivers, "Parameters", this); 
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_AttenuationChannelName,
			io_Drivers, "Parameters", this); 
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_ContrastChannelName,
			io_Drivers, "Parameters", this); 
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_BlurWidthChannelName,
			io_Drivers, "Parameters", this); 
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_BlurSharpnessChannelName,
			io_Drivers, "Parameters", this); 
		cmmDriverCreatorColor::GatherPossibleDrivers(c_ColorChannelName,
			io_Drivers, "Parameters", this); 
	}
}

//--------------------------------------------------------------------
// Create driver for this object based on the name used in
//	GatherPossibleDrivers and attach it to the channels
//	on this script object.
//--------------------------------------------------------------------
tmlnDriver* giDriverCreator::CreateDriverByName(	const char* i_DriverName,
													tmlnScriptObject* i_pObject )
{
	giScriptObject* gi_obj = dynamic_cast<giScriptObject*>(i_pObject);
	if (!gi_obj) return NULL;

	// Creation of drivers per channel
	tmlnDriver* pDriver = NULL;

	// Radius Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		gi_obj->RadiusChannel(), c_RadiusChannelName, c_RadiusChannelCode, 
		false );
	if (pDriver != NULL) 
		return pDriver;

	// RadiusFar Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		gi_obj->RadiusFarChannel(), c_RadiusFarChannelName, c_RadiusFarChannelCode, 
		false );
	if (pDriver != NULL) 
		return pDriver;

	// AngleBias Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		gi_obj->AngleBiasChannel(), c_AngleBiasChannelName, c_AngleBiasChannelCode, 
		false );
	if (pDriver != NULL) 
		return pDriver;

	// Attenuation Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		gi_obj->AttenuationChannel(), c_AttenuationChannelName, c_AttenuationChannelCode, 
		false );
	if (pDriver != NULL) 
		return pDriver;

	// Contrast Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		gi_obj->ContrastChannel(), c_ContrastChannelName, c_ContrastChannelCode, 
		false );
	if (pDriver != NULL) 
		return pDriver;

	// BlurWidth Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		gi_obj->BlurWidthChannel(), c_BlurWidthChannelName, c_BlurWidthChannelCode, 
		false );
	if (pDriver != NULL) 
		return pDriver;

	// BlurSharpness Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		gi_obj->BlurSharpnessChannel(), c_BlurSharpnessChannelName, c_BlurSharpnessChannelCode, 
		false );
	if (pDriver != NULL) 
		return pDriver;

	// Color Channel
	pDriver = cmmDriverCreatorColor::CreateDriverByName(i_DriverName, 
		gi_obj->ColorChannel(), c_ColorChannelName, c_ColorChannelCode, 
		false );
	if (pDriver != NULL) 
		return pDriver;

	return NULL;
}

//--------------------------------------------------------------------
// Create static key driver for the given channel
//--------------------------------------------------------------------
tmlnDriver* giDriverCreator::CreateKeyForChannel( tmlnScriptObject* io_pObject,
												    tmlnChannel* i_pChannel)
{
	giScriptObject* gi_obj = dynamic_cast<giScriptObject*>(io_pObject);
	if (!gi_obj) return NULL;

	if (i_pChannel == &gi_obj->RadiusChannel())
	{
		// Radius Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			gi_obj->RadiusChannel(), c_RadiusChannelName, c_RadiusChannelCode, 
			false );
	}
	else if (i_pChannel == &gi_obj->RadiusFarChannel())
	{
		// RadiusFar Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			gi_obj->RadiusFarChannel(), c_RadiusFarChannelName, c_RadiusFarChannelCode, 
			false );
	}
	else if (i_pChannel == &gi_obj->AngleBiasChannel())
	{
		// AngleBias Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			gi_obj->AngleBiasChannel(), c_AngleBiasChannelName, c_AngleBiasChannelCode, 
			false );
	}
	else if (i_pChannel == &gi_obj->AttenuationChannel())
	{
		// Attenuation Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			gi_obj->AttenuationChannel(), c_AttenuationChannelName, c_AttenuationChannelCode, 
			false );
	}
	else if (i_pChannel == &gi_obj->ContrastChannel())
	{
		// Contrast Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			gi_obj->ContrastChannel(), c_ContrastChannelName, c_ContrastChannelCode, 
			false );
	}
	else if (i_pChannel == &gi_obj->BlurWidthChannel())
	{
		// BlurWidth Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			gi_obj->BlurWidthChannel(), c_BlurWidthChannelName, c_BlurWidthChannelCode, 
			false );
	}
	else if (i_pChannel == &gi_obj->BlurSharpnessChannel())
	{
		// BlurSharpness Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			gi_obj->BlurSharpnessChannel(), c_BlurSharpnessChannelName, c_BlurSharpnessChannelCode, 
			false );
	}
	else if (i_pChannel == &gi_obj->ColorChannel())
	{
		// Color Channel
		return cmmDriverCreatorColor::CreateKeyForChannel(
			gi_obj->ColorChannel(), c_ColorChannelName, c_ColorChannelCode, 
			false );
	}

	return NULL;
}

//--------------------------------------------------------------------
// Create driver for this object based on the the info structure
//--------------------------------------------------------------------
tmlnDriver* giDriverCreator::CreateDriverFromInfo( tmlnScriptObject* io_pObject,
													 const tmlnDriverInfo& i_Info)
{
	giScriptObject* gi_obj = dynamic_cast<giScriptObject*>(io_pObject);
	if (!gi_obj) return NULL;

	chDefs::Name name = i_Info.GetBaseChunkName();

	// Creation of drivers per channel
	tmlnDriver* pDriver = NULL;

	// Radius channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		gi_obj->RadiusChannel(), c_RadiusChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// RadiusFar channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		gi_obj->RadiusFarChannel(), c_RadiusFarChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// AngleBias channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		gi_obj->AngleBiasChannel(), c_AngleBiasChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// Attenuation channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		gi_obj->AttenuationChannel(), c_AttenuationChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// Contrast channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		gi_obj->ContrastChannel(), c_ContrastChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// BlurWidth channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		gi_obj->BlurWidthChannel(), c_BlurWidthChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// BlurSharpness channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		gi_obj->BlurSharpnessChannel(), c_BlurSharpnessChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// Color channel
	pDriver = cmmDriverCreatorColor::CreateDriverFromInfo(
		gi_obj->ColorChannel(), c_ColorChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	return NULL;
}
