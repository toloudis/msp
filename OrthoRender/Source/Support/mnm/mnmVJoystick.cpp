/*****************************************************************************
**  mnmVJoystick.cpp
**
**		mnmVJoystick contains a shared virtual joystick
**
**	Extra Large Technology
**	Copyright(C) 2000-2002 - All Rights Reserved
\****************************************************************************/

#include "Core/env/envType.hpp"	// quiets warnings
#include "Support/mnm/mnmVJoystick.hpp"

#include "InputDI/in/inDeviceMgr.hpp"
#include "InputDI/in/inPCVJoy.hpp"

#include <vector>


//========================================================================
//	Init() and CleanUp() should be called before and after the component
//	is being used.
//========================================================================
void
mnmVJoystick::Init()
{
	inPCVJoy *pVJoy = inDeviceMgr::GetPCVJoy();

	inKeyboard *keyboard = inDeviceMgr::GetKeyboard();//defaults to the first device
	std::vector<int> terawatt_vector;
	std::vector<int> actual_vector;

	//	map our keys
	terawatt_vector.push_back(e_PanLeft);
	//actual_vector.push_back(inKeys::e_A);
	actual_vector.push_back(inKeys::e_NUMPAD4);
	terawatt_vector.push_back(e_PanRight);
	//actual_vector.push_back(inKeys::e_D);
	actual_vector.push_back(inKeys::e_NUMPAD6);
	terawatt_vector.push_back(e_PanUp);
	//actual_vector.push_back(inKeys::e_E);
	actual_vector.push_back(inKeys::e_NUMPAD9);
	terawatt_vector.push_back(e_PanDown);
	//actual_vector.push_back(inKeys::e_C);
	actual_vector.push_back(inKeys::e_NUMPAD3);
	terawatt_vector.push_back(e_DollyForward);
	//actual_vector.push_back(inKeys::e_W);
	actual_vector.push_back(inKeys::e_NUMPAD8);
	terawatt_vector.push_back(e_DollyBack);
	//actual_vector.push_back(inKeys::e_S);
	actual_vector.push_back(inKeys::e_NUMPAD5);
	terawatt_vector.push_back(e_DollyBack);
	actual_vector.push_back(inKeys::e_NUMPAD2);
	terawatt_vector.push_back(e_OrbitLeft);
	actual_vector.push_back(inKeys::e_LEFT);
	terawatt_vector.push_back(e_OrbitRight);
	actual_vector.push_back(inKeys::e_RIGHT);
	terawatt_vector.push_back(e_OrbitUp);
	actual_vector.push_back(inKeys::e_UP);
	terawatt_vector.push_back(e_OrbitDown);
	actual_vector.push_back(inKeys::e_DOWN);
	terawatt_vector.push_back(e_MultiplierKey);
	actual_vector.push_back(inKeys::e_LSHIFT);
	terawatt_vector.push_back(e_MultiplierKey);
	actual_vector.push_back(inKeys::e_RSHIFT);

	pVJoy->MapButtons(keyboard, terawatt_vector, actual_vector);

	terawatt_vector.clear();
	actual_vector.clear();

	//map our mouse
	inMouse *Mouse = inDeviceMgr::GetMouse();

	if (Mouse)
	{
		pVJoy->MapStick(Mouse, 0, 0 );

		terawatt_vector.push_back(e_LeftClick);
		actual_vector.push_back(0);
		terawatt_vector.push_back(e_RightClick);
		actual_vector.push_back(1);
		terawatt_vector.push_back(e_MiddleClick);
		actual_vector.push_back(2);

		pVJoy->MapButtons(Mouse, terawatt_vector, actual_vector);
	}
}

void
mnmVJoystick::CleanUp() throw()
{

}
