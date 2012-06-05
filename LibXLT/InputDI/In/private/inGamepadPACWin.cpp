/****************************************************************************\
**  inGamepadPACWin.cpp
**
**      see .hpp
**
**		NOTICE!!!!!!!!
**		Be sure to include both dinput.lib and dxguid.lib in any projects using
**		this package or you will have link problems
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "InputDI/in/private/inGamepadPACWin.hpp"

#include "Core/app/private/appApplicationPACWin.hpp"
#include "Core/env/envInitX.hpp"
#include "Core/ma/maConstants.hpp"
#include "Core/ma/maFunctions.hpp"
#include "InputDI/in/private/inDirectInputWin.hpp"

#include <math.h>


//============================================================================
//============================================================================
namespace
{
	const int lc_NumSticks = 5;
	const int lc_NumButtons = 128;
}


//------------------------------------------------------------------------
//	Constructor i_Ranges set the
//	range of values for the axes, so i_Range = 1000 sets a range of
//	-1000 to 1000. i_DeadZone is a percentage representing how far you
//	can press the gamepad before values start being counted. i_Saturation
//	is a percentage representing when to start counting the value returned
//	as at the full range. The ranges returned are unaffected by dead zone
//	and saturation. If deadzone plus saturation is greater than 99%, the
//	axes will be unable to return data. For this reason, deadzone will be 
//	truncated to the range 0-49% and saturation to 51-100%
//------------------------------------------------------------------------
inGamepadPAC::inGamepadPAC(int i_Range, float i_DeadZone, float i_Saturation, LPCDIDEVICEINSTANCE i_DeviceInstance)
{
	m_Range = i_Range;

	HRESULT hresult;

	hresult = inDirectInputWin::g_lpInput->CreateDevice(	i_DeviceInstance->guidInstance, 
															&m_lpDevice, 
															NULL);
	if (DI_OK != hresult)
	{
		throw envInitX("in");
	}

	hresult = m_lpDevice->SetDataFormat(&c_dfDIJoystick2);
	if (DI_OK != hresult)
	{
		throw envInitX("in");
	}

	//set range
	DIPROPRANGE diprg;
	diprg.diph.dwSize = sizeof(diprg);
	diprg.diph.dwHeaderSize = sizeof(diprg.diph);
	diprg.diph.dwObj = 0;
	diprg.diph.dwHow = DIPH_DEVICE;
	diprg.lMin = -i_Range;
	diprg.lMax = i_Range;
	hresult = m_lpDevice->SetProperty(DIPROP_RANGE, &diprg.diph);

	//set dead zone
	DIPROPDWORD dipdw;
	dipdw.diph.dwSize = sizeof(dipdw);
	dipdw.diph.dwHeaderSize = sizeof(dipdw.diph);
	dipdw.diph.dwObj = 0;
	dipdw.diph.dwHow = DIPH_DEVICE;

	//deadzone and saturation are passed as %, and DINPUT bases the raw values off of 10000
	//double the dead zone, since our percentage is radial, but DINPUT wants diameter
	dipdw.dwData = i_DeadZone * 2 * 10000;
	hresult = m_lpDevice->SetProperty(DIPROP_DEADZONE, &dipdw.diph);

	dipdw.dwData = i_Saturation * 10000;
	hresult = m_lpDevice->SetProperty(DIPROP_SATURATION, &dipdw.diph);

	hresult = m_lpDevice->SetCooperativeLevel(appApplicationPAC::GetHWND(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	hresult = m_lpDevice->Acquire();
}

//------------------------------------------------------------------------
//	Destructor
//------------------------------------------------------------------------
inGamepadPAC::~inGamepadPAC()
{
	m_lpDevice->Unacquire();
	ULONG refcount = m_lpDevice->Release();
	if (refcount != 0)
	{
		DBG_WARNING("gamepad device ref count is not 0 but " << refcount);
	}
}

//------------------------------------------------------------------------
//	Think gives the device a chance to update its state once per frame
//	i_State will be updated with the current state of the mouse
//------------------------------------------------------------------------
void inGamepadPAC::Think(	int i_NumSticks,
							int i_NumButtons,
							float o_Dir[], 
							float o_AnalogPressure[], 
							float o_Twist[], 
							int o_X[], 
							int o_Y[], 
							int o_Z[], 
							bool o_CurState[], 
							float o_ButtonPressure[])
{
	int i;
	DIJOYSTATE2 js;
	HRESULT hresult = m_lpDevice->Poll();
	//DI_NOEFFECT means the Poll() was successful, but that the gamepad does not require polling
	if (DI_OK != hresult && DI_NOEFFECT != hresult)
	{
		if (DIERR_INPUTLOST == hresult || DIERR_NOTACQUIRED == hresult)
		{
			//we try to reacquire the device here
			hresult = m_lpDevice->SetCooperativeLevel(appApplicationPAC::GetHWND(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
			hresult = m_lpDevice->Acquire();
			if (DIERR_INPUTLOST == hresult || DIERR_NOTACQUIRED == hresult)
			{
				//we'll pick up the input on the next think, more than likely an alt tab has occurred
				return;
			}
			else
			{
				hresult = m_lpDevice->Poll();
			}
		}
		else
		{
			DBG_LOG("Gamepad problem: Device->Poll failed #"<< hresult);
			return;
		}
	}

	hresult = m_lpDevice->GetDeviceState(sizeof(DIJOYSTATE2), &js);
	if (DIERR_INPUTLOST == hresult || DIERR_NOTACQUIRED == hresult)
	{
		//we try to reacquire the device here
		hresult = m_lpDevice->SetCooperativeLevel(appApplicationPAC::GetHWND(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
		hresult = m_lpDevice->Acquire();
		hresult = m_lpDevice->GetDeviceState(sizeof(DIJOYSTATE2), &js);
		if (DIERR_INPUTLOST == hresult || DIERR_NOTACQUIRED == hresult)
		{
			//we'll pick up the input on the next think, more than likely an alt tab has occurred
			return;
		}
	}

	if (DI_OK != hresult)
	{
		DBG_LOG("Gamepad problem: hresult is " << hresult);
		return;
	}

	int num_buttons = maFunctions::Lowest(int(lc_NumButtons), i_NumButtons);
	for (i = 0; i < num_buttons ; ++i)
	{
		if (js.rgbButtons[i] & 0x80)
		{
			o_CurState[i] = 1;
			o_ButtonPressure[i] = 1;
		}
		else
		{
			o_CurState[i] = 0;
			o_ButtonPressure[i] = 0;
		}
	}

	o_X[0] = js.lX;
	o_Y[0] = js.lY;
	//not allowing gamepads to support Z axis or twist on a stick, only an X and Y
	o_Z[0] = 0;
	o_Twist[0] = 0;
	//calculate dir and pressure from x and y vals
	if (0 == o_X[0] && 0 == o_Y[0])
	{
		o_Dir[0] = -1;
		o_AnalogPressure[0] = 0;
	}
	else
	{
		//we have to fix the radians so that 0 is North and it goes clockwise areound to 2pi
		float rads = atan2f(o_X[0], -o_Y[0]);
		if (0 > rads)
		{
			rads += maConstants::c_fPI_Times_2;
		}

		o_Dir[0] = rads;

		maVector2d vect(o_X[0], o_Y[0]);
		o_AnalogPressure[0] = vect.Length() / static_cast<float>(m_Range);
		if (1 < o_AnalogPressure[0])
		{
			o_AnalogPressure[0] = 1;
		}
		//DBG_LOG4("X: %d Y: %d Length: %f Pressure: %f", o_X[0], o_Y[0], vect.Length(), o_AnalogPressure[0]);
	}

	if (!i_NumSticks)
	{
		//info for the all sticks has already been entered
		return;
	}

	//now repeat the above for the second stick, using lZ and lRz
	o_X[1] = js.lRz;
	o_Y[1] = js.lZ;
	//not allowing gamepads to support Z axis or twist on a stick, only an X and Y
	o_Z[1] = 0;
	o_Twist[1] = 0;
	//calculate dir and pressure from x and y vals
	if (0 == o_X[1] && 0 == o_Y[1])
	{
		o_Dir[1] = -1;
		o_AnalogPressure[1] = 0;
	}
	else
	{
		//we have to fix the radians so that 0 is North and it goes clockwise areound to 2pi
		float rads = atan2f(o_X[1], -o_Y[1]);
		if (0 > rads)
		{
			rads += maConstants::c_fPI_Times_2;
		}

		o_Dir[1] = rads;

		maVector2d vect(o_X[1], o_Y[1]);
		o_AnalogPressure[1] = vect.Length() / static_cast<float>(m_Range);
		if (1 < o_AnalogPressure[1])
		{
			o_AnalogPressure[1] = 1;
		}
		//DBG_LOG4("X: %d Y: %d Length: %f Pressure: %f", o_X[1], o_Y[1], vect.Length(), o_AnalogPressure[1]);
	}
}
