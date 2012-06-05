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
inKeyboard::inKeyboard() : m_pPAC(new inKeyboardPAC)
{
	memset(&m_PastState, 0, sizeof(m_PastState));
	memset(&m_CurState, 0, sizeof(m_CurState));

}

//========================================================================
//	Destructor
//========================================================================
inKeyboard::~inKeyboard()
{
	delete m_pPAC;
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
	memset(m_CurState, 0, sizeof(m_CurState));
	m_pPAC->Think(m_CurState);
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
