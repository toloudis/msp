/****************************************************************************\
**  inKeyboardPACWin.hpp
**
**      inKeyboardPACWin.hpp defines the PAC component of inKeyboard.
**
**		NOTICE!!!!!!!!
**		Be sure to include both dinput.lib and dxguid.lib in any projects using
**		this package or you will have link problems
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef IN_KEYBOARDPACWIN_HPP
#error inKeyboardPACWin.hpp multiply included
#endif
#define IN_KEYBOARDPACWIN_HPP

#ifndef IN_DITYPES_HPP
#include "InputDI/in/private/inDITypes.hpp"
#endif


class inKeyboardPAC
{
public:
	//========================================================================
	//	Constructor
	//========================================================================
	inKeyboardPAC();

	//========================================================================
	//	Destructor
	//========================================================================
	~inKeyboardPAC();

	//========================================================================
	//	Think gives the device a chance to update its state once per frame
	//	i_State will be updated with the current state of the keyboard
	//========================================================================
	void Think(bool o_State[]);

	//========================================================================
	//	SetNonExclusiveKeyboard will initialize keyboard devices in NON_EXCLUSIVE
	//	which results in the app receiving charevents in general from Windows
	//	Generally for games this is a bad thing as it makes it easier for windows
	//	to tak focus away due to an accidental keypress (i.e. oops! i hit the WIN
	//	key). This function is here for rapid prototyping purposes since the
	//	default configuration for keyboard is now EXCLUSIVE, preventing needless
	//	key events, but as a side effect appCharEvents cease to function properly
	//	NOTE: u must call this before the inPackage::Init() to take effect
	//========================================================================
	static void SetNonExclusiveKeyboard();

	//========================================================================
	//	SetNonExclusiveKeyboard will initialize keyboard devices in EXCLUSIVE
	//	which results in the app NOT receiving charevents in general from Windows.
	//	NOTE: u must call this before the inPackage::Init() to take effect
	//========================================================================
	void inKeyboardPAC::SetExclusiveKeyboard();

private:

	inDIDevicePtr m_lpDevice;

	BYTE m_State[256];
};