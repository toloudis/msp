/*****************************************************************************
**  mnmVJoystick.hpp
**
**      mnmVJoystick contains a shared virtual joystick
**
**	Extra Large Technology
**	Copyright(C) 2000-2002 - All Rights Reserved
\****************************************************************************/

#ifdef	MNM_VJOYSTICK_HPP
#error	mnmVJoystick.hpp included recursively
#endif
#define	MNM_VJOYSTICK_HPP

#ifndef IN_KEYS_HPP
#include "InputDI/in/inKeys.hpp"
#endif

class inVirtualJoystick;

namespace mnmVJoystick
{

enum
{
	e_PanLeft = inKeys::e_NUMTERAWATTKEYS,
	e_PanRight,
	e_PanUp,
	e_PanDown,
	e_DollyForward,
	e_DollyBack,
	e_OrbitLeft,
	e_OrbitRight,
	e_OrbitUp,
	e_OrbitDown,
	e_MultiplierKey,
	e_LeftClick,
	e_MiddleClick,
	e_RightClick,
	e_NUMROBOTKEYS,
};

//========================================================================
//	Init() and CleanUp() should be called before and after the component
//	is being used.
//========================================================================
void Init();
void CleanUp() throw();

}
