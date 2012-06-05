/*****************************************************************************
**	dcutDriverCreator.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/DirectorsCut/Drivers/dcutDriverCreator.hpp"

#include "Systems/DirectorsCut/Timeline/dcutChannelCamera.hpp"
#include "Systems/DirectorsCut/Data/dcutDataParser.hpp"
#include "Systems/DirectorsCut/Drivers/dcutDriverCamera.hpp"
#include "Systems/DirectorsCut/Drivers/dcutDriverCameraInfo.hpp"
#include "Systems/DirectorsCut/Drivers/dcutDriverCameraParser.hpp"
#include "Systems/DirectorsCut/Timeline/dcutChannelCapture.hpp"
#include "Systems/DirectorsCut/Drivers/dcutDriverCapture.hpp"
#include "Systems/DirectorsCut/Drivers/dcutDriverCaptureInfo.hpp"
#include "Systems/DirectorsCut/Drivers/dcutDriverCaptureParser.hpp"
#include "Systems/DirectorsCut/Object/dcutObjectMgr.hpp"
#include "Systems/DirectorsCut/Object/dcutScriptObject.hpp"

#include "Support/tmln/tmlnTimeLine.hpp"


namespace
{

	const char* CAMDRIVERNAME_CAPTURE = "Capture";
	const char* CAMDRIVERNAME_CAMERA = "Camera Cut";

	//========================================================================
	//========================================================================
}

//--------------------------------------------------------------------
// Create parsers for driver types created by this creator
//--------------------------------------------------------------------
//static
void dcutDriverCreator::CreateParsers()
{
	chDefs::Name sys_code = dcutCamerasDataParser::GetChunkName();
	tmlnParser::AddDriverParser(sys_code, dcutDriverCaptureParser::GetChunkName(), new dcutDriverCaptureParser());
	tmlnParser::AddDriverParser(sys_code, dcutDriverCameraParser::GetChunkName(), new dcutDriverCameraParser());
}

//--------------------------------------------------------------------
// Gather the possible types of drivers that can be created for
//	for this object.  Driver names are added through the
//	DriverNameList's API
//--------------------------------------------------------------------
void dcutDriverCreator::GatherPossibleDrivers(const tmlnScriptObject* i_pObject,
											   tmlnDriverNameList& io_Drivers)
{
	if (dynamic_cast<const dcutScriptObject*>(i_pObject))
	{
		io_Drivers.AddDriver(CAMDRIVERNAME_CAMERA, "Camera", this);
		io_Drivers.AddDriver(CAMDRIVERNAME_CAPTURE, "Capture", this);
	}
}


//--------------------------------------------------------------------
// Create driver for this object based on the name used in
//	GatherPossibleDrivers and attach it to the channels
//	on this script object.
//--------------------------------------------------------------------
tmlnDriver* dcutDriverCreator::CreateDriverByName( const char* i_DriverName,
													tmlnScriptObject* i_pObject )
{
	dcutScriptObject* dcut_obj = dynamic_cast<dcutScriptObject*>(i_pObject);
	if (!dcut_obj) return NULL;

	float driverlength, time;
	std::vector<tmlnDriver*> active_drivers;

	if (!::_stricmp(i_DriverName, CAMDRIVERNAME_CAMERA))
	{
		dcutDriverCamera * pDriver = new dcutDriverCamera( dcut_obj->CameraChannel() );
		pDriver->SetName(CAMDRIVERNAME_CAMERA);
		pDriver->SetInitialTime(0.0f);

		// Attach driver to the appropriate channels
		dcutChannelCamera& channel = dcut_obj->CameraChannel();

		//	check if the drivers should be overwritten or not.
		//
		time = pDriver->GetBeginTime();
		driverlength = pDriver->GetDuration();
		channel.GetActiveDrivers( time, time+driverlength, active_drivers );
		if ( !CheckIfOverwriteDrivers( dcut_obj, active_drivers ) )
		{
			delete pDriver;
			return NULL;
		}

		channel.AddDriver( pDriver );
		return pDriver;
	}	
	else if (!::_stricmp(i_DriverName, CAMDRIVERNAME_CAPTURE))
	{
		dcutDriverCapture * pDriver = new dcutDriverCapture( dcut_obj->CaptureChannel() );
		pDriver->SetName(CAMDRIVERNAME_CAPTURE);
		pDriver->SetInitialTime();

		// Attach driver to the appropriate channels
		dcutChannelCapture& channel = dcut_obj->CaptureChannel();

		//	check if the drivers should be overwritten or not.
		//
		time = pDriver->GetBeginTime();
		driverlength = pDriver->GetDuration();
		channel.GetActiveDrivers( time, time+driverlength, active_drivers );
		if ( !CheckIfOverwriteDrivers( dcut_obj, active_drivers ) )
		{
			delete pDriver;
			return NULL;
		}

		channel.AddDriver( pDriver );
		return pDriver;
	}

	return NULL;
}

//--------------------------------------------------------------------
// Create static key driver for the given channel
//--------------------------------------------------------------------
tmlnDriver* dcutDriverCreator::CreateKeyForChannel( tmlnScriptObject* io_pObject,
												    tmlnChannel* i_pChannel)
{
	// No channels to key for now
	return NULL;
}


//--------------------------------------------------------------------
// Create driver for this object based on the the info structure
//--------------------------------------------------------------------
tmlnDriver* dcutDriverCreator::CreateDriverFromInfo( tmlnScriptObject* io_pObject,
													 const tmlnDriverInfo& i_Info)
{
	dcutScriptObject* dcut_obj = dynamic_cast<dcutScriptObject*>(io_pObject);
	if (!dcut_obj) return NULL;

	chDefs::Name name = i_Info.GetBaseChunkName();

	float driverlength, time;
	std::vector<tmlnDriver*> active_drivers;

	if (name == dcutDriverCameraParser::GetChunkName())
	{
		const dcutDriverCameraInfo& driver_info = dynamic_cast<const dcutDriverCameraInfo&>(i_Info);
		dcutDriverCamera * pDriver = new dcutDriverCamera( dcut_obj->CameraChannel() );
		pDriver->SetDriverInfo(driver_info);

		// Attach driver to the appropriate channels
		tmlnChannel& channel = dcut_obj->CameraChannel();

		//	check if the drivers should be overwritten or not.
		//
		time = pDriver->GetBeginTime();
		driverlength = pDriver->GetDuration();
		channel.GetActiveDrivers( time, time+driverlength, active_drivers );
		if ( !CheckIfOverwriteDrivers( dcut_obj, active_drivers ) )
		{
			delete pDriver;
			return NULL;
		}

		channel.AddDriver( pDriver );
		return pDriver;
	}
	else if (name == dcutDriverCaptureParser::GetChunkName())
	{
		const dcutDriverCaptureInfo& driver_info = dynamic_cast<const dcutDriverCaptureInfo&>(i_Info);
		dcutDriverCapture * pDriver = new dcutDriverCapture( dcut_obj->CaptureChannel() );
		pDriver->SetDriverInfo(driver_info);

		// Attach driver to the appropriate channels
		tmlnChannel& channel = dcut_obj->CaptureChannel();

		//	check if the drivers should be overwritten or not.
		//
		time = pDriver->GetBeginTime();
		driverlength = pDriver->GetDuration();
		channel.GetActiveDrivers( time, time+driverlength, active_drivers );
		if ( !CheckIfOverwriteDrivers( dcut_obj, active_drivers ) )
		{
			delete pDriver;
			return NULL;
		}

		channel.AddDriver( pDriver );
		return pDriver;
	}

	return NULL;
}
