/*****************************************************************************
**  inPackage.cpp
**
**      inPackage contains the initialization functions
**	for the inpackage.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "Input/in/inPackage.hpp"

#include "Core/app/appPackage.hpp"
#include "Core/dbg/dbgPackage.hpp"
#include "Core/env/envPackage.hpp"
#include "Input/in/inDeviceMgr.hpp"
#include "Core/it/itPackage.hpp"

namespace
{

int l_RefCount = 0;

}

//============================================================================
//	Init must be called before you use the in package.  A good place to
//	do this is in your main function, before you do anything else.
//============================================================================
void inPackage::Init(int i_NumSticksOnVJoystick/* = 5*/)
{
	// use ref count to only initialize once
	if ( l_RefCount == 0 )
	{
		// initialize packages we depend on
		//
		envPackage::Init();
		dbgPackage::Init();
		appPackage::Init();
		itPackage::Init();

		// initialize our internal stuff
		//
		inDeviceMgr::Init(i_NumSticksOnVJoystick);
	}

	l_RefCount++;
}

//========================================================================
//	use the parametrized version of Init if you want to specify different values
//	Init must be called before you use the inDeviceMgr. i_Ranges set the
//	range of values for the axes, so i_Range = 1000 sets a range of
//	-1000 to 1000. i_Ranges must be a minimum of 100 to ensure that there
//	is a range to report. i_DeadZone is a percentage representing how far you
//	can press the gamepad before values start being counted. i_Saturation
//	is a percentage representing when to start counting the value returned
//	as at the full range. The ranges returned are unaffected by dead zone
//	and saturation. If deadzone plus saturation is greater than 99%, the
//	axes will be unable to return data. For this reason, deadzone will be 
//	truncated to the range 0-49% and saturation to 51-100%
//========================================================================
void inPackage::Init(int i_Range, float i_DeadZone, float i_Saturation, int i_NumSticksOnVJoystick/* = 5*/)
{
	// use ref count to only initialize once
	if ( l_RefCount == 0 )
	{
		// initialize packages we depend on
		//
		envPackage::Init();
		dbgPackage::Init();
		appPackage::Init();
		itPackage::Init();

		// initialize our internal stuff
		//
		inDeviceMgr::Init(i_Range, i_DeadZone, i_Saturation, i_NumSticksOnVJoystick);
	}

	l_RefCount++;

}

//============================================================================
//	CleanUp should be called after you are done with the in package.
//	A good place to do this is in your main function, after you are done
//	with other deinitialization and cleanup tasks.
//============================================================================
void inPackage::CleanUp() throw()
{
	l_RefCount--;

	// use ref count to make sure we only clean up once, when everyone is done
	if ( l_RefCount == 0 )
	{
		// clean up our internal stuff, in reverse order
		//
		inDeviceMgr::CleanUp();

		// clean up packages we depend on
		//
		itPackage::CleanUp();
		appPackage::CleanUp();
		dbgPackage::CleanUp();
		envPackage::CleanUp();
	}
}
