/*****************************************************************************
**  tqtMessaging.hpp
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

#ifdef QT_FINISH_PORT

namespace tqtMessaging
{
	std::set<QWidget*> l_HotKeyWindows;

	//--------------------------------------------------------------------
	// Convert just the wxWidgets key code to a string,
	//	this will not include the modifier keys.
	//--------------------------------------------------------------------
	bool ConvertKeyCodeToString(int i_KeyCode, 
								 std::string& o_HotKeyString)
	{
		std::string hot_key_str;

		bool bHotKeyOkay = true;
		if (i_KeyCode >= 'A' && i_KeyCode <= 'Z')
			hot_key_str = (char)(i_KeyCode);
		else if (i_KeyCode >= '0' && i_KeyCode <= '9')
			hot_key_str = (char)(i_KeyCode);
		else
		{
			switch(i_KeyCode)
			{   
			case WXK_BACK:
			case WXK_TAB:
			default:
				bHotKeyOkay = false;
				break;
			case WXK_RETURN:
				hot_key_str = "Enter";
				break;
			case WXK_ESCAPE:
				hot_key_str = "Escape";
				break;
			case WXK_SPACE:
				hot_key_str = "Space";
				break;
			case WXK_DELETE:
				hot_key_str = "Del";
				break;
			case WXK_END:
				hot_key_str = "End";
				break;
			case WXK_HOME:
				hot_key_str = "Home";
				break;
			case WXK_LEFT:
				hot_key_str = "Left";
				break;
			case WXK_UP:
				hot_key_str = "Up";
				break;
			case WXK_RIGHT:
				hot_key_str = "Right";
				break;
			case WXK_DOWN:
				hot_key_str = "Down";
				break;

			case WXK_F1:
				hot_key_str = "F1";
				break;
			case WXK_F2:
				hot_key_str = "F2";
				break;
			case WXK_F3:
				hot_key_str = "F3";
				break;
			case WXK_F4:
				hot_key_str = "F4";
				break;
			case WXK_F5:
				hot_key_str = "F5";
				break;
			case WXK_F6:
				hot_key_str = "F6";
				break;
			case WXK_F7:
				hot_key_str = "F7";
				break;
			case WXK_F8:
				hot_key_str = "F8";
				break;
			case WXK_F9:
				hot_key_str = "F9";
				break;
			case WXK_F10:
				hot_key_str = "F10";
				break;
			case WXK_F11:
				hot_key_str = "F11";
				break;
			case WXK_F12:
				hot_key_str = "F12";
				break;

			case WXK_NUMPAD0:
				hot_key_str = "NumPad0";
				break;
			case WXK_NUMPAD1:
				hot_key_str = "NumPad1";
				break;
			case WXK_NUMPAD2:
				hot_key_str = "NumPad2";
				break;
			case WXK_NUMPAD3:
				hot_key_str = "NumPad3";
				break;
			case WXK_NUMPAD4:
				hot_key_str = "NumPad4";
				break;
			case WXK_NUMPAD5:
				hot_key_str = "NumPad5";
				break;
			case WXK_NUMPAD6:
				hot_key_str = "NumPad6";
				break;
			case WXK_NUMPAD7:
				hot_key_str = "NumPad7";
				break;
			case WXK_NUMPAD8:
				hot_key_str = "NumPad8";
				break;
			case WXK_NUMPAD9:
				hot_key_str = "NumPad9";
				break;

			case WXK_MULTIPLY:
				hot_key_str = "Multiply";
				break;
			case WXK_ADD:
				hot_key_str = "Add";
				break;
			case WXK_SEPARATOR:
				hot_key_str = "Separator";
				break;
			case WXK_SUBTRACT:
				hot_key_str = "Subtract";
				break;
			case WXK_DECIMAL:
				hot_key_str = "Decimal";
				break;
			case WXK_DIVIDE:
				hot_key_str = "Divide";
				break;
			
			case WXK_NUMLOCK:
				hot_key_str = "NumLock";
				break;
			case WXK_SCROLL:
				hot_key_str = "Scroll";
				break;
			case WXK_PAGEUP:
				hot_key_str = "PageUp";
				break;
			case WXK_PAGEDOWN:
				hot_key_str = "PageDown";
				break;

			// The capitalization really is messed up here on purpose.
			// This is based on the .NET strings for these keys.
			// After we switch to wxWidgets completely, maybe we can
			// change these?
			case ',':
				hot_key_str = "Oemcomma";
				break;
			case '.':
				hot_key_str = "OemPeriod";
				break;
			case '+':
				hot_key_str = "Oemplus";
				break;
			case '-':
				hot_key_str = "OemMinus";
				break;
			case '?':
				hot_key_str = "OemQuestion";
				break;
			case ';':
				hot_key_str = "OemSemicolon";
				break;
			case '~':
				hot_key_str = "Oemtilde";
				break;
			case '[':
				hot_key_str = "OemOpenBrackets";	// not sure of this one, '{'?
				break;
			case ']':
				hot_key_str = "OemCloseBrackets"; // not sure of this one, '}'?
				break;

			case '\\':
				hot_key_str = "OemBackslash"; 
				break;
			case '\"':
				hot_key_str = "OemQuotes"; // not sure of this one, '\''?
				break;
				
			}
		}

		if (bHotKeyOkay)
		{
			//DBG_LOG("Hot Key: " << hot_key_str.c_str());
			o_HotKeyString = hot_key_str;
		}
		return bHotKeyOkay;
	}

	//--------------------------------------------------------------------
	// Convert a key event to a string representing a hot key
	//--------------------------------------------------------------------
	bool ConvertKeyEventToString(wxKeyEvent& i_Event, 
								 std::string& o_HotKeyString)
	{
		std::string mod_key_str;

		if (i_Event.ControlDown())
			mod_key_str += "Ctrl+";
		if (i_Event.AltDown())
			mod_key_str += "Alt+";
		if (i_Event.ShiftDown())
			mod_key_str += "Shift+";

		int key_code = i_Event.GetKeyCode();
		std::string key_codestr;
		if (ConvertKeyCodeToString(key_code, key_codestr))
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
	bool ProcessKeyEvent(wxKeyEvent& i_Event)
	{	
		// Handle hot keys here
		std::string hot_key_str;
		bool bHandled = false;
		if (ConvertKeyEventToString(i_Event, hot_key_str))
		{
			try
			{
				//DBG_LOG("Hot Key: " << hot_key_str.c_str());
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
		if (!bHandled)
			i_Event.Skip();

		return bHandled;
	}

	//--------------------------------------------------------------------
	// Add a QWidget to list of windows that want to handle 
	//	hot key events.
	//--------------------------------------------------------------------
	void WindowWantsHotKeys(QWidget* i_pWindow)
	{
		l_HotKeyWindows.insert(i_pWindow);
	}

	//--------------------------------------------------------------------
	// Return true if window wants to receive hot keys.
	//--------------------------------------------------------------------
	bool WantsHotKeys(QWidget* i_pWindow)
	{
		return (l_HotKeyWindows.find(i_pWindow) != l_HotKeyWindows.end());
	}

	//--------------------------------------------------------------------
	// Return true if there are any windows that want to process hot keys.
	//--------------------------------------------------------------------
	bool ShouldProcessHotKeys()
	{
		return (!l_HotKeyWindows.empty());
	}
}

#endif // USE_QT

