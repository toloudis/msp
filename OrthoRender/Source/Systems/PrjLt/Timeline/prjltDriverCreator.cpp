/*****************************************************************************
**	prjltDriverCreator.cpp
**
**	Creates timeline channels for projected lights
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Systems/PrjLt/Timeline/prjltDriverCreator.hpp"

#include "Systems/PrjLt/Data/prjltDataParser.hpp"
#include "Systems/PrjLt/Object/prjltScriptObject.hpp"
#include "Systems/PrjLt/Object/prjltObjectMgr.hpp"

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
#include "Drivers/Enable/tmlnDriverEnableParser.hpp"
#include "Drivers/Float/tmlnDriverFloatParser.hpp"
#include "Support/tmln/tmlnDriverInfo.hpp"
#include "Drivers/Position/tmlnDriverPositionParser.hpp"
#include "Drivers/Spline/tmlnDriverSplineParser.hpp"


namespace
{
	//========================================================================
	//========================================================================
	const char *c_PositionChannelName = "Position";
	const char c_PositionChannelCode[2] = { 'L', 'P' };  // Light Position channel code
	const char *c_TargetChannelName = "Target";
	const char c_TargetChannelCode[2] = { 'L', 'T' };  // Light Target channel code
	const char *c_TiltChannelName = "Tilt";
	const char c_TiltChannelCode[2] = { 'T', 'L' };  // TiLt channel code
	const char *c_ColorChannelName = "Color";
	const char c_ColorChannelCode[2] = { 'C', 'L' };  // CoLor channel code
	const char *c_EnabledChannelName = "Enabled";
	const char c_EnabledChannelCode[2] = { 'E', 'N' };  // ENabled channel code
	const char *c_RangeChannelName = "Range";
	const char c_RangeChannelCode[2] = { 'R', 'N' };  // RaNge channel code
	const char *c_AngleChannelName = "Angle";
	const char c_AngleChannelCode[2] = { 'A', 'N' };  // ANgle channel code
	const char *c_ScaleChannelName = "Scale";
	const char c_ScaleChannelCode[2] = { 'S', 'C' };  // SCale channel code
	const char *c_ShadowSourceChannelName = "Shadow Source";
	const char c_ShadowSourceChannelCode[2] = { 'S', 'S' };  // ShadowSource channel code
	const char *c_AspectChannelName = "Aspect";
	const char c_AspectChannelCode[2] = { 'A', 'S' };  // ASpect channel code
	const char *c_ShadowIntensityChannelName = "Shadow Intensity";
	const char c_ShadowIntensityChannelCode[2] = { 'S', 'I' };  // ShadowIntensity channel code
	const char *c_LightSizeChannelName = "Shadow Softness";
	const char c_LightSizeChannelCode[2] = { 'L', 'S' };  // LightSize channel code
	const char *c_DepthMapSizeChannelName = "Depth Map Size";
	const char c_DepthMapSizeChannelCode[2] = { 'D', 'M' };  // DepthMapSize channel code
	const char *c_DepthBiasChannelName = "Depth Bias";
	const char c_DepthBiasChannelCode[2] = { 'D', 'B' };  // DepthBias channel code
	const char *c_ShaftVisibleChannelName = "Shaft Visible";
	const char c_ShaftVisibleChannelCode[2] = { 'S', 'V' };  // ShaftVisible channel code
	const char *c_ShaftAlphaChannelName = "Shaft Alpha";
	const char c_ShaftAlphaChannelCode[2] = { 'S', 'A' };  // ShaftAlpha channel code
	const char *c_ShaftDensityChannelName = "Edge Softness";
	const char c_ShaftDensityChannelCode[2] = { 'S', 'D' };  // ShaftDensity channel code
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
	const char *c_ShaftFalloffStartChannelName = "Shaft Falloff Start";
	const char c_ShaftFalloffStartChannelCode[2] = { 'F', 'S' };  // ShaftFalloffStart channel code
	const char *c_ShaftFalloffEndChannelName = "ShaftFalloff End";
	const char c_ShaftFalloffEndChannelCode[2] = { 'F', 'E' };  // ShaftFalloffEnd channel code
	const char *c_IntensityChannelName = "Intensity";
	const char c_IntensityChannelCode[2] = { 'I', 'N' };  // INtensity channel code

	//========================================================================
	//========================================================================
	const chDefs::Name c_LATC = chDefs::MakeName('L', 'A', 'C', 'H');  // Light Attachment
	const chDefs::Name c_LCLR = chDefs::MakeName('L', 'C', 'L', 'R');  // Light Color
	const chDefs::Name c_LPOS = chDefs::MakeName('L', 'P', 'O', 'S');  // Light Static position
	const chDefs::Name c_LTPS = chDefs::MakeName('L', 'T', 'P', 'S');  // Light Target Static
	const chDefs::Name c_LTLT = chDefs::MakeName('L', 'T', 'L', 'T');  // Light Tilt
	const chDefs::Name c_ENBD = chDefs::MakeName('E', 'N', 'B', 'D');  // Enabled
}

//--------------------------------------------------------------------
// Create parsers for driver types created by this creator
//--------------------------------------------------------------------
//static
void prjltDriverCreator::CreateParsers()
{
	chDefs::Name sys_code = prjltDataParser::GetChunkName();

	// Creation of driver parsers per channel
	cmmDriverCreatorPosition::CreateParsers(sys_code, c_PositionChannelCode); 
	cmmDriverCreatorPosition::CreateParsers(sys_code, c_TargetChannelCode); 
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_TiltChannelCode); 
	cmmDriverCreatorColor::CreateParsers(sys_code, c_ColorChannelCode); 
	cmmDriverCreatorBoolean::CreateParsers(sys_code, c_EnabledChannelCode); 
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_RangeChannelCode); 
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_AngleChannelCode); 
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_ScaleChannelCode); 
	cmmDriverCreatorBoolean::CreateParsers(sys_code, c_ShadowSourceChannelCode); 
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_AspectChannelCode); 
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_ShadowIntensityChannelCode); 
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_LightSizeChannelCode); 
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_DepthMapSizeChannelCode); 
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_DepthBiasChannelCode); 
	cmmDriverCreatorBoolean::CreateParsers(sys_code, c_ShaftVisibleChannelCode); 
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_ShaftAlphaChannelCode); 
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_ShaftDensityChannelCode); 
	cmmDriverCreatorBoolean::CreateParsers(sys_code, c_EnableDiffuseChannelCode); 
	cmmDriverCreatorBoolean::CreateParsers(sys_code, c_EnableSpecularChannelCode); 
	cmmDriverCreatorBoolean::CreateParsers(sys_code, c_EnableFurChannelCode); 
	cmmDriverCreatorBoolean::CreateParsers(sys_code, c_AffectsGlowChannelCode); 
	cmmDriverCreatorPosition::CreateParsers(sys_code, c_FalloffChannelCode); 
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_ShaftFalloffStartChannelCode); 
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_ShaftFalloffEndChannelCode); 
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_IntensityChannelCode); 

	// Because of old file formats, we still have to create these parsers here
	tmlnParser::AddDriverParser(sys_code, c_LTLT, new tmlnDriverFloatParser(c_LTLT));
	tmlnParser::AddDriverParser(sys_code, c_LPOS, new tmlnDriverPositionParser(c_LPOS));
	tmlnParser::AddDriverParser(sys_code, c_LATC, new tmlnDriverAttachParser(c_LATC));
	tmlnParser::AddDriverParser(sys_code, c_LTPS, new tmlnDriverPositionParser(c_LTPS));
	tmlnParser::AddDriverParser(sys_code, c_ENBD, new tmlnDriverEnableParser(c_ENBD));
	tmlnParser::AddDriverParser(sys_code, c_LCLR, new tmlnDriverColorParser(c_LCLR));
}

//--------------------------------------------------------------------
// Expose chunk names used in file format
//--------------------------------------------------------------------
//static
chDefs::Name prjltDriverCreator::GetPositionAttachChunkName()
{
	return cmmDriverCreatorPosition::GetAttachChunkName(c_PositionChannelCode);
	//return c_LATC;
}
//static
chDefs::Name prjltDriverCreator::GetTargetAttachChunkName()
{
	return cmmDriverCreatorPosition::GetAttachChunkName(c_TargetChannelCode);
	//return c_LTAT;
}

//--------------------------------------------------------------------
// Gather the possible types of drivers that can be created for
//	for this object.  Driver names are added through the
//	DriverNameList's API
//--------------------------------------------------------------------
void prjltDriverCreator::GatherPossibleDrivers(const tmlnScriptObject* i_pObject,
											   tmlnDriverNameList& io_Drivers)
{
	const prjltScriptObject* prjlt_obj = dynamic_cast<const prjltScriptObject*>(i_pObject);
	if (prjlt_obj != NULL)
	{
		// Creation of drivers per channel
		cmmDriverCreatorPosition::GatherPossibleDrivers(c_PositionChannelName,
			io_Drivers, "Motion", this); 
		cmmDriverCreatorPosition::GatherPossibleDrivers(c_TargetChannelName,
			io_Drivers, "Target", this); 
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_TiltChannelName,
			io_Drivers, "Parameters", this); 
		cmmDriverCreatorColor::GatherPossibleDrivers(c_ColorChannelName,
			io_Drivers, "Parameters", this); 
		cmmDriverCreatorBoolean::GatherPossibleDrivers(c_EnabledChannelName,
			io_Drivers, "Parameters", this); 
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_RangeChannelName,
			io_Drivers, "Parameters", this); 
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_AngleChannelName,
			io_Drivers, "Parameters", this); 
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_ScaleChannelName,
			io_Drivers, "Parameters", this); 
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_AspectChannelName,
			io_Drivers, "Parameters", this); 
		cmmDriverCreatorBoolean::GatherPossibleDrivers(c_ShadowSourceChannelName,
			io_Drivers, "Shadows", this); 
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_ShadowIntensityChannelName,
			io_Drivers, "Shadows", this); 
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_LightSizeChannelName,
			io_Drivers, "Shadows", this); 
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_DepthMapSizeChannelName,
			io_Drivers, "Shadows", this); 
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_DepthBiasChannelName,
			io_Drivers, "Shadows", this); 
		cmmDriverCreatorBoolean::GatherPossibleDrivers(c_ShaftVisibleChannelName,
			io_Drivers, "Light Shaft", this); 
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_ShaftAlphaChannelName,
			io_Drivers, "Light Shaft", this); 
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_ShaftDensityChannelName,
			io_Drivers, "Light Shaft", this); 
		cmmDriverCreatorBoolean::GatherPossibleDrivers(c_EnableDiffuseChannelName,
			io_Drivers, "Parameters", this); 
		cmmDriverCreatorBoolean::GatherPossibleDrivers(c_EnableSpecularChannelName,
			io_Drivers, "Parameters", this); 
		cmmDriverCreatorBoolean::GatherPossibleDrivers(c_EnableFurChannelName,
			io_Drivers, "Parameters", this); 
		cmmDriverCreatorBoolean::GatherPossibleDrivers(c_AffectsGlowChannelName,
			io_Drivers, "Parameters", this); 
		cmmDriverCreatorPosition::GatherPossibleDrivers(c_FalloffChannelName,
			io_Drivers, "Parameters", this); 
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_ShaftFalloffStartChannelName,
			io_Drivers, "Light Shaft", this); 
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_ShaftFalloffEndChannelName,
			io_Drivers, "Light Shaft", this); 
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_IntensityChannelName,
			io_Drivers, "Parameters", this); 
	}
}

//--------------------------------------------------------------------
// Create driver for this object based on the name used in
//	GatherPossibleDrivers and attach it to the channels
//	on this script object.
//--------------------------------------------------------------------
tmlnDriver* prjltDriverCreator::CreateDriverByName( const char* i_DriverName,
													tmlnScriptObject* i_pObject )
{
	prjltScriptObject* prjlt_obj = dynamic_cast<prjltScriptObject*>(i_pObject);
	if (!prjlt_obj) return NULL;

	// Creation of drivers per channel
	tmlnDriver* pDriver = NULL;

	// Position Channel
	pDriver = cmmDriverCreatorPosition::CreateDriverByName(i_DriverName, 
		prjlt_obj->PositionChannel(), c_PositionChannelName, c_PositionChannelCode, 
		prjltObjectMgr::IconsVisible(), prjlt_obj );
	if (pDriver != NULL) 
		return pDriver;

	// Target Channel
	pDriver = cmmDriverCreatorPosition::CreateDriverByName(i_DriverName, 
		prjlt_obj->TargetChannel(), c_TargetChannelName, c_TargetChannelCode, 
		prjltObjectMgr::IconsVisible(), prjlt_obj );
	if (pDriver != NULL) 
		return pDriver;

	// Tilt Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		prjlt_obj->TiltChannel(), c_TiltChannelName, c_TiltChannelCode, 
		prjltObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// Color Channel
	pDriver = cmmDriverCreatorColor::CreateDriverByName(i_DriverName, 
		prjlt_obj->ColorChannel(), c_ColorChannelName, c_ColorChannelCode, 
		prjltObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// Enabled Channel
	pDriver = cmmDriverCreatorBoolean::CreateDriverByName(i_DriverName, 
		prjlt_obj->EnabledChannel(), c_EnabledChannelName, c_EnabledChannelCode, 
		prjltObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// Range Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		prjlt_obj->RangeChannel(), c_RangeChannelName, c_RangeChannelCode, 
		prjltObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// Angle Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		prjlt_obj->AngleChannel(), c_AngleChannelName, c_AngleChannelCode, 
		prjltObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// Scale Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		prjlt_obj->ScaleChannel(), c_ScaleChannelName, c_ScaleChannelCode, 
		prjltObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// ShadowSource Channel
	pDriver = cmmDriverCreatorBoolean::CreateDriverByName(i_DriverName, 
		prjlt_obj->ShadowSourceChannel(), c_ShadowSourceChannelName, c_ShadowSourceChannelCode, 
		prjltObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// Aspect Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		prjlt_obj->AspectChannel(), c_AspectChannelName, c_AspectChannelCode, 
		prjltObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// ShadowIntensity Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		prjlt_obj->ShadowIntensityChannel(), c_ShadowIntensityChannelName, c_ShadowIntensityChannelCode, 
		prjltObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// LightSize Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		prjlt_obj->LightSizeChannel(), c_LightSizeChannelName, c_LightSizeChannelCode, 
		prjltObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// DepthMapSize Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		prjlt_obj->DepthMapSizeChannel(), c_DepthMapSizeChannelName, c_DepthMapSizeChannelCode, 
		prjltObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// DepthBias Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		prjlt_obj->DepthBiasChannel(), c_DepthBiasChannelName, c_DepthBiasChannelCode, 
		prjltObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// ShaftVisible Channel
	pDriver = cmmDriverCreatorBoolean::CreateDriverByName(i_DriverName, 
		prjlt_obj->ShaftVisibleChannel(), c_ShaftVisibleChannelName, c_ShaftVisibleChannelCode, 
		prjltObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// ShaftAlpha Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		prjlt_obj->ShaftAlphaChannel(), c_ShaftAlphaChannelName, c_ShaftAlphaChannelCode, 
		prjltObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// ShaftDensity Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		prjlt_obj->ShaftDensityChannel(), c_ShaftDensityChannelName, c_ShaftDensityChannelCode, 
		prjltObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// EnableDiffuse Channel
	pDriver = cmmDriverCreatorBoolean::CreateDriverByName(i_DriverName, 
		prjlt_obj->EnableDiffuseChannel(), c_EnableDiffuseChannelName, c_EnableDiffuseChannelCode, 
		prjltObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// EnableSpecular Channel
	pDriver = cmmDriverCreatorBoolean::CreateDriverByName(i_DriverName, 
		prjlt_obj->EnableSpecularChannel(), c_EnableSpecularChannelName, c_EnableSpecularChannelCode, 
		prjltObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// EnableFur Channel
	pDriver = cmmDriverCreatorBoolean::CreateDriverByName(i_DriverName, 
		prjlt_obj->EnableFurChannel(), c_EnableFurChannelName, c_EnableFurChannelCode, 
		prjltObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// AffectsGlow Channel
	pDriver = cmmDriverCreatorBoolean::CreateDriverByName(i_DriverName, 
		prjlt_obj->AffectsGlowChannel(), c_AffectsGlowChannelName, c_AffectsGlowChannelCode, 
		prjltObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// Falloff Channel
	pDriver = cmmDriverCreatorPosition::CreateDriverByName(i_DriverName, 
		prjlt_obj->FalloffChannel(), c_FalloffChannelName, c_FalloffChannelCode, 
		prjltObjectMgr::IconsVisible(), prjlt_obj );
	if (pDriver != NULL) 
		return pDriver;

	// ShaftFalloffStart Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		prjlt_obj->ShaftFalloffStartChannel(), c_ShaftFalloffStartChannelName, c_ShaftFalloffStartChannelCode, 
		prjltObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// ShaftFalloffEnd Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		prjlt_obj->ShaftFalloffEndChannel(), c_ShaftFalloffEndChannelName, c_ShaftFalloffEndChannelCode, 
		prjltObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// Intensity Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		prjlt_obj->IntensityChannel(), c_IntensityChannelName, c_IntensityChannelCode, 
		prjltObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	return NULL;
}

//--------------------------------------------------------------------
// Create static key driver for the given channel
//--------------------------------------------------------------------
tmlnDriver* prjltDriverCreator::CreateKeyForChannel( tmlnScriptObject* io_pObject,
												    tmlnChannel* i_pChannel)
{
	prjltScriptObject* prjlt_obj = dynamic_cast<prjltScriptObject*>(io_pObject);
	if (!prjlt_obj) return NULL;

	if (i_pChannel == &prjlt_obj->PositionChannel())
	{
		// Position Channel
		return cmmDriverCreatorPosition::CreateKeyForChannel(
			prjlt_obj->PositionChannel(), c_PositionChannelName, c_PositionChannelCode, 
			prjltObjectMgr::IconsVisible(), prjlt_obj );
	}
	else if (i_pChannel == &prjlt_obj->TargetChannel())
	{
		// Target Channel
		return cmmDriverCreatorPosition::CreateKeyForChannel(
			prjlt_obj->TargetChannel(), c_TargetChannelName, c_TargetChannelCode, 
			prjltObjectMgr::IconsVisible(), prjlt_obj );
	}
	else if (i_pChannel == &prjlt_obj->TiltChannel())
	{
		// Tilt Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			prjlt_obj->TiltChannel(), c_TiltChannelName, c_TiltChannelCode, 
			prjltObjectMgr::IconsVisible() );
	}
	else if (i_pChannel == &prjlt_obj->ColorChannel())
	{
		// Color Channel
		return cmmDriverCreatorColor::CreateKeyForChannel(
			prjlt_obj->ColorChannel(), c_ColorChannelName, c_ColorChannelCode, 
			prjltObjectMgr::IconsVisible() );
	}
	else if (i_pChannel == &prjlt_obj->EnabledChannel())
	{
		// Enabled Channel
		return cmmDriverCreatorBoolean::CreateKeyForChannel(
			prjlt_obj->EnabledChannel(), c_EnabledChannelName, c_EnabledChannelCode, 
			prjltObjectMgr::IconsVisible() );
	}
	else if (i_pChannel == &prjlt_obj->RangeChannel())
	{
		// Range Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			prjlt_obj->RangeChannel(), c_RangeChannelName, c_RangeChannelCode, 
			prjltObjectMgr::IconsVisible() );
	}
	else if (i_pChannel == &prjlt_obj->AngleChannel())
	{
		// Angle Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			prjlt_obj->AngleChannel(), c_AngleChannelName, c_AngleChannelCode, 
			prjltObjectMgr::IconsVisible() );
	}
	else if (i_pChannel == &prjlt_obj->ScaleChannel())
	{
		// Scale Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			prjlt_obj->ScaleChannel(), c_ScaleChannelName, c_ScaleChannelCode, 
			prjltObjectMgr::IconsVisible() );
	}	
	else if (i_pChannel == &prjlt_obj->ShadowSourceChannel())
	{
		// ShadowSource Channel
		return cmmDriverCreatorBoolean::CreateKeyForChannel(
			prjlt_obj->ShadowSourceChannel(), c_ShadowSourceChannelName, c_ShadowSourceChannelCode, 
			prjltObjectMgr::IconsVisible() );
	}
	else if (i_pChannel == &prjlt_obj->AspectChannel())
	{
		// Aspect Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			prjlt_obj->AspectChannel(), c_AspectChannelName, c_AspectChannelCode, 
			prjltObjectMgr::IconsVisible() );
	}	
	else if (i_pChannel == &prjlt_obj->ShadowIntensityChannel())
	{
		// ShadowIntensity Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			prjlt_obj->ShadowIntensityChannel(), c_ShadowIntensityChannelName, c_ShadowIntensityChannelCode, 
			prjltObjectMgr::IconsVisible() );
	}	
	else if (i_pChannel == &prjlt_obj->LightSizeChannel())
	{
		// LightSize Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			prjlt_obj->LightSizeChannel(), c_LightSizeChannelName, c_LightSizeChannelCode, 
			prjltObjectMgr::IconsVisible() );
	}	
	else if (i_pChannel == &prjlt_obj->DepthMapSizeChannel())
	{
		// DepthMapSize Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			prjlt_obj->DepthMapSizeChannel(), c_DepthMapSizeChannelName, c_DepthMapSizeChannelCode, 
			prjltObjectMgr::IconsVisible() );
	}	
	else if (i_pChannel == &prjlt_obj->DepthBiasChannel())
	{
		// DepthBias Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			prjlt_obj->DepthBiasChannel(), c_DepthBiasChannelName, c_DepthBiasChannelCode, 
			prjltObjectMgr::IconsVisible() );
	}	
	else if (i_pChannel == &prjlt_obj->ShaftVisibleChannel())
	{
		// ShaftVisible Channel
		return cmmDriverCreatorBoolean::CreateKeyForChannel(
			prjlt_obj->ShaftVisibleChannel(), c_ShaftVisibleChannelName, c_ShaftVisibleChannelCode, 
			prjltObjectMgr::IconsVisible() );
	}
	else if (i_pChannel == &prjlt_obj->ShaftAlphaChannel())
	{
		// ShaftAlpha Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			prjlt_obj->ShaftAlphaChannel(), c_ShaftAlphaChannelName, c_ShaftAlphaChannelCode, 
			prjltObjectMgr::IconsVisible() );
	}	
	else if (i_pChannel == &prjlt_obj->ShaftDensityChannel())
	{
		// ShaftDensity Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			prjlt_obj->ShaftDensityChannel(), c_ShaftDensityChannelName, c_ShaftDensityChannelCode, 
			prjltObjectMgr::IconsVisible() );
	}	
	else if (i_pChannel == &prjlt_obj->EnableDiffuseChannel())
	{
		// EnableDiffuse Channel
		return cmmDriverCreatorBoolean::CreateKeyForChannel(
			prjlt_obj->EnableDiffuseChannel(), c_EnableDiffuseChannelName, c_EnableDiffuseChannelCode, 
			prjltObjectMgr::IconsVisible() );
	}	
	else if (i_pChannel == &prjlt_obj->EnableSpecularChannel())
	{
		// EnableSpecular Channel
		return cmmDriverCreatorBoolean::CreateKeyForChannel(
			prjlt_obj->EnableSpecularChannel(), c_EnableSpecularChannelName, c_EnableSpecularChannelCode, 
			prjltObjectMgr::IconsVisible() );
	}	
	else if (i_pChannel == &prjlt_obj->EnableFurChannel())
	{
		// EnableFur Channel
		return cmmDriverCreatorBoolean::CreateKeyForChannel(
			prjlt_obj->EnableFurChannel(), c_EnableFurChannelName, c_EnableFurChannelCode, 
			prjltObjectMgr::IconsVisible() );
	}	
	else if (i_pChannel == &prjlt_obj->AffectsGlowChannel())
	{
		// AffectsGlow Channel
		return cmmDriverCreatorBoolean::CreateKeyForChannel(
			prjlt_obj->AffectsGlowChannel(), c_AffectsGlowChannelName, c_AffectsGlowChannelCode, 
			prjltObjectMgr::IconsVisible() );
	}	
	else if (i_pChannel == &prjlt_obj->FalloffChannel())
	{
		// Falloff Channel
		return cmmDriverCreatorPosition::CreateKeyForChannel(
			prjlt_obj->FalloffChannel(), c_FalloffChannelName, c_FalloffChannelCode, 
			prjltObjectMgr::IconsVisible(), prjlt_obj );
	}	
	else if (i_pChannel == &prjlt_obj->ShaftFalloffStartChannel())
	{
		// ShaftFalloffStart Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			prjlt_obj->ShaftFalloffStartChannel(), c_ShaftFalloffStartChannelName, c_ShaftFalloffStartChannelCode, 
			prjltObjectMgr::IconsVisible() );
	}	
	else if (i_pChannel == &prjlt_obj->ShaftFalloffEndChannel())
	{
		// ShaftFalloffEnd Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			prjlt_obj->ShaftFalloffEndChannel(), c_ShaftFalloffEndChannelName, c_ShaftFalloffEndChannelCode, 
			prjltObjectMgr::IconsVisible() );
	}	
	else if (i_pChannel == &prjlt_obj->IntensityChannel())
	{
		// Intensity Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			prjlt_obj->IntensityChannel(), c_IntensityChannelName, c_IntensityChannelCode, 
			prjltObjectMgr::IconsVisible() );
	}	

	return NULL;
}

//--------------------------------------------------------------------
// Create driver for this object based on the the info structure
//--------------------------------------------------------------------
tmlnDriver* prjltDriverCreator::CreateDriverFromInfo( tmlnScriptObject* io_pObject,
													 const tmlnDriverInfo& i_Info)
{
	prjltScriptObject* prjlt_obj = dynamic_cast<prjltScriptObject*>(io_pObject);
	if (!prjlt_obj) return NULL;

	chDefs::Name name = i_Info.GetBaseChunkName();

	if (name == c_LPOS)
	{
		// Old parser chunk name, convert to new one
		name = cmmDriverCreatorPosition::GetKeyChunkName(c_PositionChannelCode);
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
	else if (name == c_LCLR)
	{
		// Old parser chunk name, convert to new one
		name = cmmDriverCreatorColor::GetKeyChunkName(c_ColorChannelCode);
	}
	else if (name == c_LTPS)
	{
		// Old parser chunk name, convert to new one
		name = cmmDriverCreatorPosition::GetKeyChunkName(c_TargetChannelCode);
	}
	else if (name == c_LTLT)
	{
		// Old parser chunk name, convert to new one
		name = cmmDriverCreatorFloat::GetKeyChunkName(c_TiltChannelCode);
	}

	// Creation of drivers per channel
	tmlnDriver* pDriver = NULL;

	// Position channel
	pDriver = cmmDriverCreatorPosition::CreateDriverFromInfo(
		prjlt_obj->PositionChannel(), c_PositionChannelCode, i_Info, name, prjlt_obj );
	if (pDriver != NULL) 
		return pDriver;

	// Target channel
	pDriver = cmmDriverCreatorPosition::CreateDriverFromInfo(
		prjlt_obj->TargetChannel(), c_TargetChannelCode, i_Info, name, prjlt_obj );
	if (pDriver != NULL) 
		return pDriver;

	// Tilt channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		prjlt_obj->TiltChannel(), c_TiltChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// Color channel
	pDriver = cmmDriverCreatorColor::CreateDriverFromInfo(
		prjlt_obj->ColorChannel(), c_ColorChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// Enabled channel
	pDriver = cmmDriverCreatorBoolean::CreateDriverFromInfo(
		prjlt_obj->EnabledChannel(), c_EnabledChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;
 
	// Range channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		prjlt_obj->RangeChannel(), c_RangeChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// Angle channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		prjlt_obj->AngleChannel(), c_AngleChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// Scale channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		prjlt_obj->ScaleChannel(), c_ScaleChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// ShadowSource channel
	pDriver = cmmDriverCreatorBoolean::CreateDriverFromInfo(
		prjlt_obj->ShadowSourceChannel(), c_ShadowSourceChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// Aspect channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		prjlt_obj->AspectChannel(), c_AspectChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// ShadowIntensity channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		prjlt_obj->ShadowIntensityChannel(), c_ShadowIntensityChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// LightSize channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		prjlt_obj->LightSizeChannel(), c_LightSizeChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// DepthMapSize channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		prjlt_obj->DepthMapSizeChannel(), c_DepthMapSizeChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// DepthBias channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		prjlt_obj->DepthBiasChannel(), c_DepthBiasChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// ShaftVisible channel
	pDriver = cmmDriverCreatorBoolean::CreateDriverFromInfo(
		prjlt_obj->ShaftVisibleChannel(), c_ShaftVisibleChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// ShaftAlpha channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		prjlt_obj->ShaftAlphaChannel(), c_ShaftAlphaChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// ShaftDensity channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		prjlt_obj->ShaftDensityChannel(), c_ShaftDensityChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// EnableDiffuse channel
	pDriver = cmmDriverCreatorBoolean::CreateDriverFromInfo(
		prjlt_obj->EnableDiffuseChannel(), c_EnableDiffuseChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// EnableSpecular channel
	pDriver = cmmDriverCreatorBoolean::CreateDriverFromInfo(
		prjlt_obj->EnableSpecularChannel(), c_EnableSpecularChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// EnableFur channel
	pDriver = cmmDriverCreatorBoolean::CreateDriverFromInfo(
		prjlt_obj->EnableFurChannel(), c_EnableFurChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// AffectsGlow channel
	pDriver = cmmDriverCreatorBoolean::CreateDriverFromInfo(
		prjlt_obj->AffectsGlowChannel(), c_AffectsGlowChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// Falloff channel
	pDriver = cmmDriverCreatorPosition::CreateDriverFromInfo(
		prjlt_obj->FalloffChannel(), c_FalloffChannelCode, i_Info, name, prjlt_obj );
	if (pDriver != NULL) 
		return pDriver;

	// ShaftFalloffStart channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		prjlt_obj->ShaftFalloffStartChannel(), c_ShaftFalloffStartChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// ShaftFalloffEnd channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		prjlt_obj->ShaftFalloffEndChannel(), c_ShaftFalloffEndChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// Intensity channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		prjlt_obj->IntensityChannel(), c_IntensityChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	return NULL;
}
