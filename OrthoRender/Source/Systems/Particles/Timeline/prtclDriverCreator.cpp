/*****************************************************************************
**	prtclDriverCreator.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Systems/Particles/Timeline/prtclDriverCreator.hpp"

#include "Systems/Particles/Timeline/prtclChannelAnimation.hpp"
#include "Systems/Particles/Timeline/prtclChannelEmit.hpp"
#include "Systems/Particles/Timeline/prtclChannelPos.hpp"
#include "Systems/Particles/Data/prtclDataParser.hpp"
#include "Systems/Particles/Timeline/prtclDriverAnimation.hpp"
#include "Systems/Particles/Timeline/prtclDriverAnimationInfo.hpp"
#include "Systems/Particles/Timeline/prtclDriverAnimationParser.hpp"
#include "Systems/Particles/Timeline/prtclDriverEmit.hpp"
#include "Systems/Particles/Timeline/prtclDriverEmitInfo.hpp"
#include "Systems/Particles/Timeline/prtclDriverEmitParser.hpp"
#include "Systems/Particles/Object/prtclScriptObject.hpp"
#include "Systems/Particles/Object/prtclObjectMgr.hpp"

#include "Systems/Common/DriverCreator/cmmDriverCreatorBoolean.hpp"
#include "Systems/Common/DriverCreator/cmmDriverCreatorFloat.hpp"
#include "Systems/Common/DriverCreator/cmmDriverCreatorOrientation.hpp"
#include "Systems/Common/DriverCreator/cmmDriverCreatorPosition.hpp"
#include "Support/tmln/tmlnChannelBoolean.hpp"
#include "Support/tmln/tmlnChannelFloat.hpp"
#include "Support/tmln/tmlnChannelOrientation.hpp"
#include "Drivers/Attach/tmlnDriverAttach.hpp"
#include "Drivers/Attach/tmlnDriverAttachInfo.hpp"
#include "Drivers/Attach/tmlnDriverAttachParser.hpp"
#include "Drivers/Float/tmlnDriverFloat.hpp"
#include "Drivers/Orientation/tmlnDriverOrientation.hpp"
#include "Drivers/Position/tmlnDriverPositionParser.hpp"
#include "Drivers/Spline/tmlnDriverSplineParser.hpp"
#include "Support/tmln/tmlnTimeLine.hpp"
#include "Support/spln/splnSpline.hpp"


namespace
{
	const char *c_OrientationChannelName = "Orientation";
	const char c_OrientationChannelCode[2] = { 'O', 'R' };  // ORientation channel code
	const char *c_PositionChannelName = "Position";
	const char c_PositionChannelCode[2] = { 'P', 'S' };  // PoSition channel code
	const char *c_RateChannelName = "Rate";
	const char c_RateChannelCode[2] = { 'R', 'A' };  // Rate channel code
	const char *c_MaxParticlesChannelName = "Max Particles";
	const char c_MaxParticlesChannelCode[2] = { 'M', 'X' };  // MaxParticles channel code
	const char *c_LifetimeMinChannelName = "Lifetime Min";
	const char c_LifetimeMinChannelCode[2] = { 'L', 'N' };  // LifetimeMin channel code
	const char *c_LifetimeMaxChannelName = "Lifetime Max";
	const char c_LifetimeMaxChannelCode[2] = { 'L', 'X' };  // LifetimeMax channel code
	const char *c_ScaleStartChannelName = "Scale Start";
	const char c_ScaleStartChannelCode[2] = { 'S', 'S' };  // ScaleStart channel code
	const char *c_ScaleCoefficientChannelName = "Scale Coefficient";
	const char c_ScaleCoefficientChannelCode[2] = { 'S', 'C' };  // ScaleCoefficient channel code
	const char *c_StartAngleMinChannelName = "Start Angle Min";
	const char c_StartAngleMinChannelCode[2] = { 'S', 'N' };  // StartAngleMin channel code
	const char *c_StartAngleMaxChannelName = "Start Angle Max";
	const char c_StartAngleMaxChannelCode[2] = { 'S', 'X' };  // StartAngleMax channel code
	const char *c_AngularVelocityMinChannelName = "Angular Velocity Min";
	const char c_AngularVelocityMinChannelCode[2] = { 'V', 'N' };  // AngularVelocityMin channel code
	const char *c_AngularVelocityMaxChannelName = "Angular Velocity Max";
	const char c_AngularVelocityMaxChannelCode[2] = { 'V', 'X' };  // AngularVelocityMax channel code
	const char *c_AngularAccelerationMinChannelName = "Angular Acceleration Min";
	const char c_AngularAccelerationMinChannelCode[2] = { 'A', 'N' };  // AngularAccelerationMin channel code
	const char *c_AngularAccelerationMaxChannelName = "Angular Acceleration Max";
	const char c_AngularAccelerationMaxChannelCode[2] = { 'A', 'X' };  // AngularAccelerationMax channel code
	const char *c_EmitterScaleChannelName = "Emitter Scale";
	const char c_EmitterScaleChannelCode[2] = { 'E', 'S' };  // EmitterScale channel code
	const char *c_ConeAngleChannelName = "Cone Angle";
	const char c_ConeAngleChannelCode[2] = { 'C', 'A' };  // ConeAngle channel code
	const char *c_MinSpeedChannelName = "Min Speed";
	const char c_MinSpeedChannelCode[2] = { 'N', 'S' };  // MinSpeed channel code
	const char *c_MaxSpeedChannelName = "Max Speed";
	const char c_MaxSpeedChannelCode[2] = { 'X', 'S' };  // MaxSpeed channel code
	const char *c_AccelerationXChannelName = "Acceleration X";
	const char c_AccelerationXChannelCode[2] = { 'C', 'X' };  // AccelerationX channel code
	const char *c_AccelerationYChannelName = "Acceleration Y";
	const char c_AccelerationYChannelCode[2] = { 'C', 'Y' };  // AccelerationY channel code
	const char *c_AccelerationZChannelName = "Acceleration Z";
	const char c_AccelerationZChannelCode[2] = { 'C', 'Z' };  // AccelerationZ channel code
	const char *c_MinEmitSpeedChannelName = "Min Emit Speed";
	const char c_MinEmitSpeedChannelCode[2] = { 'E', 'N' };  // MinEmitSpeed channel code
	const char *c_MaxEmitSpeedChannelName = "Max Emit Speed";
	const char c_MaxEmitSpeedChannelCode[2] = { 'E', 'X' };  // MaxEmitSpeed channel code
	const char *c_EmitDirectionXChannelName = "Emit Direction X";
	const char c_EmitDirectionXChannelCode[2] = { 'T', 'X' };  // EmitDirectionX channel code
	const char *c_EmitDirectionYChannelName = "Emit Direction Y";
	const char c_EmitDirectionYChannelCode[2] = { 'T', 'Y' };  // EmitDirectionY channel code
	const char *c_EmitDirectionZChannelName = "Emit Direction Z";
	const char c_EmitDirectionZChannelCode[2] = { 'T', 'Z' };  // EmitDirectionZ channel code
	const char *c_MinRotStartAngleChannelName = "Min Rot Start Angle";
	const char c_MinRotStartAngleChannelCode[2] = { 'R', 'N' };  // MinRotStartAngle channel code
	const char *c_MaxRotStartAngleChannelName = "Max Rot Start Angle";
	const char c_MaxRotStartAngleChannelCode[2] = { 'R', 'X' };  // MaxRotStartAngle channel code
	const char *c_MinRotAngularVelChannelName = "Min Rot Angular Vel";
	const char c_MinRotAngularVelChannelCode[2] = { 'N', 'V' };  // MinRotAngularVel channel code
	const char *c_MaxRotAngularVelChannelName = "Max Rot Angular Vel";
	const char c_MaxRotAngularVelChannelCode[2] = { 'X', 'V' };  // MaxRotAngularVel channel code
	const char *c_RotRadiusChannelName = "Rot Radius";
	const char c_RotRadiusChannelCode[2] = { 'R', 'R' };  // RotRadius channel code
	const char *c_RotRadiusScaleRateChannelName = "Rot Radius Scale Rate";
	const char c_RotRadiusScaleRateChannelCode[2] = { 'R', 'S' };  // RotRadiusScaleRate channel code
	const char *c_TextureAlphaStartChannelName = "Texture Alpha Start";
	const char c_TextureAlphaStartChannelCode[2] = { 'A', 'S' };  // TextureAlphaStart channel code
	const char *c_TextureAlphaMiddleChannelName = "Texture Alpha Middle";
	const char c_TextureAlphaMiddleChannelCode[2] = { 'A', 'M' };  // TextureAlphaMiddle channel code
	const char *c_TextureAlphaEndChannelName = "Texture Alpha End";
	const char c_TextureAlphaEndChannelCode[2] = { 'A', 'E' };  // TextureAlphaEnd channel code
	const char *c_TextureAlphaMiddlePercentStartChannelName = "Texture Alpha MiddleStart";
	const char c_TextureAlphaMiddlePercentStartChannelCode[2] = { 'M', 'S' };  // TextureAlphaMiddleStart channel code
	const char *c_TextureAlphaMiddlePercentEndChannelName = "Texture Alpha MiddleEnd";
	const char c_TextureAlphaMiddlePercentEndChannelCode[2] = { 'M', 'E' };  // TextureAlphaMiddleEnd channel code
	const char *c_RenderStreaksChannelName = "Render Streaks";
	const char c_RenderStreaksChannelCode[2] = { 'R', 'K' };  // Render Streaks channel code
	const char *c_StreakLengthChannelName = "Streak Length";
	const char c_StreakLengthChannelCode[2] = { 'K', 'L' };  // Streak Length channel code
	const char *c_StreakTaperChannelName = "Streak Taper";
	const char c_StreakTaperChannelCode[2] = { 'K', 'T' };  // Streak Taper channel code
	const char *c_StreakFadeChannelName = "Streak Fade";
	const char c_StreakFadeChannelCode[2] = { 'K', 'F' };  // Streak Fade channel code
	const char *c_ShowInCubeReflectionsChannelName = "Show in Cube Reflections";
	const char c_ShowInCubeReflectionsChannelCode[2] = { 'F', 'C' }; // Show in Cube reflections channel code
	const char *c_ShowInPlanarReflectionsChannelName = "Show in Planar Reflections";
	const char c_ShowInPlanarReflectionsChannelCode[2] = { 'F', 'P' }; // Show in Planar reflections channel code
	const char *c_CastShadowsChannelName = "Cast Shadows";
	const char c_CastShadowsChannelCode[2] = { 'S', 'H' }; // Cast Shadows channel code
	const char *c_UseDitheredShadowsChannelName = "Use Dithered Shadows";
	const char c_UseDitheredShadowsChannelCode[2] = { 'D', 'T' }; // Use Dithered Shadows channel code
	const char *c_ShadowDitherBiasChannelName = "Shadow Dither Bias";
	const char c_ShadowDitherBiasChannelCode[2] = { 'D', 'B' }; // Shadow Dither Bias channel code
	const char *c_AdditiveChannelName = "Additive";
	const char c_AdditiveChannelCode[2] = { 'A', 'D' }; // Additive channel code

	const char* PRTCLDRIVERNAME_ANIMATION = "Animation";
	const char* PRTCLDRIVERNAME_EMIT = "Emit";

	const chDefs::Name c_PPSP = chDefs::MakeName('P', 'P', 'S', 'P');  // Particle Position SPline
	const chDefs::Name c_PATC = chDefs::MakeName('P', 'A', 'C', 'H');  // Particle Attachment
	const chDefs::Name c_PPOS = chDefs::MakeName('P', 'P', 'O', 'S');  // Particle Static position
	const chDefs::Name c_PDAF = chDefs::MakeName('P', 'D', 'A', 'F');  // Particle animation
}


//--------------------------------------------------------------------
// Create parsers for driver types created by this creator
//--------------------------------------------------------------------
//static
void prtclDriverCreator::CreateParsers()
{
	chDefs::Name sys_code = prtclDataParser::GetChunkName();

	// Creation of driver parsers per channel
	cmmDriverCreatorPosition::CreateParsers(sys_code, c_PositionChannelCode); 
	cmmDriverCreatorOrientation::CreateParsers(sys_code, c_OrientationChannelCode); 

	cmmDriverCreatorFloat::CreateParsers(sys_code, c_RateChannelCode); 
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_MaxParticlesChannelCode); 
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_LifetimeMinChannelCode); 
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_LifetimeMaxChannelCode); 
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_ScaleStartChannelCode); 
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_ScaleCoefficientChannelCode); 
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_StartAngleMinChannelCode); 
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_StartAngleMaxChannelCode); 
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_AngularVelocityMinChannelCode); 
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_AngularVelocityMaxChannelCode); 
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_AngularAccelerationMinChannelCode); 
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_AngularAccelerationMaxChannelCode); 
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_EmitterScaleChannelCode); 
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_ConeAngleChannelCode);	// prtConeParticleGenerator
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_MinSpeedChannelCode);
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_MaxSpeedChannelCode);
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_AccelerationXChannelCode);
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_AccelerationYChannelCode);
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_AccelerationZChannelCode);
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_MinEmitSpeedChannelCode);	// prtSpiralParticleGenerator
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_MaxEmitSpeedChannelCode);
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_EmitDirectionXChannelCode);
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_EmitDirectionYChannelCode);
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_EmitDirectionZChannelCode);
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_MinRotStartAngleChannelCode);
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_MaxRotStartAngleChannelCode);
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_MinRotAngularVelChannelCode);
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_MaxRotAngularVelChannelCode);
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_RotRadiusChannelCode);
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_RotRadiusScaleRateChannelCode);
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_TextureAlphaStartChannelCode);
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_TextureAlphaMiddleChannelCode);
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_TextureAlphaEndChannelCode);
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_TextureAlphaMiddlePercentStartChannelCode);
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_TextureAlphaMiddlePercentEndChannelCode);
	cmmDriverCreatorBoolean::CreateParsers(sys_code, c_RenderStreaksChannelCode);
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_StreakLengthChannelCode);
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_StreakTaperChannelCode);
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_StreakFadeChannelCode);

	// System specific drivers
	tmlnParser::AddDriverParser(sys_code, prtclDriverEmitParser::GetChunkName(), new prtclDriverEmitParser());
	tmlnParser::AddDriverParser(sys_code, c_PDAF, new prtclDriverAnimationParser(c_PDAF));

	// Because of old file formats, we still have to create these parsers here
	tmlnParser::AddDriverParser(sys_code, c_PPSP, new tmlnDriverSplineParser(c_PPSP));
	tmlnParser::AddDriverParser(sys_code, c_PATC, new tmlnDriverAttachParser(c_PATC));
	tmlnParser::AddDriverParser(sys_code, c_PPOS, new tmlnDriverPositionParser(c_PPOS));
}

//--------------------------------------------------------------------
// Gather the possible types of drivers that can be created for
//	for this object.  Driver names are added through the
//	DriverNameList's API
//--------------------------------------------------------------------
//virtual
void prtclDriverCreator::GatherPossibleDrivers( const tmlnScriptObject* i_pObject,
											    tmlnDriverNameList& io_Drivers )
{
	if (dynamic_cast<const prtclScriptObject*>(i_pObject))
	{
		// Creation of drivers per channel
		cmmDriverCreatorPosition::GatherPossibleDrivers(c_PositionChannelName,
			io_Drivers, "Motion", this); 
		cmmDriverCreatorOrientation::GatherPossibleDrivers(c_OrientationChannelName, 
			io_Drivers, "Parameters", this); 
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_RateChannelName, 
			io_Drivers, "Generation", this); 
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_MaxParticlesChannelName, 
			io_Drivers, "Generation", this); 
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_LifetimeMinChannelName, 
			io_Drivers, "Generation", this); 
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_LifetimeMaxChannelName, 
			io_Drivers, "Generation", this); 
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_ScaleStartChannelName, 
			io_Drivers, "Generation", this); 
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_ScaleCoefficientChannelName, 
			io_Drivers, "Generation", this); 
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_StartAngleMinChannelName, 
			io_Drivers, "Generation", this); 
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_StartAngleMaxChannelName, 
			io_Drivers, "Generation", this); 
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_AngularVelocityMaxChannelName, 
			io_Drivers, "Generation", this); 
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_AngularVelocityMinChannelName, 
			io_Drivers, "Generation", this); 
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_AngularAccelerationMaxChannelName, 
			io_Drivers, "Generation", this); 
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_AngularAccelerationMinChannelName, 
			io_Drivers, "Generation", this); 
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_EmitterScaleChannelName, 
			io_Drivers, "Generation", this); 
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_ConeAngleChannelName, 
			io_Drivers, "Generation", this); 
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_MinSpeedChannelName, 
			io_Drivers, "Generation", this); 
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_MaxSpeedChannelName, 
			io_Drivers, "Generation", this); 
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_AccelerationXChannelName, 
			io_Drivers, "Generation", this); 
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_AccelerationYChannelName, 
			io_Drivers, "Generation", this); 
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_AccelerationZChannelName, 
			io_Drivers, "Generation", this); 
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_MinEmitSpeedChannelName, 
			io_Drivers, "Generation", this); 
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_MaxEmitSpeedChannelName, 
			io_Drivers, "Generation", this); 
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_EmitDirectionXChannelName, 
			io_Drivers, "Generation", this); 
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_EmitDirectionYChannelName, 
			io_Drivers, "Generation", this); 
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_EmitDirectionZChannelName, 
			io_Drivers, "Generation", this); 
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_MinRotStartAngleChannelName, 
			io_Drivers, "Generation", this); 
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_MaxRotStartAngleChannelName, 
			io_Drivers, "Generation", this); 
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_MinRotAngularVelChannelName, 
			io_Drivers, "Generation", this); 
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_MaxRotAngularVelChannelName, 
			io_Drivers, "Generation", this); 
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_RotRadiusChannelName, 
			io_Drivers, "Generation", this); 
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_RotRadiusScaleRateChannelName, 
			io_Drivers, "Generation", this); 
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_TextureAlphaStartChannelName, 
			io_Drivers, "Texture", this); 
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_TextureAlphaMiddleChannelName, 
			io_Drivers, "Texture", this); 
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_TextureAlphaEndChannelName, 
			io_Drivers, "Texture", this); 
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_TextureAlphaMiddlePercentStartChannelName, 
			io_Drivers, "Texture", this); 
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_TextureAlphaMiddlePercentEndChannelName, 
			io_Drivers, "Texture", this); 

		cmmDriverCreatorBoolean::GatherPossibleDrivers(c_RenderStreaksChannelName, 
			io_Drivers, "Streaks", this); 
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_StreakLengthChannelName, 
			io_Drivers, "Streaks", this); 
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_StreakTaperChannelName, 
			io_Drivers, "Streaks", this); 
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_StreakFadeChannelName, 
			io_Drivers, "Streaks", this); 

		cmmDriverCreatorBoolean::GatherPossibleDrivers(c_ShowInCubeReflectionsChannelName, 
			io_Drivers, "Rendering", this); 
		cmmDriverCreatorBoolean::GatherPossibleDrivers(c_ShowInPlanarReflectionsChannelName, 
			io_Drivers, "Rendering", this); 
		cmmDriverCreatorBoolean::GatherPossibleDrivers(c_CastShadowsChannelName, 
			io_Drivers, "Rendering", this); 
		cmmDriverCreatorBoolean::GatherPossibleDrivers(c_UseDitheredShadowsChannelName, 
			io_Drivers, "Rendering", this); 
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_ShadowDitherBiasChannelName, 
			io_Drivers, "Rendering", this); 
		cmmDriverCreatorBoolean::GatherPossibleDrivers(c_AdditiveChannelName, 
			io_Drivers, "Rendering", this); 

		// System level
		io_Drivers.AddDriver(PRTCLDRIVERNAME_EMIT, "Parameters", this);
		io_Drivers.AddDriver(PRTCLDRIVERNAME_ANIMATION, "Motion", this);
	}
}

//--------------------------------------------------------------------
// Create driver for this object based on the name used in
//	GatherPossibleDrivers and attach it to the channels
//	on this script object.
//--------------------------------------------------------------------
//virtual
tmlnDriver* prtclDriverCreator::CreateDriverByName( const char* i_DriverName,
													tmlnScriptObject* i_pObject )
{
	prtclScriptObject* prtcl_obj = dynamic_cast<prtclScriptObject*>(i_pObject);
	if (!prtcl_obj) return NULL;

	// Creation of drivers per channel
	tmlnDriver* pDriver = NULL;

	// Position Channel
	pDriver = cmmDriverCreatorPosition::CreateDriverByName(i_DriverName, 
		prtcl_obj->ChannelPos(), c_PositionChannelName, c_PositionChannelCode, 
		prtclObjectMgr::IconsVisible(), prtcl_obj );
	if (pDriver != NULL) 
		return pDriver;

	// Orientation Channel
	pDriver = cmmDriverCreatorOrientation::CreateDriverByName(i_DriverName, 
		prtcl_obj->OrientationChannel(), c_OrientationChannelName, c_OrientationChannelCode, 
		prtclObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// Rate Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		prtcl_obj->RateChannel(), c_RateChannelName, c_RateChannelCode, 
		prtclObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// MaxParticles Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		prtcl_obj->MaxParticlesChannel(), c_MaxParticlesChannelName, c_MaxParticlesChannelCode, 
		prtclObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// LifetimeMin Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		prtcl_obj->LifetimeMinChannel(), c_LifetimeMinChannelName, c_LifetimeMinChannelCode, 
		prtclObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// LifetimeMax Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		prtcl_obj->LifetimeMaxChannel(), c_LifetimeMaxChannelName, c_LifetimeMaxChannelCode, 
		prtclObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// ScaleStart Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		prtcl_obj->ScaleStartChannel(), c_ScaleStartChannelName, c_ScaleStartChannelCode, 
		prtclObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// ScaleCoefficient Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		prtcl_obj->ScaleCoefficientChannel(), c_ScaleCoefficientChannelName, c_ScaleCoefficientChannelCode, 
		prtclObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// StartAngleMin Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		prtcl_obj->StartAngleMinChannel(), c_StartAngleMinChannelName, c_StartAngleMinChannelCode, 
		prtclObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// StartAngleMax Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		prtcl_obj->StartAngleMaxChannel(), c_StartAngleMaxChannelName, c_StartAngleMaxChannelCode, 
		prtclObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// AngularVelocityMin Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		prtcl_obj->AngularVelocityMinChannel(), c_AngularVelocityMinChannelName, c_AngularVelocityMinChannelCode, 
		prtclObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// AngularVelocityMax Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		prtcl_obj->AngularVelocityMaxChannel(), c_AngularVelocityMaxChannelName, c_AngularVelocityMaxChannelCode, 
		prtclObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// AngularAccelerationMin Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		prtcl_obj->AngularAccelerationMinChannel(), c_AngularAccelerationMinChannelName, c_AngularAccelerationMinChannelCode, 
		prtclObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// AngularAccelerationMax Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		prtcl_obj->AngularAccelerationMaxChannel(), c_AngularAccelerationMaxChannelName, c_AngularAccelerationMaxChannelCode, 
		prtclObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// EmitterScale Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		prtcl_obj->EmitterScaleChannel(), c_EmitterScaleChannelName, c_EmitterScaleChannelCode, 
		prtclObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// ConeAngle Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		prtcl_obj->ConeAngleChannel(), c_ConeAngleChannelName, c_ConeAngleChannelCode, 
		prtclObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// MinSpeed Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		prtcl_obj->MinSpeedChannel(), c_MinSpeedChannelName, c_MinSpeedChannelCode, 
		prtclObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// MaxSpeed Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		prtcl_obj->MaxSpeedChannel(), c_MaxSpeedChannelName, c_MaxSpeedChannelCode, 
		prtclObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// AccelerationX Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		prtcl_obj->AccelerationXChannel(), c_AccelerationXChannelName, c_AccelerationXChannelCode, 
		prtclObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// AccelerationY Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		prtcl_obj->AccelerationYChannel(), c_AccelerationYChannelName, c_AccelerationYChannelCode, 
		prtclObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// AccelerationZ Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		prtcl_obj->AccelerationZChannel(), c_AccelerationZChannelName, c_AccelerationZChannelCode, 
		prtclObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// MinEmitSpeed Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		prtcl_obj->MinEmitSpeedChannel(), c_MinEmitSpeedChannelName, c_MinEmitSpeedChannelCode, 
		prtclObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// MaxEmitSpeed Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		prtcl_obj->MaxEmitSpeedChannel(), c_MaxEmitSpeedChannelName, c_MaxEmitSpeedChannelCode, 
		prtclObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// EmitDirectionX Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		prtcl_obj->EmitDirectionXChannel(), c_EmitDirectionXChannelName, c_EmitDirectionXChannelCode, 
		prtclObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// EmitDirectionY Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		prtcl_obj->EmitDirectionYChannel(), c_EmitDirectionYChannelName, c_EmitDirectionYChannelCode, 
		prtclObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// EmitDirectionZ Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		prtcl_obj->EmitDirectionZChannel(), c_EmitDirectionZChannelName, c_EmitDirectionZChannelCode, 
		prtclObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// MinRotStartAngle Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		prtcl_obj->MinRotStartAngleChannel(), c_MinRotStartAngleChannelName, c_MinRotStartAngleChannelCode, 
		prtclObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// MaxRotStartAngle Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		prtcl_obj->MaxRotStartAngleChannel(), c_MaxRotStartAngleChannelName, c_MaxRotStartAngleChannelCode, 
		prtclObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// MinRotAngularVel Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		prtcl_obj->MinRotAngularVelChannel(), c_MinRotAngularVelChannelName, c_MinRotAngularVelChannelCode, 
		prtclObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// MaxRotAngularVel Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		prtcl_obj->MaxRotAngularVelChannel(), c_MaxRotAngularVelChannelName, c_MaxRotAngularVelChannelCode, 
		prtclObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// RotRadius Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		prtcl_obj->RotRadiusChannel(), c_RotRadiusChannelName, c_RotRadiusChannelCode, 
		prtclObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// RotRadiusScaleRate Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		prtcl_obj->RotRadiusScaleRateChannel(), c_RotRadiusScaleRateChannelName, c_RotRadiusScaleRateChannelCode, 
		prtclObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// TextureAlphaStart Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		prtcl_obj->TextureAlphaStartChannel(), c_TextureAlphaStartChannelName, c_TextureAlphaStartChannelCode, 
		prtclObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// TextureAlphaMiddle Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		prtcl_obj->TextureAlphaMiddleChannel(), c_TextureAlphaMiddleChannelName, c_TextureAlphaMiddleChannelCode, 
		prtclObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// TextureAlphaEnd Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		prtcl_obj->TextureAlphaEndChannel(), c_TextureAlphaEndChannelName, c_TextureAlphaEndChannelCode, 
		prtclObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// TextureAlphaMiddleStart Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		prtcl_obj->TextureAlphaMiddlePercentStartChannel(), c_TextureAlphaMiddlePercentStartChannelName, c_TextureAlphaMiddlePercentStartChannelCode, 
		prtclObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// TextureAlphaMiddleEnd Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		prtcl_obj->TextureAlphaMiddlePercentEndChannel(), c_TextureAlphaMiddlePercentEndChannelName, c_TextureAlphaMiddlePercentEndChannelCode, 
		prtclObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// Render Streaks Channel
	pDriver = cmmDriverCreatorBoolean::CreateDriverByName(i_DriverName, 
		prtcl_obj->RenderStreaksChannel(), c_RenderStreaksChannelName, c_RenderStreaksChannelCode, 
		prtclObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// Streak Length Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		prtcl_obj->StreakLengthChannel(), c_StreakLengthChannelName, c_StreakLengthChannelCode, 
		prtclObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// Streak Taper Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		prtcl_obj->StreakTaperChannel(), c_StreakTaperChannelName, c_StreakTaperChannelCode, 
		prtclObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// Streak Fade Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		prtcl_obj->StreakFadeChannel(), c_StreakFadeChannelName, c_StreakFadeChannelCode, 
		prtclObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// Show In Cube Reflections Channel
	pDriver = cmmDriverCreatorBoolean::CreateDriverByName(i_DriverName, 
		prtcl_obj->ShowInCubeReflectionsChannel(), c_ShowInCubeReflectionsChannelName, c_ShowInCubeReflectionsChannelCode, 
		prtclObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// Show In Planar Reflections Channel
	pDriver = cmmDriverCreatorBoolean::CreateDriverByName(i_DriverName, 
		prtcl_obj->ShowInPlanarReflectionsChannel(), c_ShowInPlanarReflectionsChannelName, c_ShowInPlanarReflectionsChannelCode, 
		prtclObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// Cast Shadows Channel
	pDriver = cmmDriverCreatorBoolean::CreateDriverByName(i_DriverName, 
		prtcl_obj->CastShadowsChannel(), c_CastShadowsChannelName, c_CastShadowsChannelCode, 
		prtclObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// Use Dithered Shadows Channel
	pDriver = cmmDriverCreatorBoolean::CreateDriverByName(i_DriverName, 
		prtcl_obj->UseDitheredShadowsChannel(), c_UseDitheredShadowsChannelName, c_UseDitheredShadowsChannelCode, 
		prtclObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// Shadow Dither Bias Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		prtcl_obj->ShadowDitherBiasChannel(), c_ShadowDitherBiasChannelName, c_ShadowDitherBiasChannelCode, 
		prtclObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// Additive Channel
	pDriver = cmmDriverCreatorBoolean::CreateDriverByName(i_DriverName, 
		prtcl_obj->AdditiveChannel(), c_AdditiveChannelName, c_AdditiveChannelCode, 
		prtclObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	//
	//	System Level
	//
	if ( !::_stricmp(i_DriverName,PRTCLDRIVERNAME_EMIT) )
	{
		prtclDriverEmit* pDriver = new prtclDriverEmit( prtcl_obj->ChannelEmit() );
		pDriver->SetName( PRTCLDRIVERNAME_EMIT );
		pDriver->SetInitialTime();
		pDriver->ShowIcons( prtclObjectMgr::IconsVisible() );

		// Attach driver to the apprtclriate channels
		tmlnChannel& channel = prtcl_obj->ChannelEmit();
		channel.AddDriver( pDriver );
		return pDriver;
	}
	else if ( !::_stricmp(i_DriverName,PRTCLDRIVERNAME_ANIMATION) )
	{
		prtclDriverAnimation* pDriver = new prtclDriverAnimation( prtcl_obj->ChannelAnimation(), c_PDAF );
		pDriver->SetName( PRTCLDRIVERNAME_ANIMATION );
		pDriver->SetInitialTime(0.0f);
		pDriver->ShowIcons( prtclObjectMgr::IconsVisible() );

		// Attach driver to the apprtclriate channels
		tmlnChannel& channel = prtcl_obj->ChannelAnimation();
		channel.AddDriver( pDriver );
		return pDriver;
	}

	return NULL;
}

//--------------------------------------------------------------------
// Create static key driver for the given channel
//--------------------------------------------------------------------
tmlnDriver* prtclDriverCreator::CreateKeyForChannel( tmlnScriptObject* io_pObject,
												    tmlnChannel* i_pChannel)
{
	prtclScriptObject* prtcl_obj = dynamic_cast<prtclScriptObject*>(io_pObject);
	if (!prtcl_obj) return NULL;

	if (i_pChannel == &prtcl_obj->ChannelPos())
	{
		// Position Channel
		return cmmDriverCreatorPosition::CreateKeyForChannel(
			prtcl_obj->ChannelPos(), c_PositionChannelName, c_PositionChannelCode, 
			prtclObjectMgr::IconsVisible(), prtcl_obj );
	}
	else if (i_pChannel == &prtcl_obj->OrientationChannel())
	{
		// Orientation Channel
		return cmmDriverCreatorOrientation::CreateKeyForChannel(
			prtcl_obj->OrientationChannel(), c_OrientationChannelName, c_OrientationChannelCode, 
			prtclObjectMgr::IconsVisible() );
	}
	else if (i_pChannel == &prtcl_obj->RateChannel())
	{
		// Rate Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			prtcl_obj->RateChannel(), c_RateChannelName, c_RateChannelCode, 
			prtclObjectMgr::IconsVisible() );
	}
	else if (i_pChannel == &prtcl_obj->MaxParticlesChannel())
	{
		// MaxParticles Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			prtcl_obj->MaxParticlesChannel(), c_MaxParticlesChannelName, c_MaxParticlesChannelCode, 
			prtclObjectMgr::IconsVisible() );
	}
	else if (i_pChannel == &prtcl_obj->LifetimeMinChannel())
	{
		// LifetimeMin Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			prtcl_obj->LifetimeMinChannel(), c_LifetimeMinChannelName, c_LifetimeMinChannelCode, 
			prtclObjectMgr::IconsVisible() );
	}
	else if (i_pChannel == &prtcl_obj->LifetimeMaxChannel())
	{
		// LifetimeMax Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			prtcl_obj->LifetimeMaxChannel(), c_LifetimeMaxChannelName, c_LifetimeMaxChannelCode, 
			prtclObjectMgr::IconsVisible() );
	}
	else if (i_pChannel == &prtcl_obj->ScaleStartChannel())
	{
		// ScaleStart Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			prtcl_obj->ScaleStartChannel(), c_ScaleStartChannelName, c_ScaleStartChannelCode, 
			prtclObjectMgr::IconsVisible() );
	}
	else if (i_pChannel == &prtcl_obj->ScaleCoefficientChannel())
	{
		// ScaleCoefficient Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			prtcl_obj->ScaleCoefficientChannel(), c_ScaleCoefficientChannelName, c_ScaleCoefficientChannelCode, 
			prtclObjectMgr::IconsVisible() );
	}
	else if (i_pChannel == &prtcl_obj->StartAngleMinChannel())
	{
		// StartAngleMin Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			prtcl_obj->StartAngleMinChannel(), c_StartAngleMinChannelName, c_StartAngleMinChannelCode, 
			prtclObjectMgr::IconsVisible() );
	}
	else if (i_pChannel == &prtcl_obj->StartAngleMinChannel())
	{
		// StartAngleMin Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			prtcl_obj->StartAngleMinChannel(), c_StartAngleMinChannelName, c_StartAngleMinChannelCode, 
			prtclObjectMgr::IconsVisible() );
	}
	else if (i_pChannel == &prtcl_obj->StartAngleMaxChannel())
	{
		// StartAngleMax Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			prtcl_obj->StartAngleMaxChannel(), c_StartAngleMaxChannelName, c_StartAngleMaxChannelCode, 
			prtclObjectMgr::IconsVisible() );
	}
	else if (i_pChannel == &prtcl_obj->AngularVelocityMinChannel())
	{
		// AngularVelocityMin Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			prtcl_obj->AngularVelocityMinChannel(), c_AngularVelocityMinChannelName, c_AngularVelocityMinChannelCode, 
			prtclObjectMgr::IconsVisible() );
	}
	else if (i_pChannel == &prtcl_obj->AngularVelocityMaxChannel())
	{
		// AngularVelocityMax Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			prtcl_obj->AngularVelocityMaxChannel(), c_AngularVelocityMaxChannelName, c_AngularVelocityMaxChannelCode, 
			prtclObjectMgr::IconsVisible() );
	}
	else if (i_pChannel == &prtcl_obj->AngularAccelerationMinChannel())
	{
		// AngularAccelerationMin Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			prtcl_obj->AngularAccelerationMinChannel(), c_AngularAccelerationMinChannelName, c_AngularAccelerationMinChannelCode, 
			prtclObjectMgr::IconsVisible() );
	}
	else if (i_pChannel == &prtcl_obj->EmitterScaleChannel())
	{
		// EmitterScale Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			prtcl_obj->EmitterScaleChannel(), c_EmitterScaleChannelName, c_EmitterScaleChannelCode, 
			prtclObjectMgr::IconsVisible() );
	}
// prtConeParticleGenerator
	else if (i_pChannel == &prtcl_obj->ConeAngleChannel())
	{
		// ConeAngle Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			prtcl_obj->ConeAngleChannel(), c_ConeAngleChannelName, c_ConeAngleChannelCode, 
			prtclObjectMgr::IconsVisible() );
	}

	else if (i_pChannel == &prtcl_obj->MinSpeedChannel())
	{
		// MinSpeed Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			prtcl_obj->MinSpeedChannel(), c_MinSpeedChannelName, c_MinSpeedChannelCode, 
			prtclObjectMgr::IconsVisible() );
	}

	else if (i_pChannel == &prtcl_obj->MaxSpeedChannel())
	{
		// MaxSpeed Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			prtcl_obj->MaxSpeedChannel(), c_MaxSpeedChannelName, c_MaxSpeedChannelCode, 
			prtclObjectMgr::IconsVisible() );
	}
	else if (i_pChannel == &prtcl_obj->AccelerationXChannel())
	{
		// AccelerationX Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			prtcl_obj->AccelerationXChannel(), c_AccelerationXChannelName, c_AccelerationXChannelCode, 
			prtclObjectMgr::IconsVisible() );
	}
	else if (i_pChannel == &prtcl_obj->AccelerationYChannel())
	{
		// AccelerationY Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			prtcl_obj->AccelerationYChannel(), c_AccelerationYChannelName, c_AccelerationYChannelCode, 
			prtclObjectMgr::IconsVisible() );
	}
	else if (i_pChannel == &prtcl_obj->AccelerationZChannel())
	{
		// AccelerationZ Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			prtcl_obj->AccelerationZChannel(), c_AccelerationZChannelName, c_AccelerationZChannelCode, 
			prtclObjectMgr::IconsVisible() );
	}
	else if (i_pChannel == &prtcl_obj->MinEmitSpeedChannel())
	{
		// MinEmitSpeed Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			prtcl_obj->MinEmitSpeedChannel(), c_MinEmitSpeedChannelName, c_MinEmitSpeedChannelCode, 
			prtclObjectMgr::IconsVisible() );
	}
	else if (i_pChannel == &prtcl_obj->MaxEmitSpeedChannel())
	{
		// MaxEmitSpeed Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			prtcl_obj->MaxEmitSpeedChannel(), c_MaxEmitSpeedChannelName, c_MaxEmitSpeedChannelCode, 
			prtclObjectMgr::IconsVisible() );
	}
	else if (i_pChannel == &prtcl_obj->EmitDirectionXChannel())
	{
		// EmitDirectionX Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			prtcl_obj->EmitDirectionXChannel(), c_EmitDirectionXChannelName, c_EmitDirectionXChannelCode, 
			prtclObjectMgr::IconsVisible() );
	}
	else if (i_pChannel == &prtcl_obj->EmitDirectionYChannel())
	{
		// EmitDirectionY Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			prtcl_obj->EmitDirectionYChannel(), c_EmitDirectionYChannelName, c_EmitDirectionYChannelCode, 
			prtclObjectMgr::IconsVisible() );
	}
	else if (i_pChannel == &prtcl_obj->EmitDirectionZChannel())
	{
		// EmitDirectionZ Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			prtcl_obj->EmitDirectionZChannel(), c_EmitDirectionZChannelName, c_EmitDirectionZChannelCode, 
			prtclObjectMgr::IconsVisible() );
	}
	else if (i_pChannel == &prtcl_obj->MinRotStartAngleChannel())
	{
		// MinRotStartAngle Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			prtcl_obj->MinRotStartAngleChannel(), c_MinRotStartAngleChannelName, c_MinRotStartAngleChannelCode, 
			prtclObjectMgr::IconsVisible() );
	}
	else if (i_pChannel == &prtcl_obj->MaxRotStartAngleChannel())
	{
		// MaxRotStartAngle Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			prtcl_obj->MaxRotStartAngleChannel(), c_MaxRotStartAngleChannelName, c_MaxRotStartAngleChannelCode, 
			prtclObjectMgr::IconsVisible() );
	}
	else if (i_pChannel == &prtcl_obj->MinRotAngularVelChannel())
	{
		// MinRotAngularVel Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			prtcl_obj->MinRotAngularVelChannel(), c_MinRotAngularVelChannelName, c_MinRotAngularVelChannelCode, 
			prtclObjectMgr::IconsVisible() );
	}
	else if (i_pChannel == &prtcl_obj->MaxRotAngularVelChannel())
	{
		// MaxRotAngularVel Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			prtcl_obj->MaxRotAngularVelChannel(), c_MaxRotAngularVelChannelName, c_MaxRotAngularVelChannelCode, 
			prtclObjectMgr::IconsVisible() );
	}
	else if (i_pChannel == &prtcl_obj->RotRadiusChannel())
	{
		// RotRadius Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			prtcl_obj->RotRadiusChannel(), c_RotRadiusChannelName, c_RotRadiusChannelCode, 
			prtclObjectMgr::IconsVisible() );
	}
	else if (i_pChannel == &prtcl_obj->RotRadiusScaleRateChannel())
	{
		// RotRadiusScaleRate Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			prtcl_obj->RotRadiusScaleRateChannel(), c_RotRadiusScaleRateChannelName, c_RotRadiusScaleRateChannelCode, 
			prtclObjectMgr::IconsVisible() );
	}
	else if (i_pChannel == &prtcl_obj->TextureAlphaStartChannel())
	{
		// TextureAlphaStart Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			prtcl_obj->TextureAlphaStartChannel(), c_TextureAlphaStartChannelName, c_TextureAlphaStartChannelCode, 
			prtclObjectMgr::IconsVisible() );
	}
	else if (i_pChannel == &prtcl_obj->TextureAlphaMiddleChannel())
	{
		// TextureAlphaMiddle Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			prtcl_obj->TextureAlphaMiddleChannel(), c_TextureAlphaMiddleChannelName, c_TextureAlphaMiddleChannelCode, 
			prtclObjectMgr::IconsVisible() );
	}
	else if (i_pChannel == &prtcl_obj->TextureAlphaEndChannel())
	{
		// TextureAlphaEnd Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			prtcl_obj->TextureAlphaEndChannel(), c_TextureAlphaEndChannelName, c_TextureAlphaEndChannelCode, 
			prtclObjectMgr::IconsVisible() );
	}
	else if (i_pChannel == &prtcl_obj->TextureAlphaMiddlePercentStartChannel())
	{
		// TextureAlphaMiddleStart Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			prtcl_obj->TextureAlphaMiddlePercentStartChannel(), c_TextureAlphaMiddlePercentStartChannelName, c_TextureAlphaMiddlePercentStartChannelCode, 
			prtclObjectMgr::IconsVisible() );
	}
	else if (i_pChannel == &prtcl_obj->TextureAlphaMiddlePercentEndChannel())
	{
		// TextureAlphaMiddleEnd Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			prtcl_obj->TextureAlphaMiddlePercentEndChannel(), c_TextureAlphaMiddlePercentEndChannelName, c_TextureAlphaMiddlePercentEndChannelCode, 
			prtclObjectMgr::IconsVisible() );
	}
	else if (i_pChannel == &prtcl_obj->RenderStreaksChannel())
	{
		// RenderStreaks Channel
		return cmmDriverCreatorBoolean::CreateKeyForChannel(
			prtcl_obj->RenderStreaksChannel(), c_RenderStreaksChannelName, c_RenderStreaksChannelCode, 
			prtclObjectMgr::IconsVisible() );
	}
	else if (i_pChannel == &prtcl_obj->StreakLengthChannel())
	{
		// StreakLength Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			prtcl_obj->StreakLengthChannel(), c_StreakLengthChannelName, c_StreakLengthChannelCode, 
			prtclObjectMgr::IconsVisible() );
	}
	else if (i_pChannel == &prtcl_obj->StreakTaperChannel())
	{
		// StreakTaper Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			prtcl_obj->StreakTaperChannel(), c_StreakTaperChannelName, c_StreakTaperChannelCode, 
			prtclObjectMgr::IconsVisible() );
	}
	else if (i_pChannel == &prtcl_obj->StreakFadeChannel())
	{
		// StreakFade Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			prtcl_obj->StreakFadeChannel(), c_StreakFadeChannelName, c_StreakFadeChannelCode, 
			prtclObjectMgr::IconsVisible() );
	}
	else if (i_pChannel == &prtcl_obj->ShowInCubeReflectionsChannel())
	{
		// ShowInCubeReflections Channel
		return cmmDriverCreatorBoolean::CreateKeyForChannel(
			prtcl_obj->ShowInCubeReflectionsChannel(), c_ShowInCubeReflectionsChannelName, c_ShowInCubeReflectionsChannelCode, 
			prtclObjectMgr::IconsVisible() );
	}
	else if (i_pChannel == &prtcl_obj->ShowInPlanarReflectionsChannel())
	{
		// ShowInPlanarReflections Channel
		return cmmDriverCreatorBoolean::CreateKeyForChannel(
			prtcl_obj->ShowInPlanarReflectionsChannel(), c_ShowInPlanarReflectionsChannelName, c_ShowInPlanarReflectionsChannelCode, 
			prtclObjectMgr::IconsVisible() );
	}
	else if (i_pChannel == &prtcl_obj->CastShadowsChannel())
	{
		// CastShadows Channel
		return cmmDriverCreatorBoolean::CreateKeyForChannel(
			prtcl_obj->CastShadowsChannel(), c_CastShadowsChannelName, c_CastShadowsChannelCode, 
			prtclObjectMgr::IconsVisible() );
	}
	else if (i_pChannel == &prtcl_obj->UseDitheredShadowsChannel())
	{
		// UseDitheredShadows Channel
		return cmmDriverCreatorBoolean::CreateKeyForChannel(
			prtcl_obj->UseDitheredShadowsChannel(), c_UseDitheredShadowsChannelName, c_UseDitheredShadowsChannelCode, 
			prtclObjectMgr::IconsVisible() );
	}
	else if (i_pChannel == &prtcl_obj->ShadowDitherBiasChannel())
	{
		// ShadowDitherBias Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			prtcl_obj->ShadowDitherBiasChannel(), c_ShadowDitherBiasChannelName, c_ShadowDitherBiasChannelCode, 
			prtclObjectMgr::IconsVisible() );
	}
	else if (i_pChannel == &prtcl_obj->AdditiveChannel())
	{
		// Additive Channel
		return cmmDriverCreatorBoolean::CreateKeyForChannel(
			prtcl_obj->AdditiveChannel(), c_AdditiveChannelName, c_AdditiveChannelCode, 
			prtclObjectMgr::IconsVisible() );
	}

	return NULL;
}

//--------------------------------------------------------------------
// Create driver for this object based on the the info structure
//--------------------------------------------------------------------
//virtual
tmlnDriver* prtclDriverCreator::CreateDriverFromInfo( tmlnScriptObject* io_pObject,
													  const tmlnDriverInfo& i_Info)
{
	prtclScriptObject* prtcl_obj = dynamic_cast<prtclScriptObject*>(io_pObject);
	if (!prtcl_obj) return NULL;

	chDefs::Name name = i_Info.GetBaseChunkName();

	if (name == prtclDriverEmitParser::GetChunkName())
	{
		const prtclDriverEmitInfo& driver_info = dynamic_cast<const prtclDriverEmitInfo&>(i_Info);
		prtclDriverEmit * pDriver = new prtclDriverEmit( prtcl_obj->ChannelEmit() );
		pDriver->SetDriverInfo(driver_info);

		tmlnChannel& channel = prtcl_obj->ChannelEmit();

		// Attach driver to the apprtclriate channels
		channel.AddDriver( pDriver );
		return pDriver;
	}
	else if (name == c_PDAF)
	{
		const prtclDriverAnimationInfo& driver_info = dynamic_cast<const prtclDriverAnimationInfo&>(i_Info);
		prtclDriverAnimation * pDriver = new prtclDriverAnimation( prtcl_obj->ChannelAnimation(), c_PDAF );
		pDriver->SetDriverInfo(driver_info);

		tmlnChannel& channel = prtcl_obj->ChannelAnimation();

		// Attach driver to the apprtclriate channels
		channel.AddDriver( pDriver );
		return pDriver;
	}
	else if (name == c_PPSP)	// particle position spline
	{
		// Old parser chunk name, convert to new one
		name = cmmDriverCreatorPosition::GetSplineChunkName(c_PositionChannelCode);
	}
	else if (name == c_PATC)	// particle attachment
	{
		// Old parser chunk name, convert to new one
		name = cmmDriverCreatorPosition::GetAttachChunkName(c_PositionChannelCode);
	}
	else if (name == c_PPOS)	// particle static position
	{
		// Old parser chunk name, convert to new one
		name = cmmDriverCreatorPosition::GetKeyChunkName(c_PositionChannelCode);
	}

	// Creation of drivers per channel
	tmlnDriver* pDriver = NULL;

	// Position channel
	pDriver = cmmDriverCreatorPosition::CreateDriverFromInfo(
		prtcl_obj->ChannelPos(), c_PositionChannelCode, i_Info, name, prtcl_obj );
	if (pDriver != NULL) 
		return pDriver;

	// Orientation channel
	pDriver = cmmDriverCreatorOrientation::CreateDriverFromInfo(
		prtcl_obj->OrientationChannel(), c_OrientationChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// Rate channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		prtcl_obj->RateChannel(), c_RateChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// MaxParticles channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		prtcl_obj->MaxParticlesChannel(), c_MaxParticlesChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// LifetimeMin channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		prtcl_obj->LifetimeMinChannel(), c_LifetimeMinChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// LifetimeMax channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		prtcl_obj->LifetimeMaxChannel(), c_LifetimeMaxChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// ScaleStart channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		prtcl_obj->ScaleStartChannel(), c_ScaleStartChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// ScaleCoefficient channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		prtcl_obj->ScaleCoefficientChannel(), c_ScaleCoefficientChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// StartAngleMin channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		prtcl_obj->StartAngleMinChannel(), c_StartAngleMinChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// StartAngleMax channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		prtcl_obj->StartAngleMaxChannel(), c_StartAngleMaxChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// AngularVelocityMax channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		prtcl_obj->AngularVelocityMaxChannel(), c_AngularVelocityMaxChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// AngularVelocityMin channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		prtcl_obj->AngularVelocityMinChannel(), c_AngularVelocityMinChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// AngularAccelerationMax channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		prtcl_obj->AngularAccelerationMaxChannel(), c_AngularAccelerationMaxChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// AngularAccelerationMin channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		prtcl_obj->AngularAccelerationMinChannel(), c_AngularAccelerationMinChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// EmitterScale channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		prtcl_obj->EmitterScaleChannel(), c_EmitterScaleChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// prtConeParticleGenerator

	// ConeAngle channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		prtcl_obj->ConeAngleChannel(), c_ConeAngleChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// MinSpeed channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		prtcl_obj->MinSpeedChannel(), c_MinSpeedChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// MaxSpeed channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		prtcl_obj->MaxSpeedChannel(), c_MaxSpeedChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// AccelerationX channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		prtcl_obj->AccelerationXChannel(), c_AccelerationXChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// AccelerationY channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		prtcl_obj->AccelerationYChannel(), c_AccelerationYChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// AccelerationZ channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		prtcl_obj->AccelerationZChannel(), c_AccelerationZChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// prtSpiralParticleGenerator
	// MinEmitSpeed channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		prtcl_obj->MinEmitSpeedChannel(), c_MinEmitSpeedChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// MaxEmitSpeed channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		prtcl_obj->MaxEmitSpeedChannel(), c_MaxEmitSpeedChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// EmitDirectionX channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		prtcl_obj->EmitDirectionXChannel(), c_EmitDirectionXChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// EmitDirectionY channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		prtcl_obj->EmitDirectionYChannel(), c_EmitDirectionYChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// EmitDirectionZ channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		prtcl_obj->EmitDirectionZChannel(), c_EmitDirectionZChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// MinRotStartAngle channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		prtcl_obj->MinRotStartAngleChannel(), c_MinRotStartAngleChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// MaxRotStartAngle channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		prtcl_obj->MaxRotStartAngleChannel(), c_MaxRotStartAngleChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// MinRotAngularVel channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		prtcl_obj->MinRotAngularVelChannel(), c_MinRotAngularVelChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// MaxRotAngularVel channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		prtcl_obj->MaxRotAngularVelChannel(), c_MaxRotAngularVelChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// RotRadius channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		prtcl_obj->RotRadiusChannel(), c_RotRadiusChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// RotRadiusScaleRate channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		prtcl_obj->RotRadiusScaleRateChannel(), c_RotRadiusScaleRateChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// TextureAlphaStart channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		prtcl_obj->TextureAlphaStartChannel(), c_TextureAlphaStartChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// TextureAlphaMiddle channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		prtcl_obj->TextureAlphaMiddleChannel(), c_TextureAlphaMiddleChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// TextureAlphaEnd channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		prtcl_obj->TextureAlphaEndChannel(), c_TextureAlphaEndChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// TextureAlphaMiddleStart channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		prtcl_obj->TextureAlphaMiddlePercentStartChannel(), c_TextureAlphaMiddlePercentStartChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// TextureAlphaMiddleEnd channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		prtcl_obj->TextureAlphaMiddlePercentEndChannel(), c_TextureAlphaMiddlePercentEndChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// RenderStreaks channel
	pDriver = cmmDriverCreatorBoolean::CreateDriverFromInfo(
		prtcl_obj->RenderStreaksChannel(), c_RenderStreaksChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// StreakLength channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		prtcl_obj->StreakLengthChannel(), c_StreakLengthChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// StreakTaper channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		prtcl_obj->StreakTaperChannel(), c_StreakTaperChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// StreakFade channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		prtcl_obj->StreakFadeChannel(), c_StreakFadeChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// ShowInCubeReflections channel
	pDriver = cmmDriverCreatorBoolean::CreateDriverFromInfo(
		prtcl_obj->ShowInCubeReflectionsChannel(), c_ShowInCubeReflectionsChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// ShowInPlanarReflections channel
	pDriver = cmmDriverCreatorBoolean::CreateDriverFromInfo(
		prtcl_obj->ShowInPlanarReflectionsChannel(), c_ShowInPlanarReflectionsChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// CastShadows channel
	pDriver = cmmDriverCreatorBoolean::CreateDriverFromInfo(
		prtcl_obj->CastShadowsChannel(), c_CastShadowsChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// UseDitheredShadows channel
	pDriver = cmmDriverCreatorBoolean::CreateDriverFromInfo(
		prtcl_obj->UseDitheredShadowsChannel(), c_UseDitheredShadowsChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// ShadowDitherBias channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		prtcl_obj->ShadowDitherBiasChannel(), c_ShadowDitherBiasChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// Additive channel
	pDriver = cmmDriverCreatorBoolean::CreateDriverFromInfo(
		prtcl_obj->AdditiveChannel(), c_AdditiveChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;
		
	return NULL;
}


//--------------------------------------------------------------------
//	create a driver and add it to the object
//--------------------------------------------------------------------
//static
void prtclDriverCreator::CreateDriverEmit( tmlnScriptObject* i_pObject )
{
	prtclScriptObject* prtcl_obj = dynamic_cast<prtclScriptObject*>(i_pObject);
	if (!prtcl_obj) return;

	prtclDriverEmit* pDriver = new prtclDriverEmit( prtcl_obj->ChannelEmit() );
	pDriver->SetName( PRTCLDRIVERNAME_EMIT );
	pDriver->SetInitialTime();
	pDriver->ShowIcons( prtclObjectMgr::IconsVisible() );

	// Attach driver to the apprtclriate channels
	tmlnChannel& channel = prtcl_obj->ChannelEmit();
	channel.AddDriver( pDriver );

	// Since this is being done outside of Channel Trax Editor, we
	// need to add the driver to the object explicitly
	prtcl_obj->AddDriver( pDriver );
}

