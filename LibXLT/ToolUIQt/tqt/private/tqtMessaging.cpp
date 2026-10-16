/*****************************************************************************
**  tqtMessaging.cpp
**
**     Handles keypresses, converting into commands
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/tqt/tqtMessaging.hpp"

#include "Core/Dbg/dbgMsg.hpp"
#include "Core/Env/envExceptionX.hpp"
#include "Tool/cma/cmaCommandMgr.hpp"
#include "Tool/gui/guiMessageBox.hpp"
#include "Tool/gui/guiStatusBarMgr.hpp"

#include <set>

#ifdef USE_QT

namespace tqtMessaging
{
	namespace
	{
		std::set<QWidget*> l_HotKeyWindows;
	}

	//--------------------------------------------------------------------
	// Convert just the Qt key code to a string,
	//	this will not include the modifier keys.
	//--------------------------------------------------------------------
	bool ConvertKeyCodeToString(int i_KeyCode,
								bool i_bKeypad,
								std::string& o_HotKeyString)
	{
		std::string hot_key_str;
		bool bHotKeyOkay = true;

		if (i_bKeypad && i_KeyCode >= Qt::Key_0 && i_KeyCode <= Qt::Key_9)
		{
			hot_key_str = "NumPad";
			hot_key_str += (char)(i_KeyCode);
		}
		else if (i_bKeypad && i_KeyCode == Qt::Key_Asterisk)
			hot_key_str = "Multiply";
		else if (i_bKeypad && i_KeyCode == Qt::Key_Plus)
			hot_key_str = "Add";
		else if (i_bKeypad && i_KeyCode == Qt::Key_Minus)
			hot_key_str = "Subtract";
		else if (i_bKeypad && i_KeyCode == Qt::Key_Period)
			hot_key_str = "Decimal";
		else if (i_bKeypad && i_KeyCode == Qt::Key_Slash)
			hot_key_str = "Divide";
		else if (i_KeyCode >= Qt::Key_A && i_KeyCode <= Qt::Key_Z)
			hot_key_str = (char)(i_KeyCode);
		else if (i_KeyCode >= Qt::Key_0 && i_KeyCode <= Qt::Key_9)
			hot_key_str = (char)(i_KeyCode);
		else if (i_KeyCode >= Qt::Key_F1 && i_KeyCode <= Qt::Key_F12)
			hot_key_str = "F" + std::to_string(i_KeyCode - Qt::Key_F1 + 1);
		else
		{
			switch(i_KeyCode)
			{
			case Qt::Key_Backspace:
			case Qt::Key_Tab:
			default:
				bHotKeyOkay = false;
				break;
			case Qt::Key_Return:
			case Qt::Key_Enter:
				hot_key_str = "Enter";
				break;
			case Qt::Key_Escape:
				hot_key_str = "Escape";
				break;
			case Qt::Key_Space:
				hot_key_str = "Space";
				break;
			case Qt::Key_Delete:
				hot_key_str = "Del";
				break;
			case Qt::Key_End:
				hot_key_str = "End";
				break;
			case Qt::Key_Home:
				hot_key_str = "Home";
				break;
			case Qt::Key_Left:
				hot_key_str = "Left";
				break;
			case Qt::Key_Up:
				hot_key_str = "Up";
				break;
			case Qt::Key_Right:
				hot_key_str = "Right";
				break;
			case Qt::Key_Down:
				hot_key_str = "Down";
				break;
			case Qt::Key_NumLock:
				hot_key_str = "NumLock";
				break;
			case Qt::Key_ScrollLock:
				hot_key_str = "Scroll";
				break;
			case Qt::Key_PageUp:
				hot_key_str = "PageUp";
				break;
			case Qt::Key_PageDown:
				hot_key_str = "PageDown";
				break;
			// The capitalization really is messed up here on purpose.
			// This is based on the .NET strings for these keys, which
			// are what the hot key files store.
			case Qt::Key_Comma:
				hot_key_str = "Oemcomma";
				break;
			case Qt::Key_Period:
				hot_key_str = "OemPeriod";
				break;
			case Qt::Key_Plus:
			case Qt::Key_Equal:
				hot_key_str = "Oemplus";
				break;
			case Qt::Key_Minus:
			case Qt::Key_Underscore:
				hot_key_str = "OemMinus";
				break;
			case Qt::Key_Question:
			case Qt::Key_Slash:
				hot_key_str = "OemQuestion";
				break;
			case Qt::Key_Semicolon:
			case Qt::Key_Colon:
				hot_key_str = "OemSemicolon";
				break;
			case Qt::Key_AsciiTilde:
			case Qt::Key_QuoteLeft:
				hot_key_str = "Oemtilde";
				break;
			case Qt::Key_BracketLeft:
			case Qt::Key_BraceLeft:
				hot_key_str = "OemOpenBrackets";
				break;
			case Qt::Key_BracketRight:
			case Qt::Key_BraceRight:
				hot_key_str = "OemCloseBrackets";
				break;
			case Qt::Key_Backslash:
			case Qt::Key_Bar:
				hot_key_str = "OemBackslash";
				break;
			case Qt::Key_QuoteDbl:
			case Qt::Key_Apostrophe:
				hot_key_str = "OemQuotes";
				break;
			}
		}

		if (bHotKeyOkay)
		{
			o_HotKeyString = hot_key_str;
		}
		return bHotKeyOkay;
	}

	//--------------------------------------------------------------------
	// Convert a key event to a string representing a hot key
	//--------------------------------------------------------------------
	bool ConvertKeyEventToString(const QKeyEvent& i_Event,
								 std::string& o_HotKeyString)
	{
		const Qt::KeyboardModifiers modifiers = i_Event.modifiers();
		std::string mod_key_str;
		if (modifiers & Qt::ControlModifier)
			mod_key_str += "Ctrl+";
		if (modifiers & Qt::AltModifier)
			mod_key_str += "Alt+";
		if (modifiers & Qt::ShiftModifier)
			mod_key_str += "Shift+";

		std::string key_codestr;
		const bool bKeypad = (modifiers & Qt::KeypadModifier) != 0;
		if (ConvertKeyCodeToString(i_Event.key(), bKeypad, key_codestr))
		{
			o_HotKeyString = mod_key_str + key_codestr;
			return true;
		}
		return false;
	}

	//--------------------------------------------------------------------
	// Pass in a key event to check to see if it can be converted
	//	to a command
	//--------------------------------------------------------------------
	bool ProcessKeyEvent(const QKeyEvent& i_Event)
	{
		// Handle hot keys here
		std::string hot_key_str;
		bool bHandled = false;
		if (ConvertKeyEventToString(i_Event, hot_key_str))
		{
			try
			{
				guiStatusBarMgr::ClearErrorMessage();
				bHandled = cmaCommandMgr::ExecuteCommand(hot_key_str);
			}
			catch ( const envExceptionX& i_Ex)
			{
				DBG_ERROR("Problem occurred executing hotkey command: " << i_Ex.GetErrorMessage());
				guiMessageBox::Show(i_Ex.GetErrorMessage().c_str(), "Error");
			}
			catch (const std::bad_alloc&)
			{
				std::string msg = "Out of system memory, could not execute hotkey command";
				DBG_ERROR(msg);
				guiMessageBox::Show(msg.c_str(), "Error");
			}
			catch (const std::exception& i_Ex)
			{
				std::string msg = "Problem occurred executing hotkey command: " + std::string(i_Ex.what());
				DBG_ERROR(msg);
				guiMessageBox::Show(msg.c_str(), "Error");
			}
		}
		return bHandled;
	}

	//--------------------------------------------------------------------
	// Add a widget to list of widgets that want to handle
	//	hot key events.
	//--------------------------------------------------------------------
	void WindowWantsHotKeys(QWidget* i_pWindow)
	{
		if (i_pWindow && l_HotKeyWindows.insert(i_pWindow).second)
		{
			// forget the widget when it goes away so a new widget at the
			// same address doesn't inherit the setting
			QObject::connect(i_pWindow, &QObject::destroyed, [i_pWindow]()
			{
				l_HotKeyWindows.erase(i_pWindow);
			});
		}
	}

	//--------------------------------------------------------------------
	// Return true if widget wants to receive hot keys.
	//--------------------------------------------------------------------
	bool WantsHotKeys(QWidget* i_pWindow)
	{
		return (l_HotKeyWindows.find(i_pWindow) != l_HotKeyWindows.end());
	}

	//--------------------------------------------------------------------
	// Return true if there are any widgets that want to process hot keys.
	//--------------------------------------------------------------------
	bool ShouldProcessHotKeys()
	{
		return (!l_HotKeyWindows.empty());
	}
}

#endif // USE_QT
