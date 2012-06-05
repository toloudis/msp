/*****************************************************************************
**	aoDriverCreator.cpp
**
**	Creates timeline channels for point lights
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Systems/AmbientOcclusion/Timeline/aoDriverCreator.hpp"

#include "Systems/AmbientOcclusion/Data/aoAODataParser.hpp"
#include "Systems/AmbientOcclusion/Object/aoScriptObject.hpp"
#include "Systems/AmbientOcclusion/Object/aoObjectMgr.hpp"

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
	const char *c_ClipPlaneEpsilonChannelName = "ClipPlaneE";
	const char c_ClipPlaneEpsilonChannelCode[2] = { 'C', 'E' };  // Clip plane Epsilon channel code
	const char *c_NoClipPlaneEpsilonChannelName = "NoClipPlaneE";
	const char c_NoClipPlaneEpsilonChannelCode[2] = { 'N', 'E' };  // No-clip plane Epsilon channel code
	const char *c_AreaRatioChannelName = "AreaRatio";
	const char c_AreaRatioChannelCode[2] = { 'A', 'R' };  // Area Ratio channel code
	const char *c_BehindPlaneEpsilonChannelName = "BehindPlaneE";
	const char c_BehindPlaneEpsilonChannelCode[2] = { 'B', 'E' };  // Behind plane Epsilon channel code
}

//--------------------------------------------------------------------
// Create parsers for driver types created by this creator
//--------------------------------------------------------------------
//static
void aoDriverCreator::CreateParsers()
{
	chDefs::Name sys_code = aoAODataParser::GetChunkName();

	// Creation of driver parsers per channel
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_RadiusChannelCode); 
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_RadiusFarChannelCode); 
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_AngleBiasChannelCode); 
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_AttenuationChannelCode); 
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_ContrastChannelCode); 
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_BlurWidthChannelCode); 
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_BlurSharpnessChannelCode); 
	cmmDriverCreatorColor::CreateParsers(sys_code, c_ColorChannelCode); 
	cmmDriverCreatorColor::CreateParsers(sys_code, c_ClipPlaneEpsilonChannelCode); 
	cmmDriverCreatorColor::CreateParsers(sys_code, c_NoClipPlaneEpsilonChannelCode); 
	cmmDriverCreatorColor::CreateParsers(sys_code, c_AreaRatioChannelCode); 
	cmmDriverCreatorColor::CreateParsers(sys_code, c_BehindPlaneEpsilonChannelCode); 
}

//--------------------------------------------------------------------
// Gather the possible types of drivers that can be created for
//	for this object.  Driver names are added through the
//	DriverNameList's API
//--------------------------------------------------------------------
void aoDriverCreator::GatherPossibleDrivers(	const tmlnScriptObject* i_pObject,
												tmlnDriverNameList& io_Drivers)
{
	if (dynamic_cast<const aoScriptObject*>(i_pObject))
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
		cmmDriverCreatorColor::GatherPossibleDrivers(c_ClipPlaneEpsilonChannelName,
			io_Drivers, "Parameters", this); 
		cmmDriverCreatorColor::GatherPossibleDrivers(c_NoClipPlaneEpsilonChannelName,
			io_Drivers, "Parameters", this); 
		cmmDriverCreatorColor::GatherPossibleDrivers(c_AreaRatioChannelName,
			io_Drivers, "Parameters", this); 
		cmmDriverCreatorColor::GatherPossibleDrivers(c_BehindPlaneEpsilonChannelName,
			io_Drivers, "Parameters", this); 
	}
}

//--------------------------------------------------------------------
// Create driver for this object based on the name used in
//	GatherPossibleDrivers and attach it to the channels
//	on this script object.
//--------------------------------------------------------------------
tmlnDriver* aoDriverCreator::CreateDriverByName(	const char* i_DriverName,
													tmlnScriptObject* i_pObject )
{
	aoScriptObject* ao_obj = dynamic_cast<aoScriptObject*>(i_pObject);
	if (!ao_obj) return NULL;

	// Creation of drivers per channel
	tmlnDriver* pDriver = NULL;

	// Radius Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		ao_obj->RadiusChannel(), c_RadiusChannelName, c_RadiusChannelCode, 
		false );
	if (pDriver != NULL) 
		return pDriver;

	// RadiusFar Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		ao_obj->RadiusFarChannel(), c_RadiusFarChannelName, c_RadiusFarChannelCode, 
		false );
	if (pDriver != NULL) 
		return pDriver;

	// AngleBias Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		ao_obj->AngleBiasChannel(), c_AngleBiasChannelName, c_AngleBiasChannelCode, 
		false );
	if (pDriver != NULL) 
		return pDriver;

	// Attenuation Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		ao_obj->AttenuationChannel(), c_AttenuationChannelName, c_AttenuationChannelCode, 
		false );
	if (pDriver != NULL) 
		return pDriver;

	// Contrast Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		ao_obj->ContrastChannel(), c_ContrastChannelName, c_ContrastChannelCode, 
		false );
	if (pDriver != NULL) 
		return pDriver;

	// BlurWidth Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		ao_obj->BlurWidthChannel(), c_BlurWidthChannelName, c_BlurWidthChannelCode, 
		false );
	if (pDriver != NULL) 
		return pDriver;

	// BlurSharpness Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		ao_obj->BlurSharpnessChannel(), c_BlurSharpnessChannelName, c_BlurSharpnessChannelCode, 
		false );
	if (pDriver != NULL) 
		return pDriver;

	// Color Channel
	pDriver = cmmDriverCreatorColor::CreateDriverByName(i_DriverName, 
		ao_obj->ColorChannel(), c_ColorChannelName, c_ColorChannelCode, 
		false );
	if (pDriver != NULL) 
		return pDriver;

	// ClipPlaneEpsilon Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		ao_obj->ClipPlaneEpsilonChannel(), c_ClipPlaneEpsilonChannelName, c_ClipPlaneEpsilonChannelCode, 
		false );
	if (pDriver != NULL) 
		return pDriver;
	
	// NoClipPlaneEpsilon Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		ao_obj->NoClipPlaneEpsilonChannel(), c_NoClipPlaneEpsilonChannelName, c_NoClipPlaneEpsilonChannelCode, 
		false );
	if (pDriver != NULL) 
		return pDriver;

	// AreaRatio Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		ao_obj->AreaRatioChannel(), c_AreaRatioChannelName, c_AreaRatioChannelCode, 
		false );
	if (pDriver != NULL) 
		return pDriver;

	// BehindPlaneEpsilon Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		ao_obj->BehindPlaneEpsilonChannel(), c_BehindPlaneEpsilonChannelName, c_BehindPlaneEpsilonChannelCode, 
		false );
	if (pDriver != NULL) 
		return pDriver;

	return NULL;
}

//--------------------------------------------------------------------
// Create static key driver for the given channel
//--------------------------------------------------------------------
tmlnDriver* aoDriverCreator::CreateKeyForChannel( tmlnScriptObject* io_pObject,
												    tmlnChannel* i_pChannel)
{
	aoScriptObject* ao_obj = dynamic_cast<aoScriptObject*>(io_pObject);
	if (!ao_obj) return NULL;

	if (i_pChannel == &ao_obj->RadiusChannel())
	{
		// Radius Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			ao_obj->RadiusChannel(), c_RadiusChannelName, c_RadiusChannelCode, 
			false );
	}
	else if (i_pChannel == &ao_obj->RadiusFarChannel())
	{
		// RadiusFar Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			ao_obj->RadiusFarChannel(), c_RadiusFarChannelName, c_RadiusFarChannelCode, 
			false );
	}
	else if (i_pChannel == &ao_obj->AngleBiasChannel())
	{
		// AngleBias Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			ao_obj->AngleBiasChannel(), c_AngleBiasChannelName, c_AngleBiasChannelCode, 
			false );
	}
	else if (i_pChannel == &ao_obj->AttenuationChannel())
	{
		// Attenuation Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			ao_obj->AttenuationChannel(), c_AttenuationChannelName, c_AttenuationChannelCode, 
			false );
	}
	else if (i_pChannel == &ao_obj->ContrastChannel())
	{
		// Contrast Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			ao_obj->ContrastChannel(), c_ContrastChannelName, c_ContrastChannelCode, 
			false );
	}
	else if (i_pChannel == &ao_obj->BlurWidthChannel())
	{
		// BlurWidth Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			ao_obj->BlurWidthChannel(), c_BlurWidthChannelName, c_BlurWidthChannelCode, 
			false );
	}
	else if (i_pChannel == &ao_obj->BlurSharpnessChannel())
	{
		// BlurSharpness Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			ao_obj->BlurSharpnessChannel(), c_BlurSharpnessChannelName, c_BlurSharpnessChannelCode, 
			false );
	}
	else if (i_pChannel == &ao_obj->ColorChannel())
	{
		// Color Channel
		return cmmDriverCreatorColor::CreateKeyForChannel(
			ao_obj->ColorChannel(), c_ColorChannelName, c_ColorChannelCode, 
			false );
	}
	else if (i_pChannel == &ao_obj->ClipPlaneEpsilonChannel())
	{
		// ClipPlaneEpsilon Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			ao_obj->ClipPlaneEpsilonChannel(), c_ClipPlaneEpsilonChannelName, c_ClipPlaneEpsilonChannelCode, 
			false );
	}
	else if (i_pChannel == &ao_obj->NoClipPlaneEpsilonChannel())
	{
		// NoClipPlaneEpsilon Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			ao_obj->NoClipPlaneEpsilonChannel(), c_NoClipPlaneEpsilonChannelName, c_NoClipPlaneEpsilonChannelCode, 
			false );
	}
	else if (i_pChannel == &ao_obj->AreaRatioChannel())
	{
		// AreaRatio Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			ao_obj->AreaRatioChannel(), c_AreaRatioChannelName, c_AreaRatioChannelCode, 
			false );
	}
	else if (i_pChannel == &ao_obj->BehindPlaneEpsilonChannel())
	{
		// BehindPlaneEpsilon Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			ao_obj->BehindPlaneEpsilonChannel(), c_BehindPlaneEpsilonChannelName, c_BehindPlaneEpsilonChannelCode, 
			false );
	}

	return NULL;
}

//--------------------------------------------------------------------
// Create driver for this object based on the the info structure
//--------------------------------------------------------------------
tmlnDriver* aoDriverCreator::CreateDriverFromInfo( tmlnScriptObject* io_pObject,
													 const tmlnDriverInfo& i_Info)
{
	aoScriptObject* ao_obj = dynamic_cast<aoScriptObject*>(io_pObject);
	if (!ao_obj) return NULL;

	chDefs::Name name = i_Info.GetBaseChunkName();

	// Creation of drivers per channel
	tmlnDriver* pDriver = NULL;

	// Radius channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		ao_obj->RadiusChannel(), c_RadiusChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// RadiusFar channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		ao_obj->RadiusFarChannel(), c_RadiusFarChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// AngleBias channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		ao_obj->AngleBiasChannel(), c_AngleBiasChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// Attenuation channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		ao_obj->AttenuationChannel(), c_AttenuationChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// Contrast channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		ao_obj->ContrastChannel(), c_ContrastChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// BlurWidth channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		ao_obj->BlurWidthChannel(), c_BlurWidthChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// BlurSharpness channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		ao_obj->BlurSharpnessChannel(), c_BlurSharpnessChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// Color channel
	pDriver = cmmDriverCreatorColor::CreateDriverFromInfo(
		ao_obj->ColorChannel(), c_ColorChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// ClipPlaneEpsilon channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		ao_obj->ClipPlaneEpsilonChannel(), c_ClipPlaneEpsilonChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// NoClipPlaneEpsilon channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		ao_obj->NoClipPlaneEpsilonChannel(), c_NoClipPlaneEpsilonChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// AreaRatio channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		ao_obj->AreaRatioChannel(), c_AreaRatioChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// BehindPlaneEpsilon channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		ao_obj->BehindPlaneEpsilonChannel(), c_BehindPlaneEpsilonChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	return NULL;
}
