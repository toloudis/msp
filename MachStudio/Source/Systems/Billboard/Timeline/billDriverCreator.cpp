/*****************************************************************************
**	billDriverCreator.cpp
**
**	Creates timeline channels for billboards
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Systems/Billboard/Timeline/billDriverCreator.hpp"

#include "Systems/Billboard/Data/billDataParser.hpp"
#include "Systems/Billboard/Object/billScriptObject.hpp"
#include "Systems/Billboard/Object/billObjectMgr.hpp"

#include "Drivers/Attach/tmlnDriverAttachParser.hpp"
#include "Drivers/Color/tmlnDriverColorParser.hpp"
#include "Drivers/Enable/tmlnDriverEnableParser.hpp"
#include "Drivers/FileName/tmlnDriverAnimatedFileNameParser.hpp"
#include "Drivers/Orientation/tmlnDriverOrientation.hpp"
#include "Drivers/Orientation/tmlnDriverOrientationInfo.hpp"
#include "Drivers/Orientation/tmlnDriverOrientationParser.hpp"
#include "Drivers/Position/tmlnDriverPositionParser.hpp"
#include "Drivers/Spline/tmlnDriverSplineParser.hpp"
#include "Drivers/Video/tmlnDriverVideo.hpp"
#include "Drivers/Video/tmlnDriverVideoParser.hpp"
#include "Support/tmln/tmlnChannelBoolean.hpp"
#include "Support/tmln/tmlnChannelColor.hpp"
#include "Support/tmln/tmlnChannelFilePath.hpp"
#include "Support/tmln/tmlnChannelFloat.hpp"
#include "Support/tmln/tmlnChannelOrientation.hpp"
#include "Support/tmln/tmlnChannelPosition.hpp"
#include "Systems/Common/DriverCreator/cmmDriverCreatorBoolean.hpp"
#include "Systems/Common/DriverCreator/cmmDriverCreatorColor.hpp"
#include "Systems/Common/DriverCreator/cmmDriverCreatorFloat.hpp"
#include "Systems/Common/DriverCreator/cmmDriverCreatorOrientation.hpp"
#include "Systems/Common/DriverCreator/cmmDriverCreatorPosition.hpp"
#include "Systems/Common/DriverCreator/cmmDriverCreatorTexture.hpp"
#include "Systems/Common/DriverCreator/cmmDriverCreatorVideo.hpp"


//============================================================================
//============================================================================
namespace
{
	//========================================================================
	//========================================================================
	const char *c_PositionChannelName = "Position";
	const char c_PositionChannelCode[2] = { 'P', 'S' };  // PoSition channel code
	const char *c_ScaleChannelName = "Scale";
	const char c_ScaleChannelCode[2] = { 'S', 'C' };  // SCale channel code
	const char *c_BrightnessChannelName = "Brightness";
	const char c_BrightnessChannelCode[2] = { 'B', 'R' };  // BRightness channel code
	const char *c_ColorChannelName = "Color";
	const char c_ColorChannelCode[2] = { 'C', 'L' };  // CoLor channel code
	const char *c_OrientationChannelName = "Orientation";
	const char c_OrientationChannelCode[2] = { 'O', 'R' };  // ORientation channel code
	const char *c_TextureChannelName = "Texture";
	const char c_TextureChannelCode[2] = { 'T', 'X' };  // TeXture channel code
	const char *c_VideoChannelName = "Video";
	const char c_VideoChannelCode[2] = { 'V', 'D' };  // ViDeo channel code
	const char *c_VisibleChannelName = "Visible";
	const char c_VisibleChannelCode[2] = { 'V', 'S' };  // ViSible channel code

	//========================================================================
	//========================================================================
	const chDefs::Name c_BPSP = chDefs::MakeName('B', 'P', 'S', 'P');  // Billboard Position SPline
	const chDefs::Name c_BATC = chDefs::MakeName('B', 'A', 'C', 'H');  // Billboard Attachment
	const chDefs::Name c_BPOS = chDefs::MakeName('B', 'P', 'O', 'S');  // Billboard Static position
	const chDefs::Name c_BCLR = chDefs::MakeName('B', 'C', 'L', 'R');  // Billboard Color
	const chDefs::Name c_BORI = chDefs::MakeName('B', 'O', 'R', 'I');  // Billboard static ORIentation
	const chDefs::Name c_ATEX = chDefs::MakeName('A', 'T', 'E', 'X');  // Animated texture driver - old format
	const chDefs::Name c_VIDF = chDefs::MakeName('V', 'I', 'D', 'F');  // Video file
	const chDefs::Name c_ENBD = chDefs::MakeName('E', 'N', 'B', 'D');  // Enabled
}

//--------------------------------------------------------------------
// Create parsers for driver types created by this creator
//--------------------------------------------------------------------
//static
void billDriverCreator::CreateParsers()
{
	chDefs::Name sys_code = billDataParser::GetChunkName();
	
	// Creation of driver parsers per channel
	cmmDriverCreatorPosition::CreateParsers(sys_code, c_PositionChannelCode);
	cmmDriverCreatorColor::CreateParsers(sys_code, c_ColorChannelCode);
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_ScaleChannelCode);
	cmmDriverCreatorFloat::CreateParsers(sys_code, c_BrightnessChannelCode);
	cmmDriverCreatorOrientation::CreateParsers(sys_code, c_OrientationChannelCode);
	cmmDriverCreatorTexture::CreateParsers(sys_code, c_TextureChannelCode);
	cmmDriverCreatorVideo::CreateParsers(sys_code, c_VideoChannelCode);
	cmmDriverCreatorBoolean::CreateParsers(sys_code, c_VisibleChannelCode);

	// System specific drivers
	//tmlnParser::AddDriverParser(sys_code, billDriverAnimatedTextureParser::GetChunkName(),
	//	new billDriverAnimatedTextureParser());

	// Because of old file formats, we still have to create these parsers here
	tmlnParser::AddDriverParser(sys_code, c_BORI, new tmlnDriverOrientationParser(c_BORI));
	tmlnParser::AddDriverParser(sys_code, c_BCLR, new tmlnDriverColorParser(c_BCLR));
	tmlnParser::AddDriverParser(sys_code, c_BPSP, new tmlnDriverSplineParser(c_BPSP));
	tmlnParser::AddDriverParser(sys_code, c_BATC, new tmlnDriverAttachParser(c_BATC));
	tmlnParser::AddDriverParser(sys_code, c_BPOS, new tmlnDriverPositionParser(c_BPOS));
	tmlnParser::AddDriverParser(sys_code, c_ATEX, new tmlnDriverAnimatedFileNameParser(c_ATEX));
	tmlnParser::AddDriverParser(sys_code, c_VIDF, new tmlnDriverVideoParser(c_VIDF));
	tmlnParser::AddDriverParser(sys_code, c_ENBD, new tmlnDriverEnableParser(c_ENBD));
}

//--------------------------------------------------------------------
// Gather the possible types of drivers that can be created for
//	for this object.  Driver names are added through the
//	DriverNameList's API
//--------------------------------------------------------------------
void billDriverCreator::GatherPossibleDrivers(const tmlnScriptObject* i_pObject,
											   tmlnDriverNameList& io_Drivers)
{
	if (dynamic_cast<const billScriptObject*>(i_pObject))
	{
		// Creation of drivers per channel
		cmmDriverCreatorPosition::GatherPossibleDrivers(c_PositionChannelName,
			io_Drivers, "Motion", this);
		cmmDriverCreatorColor::GatherPossibleDrivers(c_ColorChannelName,
			io_Drivers, "Parameters", this);
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_ScaleChannelName,
			io_Drivers, "Parameters", this);
		cmmDriverCreatorFloat::GatherPossibleDrivers(c_BrightnessChannelName,
			io_Drivers, "Parameters", this);
		cmmDriverCreatorOrientation::GatherPossibleDrivers(c_OrientationChannelName, 
			io_Drivers, "Parameters", this);
		cmmDriverCreatorTexture::GatherPossibleDrivers(c_TextureChannelName, 
			io_Drivers, "Parameters", this);
		cmmDriverCreatorVideo::GatherPossibleDrivers(c_VideoChannelName,
			io_Drivers, "Parameters", this);
		cmmDriverCreatorBoolean::GatherPossibleDrivers(c_VisibleChannelName,
			io_Drivers, "Other", this);

		// System specific drivers
		//io_Drivers.AddDriver("Animated Texture", "Texture Parameters", this);
	}
}

//--------------------------------------------------------------------
// Create driver for this object based on the name used in
//	GatherPossibleDrivers and attach it to the channels
//	on this script object.
//--------------------------------------------------------------------
tmlnDriver* billDriverCreator::CreateDriverByName( const char* i_DriverName,
													tmlnScriptObject* i_pObject )
{
	billScriptObject* bill_obj = dynamic_cast<billScriptObject*>(i_pObject);
	if (!bill_obj) return NULL;

	// Creation of drivers per channel
	tmlnDriver* pDriver = NULL;

	// Position Channel
	pDriver = cmmDriverCreatorPosition::CreateDriverByName(i_DriverName, 
		bill_obj->ChannelPosition(), c_PositionChannelName, c_PositionChannelCode, 
		billObjectMgr::IconsVisible() );
	if (pDriver != NULL)
		return pDriver;

	// Color Channel
	pDriver = cmmDriverCreatorColor::CreateDriverByName(i_DriverName, 
		bill_obj->ChannelColor(), c_ColorChannelName, c_ColorChannelCode, 
		billObjectMgr::IconsVisible() );
	if (pDriver != NULL)
		return pDriver;

	// Scale Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		bill_obj->ScaleChannel(), c_ScaleChannelName, c_ScaleChannelCode, 
		billObjectMgr::IconsVisible() );
	if (pDriver != NULL)
		return pDriver;

	// Brightness Channel
	pDriver = cmmDriverCreatorFloat::CreateDriverByName(i_DriverName, 
		bill_obj->BrightnessChannel(), c_BrightnessChannelName, c_BrightnessChannelCode, 
		billObjectMgr::IconsVisible() );
	if (pDriver != NULL)
		return pDriver;

	// Orientation Channel
	pDriver = cmmDriverCreatorOrientation::CreateDriverByName(i_DriverName, 
		bill_obj->ChannelOrientation(), c_OrientationChannelName, c_OrientationChannelCode, 
		billObjectMgr::IconsVisible() );
	if (pDriver != NULL)
		return pDriver;

	// Texture Channel
	pDriver = cmmDriverCreatorTexture::CreateDriverByName(i_DriverName, 
		bill_obj->ChannelTexture(), c_TextureChannelName, c_TextureChannelCode, 
		billObjectMgr::IconsVisible() );
	if (pDriver != NULL)
		return pDriver;

	// Video Channel
	pDriver = cmmDriverCreatorVideo::CreateDriverByName(i_DriverName, 
		bill_obj->ChannelVideo(), c_VideoChannelName, c_VideoChannelCode, 
		billObjectMgr::IconsVisible() );
	if (pDriver != NULL)
		return pDriver;
	
	// Visible Channel
	pDriver = cmmDriverCreatorBoolean::CreateDriverByName(i_DriverName, 
		bill_obj->ChannelVisible(), c_VisibleChannelName, c_VisibleChannelCode, 
		billObjectMgr::IconsVisible() );
	if (pDriver != NULL)
		return pDriver;

	if (!::_stricmp(i_DriverName, "Static Orientation"))
	{
		tmlnDriverOrientation * pDriver = new tmlnDriverOrientation( bill_obj->ChannelOrientation(), c_BORI );
		pDriver->SetName("Orientation");
		pDriver->SetInitialTime( 0.0f ); // short key
		pDriver->ShowIcons( billObjectMgr::IconsVisible() );

		// Attach driver to the appropriate channels
		tmlnChannelOrientation& channel = bill_obj->ChannelOrientation();
		channel.AddDriver( pDriver );
		return pDriver;
	}

	return NULL;
}

//--------------------------------------------------------------------
// Create static key driver for the given channel
//--------------------------------------------------------------------
tmlnDriver* billDriverCreator::CreateKeyForChannel( tmlnScriptObject* io_pObject,
													tmlnChannel* i_pChannel)
{
	billScriptObject* bill_obj = dynamic_cast<billScriptObject*>(io_pObject);
	if (!bill_obj) return NULL;

	if (i_pChannel == &bill_obj->ChannelPosition())
	{
		// Position Channel
		return cmmDriverCreatorPosition::CreateKeyForChannel(
			bill_obj->ChannelPosition(), c_PositionChannelName, c_PositionChannelCode, 
			billObjectMgr::IconsVisible() );
	}
	else if (i_pChannel == &bill_obj->ChannelColor())
	{
		// Color Channel
		return cmmDriverCreatorColor::CreateKeyForChannel(
			bill_obj->ChannelColor(), c_ColorChannelName, c_ColorChannelCode, 
			billObjectMgr::IconsVisible() );
	}
	else if (i_pChannel == &bill_obj->ScaleChannel())
	{
		// Scale Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			bill_obj->ScaleChannel(), c_ScaleChannelName, c_ScaleChannelCode, 
			billObjectMgr::IconsVisible() );
	}
	else if (i_pChannel == &bill_obj->BrightnessChannel())
	{
		// Scale Channel
		return cmmDriverCreatorFloat::CreateKeyForChannel(
			bill_obj->BrightnessChannel(), c_BrightnessChannelName, c_BrightnessChannelCode, 
			billObjectMgr::IconsVisible() );
	}
	else if (i_pChannel == &bill_obj->ChannelOrientation())
	{
		// Orientation Channel
		return cmmDriverCreatorOrientation::CreateKeyForChannel(
			bill_obj->ChannelOrientation(), c_OrientationChannelName, c_OrientationChannelCode, 
			billObjectMgr::IconsVisible() );
	}
	else if (i_pChannel == &bill_obj->ChannelTexture())
	{
		// Texture Channel
		return cmmDriverCreatorTexture::CreateKeyForChannel(
			bill_obj->ChannelTexture(), c_TextureChannelName, c_TextureChannelCode, 
			billObjectMgr::IconsVisible() );
	}
	else if (i_pChannel == &bill_obj->ChannelVisible())
	{
		// Visible Channel
		return cmmDriverCreatorBoolean::CreateKeyForChannel(
			bill_obj->ChannelVisible(), c_VisibleChannelName, c_VisibleChannelCode, 
			billObjectMgr::IconsVisible() );
	}

	return NULL;
}

//--------------------------------------------------------------------
// Create driver for this object based on the the info structure
//--------------------------------------------------------------------
tmlnDriver* billDriverCreator::CreateDriverFromInfo( tmlnScriptObject* io_pObject,
													 const tmlnDriverInfo& i_Info)
{
	billScriptObject* bill_obj = dynamic_cast<billScriptObject*>(io_pObject);
	if (!bill_obj) return NULL;

	chDefs::Name name = i_Info.GetBaseChunkName();

	if (name == c_BORI)	// static orientation
	{
		const tmlnDriverOrientationInfo& driver_info = dynamic_cast<const tmlnDriverOrientationInfo&>(i_Info);
		tmlnDriverOrientation * pDriver = new tmlnDriverOrientation( bill_obj->ChannelOrientation(), c_BORI );
		pDriver->SetDriverInfo(driver_info);

		// Attach driver to the appropriate channels
		tmlnChannel& channel = bill_obj->ChannelOrientation();
		channel.AddDriver( pDriver );
		return pDriver;
	}
	else if (name == c_BPSP)
	{
		// Old parser chunk name, convert to new one
		name = cmmDriverCreatorPosition::GetSplineChunkName(c_PositionChannelCode);
	}
	else if (name == c_BATC)
	{
		// Old parser chunk name, convert to new one
		name = cmmDriverCreatorPosition::GetAttachChunkName(c_PositionChannelCode);
	}
	else if (name == c_BPOS)
	{
		// Old parser chunk name, convert to new one
		name = cmmDriverCreatorPosition::GetKeyChunkName(c_PositionChannelCode);
	}
	else if (name == c_BCLR)
	{
		// Old parser chunk name, convert to new one
		name = cmmDriverCreatorColor::GetKeyChunkName(c_ColorChannelCode);
	}
	else if (name == c_ATEX)
	{
		// Old parser chunk name, convert to new one
		name = cmmDriverCreatorTexture::GetTextureListChunkName(c_TextureChannelCode);
	}
	else if (name == c_VIDF)
	{
		// Old parser chunk name, convert to new one
		name = cmmDriverCreatorVideo::GetKeyChunkName(c_VideoChannelCode);
	}
	else if (name == c_ENBD)
	{
		// Old parser chunk name, convert to new one
		name = cmmDriverCreatorBoolean::GetKeyChunkName(c_VisibleChannelCode);
	}

	// Creation of drivers per channel
	tmlnDriver* pDriver = NULL;

	// Position channel
	pDriver = cmmDriverCreatorPosition::CreateDriverFromInfo(
		bill_obj->ChannelPosition(), c_PositionChannelCode, i_Info, name );
	if (pDriver != NULL)
		return pDriver;

	// Color channel
	pDriver = cmmDriverCreatorColor::CreateDriverFromInfo(
		bill_obj->ChannelColor(), c_ColorChannelCode, i_Info, name );
	if (pDriver != NULL)
		return pDriver;

	// Orientation channel
	pDriver = cmmDriverCreatorOrientation::CreateDriverFromInfo(
		bill_obj->ChannelOrientation(), c_OrientationChannelCode, i_Info, name );
	if (pDriver != NULL)
		return pDriver;

	// Scale channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		bill_obj->ScaleChannel(), c_ScaleChannelCode, i_Info, name );
	if (pDriver != NULL)
		return pDriver;

	// Scale channel
	pDriver = cmmDriverCreatorFloat::CreateDriverFromInfo(
		bill_obj->BrightnessChannel(), c_BrightnessChannelCode, i_Info, name );
	if (pDriver != NULL)
		return pDriver;

	// Texture channel
	pDriver = cmmDriverCreatorTexture::CreateDriverFromInfo(
		bill_obj->ChannelTexture(), c_TextureChannelCode, i_Info, name );
	if (pDriver != NULL)
		return pDriver;

	// Video channel
	pDriver = cmmDriverCreatorVideo::CreateDriverFromInfo(
		bill_obj->ChannelVideo(), c_VideoChannelCode, i_Info, name );
	if (pDriver != NULL)
		return pDriver;

	// Visible channel
	pDriver = cmmDriverCreatorBoolean::CreateDriverFromInfo(
		bill_obj->ChannelVisible(), c_VisibleChannelCode, i_Info, name );
	if (pDriver != NULL)
		return pDriver;

	return NULL;
}
