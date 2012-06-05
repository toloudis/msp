/****************************************************************************\
**  inMousePACWin.cpp
**
**      inMousePACWin.cpp defines the PAC component of inMouse.
**
**		NOTICE!!!!!!!!
**		Be sure to include both dinput.lib and dxguid.lib in any projects using
**		this package or you will have link problems
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "InputDI/in/private/inMousePACWin.hpp"

#include "Core/app/private/appApplicationPAC.hpp"
#include "Core/env/envInitX.hpp"
#include "InputDI/in/private/inDirectInputWin.hpp"

#include <windows.h>


//------------------------------------------------------------------------
//	Constructor
//------------------------------------------------------------------------
inMousePAC::inMousePAC()
{
	HRESULT hresult;

	hresult = inDirectInputWin::g_lpInput->CreateDevice(GUID_SysMouse, 
														&m_lpDevice, 
														NULL);
	if (DI_OK != hresult)
	{
		throw envInitX("in");
	}

	hresult = m_lpDevice->SetDataFormat(&c_dfDIMouse2);
	if (DI_OK != hresult)
	{
		throw envInitX("in");
	}

	hresult = m_lpDevice->SetCooperativeLevel(appApplicationPAC::GetHWND(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

	hresult = m_lpDevice->Acquire();

}

//------------------------------------------------------------------------
//	Destructor
//------------------------------------------------------------------------
inMousePAC::~inMousePAC()
{
	m_lpDevice->Unacquire();
	ULONG refcount = m_lpDevice->Release();
	if (refcount != 0)
	{
		DBG_WARNING("Mouse device ref count is not 0 but " << refcount );
	}
}

//------------------------------------------------------------------------
//	Think gives the device a chance to update its state once per frame
//	i_State will be updated with the current state of the mouse
//------------------------------------------------------------------------
void inMousePAC::Think(int& o_dX, int& o_dY, float& o_dZ, bool o_State[])
{
	HRESULT hresult;
	DIMOUSESTATE2 dims;
	memset(&dims, 0, sizeof(DIMOUSESTATE2));

	hresult = m_lpDevice->GetDeviceState(sizeof(DIMOUSESTATE2), &dims);
	if (DIERR_INPUTLOST == hresult || DIERR_NOTACQUIRED == hresult)
	{
		//we try to reacquire the device here
		hresult = m_lpDevice->SetCooperativeLevel(appApplicationPAC::GetHWND(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
		hresult = m_lpDevice->Acquire();
		hresult = m_lpDevice->GetDeviceState(sizeof(DIMOUSESTATE2), &dims);
		if (DIERR_INPUTLOST == hresult || DIERR_NOTACQUIRED == hresult)
		{
			//we'll pick up the input on the next think, more than likely an alt tab has occurred
			return;
		}
	}

	if (DI_OK != hresult)
	{
		DBG_LOG("Mouse problem: hresult is " << hresult);
		return;
	}

	o_dX = dims.lX;
	o_dY = dims.lY;
	//one "click rotation" returns 120, we want to think of this as one unit
	o_dZ = dims.lZ;
	o_dZ /= 120;

	o_State[0] = (dims.rgbButtons[0] & 0x80) ? 1 : 0;
	o_State[1] = (dims.rgbButtons[1] & 0x80) ? 1 : 0;
	o_State[2] = (dims.rgbButtons[2] & 0x80) ? 1 : 0;
	o_State[3] = (dims.rgbButtons[3] & 0x80) ? 1 : 0;
	o_State[4] = (dims.rgbButtons[4] & 0x80) ? 1 : 0;
	o_State[5] = (dims.rgbButtons[5] & 0x80) ? 1 : 0;
	o_State[6] = (dims.rgbButtons[6] & 0x80) ? 1 : 0;
	o_State[7] = (dims.rgbButtons[7] & 0x80) ? 1 : 0;
}
