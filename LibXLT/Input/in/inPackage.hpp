/*****************************************************************************
**  inPackage.hpp
**
**      inPackage contains the initialization functions
**	for the in package.
**
**		NOTICE!!!!!!!!
**		Be sure to include both dinput.lib and dxguid.lib in any projects using
**		this package or you will have link problems when under Windows
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef IN_PACKAGE_HPP
#error inPackage.hpp multiply included
#endif
#define IN_PACKAGE_HPP

class inPackage
{
	public:

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
		static void Init(int i_NumSticksOnVJoystick = 5);
		static void Init(int i_Range, float i_DeadZone, float i_Saturation, int i_NumSticksOnVJoystick = 5);

		//========================================================================
		//	CleanUp should be called after you are done with the in package.
		//	A good place to do this is in your main function, after you are done
		//	with other deinitialization and cleanup tasks.
		//========================================================================
		static void CleanUp() throw();
};
