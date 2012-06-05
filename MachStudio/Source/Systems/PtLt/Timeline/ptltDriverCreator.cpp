/*****************************************************************************
**	ptltDriverCreator.cpp
**
**	Creates timeline channels for point lights
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Systems/PtLt/Timeline/ptltDriverCreator.hpp"

#include "Systems/PtLt/Data/ptltDataParser.hpp"
#include "Systems/PtLt/Object/ptltScriptObject.hpp"
#include "Systems/PtLt/Object/ptltObjectMgr.hpp"

#include "Systems/Common/DriverCreator/cmmDriverCreatorBoolean.hpp"
#include "Systems/Common/DriverCreator/cmmDriverCreatorColor.hpp"
#include "Systems/Common/DriverCreator/cmmDriverCreatorFloat.hpp"
#include "Systems/Common/DriverCreator/cmmDriverCreatorPosition.hpp"

#include "Support/tmln/tmlnChannelBoolean.hpp"
#include "Support/tmln/tmlnChannelColor.hpp"
#include "Support/tmln/tmlnChannelFloat.hpp"
#include "Support/tmln/tmlnChannelPosition.hpp"
#include "Drivers/Attach/tmlnDriverAttachParser.hpp"
#include "Drivers/Color/tmlnDriverColorParser.hpp"
#include "Drivers/ColorFlicker/tmlnDriverColorFlickerParser.hpp"
#include "Drivers/Enable/tmlnDriverEnableParser.hpp"
#include "Support/tmln/tmlnDriverInfo.hpp"
#include "Drivers/Position/tmlnDriverPositionParser.hpp"
#include "Drivers/Spline/tmlnDriverSplineParser.hpp"


namespace
{
	//========================================================================
	//========================================================================
	const char *c_PositionChannelName = "Position";
	const char c_PositionChannelCode[2] = { 'L', 'P' };  // Light Position channel code
	const char *c_ColorChannelName		= "Color";
	const char c_ColorChannelCode[2]	= { 'C', 'K' };  // Color Key
	const char *c_EnabledChannelName = "Enabled";
	const char c_EnabledChannelCode[2] = { 'E', 'N' };  // ENabled channel code
	const char *c_RangeChannelName = "Range";
	const char c_RangeChannelCode[2] = { 'R', 'N' };  // RaNge channel code
	const char *c_ShadowSourceChannelName = "Shadow Source";
	const char c_ShadowSourceChannelCode[2] = { 'S', 'S' };  // ShadowSource channel code
	const char *c_EnableDiffuseChannelName = "Enable Diffuse";
	const char c_EnableDiffuseChannelCode[2] = { 'E', 'D' };  // EnableDiffuse channel code
	const char *c_EnableSpecularChannelName = "Enable Specular";
	const char c_EnableSpecularChannelCode[2] = { 'E', 'S' };  // EnableSpecular channel code
	const char *c_EnableFurChannelName = "Enable Fur";
	const char c_EnableFurChannelCode[2] = { 'E', 'F' };  // EnableFur channel code
	const char *c_AffectsGlowChannelName = "Affects Glow";
	const char c_AffectsGlowChannelCode[2] = { 'A', 'G' };  // AffectsGlow channel code
	const char *c_FalloffChannelName = "Falloff";
	const char c_FalloffChannelCode[2] = { 'F', 'A' };  // Falloff channel code
	const char *c_IntensityChannelName = "Intensity";
	const char c_IntensityChannelCode[2] = { 'I', 'N' };  // Intensity channel code

	//========================================================================
	//========================================================================
	const chDefs::Name c_LPSP = chDefs::MakeName('L', 'P', 'S', 'P');  // Light Position SPline
	const chDefs::Name c_LATC = chDefs::MakeName('L', 'A', 'C', 'H');  // Light Attachment
	const chDefs::Name c_LCLR = chDefs::MakeName('L', 'C', 'L', 'R');  // Light Color
	const chDefs::Name c_LPOS = chDefs::MakeName('L', 'P', 'O', 'S');  // Light Static position
	const chDefs::Name c_CLRF = chDefs::MakeName('C', 'L', 'R', 'F');  // Light Color Flicker
	const chDefs::Name c_ENBD = chDefs::MakeName('E', 'N', 'B', 'D');  // Enabled
}

//--------------------------------------------------------------------
// Create parsers for driver types created by this creator
//--------------------------------------------------------------------
//static
void ptltDriverCreator::CreateParsers()
{
	chDefs::Name sys_code = ptltDataParser::GetChunkName();

	// Creation of driver parsers per channel
	cmmDriverCreatorPosition::CreateParsers(sys_code, c_PositionChannelCode); 
	cmmDriverCreatorColor::CreateParsers(sys_code, c_ColorChannelCode); 
	cmmDriverCreatorBoolean::CreateParsers(sys_code, c_EnabledChannelCode); 
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_RangeChannelCode); 
	cmmDriverCreatorBoolean::CreateParsers(sys_code, c_ShadowSourceChannelCode); 
	cmmDriverCreatorBoolean::CreateParsers(sys_code, c_EnableDiffuseChannelCode); 
	cmmDriverCreatorBoolean::CreateParsers(sys_code, c_EnableSpecularChannelCode); 
	cmmDriverCreatorBoolean::CreateParsers(sys_code, c_EnableFurChannelCode); 
	cmmDriverCreatorBoolean::CreateParsers(sys_code, c_AffectsGlowChannelCode); 
	cmmDriverCreatorPosition::CreateParsers(sys_code, c_FalloffChannelCode); 
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_IntensityChannelCode); 

	//	old format
	tmlnParser::AddDriverParser(sys_code, c_LPOS, new tmlnDriverPositionParser(c_LPOS));
	tmlnParser::AddDriverParser(sys_code, c_LATC, new tmlnDriverAttachParser(c_LATC));
	tmlnParser::AddDriverParser(sys_code, c_ENBD, new tmlnDriverEnableParser(c_ENBD));
	tmlnParser::AddDriverParser(sys_code, c_LCLR, new tmlnDriverColorParser(c_LCLR));
	tmlnParser::AddDriverParser(sys_code, c_CLRF, new tmlnDriverColorFlickerParser(c_CLRF) );

}

//--------------------------------------------------------------------
// Expose chunk names used in file format
//--------------------------------------------------------------------
//static
chDefs::Name ptltDriverCreator::GetPositionAttachChunkName()
{
	return cmmDriverCreatorPosition::GetAttachChunkName(c_PositionChannelCode);
	//return c_LATC;
}

//--------------------------------------------------------------------
// Gather the possible types of drivers that can be created for
//	for this object.  Driver names are added through the
//	DriverNameList's API
//--------------------------------------------------------------------
void ptltDriverCreator::GatherPossibleDrivers(	const tmlnScriptObject* i_pObject,
												tmlnDriverNameList& io_Drivers)
{
	if (dynamic_cast<const ptltScriptObject*>(i_pObject))
	{
		cmmDriverCreatorPosition::GatherPossibleDrivers(c_PositionChannelName,
			io_Drivers, "Motion", this); 
		cmmDriverCreatorBoolean::GatherPossibleDrivers(c_EnabledChannelName,
			io_Drivers, "Parameters", this); 
		cmmDriverCreatorColor::GatherPossibleDrivers(c_ColorChannelName,
			io_Drivers, "Parameters", this); 
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_RangeChannelName,
			io_Drivers, "Parameters", this); 
//		cmmDriverCreatorBoolean::GatherPossibleDrivers(c_ShadowSourceChannelName,
//			io_Drivers, "Shadows", this); 
		cmmDriverCreatorBoolean::GatherPossibleDrivers(c_EnableDiffuseChannelName,
			io_Drivers, "Parameters", this); 
		cmmDriverCreatorBoolean::GatherPossibleDrivers(c_EnableSpecularChannelName,
			io_Drivers, "Parameters", this); 
		cmmDriverCreatorBoolean::GatherPossibleDrivers(c_AffectsGlowChannelName,
			io_Drivers, "Parameters", this); 
		cmmDriverCreatorPosition::GatherPossibleDrivers(c_FalloffChannelName,
			io_Drivers, "Parameters", this); 
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_IntensityChannelName,
			io_Drivers, "Parameters", this); 
	}
}

//--------------------------------------------------------------------
// Create driver for this object based on the name used in
//	GatherPossibleDrivers and attach it to the channels
//	on this script object.
//--------------------------------------------------------------------
tmlnDriver* ptltDriverCreator::CreateDriverByName(	const char* i_DriverName,
													tmlnScriptObject* i_pObject )
{
	ptltScriptObject* ptlt_obj = dynamic_cast<ptltScriptObject*>(i_pObject);
	if (!ptlt_obj) return NULL;

	// Creation of drivers per channel
	tmlnDriver* pDriver = NULL;

	// Position Channel
	pDriver = cmmDriverCreatorPosition::CreateDriverByName(i_DriverName, 
		ptlt_obj->PositionChannel(), c_PositionChannelName, c_PositionChannelCode, 
		ptltObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// Color
	pDriver = cmmDriverCreatorColor::CreateDriverByName(i_DriverName, 
		ptlt_obj->ColorChannel(), c_ColorChannelName, c_ColorChannelCode, 
		ptltObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// Enabled Channel
	pDriver = cmmDriverCreatorBoolean::CreateDriverByName(i_DriverName, 
		ptlt_obj->EnabledChannel(), c_EnabledChannelName, c_EnabledChannelCode, 
		ptltObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// Range Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		ptlt_obj->RangeChannel(), c_RangeChannelName, c_RangeChannelCode, 
		ptltObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// ShadowSource Channel
//	pDriver = cmmDriverCreatorBoolean::CreateDriverByName(i_DriverName, 
//		ptlt_obj->ShadowSourceChannel(), c_ShadowSourceChannelName, c_ShadowSourceChannelCode, 
//		ptltObjectMgr::IconsVisible() );
//	if (pDriver != NULL) 
//		return pDriver;

	// EnableDiffuse Channel
	pDriver = cmmDriverCreatorBoolean::CreateDriverByName(i_DriverName, 
		ptlt_obj->EnableDiffuseChannel(), c_EnableDiffuseChannelName, c_EnableDiffuseChannelCode, 
		ptltObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// EnableSpecular Channel
	pDriver = cmmDriverCreatorBoolean::CreateDriverByName(i_DriverName, 
		ptlt_obj->EnableSpecularChannel(), c_EnableSpecularChannelName, c_EnableSpecularChannelCode, 
		ptltObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// AffectsGlow Channel
	pDriver = cmmDriverCreatorBoolean::CreateDriverByName(i_DriverName, 
		ptlt_obj->AffectsGlowChannel(), c_AffectsGlowChannelName, c_AffectsGlowChannelCode, 
		ptltObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// Falloff Channel
	pDriver = cmmDriverCreatorPosition::CreateDriverByName(i_DriverName, 
		ptlt_obj->FalloffChannel(), c_FalloffChannelName, c_FalloffChannelCode, 
		ptltObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// Intensity Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		ptlt_obj->IntensityChannel(), c_IntensityChannelName, c_IntensityChannelCode, 
		ptltObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	return NULL;
}

//--------------------------------------------------------------------
// Create static key driver for the given channel
//--------------------------------------------------------------------
tmlnDriver* ptltDriverCreator::CreateKeyForChannel( tmlnScriptObject* io_pObject,
												    tmlnChannel* i_pChannel)
{
	ptltScriptObject* ptlt_obj = dynamic_cast<ptltScriptObject*>(io_pObject);
	if (!ptlt_obj) return NULL;

	if (i_pChannel == &ptlt_obj->PositionChannel())
	{
		// Position Channel
		return cmmDriverCreatorPosition::CreateKeyForChannel(
			ptlt_obj->PositionChannel(), c_PositionChannelName, c_PositionChannelCode, 
			ptltObjectMgr::IconsVisible() );
	}
	else if (i_pChannel == &ptlt_obj->ColorChannel())
	{
		// Color Channel
		return cmmDriverCreatorColor::CreateKeyForChannel(
			ptlt_obj->ColorChannel(), c_ColorChannelName, c_ColorChannelCode, 
			ptltObjectMgr::IconsVisible() );
	}
	else if (i_pChannel == &ptlt_obj->EnabledChannel())
	{
		// Enabled Channel
		return cmmDriverCreatorBoolean::CreateKeyForChannel(
			ptlt_obj->EnabledChannel(), c_EnabledChannelName, c_EnabledChannelCode, 
			ptltObjectMgr::IconsVisible() );
	}
	else if (i_pChannel == &ptlt_obj->RangeChannel())
	{
		// Range Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			ptlt_obj->RangeChannel(), c_RangeChannelName, c_RangeChannelCode, 
			ptltObjectMgr::IconsVisible() );
	}
//	else if (i_pChannel == &ptlt_obj->ShadowSourceChannel())
//	{
//		// ShadowSource Channel
//		return cmmDriverCreatorBoolean::CreateKeyForChannel(
//			ptlt_obj->ShadowSourceChannel(), c_ShadowSourceChannelName, c_ShadowSourceChannelCode, 
//			ptltObjectMgr::IconsVisible() );
//	}
	else if (i_pChannel == &ptlt_obj->EnableDiffuseChannel())
	{
		// EnableDiffuse Channel
		return cmmDriverCreatorBoolean::CreateKeyForChannel(
			ptlt_obj->EnableDiffuseChannel(), c_EnableDiffuseChannelName, c_EnableDiffuseChannelCode, 
			ptltObjectMgr::IconsVisible() );
	}	
	else if (i_pChannel == &ptlt_obj->EnableSpecularChannel())
	{
		// EnableSpecular Channel
		return cmmDriverCreatorBoolean::CreateKeyForChannel(
			ptlt_obj->EnableSpecularChannel(), c_EnableSpecularChannelName, c_EnableSpecularChannelCode, 
			ptltObjectMgr::IconsVisible() );
	}	
	else if (i_pChannel == &ptlt_obj->AffectsGlowChannel())
	{
		// AffectsGlow Channel
		return cmmDriverCreatorBoolean::CreateKeyForChannel(
			ptlt_obj->AffectsGlowChannel(), c_AffectsGlowChannelName, c_AffectsGlowChannelCode, 
			ptltObjectMgr::IconsVisible() );
	}	
	else if (i_pChannel == &ptlt_obj->FalloffChannel())
	{
		// Falloff Channel
		return cmmDriverCreatorPosition::CreateKeyForChannel(
			ptlt_obj->FalloffChannel(), c_FalloffChannelName, c_FalloffChannelCode, 
			ptltObjectMgr::IconsVisible() );
	}	
	else if (i_pChannel == &ptlt_obj->IntensityChannel())
	{
		// Intensity Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			ptlt_obj->IntensityChannel(), c_IntensityChannelName, c_IntensityChannelCode, 
			ptltObjectMgr::IconsVisible() );
	}	

	return NULL;
}

//--------------------------------------------------------------------
// Create driver for this object based on the the info structure
//--------------------------------------------------------------------
tmlnDriver* ptltDriverCreator::CreateDriverFromInfo( tmlnScriptObject* io_pObject,
													 const tmlnDriverInfo& i_Info)
{
	ptltScriptObject* ptlt_obj = dynamic_cast<ptltScriptObject*>(io_pObject);
	if (!ptlt_obj) return NULL;

	chDefs::Name name = i_Info.GetBaseChunkName();

	if (name == c_LPSP)
	{
		// Old parser chunk name, convert to new one
		name = cmmDriverCreatorPosition::GetSplineChunkName(c_PositionChannelCode);
	}
	else if (name == c_LATC)
	{
		// Old parser chunk name, convert to new one
		name = cmmDriverCreatorPosition::GetAttachChunkName(c_PositionChannelCode);
	}
	else if (name == c_ENBD)
	{
		// Old parser chunk name, convert to new one
		name = cmmDriverCreatorBoolean::GetKeyChunkName(c_EnabledChannelCode);
	}
	else if (name == c_LPOS)
	{
		// Old parser chunk name, convert to new one
		name = cmmDriverCreatorPosition::GetKeyChunkName(c_PositionChannelCode);
	}
	else if (name == c_LCLR)
	{
		// Old parser chunk name, convert to new one
		name = cmmDriverCreatorColor::GetKeyChunkName(c_ColorChannelCode);
	}
	else if (name == c_CLRF)
	{
		// Old parser chunk name, convert to new one
		name = cmmDriverCreatorColor::GetFlickerChunkName(c_ColorChannelCode);
	}

	// Creation of drivers per channel
	tmlnDriver* pDriver = NULL;

	// Position channel
	pDriver = cmmDriverCreatorPosition::CreateDriverFromInfo(
		ptlt_obj->PositionChannel(), c_PositionChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// Color 
	pDriver = cmmDriverCreatorColor::CreateDriverFromInfo(
		ptlt_obj->ColorChannel(), c_ColorChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// Enabled channel
	pDriver = cmmDriverCreatorBoolean::CreateDriverFromInfo(
		ptlt_obj->EnabledChannel(), c_EnabledChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// Range channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		ptlt_obj->RangeChannel(), c_RangeChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// ShadowSource channel
//	pDriver = cmmDriverCreatorBoolean::CreateDriverFromInfo(
//		ptlt_obj->ShadowSourceChannel(), c_ShadowSourceChannelCode, i_Info, name );
//	if (pDriver != NULL) 
//		return pDriver;

	// EnableDiffuse channel
	pDriver = cmmDriverCreatorBoolean::CreateDriverFromInfo(
		ptlt_obj->EnableDiffuseChannel(), c_EnableDiffuseChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// EnableSpecular channel
	pDriver = cmmDriverCreatorBoolean::CreateDriverFromInfo(
		ptlt_obj->EnableSpecularChannel(), c_EnableSpecularChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// AffectsGlow channel
	pDriver = cmmDriverCreatorBoolean::CreateDriverFromInfo(
		ptlt_obj->AffectsGlowChannel(), c_AffectsGlowChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// Falloff channel
	pDriver = cmmDriverCreatorPosition::CreateDriverFromInfo(
		ptlt_obj->FalloffChannel(), c_FalloffChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// Intensity channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		ptlt_obj->IntensityChannel(), c_IntensityChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	return NULL;
}
