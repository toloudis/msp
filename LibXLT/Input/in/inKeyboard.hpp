/*****************************************************************************
**  inKeyboard.hpp
**
**      inKeyboard is derived from inDevice. It provides a Think function to poll
**		for its current state, as well as appropriate accessors for retrieving
**		info about the state.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef IN_KEYBOARD_HPP
#error inKeyboard.hpp multiply included
#endif
#define IN_KEYBOARD_HPP

#ifndef IN_DEVICE_HPP
#include "Input/in/inDevice.hpp"
#endif

#ifndef IN_KEYS_HPP
#include "Input/in/inKeys.hpp"
#endif

class inKeyboardPAC;

class inKeyboard : public inDevice
{
public:
	enum { e_ANYBUTTON = -1 };

	//========================================================================
	//	Constructor
	//========================================================================
	inKeyboard();

	//========================================================================
	//	Destructor
	//========================================================================
	~inKeyboard();

	//========================================================================
	//	Think gives the device a chance to update its state once per frame
	//========================================================================
	void Think();

	//========================================================================
	//	IsPressed
	//========================================================================
	bool IsPressed(inKeys::Keys i_Key);
	bool IsPressed(inKeys::Keys i_Key, float& o_AnalogPressure);

	//========================================================================
	//	IsHeld
	//========================================================================
	bool IsHeld(inKeys::Keys i_Key);
	bool IsHeld(inKeys::Keys i_Key, float& o_AnalogPressure);

	//========================================================================
	//	IsDown - unlike IsHeld, this is true for all frames when the
	//		key is pressed, including the first frame.
	//========================================================================
	bool IsDown(inKeys::Keys i_Key);

	//========================================================================
	//	IsReleased
	//========================================================================
	bool IsReleased(inKeys::Keys i_Key);


private:
	enum { e_KEYBUFSIZE = 256 };
	bool m_PastState[e_KEYBUFSIZE];
	bool m_CurState[e_KEYBUFSIZE];
	inKeyboardPAC *m_pPAC;
};