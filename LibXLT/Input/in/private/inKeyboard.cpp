/*****************************************************************************
**  inKeyboard.cpp
**
**      inKeyboard is derived from inDevice. It provides a Think function to poll
**		for its current state, as well as appropriate accessors for retrieving
**		info about the state.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "Input/in/inKeyboard.hpp"

#include "Input/in/private/inKeyboardPAC.hpp"

//========================================================================
//	Constructor
//========================================================================
inKeyboard::inKeyboard()
{
	memset(m_PastState, 0, sizeof(m_PastState));
	memset(m_CurState, 0, sizeof(m_CurState));
	memset(m_LiveState, 0, sizeof(m_LiveState));
	memset(m_PressedSinceThink, 0, sizeof(m_PressedSinceThink));
}

//========================================================================
//	Destructor
//========================================================================
inKeyboard::~inKeyboard()
{
}

//========================================================================
//	Think gives the device a chance to update its state once per frame
//========================================================================
void inKeyboard::Think()
{
	if (!this->IsEnabled())
	{
		return;
	}

	memcpy(m_PastState, m_CurState, sizeof(m_PastState));
	for (int i = 0; i < e_KEYBUFSIZE; ++i)
	{
		m_CurState[i] = m_LiveState[i] || m_PressedSinceThink[i];
	}
	memset(m_PressedSinceThink, 0, sizeof(m_PressedSinceThink));
}

//========================================================================
//	SetKeyState records a key going down or up
//========================================================================
void inKeyboard::SetKeyState(inKeys::Keys i_Key, bool i_bDown)
{
	if (0 > i_Key || i_Key >= e_KEYBUFSIZE)
	{
		return;
	}

	m_LiveState[i_Key] = i_bDown;
	if (i_bDown)
	{
		m_PressedSinceThink[i_Key] = true;
	}
}

//========================================================================
//	SetNativeKeyState records a key going down or up, given the
//	platform's scan code for it
//========================================================================
void inKeyboard::SetNativeKeyState(unsigned int i_NativeScanCode, bool i_bDown)
{
	inKeys::Keys key;
	if (inKeyboardPAC::NativeScanCodeToKey(i_NativeScanCode, key))
	{
		this->SetKeyState(key, i_bDown);
	}
}

//========================================================================
//	ReleaseAll marks every key as up
//========================================================================
void inKeyboard::ReleaseAll()
{
	memset(m_LiveState, 0, sizeof(m_LiveState));
}

//========================================================================
//	IsPressed Pass in e_ANYBUTTON to check if any button is pressed, Not a 
//	valid parameter when requesting pressure info.
//========================================================================
bool inKeyboard::IsPressed(inKeys::Keys i_Key)
{
	if (0 > i_Key)
	{
		int i;
		for (i = 0; i < e_KEYBUFSIZE; ++i)
		{
			if (IsPressed((inKeys::Keys)i))
			{
				return true;
			}
		}

		return false;
	}
	else if (!this->IsEnabled() || i_Key >= e_KEYBUFSIZE)
	{
		return false;
	}

	return (!(m_PastState[i_Key]) && m_CurState[i_Key]);
}

bool inKeyboard::IsPressed(inKeys::Keys i_Key, float& o_AnalogPressure)
{
	if (!this->IsEnabled() || 0 > i_Key || i_Key > e_KEYBUFSIZE)
	{
		o_AnalogPressure = 0;
		return false;
	}

	if (IsPressed(i_Key))
	{
		o_AnalogPressure = 1;
		return true;
	}

	o_AnalogPressure = 0;
	return false;
}

//========================================================================
//	IsHeld Pass in e_ANYBUTTON to check if any button is pressed, Not a 
//	valid parameter when requesting pressure info.
//========================================================================
bool inKeyboard::IsHeld(inKeys::Keys i_Key)
{
	if (0 > i_Key)
	{
		int i;
		for (i = 0; i < e_KEYBUFSIZE; ++i)
		{
			if (IsHeld((inKeys::Keys)i))
			{
				return true;
			}
		}

		return false;
	}
	else if (!this->IsEnabled() || i_Key >= e_KEYBUFSIZE)
	{
		return false;
	}

	return (m_PastState[i_Key] && m_CurState[i_Key]);
}

bool inKeyboard::IsHeld(inKeys::Keys i_Key, float& o_AnalogPressure)
{
	if (!this->IsEnabled() || 0 > i_Key || i_Key > e_KEYBUFSIZE)
	{
		o_AnalogPressure = 0;
		return false;
	}

	if (IsHeld(i_Key))
	{
		o_AnalogPressure = 1;
		return true;
	}

	o_AnalogPressure = 0;
	return false;
}

//========================================================================
//	IsDown - unlike IsHeld, this is true for all frames when the
//		key is pressed, including the first frame.
//========================================================================
bool inKeyboard::IsDown(inKeys::Keys i_Key)
{
	if (0 > i_Key)
	{
		int i;
		for (i = 0; i < e_KEYBUFSIZE; ++i)
		{
			if (IsDown((inKeys::Keys)i))
			{
				return true;
			}
		}

		return false;
	}
	else if (!this->IsEnabled() || i_Key >= e_KEYBUFSIZE)
	{
		return false;
	}

	return (m_CurState[i_Key]);
}

//========================================================================
//	IsReleased Pass in e_ANYBUTTON to check if any button is pressed.
//========================================================================
bool inKeyboard::IsReleased(inKeys::Keys i_Key)
{
	if (0 > i_Key)
	{
		int i;
		for (i = 0; i < e_KEYBUFSIZE; ++i)
		{
			if (IsReleased((inKeys::Keys)i))
			{
				return true;
			}
		}

		return false;
	}
	else if (!this->IsEnabled() || i_Key >= e_KEYBUFSIZE)
	{
		return false;
	}

	return (m_PastState[i_Key] && !(m_CurState[i_Key]));
}
