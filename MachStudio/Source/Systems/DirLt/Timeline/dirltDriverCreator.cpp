/*****************************************************************************
**	dirltDriverCreator.cpp
**
**	Creates timeline channels for dir lights
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "dirltDriverCreator.hpp"

#include "dirltScriptObject.hpp"

#include "tmlnChannelBoolean.hpp"
#include "tmlnChannelColor.hpp"
#include "tmlnChannelPosition.hpp"
#include "tmlnDriverAttach.hpp"
#include "tmlnDriverAttachInfo.hpp"
#include "tmlnDriverAttachParser.hpp"
#include "tmlnDriverColor.hpp"
#include "tmlnDriverColorInfo.hpp"
#include "tmlnDriverColorParser.hpp"
#include "tmlnDriverEnable.hpp"
#include "tmlnDriverEnableInfo.hpp"
#include "tmlnDriverEnableParser.hpp"
#include "tmlnDriverPosition.hpp"
#include "tmlnDriverPositionInfo.hpp"
#include "tmlnDriverPositionParser.hpp"
#include "tmlnDriverSpline.hpp"
#include "tmlnDriverSplineInfo.hpp"
#include "tmlnDriverSplineParser.hpp"
#include "splnSpline.hpp"


namespace
{
	//========================================================================
	//========================================================================
	const chDefs::Name c_LPSP = chDefs::MakeName('L', 'P', 'S', 'P');  // Light Position SPline
	const chDefs::Name c_LATC = chDefs::MakeName('L', 'A', 'C', 'H');  // Light Attachment
	const chDefs::Name c_LCLR = chDefs::MakeName('L', 'C', 'L', 'R');  // Light Color
	const chDefs::Name c_LPOS = chDefs::MakeName('L', 'P', 'O', 'S');  // Light Static position
}

//--------------------------------------------------------------------
// Create parsers for driver types created by this creator
//--------------------------------------------------------------------
//static
void dirltDriverCreator::CreateParsers()
{
	tmlnParser::AddDriverParser(c_LPSP, new tmlnDriverSplineParser(c_LPSP));
	tmlnParser::AddDriverParser(c_LATC, new tmlnDriverAttachParser(c_LATC));
	tmlnParser::AddDriverParser(tmlnDriverEnableParser::GetChunkName(),
		new tmlnDriverEnableParser());
	tmlnParser::AddDriverParser(c_LCLR, new tmlnDriverColorParser(c_LCLR));
	tmlnParser::AddDriverParser(c_LPOS, new tmlnDriverPositionParser(c_LPOS));
}

//--------------------------------------------------------------------
// Gather the possible types of drivers that can be created for
//	for this object.  Driver names are added through the
//	DriverNameList's API
//--------------------------------------------------------------------
void dirltDriverCreator::GatherPossibleDrivers(const tmlnScriptObject* i_pObject,
											   tmlnDriverNameList& io_Drivers)
{
	if (dynamic_cast<const dirltScriptObject*>(i_pObject))
	{
		io_Drivers.AddDriver("Spline", "Light Motion", this);
		io_Drivers.AddDriver("Attach", "Light Motion", this);
		io_Drivers.AddDriver("Static Position", "Light Motion", this);
		io_Drivers.AddDriver("Enabled", "Light On/Off", this);
		io_Drivers.AddDriver("Color", "Light Color", this);
	}
}

//--------------------------------------------------------------------
// Create driver for this object based on the name used in
//	GatherPossibleDrivers and attach it to the channels
//	on this script object.
//--------------------------------------------------------------------
tmlnDriver* dirltDriverCreator::CreateDriverByName( const char* i_DriverName,
													tmlnScriptObject* i_pObject )
{
	dirltScriptObject* dirlt_obj = dynamic_cast<dirltScriptObject*>(i_pObject);
	if (!dirlt_obj) return NULL;

	float driverlength, time;
	std::vector<tmlnDriver*> active_drivers;

	if (!::stricmp(i_DriverName,"Spline"))
	{
		tmlnDriverSpline * pDriver = new tmlnDriverSpline( dirlt_obj->PositionChannel(), c_LPSP, dirlt_obj );
		pDriver->SetName("Spline");
		pDriver->SetInitialTime();

		// spline
		splnSpline spline;
		spline.AppendPoint( dirlt_obj->GetPosition() );
		spline.AppendPoint( dirlt_obj->GetPosition() + maPoint3d( 0.0f, 2.5f, 0.0f ) );
		pDriver->SetSpline( spline );

		tmlnChannel& channel = dirlt_obj->PositionChannel();

		//	check if the drivers should be overwritten or not.
		//
		time = pDriver->GetBeginTime();
		driverlength = pDriver->GetDuration();
		channel.GetActiveDrivers( time, time+driverlength, active_drivers );
		if ( !CheckIfOverwriteDrivers( dirlt_obj, active_drivers ) )
		{
			delete pDriver;
			return NULL;
		}

		// Attach driver to the appropriate channels
		channel.AddDriver( pDriver );
		return pDriver;
	}
	else if (!::stricmp(i_DriverName, "Attach"))
	{
		tmlnDriverAttach * pDriver = new tmlnDriverAttach( dirlt_obj->PositionChannel(), c_LATC );
		pDriver->SetName("Attach");
		pDriver->SetInitialTime();

		tmlnChannel& channel = dirlt_obj->PositionChannel();

		//	check if the drivers should be overwritten or not.
		//
		time = pDriver->GetBeginTime();
		driverlength = pDriver->GetDuration();
		channel.GetActiveDrivers( time, time+driverlength, active_drivers );
		if ( !CheckIfOverwriteDrivers( dirlt_obj, active_drivers ) )
		{
			delete pDriver;
			return NULL;
		}

		// Attach driver to the appropriate channels
		channel.AddDriver( pDriver );
		return pDriver;
	}
	else if (!::stricmp(i_DriverName, "Enabled"))
	{
		tmlnDriverEnable * pDriver = new tmlnDriverEnable( dirlt_obj->EnabledChannel() );
		pDriver->SetName("Enable/Disable");
		pDriver->SetInitialTime( 10.0f );

		tmlnChannel& channel = dirlt_obj->EnabledChannel();

		//	check if the drivers should be overwritten or not.
		//
		time = pDriver->GetBeginTime();
		driverlength = pDriver->GetDuration();
		channel.GetActiveDrivers( time, time+driverlength, active_drivers );
		if ( !CheckIfOverwriteDrivers( dirlt_obj, active_drivers ) )
		{
			delete pDriver;
			return NULL;
		}

		// Attach driver to the appropriate channels
		channel.AddDriver( pDriver );
		return pDriver;
	}
	else if (!::stricmp(i_DriverName, "Color"))
	{
		tmlnDriverColor * pDriver = new tmlnDriverColor( dirlt_obj->ColorChannel(), c_LCLR );
		pDriver->SetName("Color");
		pDriver->SetInitialTime( 0.5f ); // short key

		tmlnChannel& channel = dirlt_obj->ColorChannel();

		//	check if the drivers should be overwritten or not.
		//
		time = pDriver->GetBeginTime();
		driverlength = pDriver->GetDuration();
		channel.GetActiveDrivers( time, time+driverlength, active_drivers );
		if ( !CheckIfOverwriteDrivers( dirlt_obj, active_drivers ) )
		{
			delete pDriver;
			return NULL;
		}

		// Attach driver to the appropriate channels
		channel.AddDriver( pDriver );
		return pDriver;
	}
	else if (!::stricmp(i_DriverName, "Static Position"))
	{
		tmlnDriverPosition * pDriver = new tmlnDriverPosition( dirlt_obj->PositionChannel(), c_LPOS, dirlt_obj );
		pDriver->SetName("Static Pos");
		pDriver->SetInitialTime( 0.5f ); // short key

		pDriver->SetValue( dirlt_obj->GetPosition() );

		tmlnChannel& channel = dirlt_obj->PositionChannel();

		//	check if the drivers should be overwritten or not.
		//
		time = pDriver->GetBeginTime();
		driverlength = pDriver->GetDuration();
		channel.GetActiveDrivers( time, time+driverlength, active_drivers );
		if ( !CheckIfOverwriteDrivers( dirlt_obj, active_drivers ) )
		{
			delete pDriver;
			return NULL;
		}

		// Attach driver to the appropriate channels
		channel.AddDriver( pDriver );
		return pDriver;
	}
}


//--------------------------------------------------------------------
// Create driver for this object based on the the info structure
//--------------------------------------------------------------------
tmlnDriver* dirltDriverCreator::CreateDriverFromInfo( tmlnScriptObject* io_pObject,
													 const tmlnDriverInfo& i_Info)
{
	dirltScriptObject* dirlt_obj = dynamic_cast<dirltScriptObject*>(io_pObject);
	if (!dirlt_obj) return NULL;

	float driverlength, time;
	std::vector<tmlnDriver*> active_drivers;

	chDefs::Name name = i_Info.GetBaseChunkName();

	if (name == c_LPSP)
	{
		const tmlnDriverSplineInfo& driver_info = dynamic_cast<const tmlnDriverSplineInfo&>(i_Info);
		tmlnDriverSpline * pDriver = new tmlnDriverSpline( dirlt_obj->PositionChannel(), c_LPSP, dirlt_obj );
		pDriver->SetDriverInfo(driver_info);

		tmlnChannel& channel = dirlt_obj->PositionChannel();

		//	check if the drivers should be overwritten or not.
		//
		time = pDriver->GetBeginTime();
		driverlength = pDriver->GetDuration();
		channel.GetActiveDrivers( time, time+driverlength, active_drivers );
		if ( !CheckIfOverwriteDrivers( dirlt_obj, active_drivers ) )
		{
			delete pDriver;
			return NULL;
		}

		// Attach driver to the appropriate channels
		channel.AddDriver( pDriver );
		return pDriver;
	}
	else if (name == c_LATC)
	{
		const tmlnDriverAttachInfo& driver_info = dynamic_cast<const tmlnDriverAttachInfo&>(i_Info);
		tmlnDriverAttach * pDriver = new tmlnDriverAttach( dirlt_obj->PositionChannel(), c_LATC );
		pDriver->SetDriverInfo(driver_info);

		tmlnChannel& channel = dirlt_obj->PositionChannel();

		//	check if the drivers should be overwritten or not.
		//
		time = pDriver->GetBeginTime();
		driverlength = pDriver->GetDuration();
		channel.GetActiveDrivers( time, time+driverlength, active_drivers );
		if ( !CheckIfOverwriteDrivers( dirlt_obj, active_drivers ) )
		{
			delete pDriver;
			return NULL;
		}

		// Attach driver to the appropriate channels
		channel.AddDriver( pDriver );
		return pDriver;
	}
	else if (name == tmlnDriverEnableParser::GetChunkName())
	{
		const tmlnDriverEnableInfo& driver_info = dynamic_cast<const tmlnDriverEnableInfo&>(i_Info);
		tmlnDriverEnable * pDriver = new tmlnDriverEnable( dirlt_obj->EnabledChannel() );
		pDriver->SetDriverInfo(driver_info);

		tmlnChannel& channel = dirlt_obj->EnabledChannel();

		//	check if the drivers should be overwritten or not.
		//
		time = pDriver->GetBeginTime();
		driverlength = pDriver->GetDuration();
		channel.GetActiveDrivers( time, time+driverlength, active_drivers );
		if ( !CheckIfOverwriteDrivers( dirlt_obj, active_drivers ) )
		{
			delete pDriver;
			return NULL;
		}

		// Attach driver to the appropriate channels
		channel.AddDriver( pDriver );
		return pDriver;
	}
	else if (name == c_LCLR)
	{
		const tmlnDriverColorInfo& driver_info = dynamic_cast<const tmlnDriverColorInfo&>(i_Info);
		tmlnDriverColor * pDriver = new tmlnDriverColor( dirlt_obj->ColorChannel(), c_LCLR );
		pDriver->SetDriverInfo(driver_info);

		tmlnChannel& channel = dirlt_obj->ColorChannel();

		//	check if the drivers should be overwritten or not.
		//
		time = pDriver->GetBeginTime();
		driverlength = pDriver->GetDuration();
		channel.GetActiveDrivers( time, time+driverlength, active_drivers );
		if ( !CheckIfOverwriteDrivers( dirlt_obj, active_drivers ) )
		{
			delete pDriver;
			return NULL;
		}

		// Attach driver to the appropriate channels
		channel.AddDriver( pDriver );
		return pDriver;
	}
	else if (name == c_LPOS)
	{
		const tmlnDriverPositionInfo& driver_info = dynamic_cast<const tmlnDriverPositionInfo&>(i_Info);
		tmlnDriverPosition * pDriver = new tmlnDriverPosition( dirlt_obj->PositionChannel(), c_LPOS, dirlt_obj );
		pDriver->SetDriverInfo(driver_info);

		tmlnChannel& channel = dirlt_obj->PositionChannel();

		//	check if the drivers should be overwritten or not.
		//
		time = pDriver->GetBeginTime();
		driverlength = pDriver->GetDuration();
		channel.GetActiveDrivers( time, time+driverlength, active_drivers );
		if ( !CheckIfOverwriteDrivers( dirlt_obj, active_drivers ) )
		{
			delete pDriver;
			return NULL;
		}

		// Attach driver to the appropriate channels
		channel.AddDriver( pDriver );
		return pDriver;
	}

	return NULL;
}
