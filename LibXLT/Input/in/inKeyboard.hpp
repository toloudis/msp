/*****************************************************************************
**  inKeyboard.hpp
**
**      inKeyboard is derived from inDevice. It provides a Think function to poll
**		for its current state, as well as appropriate accessors for retrieving
**		info about the state.
**
**		The keyboard has no device of its own: the GUI toolkit reports key
**		events with SetKeyState or SetNativeKeyState as they arrive, and
**		Think turns them into the per-frame state.
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

	//========================================================================
	//	SetKeyState records a key going down or up. A key that goes down
	//	and back up between two Thinks still shows as pressed for a frame.
	//========================================================================
	void SetKeyState(inKeys::Keys i_Key, bool i_bDown);

	//========================================================================
	//	SetNativeKeyState records a key going down or up, given the
	//	platform's scan code for it (on Windows, the scan code from the
	//	key message, plus 0x100 for extended keys). Keys we don't know
	//	are ignored.
	//========================================================================
	void SetNativeKeyState(unsigned int i_NativeScanCode, bool i_bDown);

	//========================================================================
	//	ReleaseAll marks every key as up. Call it when the application loses
	//	the keyboard, since the key-up events will go elsewhere.
	//========================================================================
	void ReleaseAll();

private:
	enum { e_KEYBUFSIZE = 256 };
	bool m_PastState[e_KEYBUFSIZE];
	bool m_CurState[e_KEYBUFSIZE];
	bool m_LiveState[e_KEYBUFSIZE];			// as of the latest event
	bool m_PressedSinceThink[e_KEYBUFSIZE];	// went down since the last Think
};