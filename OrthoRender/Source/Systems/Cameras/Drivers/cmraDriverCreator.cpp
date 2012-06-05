/*****************************************************************************
**	cmraDriverCreator.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Systems/Cameras/Drivers/cmraDriverCreator.hpp"

#include "Systems/Cameras/Object/cmraScriptObject.hpp"
#include "Systems/Cameras/Timeline/cmraChannelCapture.hpp"
#include "Systems/Cameras/Data/cmraDataParser.hpp"
#include "Systems/Cameras/Drivers/cmraDriverCapture.hpp"
#include "Systems/Cameras/Drivers/cmraDriverCaptureInfo.hpp"
#include "Systems/Cameras/Drivers/cmraDriverCaptureParser.hpp"
#include "Systems/Cameras/Drivers/cmraDriverCircle.hpp"
#include "Systems/Cameras/Drivers/cmraDriverCircleInfo.hpp"
#include "Systems/Cameras/Drivers/cmraDriverCircleParser.hpp"
#include "Systems/Cameras/Drivers/cmraDriverFollow.hpp"
#include "Systems/Cameras/Drivers/cmraDriverFollowInfo.hpp"
#include "Systems/Cameras/Drivers/cmraDriverFollowParser.hpp"
#include "Systems/Cameras/Drivers/cmraDriverFocusAttach.hpp"
#include "Systems/Cameras/Drivers/cmraDriverFocusAttachInfo.hpp"
#include "Systems/Cameras/Drivers/cmraDriverFocusAttachParser.hpp"
#include "Systems/Cameras/Drivers/cmraDriverFramedArc.hpp"
#include "Systems/Cameras/Drivers/cmraDriverFramedArcInfo.hpp"
#include "Systems/Cameras/Drivers/cmraDriverFramedArcParser.hpp"
#include "Systems/Cameras/Drivers/cmraDriverFramedCloseUp.hpp"
#include "Systems/Cameras/Drivers/cmraDriverFramedCloseUpParser.hpp"
#include "Systems/Cameras/Drivers/cmraDriverFramedMedLong.hpp"
#include "Systems/Cameras/Drivers/cmraDriverFramedMedLongParser.hpp"
#include "Systems/Cameras/Drivers/cmraDriverFramedOverhead.hpp"
#include "Systems/Cameras/Drivers/cmraDriverFramedOverheadParser.hpp"
#include "Systems/Cameras/Drivers/cmraDriverKeyPositionAndTarget.hpp"
#include "Systems/Cameras/Drivers/cmraDriverKeyPositionAndTargetInfo.hpp"
#include "Systems/Cameras/Drivers/cmraDriverKeyPositionAndTargetParser.hpp"
#include "Systems/Cameras/Drivers/cmraDriverKeyPosition.hpp"
#include "Systems/Cameras/Drivers/cmraDriverKeyPositionInfo.hpp"
#include "Systems/Cameras/Drivers/cmraDriverKeyPositionParser.hpp"
#include "Systems/Cameras/Drivers/cmraDriverKeyTarget.hpp"
#include "Systems/Cameras/Drivers/cmraDriverKeyTargetInfo.hpp"
#include "Systems/Cameras/Drivers/cmraDriverKeyTargetParser.hpp"
#include "Systems/Cameras/Drivers/cmraDriverMayaScript.hpp"
#include "Systems/Cameras/Drivers/cmraDriverMayaScriptInfo.hpp"
#include "Systems/Cameras/Drivers/cmraDriverMayaScriptParser.hpp"
//#include "Systems/Cameras/Drivers/cmraDriverShakeCamera.hpp"
//#include "Systems/Cameras/Drivers/cmraDriverShakeCameraParser.hpp"
#include "Systems/Cameras/Drivers/cmraDriverTarget.hpp"
#include "Systems/Cameras/Drivers/cmraDriverTargetInfo.hpp"
#include "Systems/Cameras/Drivers/cmraDriverTargetParser.hpp"
#include "Systems/Cameras/Object/cmraObjectMgr.hpp"

#include "Systems/Common/DriverCreator/cmmDriverCreatorFloat.hpp"
#include "Systems/Common/DriverCreator/cmmDriverCreatorPosition.hpp"
#include "Support/tmln/tmlnChannelFloat.hpp"
#include "Support/tmln/tmlnChannelPosition.hpp"
#include "Drivers/Attach/tmlnDriverAttachParser.hpp"
#include "Drivers/Float/tmlnDriverFloatParser.hpp"
#include "Drivers/Spline/tmlnDriverSplineParser.hpp"
#include "Drivers/Position/tmlnDriverPositionParser.hpp"
#include "Support/tmln/tmlnTimeLine.hpp"


//============================================================================
//============================================================================
namespace
{
	//========================================================================
	//========================================================================
	const char *c_PositionChannelName = "Position";
	const char c_PositionChannelCode[2] = { 'P', 'S' };  // PoSition channel code
	const char *c_TargetChannelName = "Target";
	const char c_TargetChannelCode[2] = { 'T', 'G' };  // TarGet channel code
	const char *c_TiltChannelName = "Tilt";
	const char c_TiltChannelCode[2] = { 'T', 'L' };  // TiLt channel code
	const char *c_FOVChannelName = "FOV";
	const char c_FOVChannelCode[2] = { 'F', 'V' };  // Field of View channel code
	const char *c_OrthoWidthChannelName = "OrthoWidth";
	const char c_OrthoWidthChannelCode[2] = { 'O', 'W' };  // OrthoWidth channel code
	const char *c_NearClipChannelName = "Near Clip";
	const char c_NearClipChannelCode[2] = { 'N', 'C' };  // NearClip channel code
	const char *c_FarClipChannelName = "Far Clip";
	const char c_FarClipChannelCode[2] = { 'F', 'C' };  // FarClip channel code
	const char *c_DOFMaxFarBlurChannelName = "DOF Max Far Blur";
	const char  c_DOFMaxFarBlurChannelCode[2] = { 'D', 'F' };  // DOFMaxFarBlur channel code
	const char *c_HDRMiddleGrayChannelName = "HDRMiddleGray";
	const char  c_HDRMiddleGrayChannelCode[2] = { 'M', 'G' };  // HDRMiddleGray channel code
	const char *c_HDRBloomScaleChannelName = "HDRBloomScale";
	const char  c_HDRBloomScaleChannelCode[2] = { 'B', 'S' };  // HDRBloomScale channel code
	const char *c_HDRStarScaleChannelName = "HDRStarScale";
	const char  c_HDRStarScaleChannelCode[2] = { 'S', 'S' };  // HDRStarScale channel code
	const char *c_HDRBrightPassThreshChannelName = "HDRBrightPassThresh";
	const char  c_HDRBrightPassThreshChannelCode[2] = { 'B', 'T' };  // HDRBrightPassThresh channel code
	const char *c_HDRBrightPassOffsetChannelName = "HDRBrightPassOffset";
	const char  c_HDRBrightPassOffsetChannelCode[2] = { 'B', 'O' };  // HDRBrightPassOffset channel code
	const char *c_HDRWhiteCutoffChannelName = "HDRWhiteCutoff";
	const char  c_HDRWhiteCutoffChannelCode[2] = { 'W', 'C' };  // HDRWhiteCutoff channel code
	const char *c_HDRSceneLuminanceChannelName = "HDRSceneLuminance";
	const char  c_HDRSceneLuminanceChannelCode[2] = { 'S', 'L' };  // HDRSceneLuminance channel code

	
	const char* CAMDRIVERNAME_TAROBJ = "Target Object";
	const char* CAMDRIVERNAME_KEYPOSTAR = "Camera Key";
	const char* CAMDRIVERNAME_KEYPOS = "Camera Key Position";
	const char* CAMDRIVERNAME_KEYTAR = "Camera Key Target";
	const char* CAMDRIVERNAME_MAYASCRIPT = "Maya Script";
	const char* CAMDRIVERNAME_CAPTURE = "Capture";
	const char* CAMDRIVERNAME_CIRCLE = "Circle";
	const char* CAMDRIVERNAME_FOLLOW = "Follow";
	const char* CAMDRIVERNAME_FOCUSATTACH = "Focus Target";
	const char* CAMDRIVERNAME_FRAMEDARC = "Framed Arc";
	const char* CAMDRIVERNAME_FRAMEDCLOSEUP = "Framed Close-Up";
	const char* CAMDRIVERNAME_FRAMEDMED = "Framed Medium-Long";
	const char* CAMDRIVERNAME_FRAMEDOVERHEAD = "Framed Overhead";

	//========================================================================
	//========================================================================
	const chDefs::Name c_CPSP = chDefs::MakeName('C', 'P', 'S', 'P');  // Camera Position SPline
	const chDefs::Name c_CTSP = chDefs::MakeName('C', 'T', 'S', 'P');  // Camera Target SPline
	const chDefs::Name c_CFOV = chDefs::MakeName('C', 'F', 'O', 'V');  // Camera FOV
	const chDefs::Name c_CTLT = chDefs::MakeName('C', 'T', 'L', 'T');  // Camera Tilt
	const chDefs::Name c_CSPT = chDefs::MakeName('C', 'S', 'P', 'T');  // Camera Static Position
	const chDefs::Name c_CSTG = chDefs::MakeName('C', 'S', 'T', 'G');  // Camera Static Target
	const chDefs::Name c_CACH = chDefs::MakeName('C', 'A', 'C', 'H');  // Camera Attachment
	const chDefs::Name c_CFAT = chDefs::MakeName('C', 'F', 'A', 'T');  // Camera Focus Attach

	// Return true if there is a driver on this channel at the given time
	bool has_driver(tmlnChannel &i_Channel)
	{
		const float c_TimeThreshold = 1.0f / (g3dConstants::c_fDefaultFrameRate + 10.0f); // Threshold less than 1 frame at g3dConstants::c_fDefaultFrameRate fps

		std::vector<tmlnDriver*> drivers;
		float current_time = tmlnTimeLine::GetValue();
		i_Channel.GetActiveDrivers(current_time - c_TimeThreshold, 
									current_time + c_TimeThreshold, 
									drivers);

		return (!drivers.empty());
	}
}

//--------------------------------------------------------------------
// Create parsers for driver types created by this creator
//--------------------------------------------------------------------
//static
void cmraDriverCreator::CreateParsers()
{
	chDefs::Name sys_code = cmraCamerasDataParser::GetChunkName();

	// Creation of driver parsers per channel
	cmmDriverCreatorPosition::CreateParsers(sys_code, c_PositionChannelCode); 
	cmmDriverCreatorPosition::CreateParsers(sys_code, c_TargetChannelCode); 
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_TiltChannelCode);  
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_FOVChannelCode); 
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_OrthoWidthChannelCode); 
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_NearClipChannelCode); 
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_FarClipChannelCode); 
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_DOFMaxFarBlurChannelCode); 
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_HDRMiddleGrayChannelCode); 
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_HDRBloomScaleChannelCode); 
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_HDRStarScaleChannelCode); 
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_HDRBrightPassThreshChannelCode); 
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_HDRBrightPassOffsetChannelCode); 
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_HDRWhiteCutoffChannelCode); 
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_HDRSceneLuminanceChannelCode); 

	// System specific drivers
	tmlnParser::AddDriverParser(sys_code, cmraDriverTargetParser::GetChunkName(), new cmraDriverTargetParser());
	tmlnParser::AddDriverParser(sys_code, cmraDriverKeyPositionAndTargetParser::GetChunkName(), new cmraDriverKeyPositionAndTargetParser());
	tmlnParser::AddDriverParser(sys_code, cmraDriverKeyPositionParser::GetChunkName(), new cmraDriverKeyPositionParser());
	tmlnParser::AddDriverParser(sys_code, cmraDriverKeyTargetParser::GetChunkName(), new cmraDriverKeyTargetParser());
	tmlnParser::AddDriverParser(sys_code, cmraDriverMayaScriptParser::GetChunkName(), new cmraDriverMayaScriptParser());
	tmlnParser::AddDriverParser(sys_code, cmraDriverCaptureParser::GetChunkName(), new cmraDriverCaptureParser());
	tmlnParser::AddDriverParser(sys_code, cmraDriverCircleParser::GetChunkName(), new cmraDriverCircleParser());
	tmlnParser::AddDriverParser(sys_code, cmraDriverFollowParser::GetChunkName(), new cmraDriverFollowParser());
	tmlnParser::AddDriverParser(sys_code, c_CFAT, new cmraDriverFocusAttachParser(c_CFAT));
	tmlnParser::AddDriverParser(sys_code, cmraDriverFramedArcParser::GetChunkName(), new cmraDriverFramedArcParser());
	tmlnParser::AddDriverParser(sys_code, cmraDriverFramedCloseUpParser::GetChunkName(), new cmraDriverFramedCloseUpParser());
	tmlnParser::AddDriverParser(sys_code, cmraDriverFramedMedLongParser::GetChunkName(), new cmraDriverFramedMedLongParser());
	tmlnParser::AddDriverParser(sys_code, cmraDriverFramedOverheadParser::GetChunkName(), new cmraDriverFramedOverheadParser());

	// Because of old file formats, we still have to create these parsers here
	tmlnParser::AddDriverParser(sys_code, c_CPSP,	new tmlnDriverSplineParser(c_CPSP));
	tmlnParser::AddDriverParser(sys_code, c_CTSP,	new tmlnDriverSplineParser(c_CTSP));
	tmlnParser::AddDriverParser(sys_code, c_CSPT,	new tmlnDriverPositionParser(c_CSPT));
	tmlnParser::AddDriverParser(sys_code, c_CSTG,	new tmlnDriverPositionParser(c_CSTG));
	tmlnParser::AddDriverParser(sys_code, c_CFOV,	new tmlnDriverFloatParser(c_CFOV));
	tmlnParser::AddDriverParser(sys_code, c_CTLT,	new tmlnDriverFloatParser(c_CTLT));
	tmlnParser::AddDriverParser(sys_code, c_CACH, new tmlnDriverAttachParser(c_CACH));
}

//--------------------------------------------------------------------
// Gather the possible types of drivers that can be created for
//	for this object.  Driver names are added through the
//	DriverNameList's API
//--------------------------------------------------------------------
void cmraDriverCreator::GatherPossibleDrivers(const tmlnScriptObject* i_pObject,
											   tmlnDriverNameList& io_Drivers)
{
	if (dynamic_cast<const cmraScriptObject*>(i_pObject))
	{
		// Creation of drivers per channel
		cmmDriverCreatorPosition::GatherPossibleDrivers(c_PositionChannelName,
			io_Drivers, "Motion", this); 
		cmmDriverCreatorPosition::GatherPossibleDrivers(c_TargetChannelName,
			io_Drivers, "Target", this); 
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_TiltChannelName,
			io_Drivers, "Parameters", this); 
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_FOVChannelName,
			io_Drivers, "Parameters", this); 
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_OrthoWidthChannelName,
			io_Drivers, "Parameters", this); 
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_NearClipChannelName,
			io_Drivers, "Parameters", this); 
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_FarClipChannelName,
			io_Drivers, "Parameters", this); 
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_DOFMaxFarBlurChannelName,
			io_Drivers, "Parameters", this); 
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_HDRMiddleGrayChannelName,
			io_Drivers, "Parameters", this); 
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_HDRBloomScaleChannelName,
			io_Drivers, "Parameters", this); 
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_HDRStarScaleChannelName,
			io_Drivers, "Parameters", this); 
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_HDRBrightPassThreshChannelName,
			io_Drivers, "Parameters", this); 
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_HDRBrightPassOffsetChannelName,
			io_Drivers, "Parameters", this); 
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_HDRWhiteCutoffChannelName,
			io_Drivers, "Parameters", this); 
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_HDRSceneLuminanceChannelName,
			io_Drivers, "Parameters", this); 

		// System specific drivers
		io_Drivers.AddDriver(CAMDRIVERNAME_CAPTURE, "Capture", this);
		io_Drivers.AddDriver(CAMDRIVERNAME_MAYASCRIPT, "Motion", this);
		io_Drivers.AddDriver(CAMDRIVERNAME_CIRCLE, "Motion", this);
		io_Drivers.AddDriver(CAMDRIVERNAME_FOLLOW, "Motion", this);
		io_Drivers.AddDriver(CAMDRIVERNAME_KEYPOSTAR, "Motion", this);
//PHASING OUT		io_Drivers.AddDriver(CAMDRIVERNAME_KEYPOS, "Motion", this);
//PHASING OUT		io_Drivers.AddDriver(CAMDRIVERNAME_KEYTAR, "Target", this);
		io_Drivers.AddDriver(CAMDRIVERNAME_FOCUSATTACH, "Parameters", this);
//
//	Obsolete: don't let users pick them anymore
//
//		io_Drivers.AddDriver(CAMDRIVERNAME_FRAMEDARC, "zFramed", this);
//		io_Drivers.AddDriver(CAMDRIVERNAME_FRAMEDCLOSEUP, "zFramed", this);
//		io_Drivers.AddDriver(CAMDRIVERNAME_FRAMEDMED, "zFramed", this);
//		io_Drivers.AddDriver(CAMDRIVERNAME_FRAMEDOVERHEAD, "zFramed", this);
		// Target attachment can be used from general Attach driver instead
		// of system specific driver
		//io_Drivers.AddDriver(CAMDRIVERNAME_TAROBJ, "Target", this);
	}
}


//--------------------------------------------------------------------
// Create driver for this object based on the name used in
//	GatherPossibleDrivers and attach it to the channels
//	on this script object.
//--------------------------------------------------------------------
tmlnDriver* cmraDriverCreator::CreateDriverByName( const char* i_DriverName,
													tmlnScriptObject* i_pObject )
{
	cmraScriptObject* cmra_obj = dynamic_cast<cmraScriptObject*>(i_pObject);
	if (!cmra_obj) return NULL;

	// Creation of drivers per channel
	tmlnDriver* pDriver = NULL;

	// Position Channel
	pDriver = cmmDriverCreatorPosition::CreateDriverByName(i_DriverName, 
		cmra_obj->PositionChannel(), c_PositionChannelName, c_PositionChannelCode, 
		cmraObjectMgr::IconsVisible(), cmra_obj );
	if (pDriver != NULL) 
		return pDriver;

	// Target Channel
	pDriver = cmmDriverCreatorPosition::CreateDriverByName(i_DriverName, 
		cmra_obj->TargetChannel(), c_TargetChannelName, c_TargetChannelCode, 
		cmraObjectMgr::IconsVisible(), cmra_obj );
	if (pDriver != NULL) 
		return pDriver;

	// Tilt Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		cmra_obj->TiltChannel(), c_TiltChannelName, c_TiltChannelCode, 
		cmraObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// FOV Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		cmra_obj->FOVChannel(), c_FOVChannelName, c_FOVChannelCode, 
		cmraObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// OrthoWidth Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		cmra_obj->OrthoWidthChannel(), c_OrthoWidthChannelName, c_OrthoWidthChannelCode, 
		cmraObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// NearClip Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		cmra_obj->NearClipChannel(), c_NearClipChannelName, c_NearClipChannelCode, 
		cmraObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// FarClip Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		cmra_obj->FarClipChannel(), c_FarClipChannelName, c_FarClipChannelCode, 
		cmraObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// DOFMaxFarBlur Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		cmra_obj->DOFMaxFarBlurChannel(), c_DOFMaxFarBlurChannelName, c_DOFMaxFarBlurChannelCode, 
		cmraObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// HDRMiddleGray Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		cmra_obj->HDRMiddleGrayChannel(), c_HDRMiddleGrayChannelName, c_HDRMiddleGrayChannelCode, 
		cmraObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// HDRBloomScale Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		cmra_obj->HDRBloomScaleChannel(), c_HDRBloomScaleChannelName, c_HDRBloomScaleChannelCode, 
		cmraObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// HDRStarScale Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		cmra_obj->HDRStarScaleChannel(), c_HDRStarScaleChannelName, c_HDRStarScaleChannelCode, 
		cmraObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// HDRBrightPassThresh Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		cmra_obj->HDRBrightPassThreshChannel(), c_HDRBrightPassThreshChannelName, c_HDRBrightPassThreshChannelCode, 
		cmraObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// HDRBrightPassOffset Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		cmra_obj->HDRBrightPassOffsetChannel(), c_HDRBrightPassOffsetChannelName, c_HDRBrightPassOffsetChannelCode, 
		cmraObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// HDRWhiteCutoff Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		cmra_obj->HDRWhiteCutoffChannel(), c_HDRWhiteCutoffChannelName, c_HDRWhiteCutoffChannelCode, 
		cmraObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	// HDRSceneLuminance Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		cmra_obj->HDRSceneLuminanceChannel(), c_HDRSceneLuminanceChannelName, c_HDRSceneLuminanceChannelCode, 
		cmraObjectMgr::IconsVisible() );
	if (pDriver != NULL) 
		return pDriver;

	if (!::_stricmp(i_DriverName, CAMDRIVERNAME_KEYPOSTAR))
	{
		cmraDriverKeyPositionAndTarget * pDriver = new cmraDriverKeyPositionAndTarget( cmra_obj->PositionChannel(), cmra_obj->TargetChannel() );
		pDriver->SetName("Key");
		pDriver->SetInitialTime(0.0f);

		// Attach driver to the appropriate channels
		tmlnChannel& channelT = cmra_obj->TargetChannel();
		tmlnChannel& channelP = cmra_obj->PositionChannel();
		channelT.AddDriver( pDriver );
		channelP.AddDriver( pDriver );
		return pDriver;
	}
	else if (!::_stricmp(i_DriverName, CAMDRIVERNAME_KEYPOS))
	{
		cmraDriverKeyPosition * pDriver = new cmraDriverKeyPosition( cmra_obj->PositionChannel() );
		pDriver->SetName("KeyP");
		pDriver->SetInitialTime(0.0f);

		// Attach driver to the appropriate channels
		tmlnChannel& channelP = cmra_obj->PositionChannel();
		channelP.AddDriver( pDriver );
		return pDriver;
	}
	else if (!::_stricmp(i_DriverName, CAMDRIVERNAME_KEYTAR))
	{
		cmraDriverKeyTarget * pDriver = new cmraDriverKeyTarget( cmra_obj->TargetChannel() );
		pDriver->SetName("KeyT");
		pDriver->SetInitialTime(0.0f);

		// Attach driver to the appropriate channels
		tmlnChannel& channelT = cmra_obj->TargetChannel();
		channelT.AddDriver( pDriver );
		return pDriver;
	}
	else if (!::_stricmp(i_DriverName, CAMDRIVERNAME_MAYASCRIPT))
	{
		cmraDriverMayaScript * pDriver = new cmraDriverMayaScript( cmra_obj->PositionChannel(), 
			cmra_obj->TargetChannel(), cmra_obj->FOVChannel(), cmra_obj->TiltChannel() );
		pDriver->SetName("Maya Script");
		pDriver->SetInitialTime(0.0f);

		// Attach driver to the appropriate channels
		tmlnChannel& channelT = cmra_obj->TargetChannel();
		tmlnChannel& channelP = cmra_obj->PositionChannel();
		tmlnChannel& channelF = cmra_obj->FOVChannel();
		tmlnChannel& channelU = cmra_obj->TiltChannel();
		channelT.AddDriver( pDriver );
		channelP.AddDriver( pDriver );
		channelF.AddDriver( pDriver );
		channelU.AddDriver( pDriver );
		return pDriver;
	}
	else if (!::_stricmp(i_DriverName, CAMDRIVERNAME_CAPTURE))
	{
		cmraDriverCapture * pDriver = new cmraDriverCapture( cmra_obj->CaptureChannel() );
		pDriver->SetName(CAMDRIVERNAME_CAPTURE);
		pDriver->SetInitialTime();

		// Attach driver to the appropriate channels
		cmraChannelCapture& channel = cmra_obj->CaptureChannel();
		channel.AddDriver( pDriver );
		return pDriver;
	}
	else if (!::_stricmp(i_DriverName, CAMDRIVERNAME_CIRCLE))
	{
		cmraDriverCircle * pDriver = new cmraDriverCircle( cmra_obj->PositionChannel(), cmra_obj->TargetChannel() );
		pDriver->SetName(CAMDRIVERNAME_CIRCLE);
		pDriver->SetInitialTime();

		// Attach driver to the appropriate channels
		tmlnChannel& channel = cmra_obj->PositionChannel();
		channel.AddDriver( pDriver );
		return pDriver;
	}
	else if (!::_stricmp(i_DriverName, CAMDRIVERNAME_FOLLOW))
	{
		cmraDriverFollow * pDriver = new cmraDriverFollow( cmra_obj->PositionChannel(), cmra_obj->TargetChannel() );
		pDriver->SetName(CAMDRIVERNAME_FOLLOW);
		pDriver->SetInitialTime();

		// Attach driver to the appropriate channels
		tmlnChannel& channel = cmra_obj->PositionChannel();
		channel.AddDriver( pDriver );
		return pDriver;
	}
	else if (!::_stricmp(i_DriverName, CAMDRIVERNAME_FOCUSATTACH))
	{
		cmraDriverFocusAttach* pDriver = new cmraDriverFocusAttach( cmra_obj->GetName().GetUID(),
			cmra_obj->NearFocusDistanceChannel(), 
			cmra_obj->FarFocusDistanceChannel(),
			cmra_obj->NearBlurDistanceChannel(),
			cmra_obj->FarBlurDistanceChannel(),
			c_CFAT );
		pDriver->SetName(CAMDRIVERNAME_FOCUSATTACH);
		pDriver->SetInitialTime();

		// Attach driver to the appropriate channels
		tmlnChannel& channel = cmra_obj->NearFocusDistanceChannel();
		tmlnChannel& channel2 = cmra_obj->FarFocusDistanceChannel();
		tmlnChannel& channel3 = cmra_obj->NearBlurDistanceChannel();
		tmlnChannel& channel4 = cmra_obj->FarBlurDistanceChannel();
		channel.AddDriver( pDriver );
		channel2.AddDriver( pDriver );
		channel3.AddDriver( pDriver );
		channel4.AddDriver( pDriver );

		return pDriver;
	}
	else if (!::_stricmp(i_DriverName, CAMDRIVERNAME_FRAMEDARC))
	{
		cmraDriverFramedArc * pDriver = new cmraDriverFramedArc( cmra_obj->GetName().GetUID(), cmra_obj->PositionChannel(), cmra_obj->TargetChannel() );
		pDriver->SetName("Framed Arc");
		pDriver->SetInitialTime();

		// Attach driver to the appropriate channels
		tmlnChannel& channelT = cmra_obj->TargetChannel();
		tmlnChannel& channelP = cmra_obj->PositionChannel();
		channelT.AddDriver( pDriver );
		channelP.AddDriver( pDriver );
		return pDriver;
	}
	else if (!::_stricmp(i_DriverName, CAMDRIVERNAME_FRAMEDCLOSEUP))
	{
		cmraDriverFramedCloseUp * pDriver = new cmraDriverFramedCloseUp( cmra_obj->GetName().GetUID(), cmra_obj->PositionChannel(), cmra_obj->TargetChannel() );
		pDriver->SetName("Framed Close-Up");
		pDriver->SetInitialTime();

		// Attach driver to the appropriate channels
		tmlnChannel& channelT = cmra_obj->TargetChannel();
		tmlnChannel& channelP = cmra_obj->PositionChannel();
		channelT.AddDriver( pDriver );
		channelP.AddDriver( pDriver );
		return pDriver;
	}
	else if (!::_stricmp(i_DriverName, CAMDRIVERNAME_FRAMEDMED))
	{
		cmraDriverFramedMedLong * pDriver = new cmraDriverFramedMedLong( cmra_obj->GetName().GetUID(), cmra_obj->PositionChannel(), cmra_obj->TargetChannel() );
		pDriver->SetName("Framed Medium-Long");
		pDriver->SetInitialTime();

		// Attach driver to the appropriate channels
		tmlnChannel& channelT = cmra_obj->TargetChannel();
		tmlnChannel& channelP = cmra_obj->PositionChannel();
		channelT.AddDriver( pDriver );
		channelP.AddDriver( pDriver );
		return pDriver;
	}
	else if (!::_stricmp(i_DriverName, CAMDRIVERNAME_FRAMEDOVERHEAD))
	{
		cmraDriverFramedOverhead * pDriver = new cmraDriverFramedOverhead( cmra_obj->GetName().GetUID(), cmra_obj->PositionChannel(), cmra_obj->TargetChannel() );
		pDriver->SetName("Framed Overhead");
		pDriver->SetInitialTime();

		// Attach driver to the appropriate channels
		tmlnChannel& channelT = cmra_obj->TargetChannel();
		tmlnChannel& channelP = cmra_obj->PositionChannel();
		channelT.AddDriver( pDriver );
		channelP.AddDriver( pDriver );
		return pDriver;
	}

	return NULL;
}

//--------------------------------------------------------------------
// Create static key driver for the given channel
//--------------------------------------------------------------------
tmlnDriver* cmraDriverCreator::CreateKeyForChannel( tmlnScriptObject* io_pObject,
												    tmlnChannel* i_pChannel)
{
	cmraScriptObject* cmra_obj = dynamic_cast<cmraScriptObject*>(io_pObject);
	if (!cmra_obj) return NULL;

	if (i_pChannel == &cmra_obj->PositionChannel())
	{
		// If there is already a driver on the target channel,
		// then we can't create a linked driver, so just
		// create a key on the position channel only.
		if (has_driver(cmra_obj->TargetChannel()))
		{
			// Position Channel
			return cmmDriverCreatorPosition::CreateKeyForChannel(
				cmra_obj->PositionChannel(), c_PositionChannelName, c_PositionChannelCode, 
				cmraObjectMgr::IconsVisible(), cmra_obj );
		}
		else
		{
			// Create a linked key on target and position channels
			return cmraDriverCreator::CreateDriverByName(CAMDRIVERNAME_KEYPOSTAR, io_pObject);
		}
	}
	else if (i_pChannel == &cmra_obj->TargetChannel())
	{
		// If there is already a driver on the position channel,
		// then we can't create a linked driver, so just
		// create a key on the target channel only.
		if (has_driver(cmra_obj->PositionChannel()))
		{
			// Target Channel
			return cmmDriverCreatorPosition::CreateKeyForChannel(
				cmra_obj->TargetChannel(), c_TargetChannelName, c_TargetChannelCode, 
				cmraObjectMgr::IconsVisible(), cmra_obj );
		}
		else
		{
			// Create a linked key on target and position channels
			return cmraDriverCreator::CreateDriverByName(CAMDRIVERNAME_KEYPOSTAR, io_pObject);
		}
	}
	else if (i_pChannel == &cmra_obj->TiltChannel())
	{
		// Tilt Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			cmra_obj->TiltChannel(), c_TiltChannelName, c_TiltChannelCode, 
			cmraObjectMgr::IconsVisible() );
	}
	else if (i_pChannel == &cmra_obj->FOVChannel())
	{
		// FOV Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			cmra_obj->FOVChannel(), c_FOVChannelName, c_FOVChannelCode, 
			cmraObjectMgr::IconsVisible() );
	}
	else if (i_pChannel == &cmra_obj->OrthoWidthChannel())
	{
		// OrthoWidth Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			cmra_obj->OrthoWidthChannel(), c_OrthoWidthChannelName, c_OrthoWidthChannelCode, 
			cmraObjectMgr::IconsVisible() );
	}
	else if (i_pChannel == &cmra_obj->NearClipChannel())
	{
		// NearClip Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			cmra_obj->NearClipChannel(), c_NearClipChannelName, c_NearClipChannelCode, 
			cmraObjectMgr::IconsVisible() );
	}
	else if (i_pChannel == &cmra_obj->FarClipChannel())
	{
		// FarClip Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			cmra_obj->FarClipChannel(), c_FarClipChannelName, c_FarClipChannelCode, 
			cmraObjectMgr::IconsVisible() );
	}
	else if (i_pChannel == &cmra_obj->DOFMaxFarBlurChannel())
	{
		// DOFMaxFarBlur Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			cmra_obj->DOFMaxFarBlurChannel(), c_DOFMaxFarBlurChannelName, c_DOFMaxFarBlurChannelCode, 
			cmraObjectMgr::IconsVisible() );
	}
	else if (i_pChannel == &cmra_obj->HDRMiddleGrayChannel())
	{
		// HDRMiddleGray Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			cmra_obj->HDRMiddleGrayChannel(), c_HDRMiddleGrayChannelName, c_HDRMiddleGrayChannelCode, 
			cmraObjectMgr::IconsVisible() );
	}
	else if (i_pChannel == &cmra_obj->HDRBloomScaleChannel())
	{
		// HDRBloomScale Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			cmra_obj->HDRBloomScaleChannel(), c_HDRBloomScaleChannelName, c_HDRBloomScaleChannelCode, 
			cmraObjectMgr::IconsVisible() );
	}
	else if (i_pChannel == &cmra_obj->HDRStarScaleChannel())
	{
		// HDRStarScale Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			cmra_obj->HDRStarScaleChannel(), c_HDRStarScaleChannelName, c_HDRStarScaleChannelCode, 
			cmraObjectMgr::IconsVisible() );
	}
	else if (i_pChannel == &cmra_obj->HDRBrightPassThreshChannel())
	{
		// HDRBrightPassThresh Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			cmra_obj->HDRBrightPassThreshChannel(), c_HDRBrightPassThreshChannelName, c_HDRBrightPassThreshChannelCode, 
			cmraObjectMgr::IconsVisible() );
	}
	else if (i_pChannel == &cmra_obj->HDRBrightPassOffsetChannel())
	{
		// HDRBrightPassOffset Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			cmra_obj->HDRBrightPassOffsetChannel(), c_HDRBrightPassOffsetChannelName, c_HDRBrightPassOffsetChannelCode, 
			cmraObjectMgr::IconsVisible() );
	}
	else if (i_pChannel == &cmra_obj->HDRWhiteCutoffChannel())
	{
		// HDRWhiteCutoff Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			cmra_obj->HDRWhiteCutoffChannel(), c_HDRWhiteCutoffChannelName, c_HDRWhiteCutoffChannelCode, 
			cmraObjectMgr::IconsVisible() );
	}
	else if (i_pChannel == &cmra_obj->HDRSceneLuminanceChannel())
	{
		// HDRSceneLuminance Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			cmra_obj->HDRSceneLuminanceChannel(), c_HDRSceneLuminanceChannelName, c_HDRSceneLuminanceChannelCode, 
			cmraObjectMgr::IconsVisible() );
	}

	
	return NULL;
}

//--------------------------------------------------------------------
// Create driver for this object based on the the info structure
//--------------------------------------------------------------------
tmlnDriver* cmraDriverCreator::CreateDriverFromInfo( tmlnScriptObject* io_pObject,
													 const tmlnDriverInfo& i_Info)
{
	cmraScriptObject* cmra_obj = dynamic_cast<cmraScriptObject*>(io_pObject);
	if (!cmra_obj) return NULL;

	chDefs::Name name = i_Info.GetBaseChunkName();
	if (name == cmraDriverTargetParser::GetChunkName())
	{
		const cmraDriverTargetInfo& driver_info = dynamic_cast<const cmraDriverTargetInfo&>(i_Info);
		cmraDriverTarget * pDriver = new cmraDriverTarget( cmra_obj->TargetChannel() );
		pDriver->SetDriverInfo(driver_info);

		// Attach driver to the appropriate channels
		tmlnChannel& channel = cmra_obj->TargetChannel();
		channel.AddDriver( pDriver );
		return pDriver;
	}
	else if (name == cmraDriverKeyPositionAndTargetParser::GetChunkName())
	{
		const cmraDriverKeyPositionAndTargetInfo& driver_info = dynamic_cast<const cmraDriverKeyPositionAndTargetInfo&>(i_Info);
		cmraDriverKeyPositionAndTarget * pDriver = new cmraDriverKeyPositionAndTarget( cmra_obj->PositionChannel(), cmra_obj->TargetChannel() );
		pDriver->SetDriverInfo(driver_info);

		// Attach driver to the appropriate channels
		tmlnChannel& channelT = cmra_obj->TargetChannel();
		tmlnChannel& channelP = cmra_obj->PositionChannel();
		channelT.AddDriver( pDriver );
		channelP.AddDriver( pDriver );
		return pDriver;
	}
	else if (name == cmraDriverKeyPositionParser::GetChunkName())
	{
		const cmraDriverKeyPositionInfo& driver_info = dynamic_cast<const cmraDriverKeyPositionInfo&>(i_Info);
		cmraDriverKeyPosition * pDriver = new cmraDriverKeyPosition( cmra_obj->PositionChannel() );
		pDriver->SetDriverInfo(driver_info);

		// Attach driver to the appropriate channels
		tmlnChannel& channelP = cmra_obj->PositionChannel();
		channelP.AddDriver( pDriver );
		return pDriver;
	}
	else if (name == cmraDriverKeyTargetParser::GetChunkName())
	{
		const cmraDriverKeyTargetInfo& driver_info = dynamic_cast<const cmraDriverKeyTargetInfo&>(i_Info);
		cmraDriverKeyTarget * pDriver = new cmraDriverKeyTarget( cmra_obj->TargetChannel() );
		pDriver->SetDriverInfo(driver_info);

		// Attach driver to the appropriate channels
		tmlnChannel& channelT = cmra_obj->TargetChannel();
		channelT.AddDriver( pDriver );
		return pDriver;
	}
	else if (name == cmraDriverMayaScriptParser::GetChunkName())
	{
		const cmraDriverMayaScriptInfo& driver_info = dynamic_cast<const cmraDriverMayaScriptInfo&>(i_Info);
		cmraDriverMayaScript * pDriver = new cmraDriverMayaScript( cmra_obj->PositionChannel(), 
			cmra_obj->TargetChannel(), cmra_obj->FOVChannel(), cmra_obj->TiltChannel() );
		pDriver->SetDriverInfo(driver_info);

		// Attach driver to the appropriate channels
		tmlnChannel& channelT = cmra_obj->TargetChannel();
		tmlnChannel& channelP = cmra_obj->PositionChannel();
		tmlnChannel& channelF = cmra_obj->FOVChannel();
		tmlnChannel& channelU = cmra_obj->TiltChannel();
		channelT.AddDriver( pDriver );
		channelP.AddDriver( pDriver );
		channelF.AddDriver( pDriver );
		channelU.AddDriver( pDriver );
		return pDriver;
	}
	else if (name == cmraDriverCaptureParser::GetChunkName())
	{
		const cmraDriverCaptureInfo& driver_info = dynamic_cast<const cmraDriverCaptureInfo&>(i_Info);
		cmraDriverCapture * pDriver = new cmraDriverCapture( cmra_obj->CaptureChannel() );
		pDriver->SetDriverInfo(driver_info);

		// Attach driver to the appropriate channels
		tmlnChannel& channel = cmra_obj->CaptureChannel();
		channel.AddDriver( pDriver );
		return pDriver;
	}
	else if (name == cmraDriverCircleParser::GetChunkName())
	{
		const cmraDriverCircleInfo& driver_info = dynamic_cast<const cmraDriverCircleInfo&>(i_Info);
		cmraDriverCircle * pDriver = new cmraDriverCircle( cmra_obj->PositionChannel(), cmra_obj->TargetChannel() );
		pDriver->SetDriverInfo(driver_info);

		// Attach driver to the appropriate channels
		tmlnChannel& channel = cmra_obj->PositionChannel();
		channel.AddDriver( pDriver );
		return pDriver;
	}
	else if (name == cmraDriverFollowParser::GetChunkName())
	{
		const cmraDriverFollowInfo& driver_info = dynamic_cast<const cmraDriverFollowInfo&>(i_Info);
		cmraDriverFollow * pDriver = new cmraDriverFollow( cmra_obj->PositionChannel(), cmra_obj->TargetChannel() );
		pDriver->SetDriverInfo(driver_info);

		// Attach driver to the appropriate channels
		tmlnChannel& channel = cmra_obj->PositionChannel();
		channel.AddDriver( pDriver );
		return pDriver;
	}
	else if (name == c_CFAT)
	{
		const cmraDriverFocusAttachInfo& driver_info = dynamic_cast<const cmraDriverFocusAttachInfo&>(i_Info);
		cmraDriverFocusAttach* pDriver = new cmraDriverFocusAttach( cmra_obj->GetName().GetUID(),
			cmra_obj->NearFocusDistanceChannel(), 
			cmra_obj->FarFocusDistanceChannel(),
			cmra_obj->NearBlurDistanceChannel(),
			cmra_obj->FarBlurDistanceChannel(),
			c_CFAT );
		pDriver->SetDriverInfo(driver_info);

		// Attach driver to the appropriate channels
		tmlnChannel& channel = cmra_obj->NearFocusDistanceChannel();
		tmlnChannel& channel2 = cmra_obj->FarFocusDistanceChannel();
		tmlnChannel& channel3 = cmra_obj->NearBlurDistanceChannel();
		tmlnChannel& channel4 = cmra_obj->FarBlurDistanceChannel();
		channel.AddDriver( pDriver );
		channel2.AddDriver( pDriver );
		channel3.AddDriver( pDriver );
		channel4.AddDriver( pDriver );

		return pDriver;
	}
	else if (name == c_CPSP)	// Camera Position Spline
	{
		// Old parser chunk name, convert to new one
		name = cmmDriverCreatorPosition::GetSplineChunkName(c_PositionChannelCode);
	}
	else if (name == c_CTSP) // Camera Target Spline
	{
		// Old parser chunk name, convert to new one
		name = cmmDriverCreatorPosition::GetSplineChunkName(c_TargetChannelCode);
	}
	else if (name == c_CACH)	// attachment
	{
		// Old parser chunk name, convert to new one
		name = cmmDriverCreatorPosition::GetAttachChunkName(c_PositionChannelCode);
	}
	else if (name == c_CFOV)
	{
		// Old parser chunk name, convert to new one
		name = cmmDriverCreatorFloat::GetKeyChunkName(c_FOVChannelCode);
	}
	else if (name == c_CTLT)
	{
		// Old parser chunk name, convert to new one
		name = cmmDriverCreatorFloat::GetKeyChunkName(c_TiltChannelCode);
	}
	else if (name == c_CSPT)
	{
		// Old parser chunk name, convert to new one
		name = cmmDriverCreatorPosition::GetKeyChunkName(c_PositionChannelCode);
	}
	else if (name == c_CSTG)
	{
		// Old parser chunk name, convert to new one
		name = cmmDriverCreatorPosition::GetKeyChunkName(c_TargetChannelCode);
	}
	else if (name == cmraDriverFramedArcParser::GetChunkName())
	{
		const cmraDriverFramedArcInfo& driver_info = dynamic_cast<const cmraDriverFramedArcInfo&>(i_Info);
		cmraDriverFramedArc * pDriver = new cmraDriverFramedArc( cmra_obj->GetName().GetUID(), cmra_obj->PositionChannel(), cmra_obj->TargetChannel() );
		pDriver->SetDriverInfo(driver_info);

		// Attach driver to the appropriate channels
		tmlnChannel& channelT = cmra_obj->TargetChannel();
		tmlnChannel& channelP = cmra_obj->PositionChannel();
		channelT.AddDriver( pDriver );
		channelP.AddDriver( pDriver );
		return pDriver;
	}
	else if (name == cmraDriverFramedCloseUpParser::GetChunkName())
	{
		const cmraDriverFramedBaseInfo& driver_info = dynamic_cast<const cmraDriverFramedBaseInfo&>(i_Info);
		cmraDriverFramedCloseUp * pDriver = new cmraDriverFramedCloseUp( cmra_obj->GetName().GetUID(), cmra_obj->PositionChannel(), cmra_obj->TargetChannel() );
		pDriver->SetDriverInfo(driver_info);

		// Attach driver to the appropriate channels
		tmlnChannel& channelT = cmra_obj->TargetChannel();
		tmlnChannel& channelP = cmra_obj->PositionChannel();
		channelT.AddDriver( pDriver );
		channelP.AddDriver( pDriver );
		return pDriver;
	}
	else if (name == cmraDriverFramedMedLongParser::GetChunkName())
	{
		const cmraDriverFramedBaseInfo& driver_info = dynamic_cast<const cmraDriverFramedBaseInfo&>(i_Info);
		cmraDriverFramedMedLong * pDriver = new cmraDriverFramedMedLong( cmra_obj->GetName().GetUID(), cmra_obj->PositionChannel(), cmra_obj->TargetChannel() );
		pDriver->SetDriverInfo(driver_info);

		// Attach driver to the appropriate channels
		tmlnChannel& channelT = cmra_obj->TargetChannel();
		tmlnChannel& channelP = cmra_obj->PositionChannel();
		channelT.AddDriver( pDriver );
		channelP.AddDriver( pDriver );
		return pDriver;
	}
	else if (name == cmraDriverFramedOverheadParser::GetChunkName())
	{
		const cmraDriverFramedBaseInfo& driver_info = dynamic_cast<const cmraDriverFramedBaseInfo&>(i_Info);
		cmraDriverFramedOverhead * pDriver = new cmraDriverFramedOverhead( cmra_obj->GetName().GetUID(), cmra_obj->PositionChannel(), cmra_obj->TargetChannel() );
		pDriver->SetDriverInfo(driver_info);

		// Attach driver to the appropriate channels
		tmlnChannel& channelT = cmra_obj->TargetChannel();
		tmlnChannel& channelP = cmra_obj->PositionChannel();
		channelT.AddDriver( pDriver );
		channelP.AddDriver( pDriver );
		return pDriver;
	}

	// Creation of drivers per channel
	tmlnDriver* pDriver = NULL;

	// Position channel
	pDriver = cmmDriverCreatorPosition::CreateDriverFromInfo(
		cmra_obj->PositionChannel(), c_PositionChannelCode, i_Info, name, cmra_obj );
	if (pDriver != NULL) 
		return pDriver;

	// Target channel
	pDriver = cmmDriverCreatorPosition::CreateDriverFromInfo(
		cmra_obj->TargetChannel(), c_TargetChannelCode, i_Info, name, cmra_obj );
	if (pDriver != NULL) 
		return pDriver;

	// Tilt channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		cmra_obj->TiltChannel(), c_TiltChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// FOV channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		cmra_obj->FOVChannel(), c_FOVChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// OrthoWidth channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		cmra_obj->OrthoWidthChannel(), c_OrthoWidthChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// NearClip channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		cmra_obj->NearClipChannel(), c_NearClipChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// FarClip channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		cmra_obj->FarClipChannel(), c_FarClipChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// DOFMaxFarBlur channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		cmra_obj->DOFMaxFarBlurChannel(), c_DOFMaxFarBlurChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// HDRMiddleGray channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		cmra_obj->HDRMiddleGrayChannel(), c_HDRMiddleGrayChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// HDRBloomScale channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		cmra_obj->HDRBloomScaleChannel(), c_HDRBloomScaleChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// HDRStarScale channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		cmra_obj->HDRStarScaleChannel(), c_HDRStarScaleChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// HDRBrightPassThresh channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		cmra_obj->HDRBrightPassThreshChannel(), c_HDRBrightPassThreshChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// HDRBrightPassOffset channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		cmra_obj->HDRBrightPassOffsetChannel(), c_HDRBrightPassOffsetChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// HDRWhiteCutoff channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		cmra_obj->HDRWhiteCutoffChannel(), c_HDRWhiteCutoffChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	// HDRSceneLuminance channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		cmra_obj->HDRSceneLuminanceChannel(), c_HDRSceneLuminanceChannelCode, i_Info, name );
	if (pDriver != NULL) 
		return pDriver;

	return NULL;
}
