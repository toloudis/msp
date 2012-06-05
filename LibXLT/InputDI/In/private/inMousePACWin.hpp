/****************************************************************************\
**  inMousePACWin.hpp
**
**      inMousePACWin.hpp defines the PAC component of inMouse.
**
**		NOTICE!!!!!!!!
**		Be sure to include both dinput.lib and dxguid.lib in any projects using
**		this package or you will have link problems
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef IN_MOUSEPACWIN_HPP
#error inMousePACWin.hpp multiply included
#endif
#define IN_MOUSEPACWIN_HPP

#ifndef IN_DITYPES_HPP
#include "InputDI/in/private/inDITypes.hpp"
#endif



class inMousePAC
{
public:
	//========================================================================
	//	Constructor
	//========================================================================
	inMousePAC();

	//========================================================================
	//	Destructor
	//========================================================================
	~inMousePAC();

	//========================================================================
	//	Think gives the device a chance to update its state once per frame
	//	i_State will be updated with the current state of the mouse
	//========================================================================
	void Think(int& o_dX, int& o_dY, float& o_dZ, bool o_State[]);

private:

	inDIDevicePtr m_lpDevice;
};