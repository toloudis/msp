/****************************************************************************\
**  appApplicationPACXbox.cpp
**
**      itLocaleUtilPACXbox.cpp defines the appApplication class 
**	PAC for Xbox.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "appApplicationPACXbox.hpp"

#include <xtl.h>	
#include <ctype.h>	// for isleadbyte()

#include "appApplication.hpp"
#include "appCharEvent.hpp"
#include "appCharEventHandler.hpp"
#include "appEventHandlers.hpp"
#include "appFlowEvent.hpp"
#include "appMouseEvent.hpp"
#include "envSystemDataPAC.hpp"

#include "inDeviceMgr.hpp"
#include "inGamepadPAC.hpp"

#include <algorithm>

//============================================================================
//	anonymous namespace for local data/functions
//============================================================================

namespace
{

appApplicationPAC* l_App = NULL;
HINSTANCE l_HINSTANCE = NULL;
HICON l_HICON = NULL;
//bool l_MustUseANSI = false;
bool l_PostedStartEvent = false;
HWND l_HWND = NULL;
//bool l_Suspended = false;
bool l_NonClientValid = true;
//bool l_SuppressAltTab = false;
DWORD l_Style;
DWORD l_ExStyle;

// Pointer to the function that gets called in the SetCursor window event
void ( *l_pSetCursorFunction ) () = NULL;

//============================================================================
//============================================================================
void PostCharEvent(itString::CharType i_Char)
{
	appEventHandlers::appCharEventHandlerList& sorted_list = appEventHandlers::CharEventHandlers();
	appEventHandlers::appCharEventHandlerList::iterator it = sorted_list.begin();
	appEventHandlers::appCharEventHandlerList::iterator end = sorted_list.end();

	appCharEvent event(i_Char);

	while ( it != end )
	{
		if( (*it)->GetEnableCharEvents() )
		{
			(*it)->ReceiveCharEvent(event);

			if ( event.WasConsumed() )
				break;		
		}

		it++;
	}
}

//============================================================================
//============================================================================
void PostMouseDownEvent(int i_X, int i_Y, appMouseDownEvent::Button i_Button)
{
	appEventHandlers::appMouseEventHandlerList& sorted_list = appEventHandlers::MouseEventHandlers();
	appEventHandlers::appMouseEventHandlerList::iterator it = sorted_list.begin();
	appEventHandlers::appMouseEventHandlerList::iterator end = sorted_list.end();

	appMouseDownEvent event(i_X, i_Y, i_Button);

	while ( it != end )
	{
		if( (*it)->GetEnableMouseEvents() )
		{
			(*it)->ReceiveMouseDownEvent(event);

			if ( event.WasConsumed() )
				break;
		
		}

		it++;
	}
}

//============================================================================
//============================================================================
void PostMouseUpEvent(int i_X, int i_Y, appMouseUpEvent::Button i_Button)
{
	appEventHandlers::appMouseEventHandlerList& sorted_list = appEventHandlers::MouseEventHandlers();
	appEventHandlers::appMouseEventHandlerList::iterator it = sorted_list.begin();
	appEventHandlers::appMouseEventHandlerList::iterator end = sorted_list.end();

	appMouseUpEvent event(i_X, i_Y, i_Button);

	while ( it != end )
	{
		if( (*it)->GetEnableMouseEvents() )
		{
			(*it)->ReceiveMouseUpEvent(event);

			if ( event.WasConsumed() )
				break;
		
		}

		it++;
	}
}

//============================================================================
//============================================================================
void PostMouseMoveEvent(int i_X, int i_Y)
{
	appEventHandlers::appMouseEventHandlerList& sorted_list = appEventHandlers::MouseEventHandlers();
	appEventHandlers::appMouseEventHandlerList::iterator it = sorted_list.begin();
	appEventHandlers::appMouseEventHandlerList::iterator end = sorted_list.end();

	appMouseMoveEvent event(i_X, i_Y);

	while ( it != end )
	{
		if( (*it)->ReceiveMoveEvents() && (*it)->GetEnableMouseEvents() )
		{
			(*it)->ReceiveMouseMoveEvent(event);

			if ( event.WasConsumed() )
				break;
		}
			
		it++;
	}
}

//============================================================================
//============================================================================
void PostStartEvent()
{
	appEventHandlers::appFlowEventHandlerList& sorted_list = appEventHandlers::FlowEventHandlers();
	appEventHandlers::appFlowEventHandlerList::iterator it = sorted_list.begin();
	appEventHandlers::appFlowEventHandlerList::iterator end = sorted_list.end();

	appStartEvent event;

	while ( it != end )
	{
		if( (*it)->GetEnableFlowEvents() )
		{
			(*it)->ReceiveStartEvent(event);

			if ( event.WasConsumed() )
					break;
		}

		it++;
	}

	l_PostedStartEvent = true;
}

//============================================================================
//============================================================================
void PostStopEvent()
{
	appEventHandlers::appFlowEventHandlerList& sorted_list = appEventHandlers::FlowEventHandlers();
	appEventHandlers::appFlowEventHandlerList::iterator it = sorted_list.begin();
	appEventHandlers::appFlowEventHandlerList::iterator end = sorted_list.end();

	appStopEvent event;

	while ( it != end )
	{
		if( (*it)->GetEnableFlowEvents() )
		{
			(*it)->ReceiveStopEvent(event);

			if ( event.WasConsumed() )
				break;
		}

		++it;
	}
}

//============================================================================
//============================================================================
void PostSuspendEvent()
{
	// We don't want to suspend before we start.
	if( !l_PostedStartEvent ) return;

	appEventHandlers::appFlowEventHandlerList& sorted_list = appEventHandlers::FlowEventHandlers();
	appEventHandlers::appFlowEventHandlerList::iterator it = sorted_list.begin();
	appEventHandlers::appFlowEventHandlerList::iterator end = sorted_list.end();

	appSuspendEvent event;

	while ( it != end )
	{
		if( (*it)->GetEnableFlowEvents() )
		{
			(*it)->ReceiveSuspendEvent(event);

			if ( event.WasConsumed() )
				break;
		}

		it++;
	}
}

//============================================================================
//============================================================================
void PostResumeEvent()
{
	// We don't want to resume before we start!
	if( !l_PostedStartEvent ) return;

	appEventHandlers::appFlowEventHandlerList& sorted_list = appEventHandlers::FlowEventHandlers();
	appEventHandlers::appFlowEventHandlerList::iterator it = sorted_list.begin();
	appEventHandlers::appFlowEventHandlerList::iterator end = sorted_list.end();

	appResumeEvent event;

	while ( it != end )
	{
		if( (*it)->GetEnableFlowEvents() )
		{
			(*it)->ReceiveResumeEvent(event);

			if ( event.WasConsumed() )
				break;
		}

		it++;
	}
}

//============================================================================
//============================================================================
void PostQuitRequestEvent()
{
	// We don't want to quit before we start!
	if( !l_PostedStartEvent ) return;

	appEventHandlers::appFlowEventHandlerList& sorted_list = appEventHandlers::FlowEventHandlers();
	appEventHandlers::appFlowEventHandlerList::iterator it = sorted_list.begin();
	appEventHandlers::appFlowEventHandlerList::iterator end = sorted_list.end();

	appQuitRequestEvent event;

	while ( it != end )
	{
		if( (*it)->GetEnableFlowEvents() )
		{
			(*it)->ReceiveQuitRequestEvent(event);

			if ( event.WasConsumed() )
				break;
		}

		it++;
	}
}

/*
//====================================================================
//====================================================================
inline void TranslateMousePos(LPARAM lParam, int& o_X, int& o_Y, bool i_NonClient = false)
{
	o_X = int(lParam & 0x0000ffff);
	o_Y = int(lParam >> 16);

	if( !i_NonClient && l_NonClientValid )
	{
		POINT point;
		point.x = o_X;
		point.y = o_Y;
		::ClientToScreen(appApplicationPAC::GetHWND(), &point);
		o_X = point.x;
		o_Y = point.y;
	}
}
*/
}


//====================================================================
//====================================================================
appApplicationPAC::appApplicationPAC()
:	m_X(20),
	m_Y(20),
	m_Width(640),
	m_Height(480),
	m_Quit(false),
	m_Title("Terawatt Window"),
	m_HasLeadByte(false)
{
	DBG_ASSERT0(l_App == NULL, "Tried to create two simultaneous applications");
//	DBG_ASSERT0(l_HINSTANCE != NULL, "The HINSTANCE (from WinMain) was never set (with appApplicationPACWin::SetHINSTANCE)");
	l_App = this;

	// make sure that the m_Title is NULL terminated.
	m_Title += itString::CharType(0);
}

//====================================================================
//====================================================================
appApplicationPAC::~appApplicationPAC()
{
	l_App = NULL;
}

//====================================================================
//	Run() is called to start the application.  When control
//	returns from Run(), the application is finished.  The
//	appApplication* is used to call the Think() function of the
//	child appApplication.
//====================================================================
void appApplicationPAC::Run(appApplication* i_App)
{
	// Post the Start event, just before we start the regular message loop
	//
	PostStartEvent();

	//	Application game loop
	//
	while( !m_Quit )
	{
		inGamepadPAC *gp = inDeviceMgr::GetGamepad( 0 )->GetPAC();
		PostMouseMoveEvent( gp->GetCursorPosition().GetX(), gp->GetCursorPosition().GetY() );
		i_App->Think();
	}

	//	At this point, we are finishing the application, so post the stop event
	//
	PostStopEvent();
}

//====================================================================
//	Exit() is called by the client when it decides it is
//	ready to end the program.
//====================================================================
void appApplicationPAC::Exit()
{
	m_Quit = true;
}

//====================================================================
//	SetWindowSize is used to resize the application window.  This
//	function can be called from a appStartEvent handler, before the
//	window has been created.  The width and height refer to the client
//	area of the window (the part inside the title bar and border).
//====================================================================
void appApplicationPAC::SetWindowSize(int i_X, int i_Y, int i_Width, int i_Height)
{
	m_X = i_X;
	m_Y = i_Y;
	m_Width = i_Width;
	m_Height = i_Height;
/*
	if ( l_HWND )
	{
		BOOL ret_val;
		RECT rect;

		rect.left = 0;
		rect.top = 0;
		rect.right = m_Width;
		rect.bottom = m_Height;

		ret_val = ::AdjustWindowRectEx(&rect, l_Style, false, l_ExStyle);
		ret_val = ::SetWindowPos(	l_HWND, 
									NULL, 
									m_X, 
									m_Y, 
									rect.right - rect.left, 
									rect.bottom - rect.top,
									SWP_NOZORDER);
	}
*/
}

//====================================================================
//	SetWindowTitle is used to set the text that appears in the
//	window's title bar.  This function can be called from a 
//	appStartEvent handler, before the window has been created. 
//====================================================================
void appApplicationPAC::SetWindowTitle(const itString& i_Title)
{
//	DBG_ASSERT0(false, "Not implemented for Xbox!");
/*
	m_Title = i_Title;

	// make sure it's NULL terminated
	//
	m_Title += itString::CharType(0);

	if( l_HWND == NULL ) return; // no window yet

	if ( l_MustUseANSI )
	{
		char work_string[512];
		int num_chars = ::WideCharToMultiByte(	::GetACP(),		// ANSI code page
												0,			// no "lo-performance" flags
												m_Title.GetString(),
												m_Title.GetLength(),
												work_string,
												512,	// size of target buffer
												NULL,
												NULL);

		// for some reason, this function doesn't seem to terminate the 
		// work_string
		work_string[num_chars] = 0;

		::SetWindowTextA(l_HWND, work_string);
	}
	else
	{
		::SetWindowTextW(l_HWND, m_Title.GetString());
	}
*/
}

//====================================================================
//	The windows appApplicationPAC defines this Instance function
//	for use by other Windows PAC components. 
//====================================================================
appApplicationPAC* appApplicationPAC::Instance()
{
	return l_App;
}

//====================================================================
//	The HINSTANCE must be set by the client, from the value passed
//	to WinMain.
//====================================================================
void appApplicationPAC::SetHINSTANCE(HINSTANCE i_Instance)
{
	l_HINSTANCE = i_Instance;
}

//====================================================================
//	The HINSTANCE of the application is supplied here for use by
//	other Windows PAC components.
//====================================================================
HINSTANCE appApplicationPAC::GetHINSTANCE()
{
	return l_HINSTANCE;
}

//====================================================================
//	The HICON of the engine window can be created outside to be used in
//  other applications.
//====================================================================
void appApplicationPAC::SetHICON(HICON i_HICON)
{
	l_HICON = i_HICON;
}

//====================================================================
//	DisableStickyKeys disables this XP feature 
//====================================================================
void appApplicationPAC::DisableStickyKeys()
{
/*
	HRESULT ret_val;
	STICKYKEYS sticky_keys;
	memset(&sticky_keys, 0, sizeof(sticky_keys));
	sticky_keys.cbSize = sizeof(sticky_keys);

	ret_val = SystemParametersInfo(SPI_SETSTICKYKEYS, sizeof(sticky_keys), &sticky_keys, NULL);
	if (!ret_val)
	{
		ret_val = GetLastError();
	}
*/
}
/*
//====================================================================
//	This is the window procedure for the application.
//====================================================================
LRESULT appApplicationPAC::WindowProc(	HWND hWnd,      // handle to window
										UINT uMsg,      // message identifier
										WPARAM wParam,  // first message parameter
										LPARAM lParam   // second message parameter
										)
{
	bool pass_to_def_proc = true;
	LRESULT ret_val = 0;
	int x,y;

	switch( uMsg )
	{
		case WM_PAINT:
		{
//			PAINTSTRUCT ps;
//			BeginPaint(hWnd, &ps);
//			EndPaint(hWnd, &ps);
//			pass_to_def_proc = false;
//			ret_val = 0;
		}
		break;
	
        case WM_SETCURSOR:
			if ( l_pSetCursorFunction )
			{
				l_pSetCursorFunction();
			}
			pass_to_def_proc = false;
			ret_val = 1;
			
		break;

		case WM_ACTIVATEAPP:
			if( BOOL(wParam) == TRUE )
			{
				if( l_Suspended )
				{
					l_Suspended = false;
					PostResumeEvent();
				}
			}
			else
			{
				if( !l_Suspended )
				{
					l_Suspended = true;
					PostSuspendEvent();
				}
			}
		break;

		case WM_LBUTTONDOWN:
			TranslateMousePos(lParam, x, y);
			PostMouseDownEvent(x, y, appMouseDownEvent::e_Left);
		break;
		case WM_LBUTTONUP:
			TranslateMousePos(lParam, x, y);
			PostMouseUpEvent(x, y, appMouseUpEvent::e_Left);
		break;
		case WM_NCLBUTTONDOWN:
			if( l_NonClientValid )
			{
				TranslateMousePos(lParam, x, y, true);
				PostMouseDownEvent(x, y, appMouseDownEvent::e_Left);
				pass_to_def_proc = false;
			}
		break;
		case WM_NCLBUTTONUP:
			if( l_NonClientValid )
			{
				TranslateMousePos(lParam, x, y, true);
				PostMouseUpEvent(x, y, appMouseUpEvent::e_Left);
				pass_to_def_proc = false;
			}
		break;

		case WM_RBUTTONDOWN:
			TranslateMousePos(lParam, x, y);
			PostMouseDownEvent(x, y, appMouseDownEvent::e_Right);
		break;
		case WM_RBUTTONUP:
			TranslateMousePos(lParam, x, y);
			PostMouseUpEvent(x, y, appMouseUpEvent::e_Right);
		break;
		case WM_NCRBUTTONDOWN:
			if( l_NonClientValid )
			{
				TranslateMousePos(lParam, x, y, true);
				PostMouseDownEvent(x, y, appMouseDownEvent::e_Right);
				pass_to_def_proc = false;
			}
		break;
		case WM_NCRBUTTONUP:
			if( l_NonClientValid )
			{
				TranslateMousePos(lParam, x, y, true);
				PostMouseUpEvent(x, y, appMouseUpEvent::e_Right);
				pass_to_def_proc = false;
			}
		break;

		case WM_MBUTTONDOWN:
			TranslateMousePos(lParam, x, y);
			PostMouseDownEvent(x, y, appMouseDownEvent::e_Middle);
		break;
		case WM_MBUTTONUP:
			TranslateMousePos(lParam, x, y);
			PostMouseUpEvent(x, y, appMouseUpEvent::e_Middle);
		break;
		case WM_NCMBUTTONDOWN:
			if( l_NonClientValid )
			{
				TranslateMousePos(lParam, x, y, true);
				PostMouseDownEvent(x, y, appMouseDownEvent::e_Middle);
				pass_to_def_proc = false;
			}
		break;
		case WM_NCMBUTTONUP:
			if( l_NonClientValid )
			{
				TranslateMousePos(lParam, x, y, true);
				PostMouseUpEvent(x, y, appMouseUpEvent::e_Middle);
				pass_to_def_proc = false;
			}
		break;

		case WM_MOUSEMOVE:
			TranslateMousePos(lParam, x, y);
			PostMouseMoveEvent(x, y);
		break;
		case WM_NCMOUSEMOVE:
			if( l_NonClientValid )
			{
				TranslateMousePos(lParam, x, y, true);
				PostMouseMoveEvent(x, y);
				pass_to_def_proc = false;
			}
		break;

		case WM_CHAR:
		{
			if( l_MustUseANSI )
			{
				// check to see if this is a leadbyte
				char cur_byte = char(wParam);
				itString::CharType unicode_form[8];

				if( m_HasLeadByte )
				{
					// we must compound the next byte onto the multi-byte character
					char double_byte_char[2];
					double_byte_char[0] = m_LeadByte;
					double_byte_char[1] = char(wParam);

					// now translate it to Unicode and emit it
					// we are assuming here that it will only be translated to a couple of
					// unicode characters
					int num_chars = ::MultiByteToWideChar(	::GetACP(),
															MB_PRECOMPOSED,
															double_byte_char,
															2,
															unicode_form,
															8);

					int i;
					for ( i = 0 ; i < num_chars ; i++ )
					{
						// send a unicode char
						PostCharEvent(unicode_form[i]);
					}
				}
				else
				{
					if( ::isleadbyte(cur_byte) )
					{
						m_LeadByte = cur_byte;
						m_HasLeadByte = true;
					}
					else
					{
						m_HasLeadByte = false;
						char ansi_char = char(wParam);
						// Translate it to Unicode and emit it
						// we are assuming here that it will only be translated to a couple of
						// unicode characters
						int num_chars = ::MultiByteToWideChar(	::GetACP(),
																MB_PRECOMPOSED,
																&ansi_char,
																1,
																unicode_form,
																8);

						int i;
						for ( i = 0 ; i < num_chars ; i++ )
						{
							// send a unicode char
							PostCharEvent(unicode_form[i]);
						}
					}
				}
			}
			else
			{
				// Unicode
				itString::CharType unichar = itString::CharType(wParam);
				PostCharEvent(unichar);
			}
		}
		break;
		case WM_KEYDOWN:
			//this will let us change special keys like the arrow keys into our special enum appCharEvent::SpecialKeys
			switch (wParam)
			{
			case VK_F1:
				PostCharEvent(appCharEvent::e_F1);
			break;
			case VK_F2:
				PostCharEvent(appCharEvent::e_F2);
			break;
			case VK_F3:
				PostCharEvent(appCharEvent::e_F3);
			break;
			case VK_F4:
				PostCharEvent(appCharEvent::e_F4);
			break;
			case VK_F5:
				PostCharEvent(appCharEvent::e_F5);
			break;
			case VK_F6:
				PostCharEvent(appCharEvent::e_F6);
			break;
			case VK_F7:
				PostCharEvent(appCharEvent::e_F7);
			break;
			case VK_F8:
				PostCharEvent(appCharEvent::e_F8);
			break;
			case VK_F9:
				PostCharEvent(appCharEvent::e_F9);
			break;
			case VK_F10:
				PostCharEvent(appCharEvent::e_F10);
			break;
			case VK_F11:
				PostCharEvent(appCharEvent::e_F11);
			break;
			case VK_F12:
				PostCharEvent(appCharEvent::e_F12);
			break;
			case VK_PRINT:
				PostCharEvent(appCharEvent::e_PRINT);
			break;
			case VK_SCROLL:
				PostCharEvent(appCharEvent::e_SCROLL);
			break;
			case VK_PAUSE:
				PostCharEvent(appCharEvent::e_PAUSE);
			break;
			case VK_LCONTROL:
				PostCharEvent(appCharEvent::e_LCTRL);
			break;
			case VK_RCONTROL:
				PostCharEvent(appCharEvent::e_RCTRL);
			break;
			case VK_LWIN:
				PostCharEvent(appCharEvent::e_LWIN);
			break;
			case VK_LMENU:
				PostCharEvent(appCharEvent::e_LALT);
			break;
			case VK_RMENU:
				PostCharEvent(appCharEvent::e_RALT);
			break;
			case VK_RWIN:
				PostCharEvent(appCharEvent::e_RWIN);
			break;
			case VK_INSERT:
				PostCharEvent(appCharEvent::e_INSERT);
			break;
			case VK_HOME:
				PostCharEvent(appCharEvent::e_HOME);
			break;
			case VK_PRIOR:
				PostCharEvent(appCharEvent::e_PAGEUP);
			break;
			case VK_DELETE:
				PostCharEvent(appCharEvent::e_DELETE);
			break;
			case VK_END:
				PostCharEvent(appCharEvent::e_END);
			break;
			case VK_NEXT:
				PostCharEvent(appCharEvent::e_PAGEDOWN);
			break;
			case VK_UP:
				PostCharEvent(appCharEvent::e_UP);
			break;
			case VK_DOWN:
				PostCharEvent(appCharEvent::e_DOWN);
			break;
			case VK_LEFT:
				PostCharEvent(appCharEvent::e_LEFT);
			break;
			case VK_RIGHT:
				PostCharEvent(appCharEvent::e_RIGHT);
			break;
			case VK_NUMLOCK:
				PostCharEvent(appCharEvent::e_NUMLOCK);
			break;
			// The rest of these will post BOTH a WM_KEYDOWN and a WM_CHAR, 
			// we choose to handle them in WM_CHAR, not here
			//already covered in WM_Char
			//case VK_BACK:
			//	PostCharEvent(appCharEvent::e_BACKSPACE);
			//break;
			//already covered in WM_Char 
			//case VK_TAB:
			//	PostCharEvent(appCharEvent::e_TAB);
			//break;
			//already covered in WM_Char
			//case VK_RETURN:
			//	PostCharEvent(appCharEvent::e_ENTER);
			//break;
			//already covered in WM_Char, besides, there's no specific mapping for this one
			//case VK_NUMPADENTER:
			//	PostCharEvent(appCharEvent::e_NUMPADENTER);
			//break;
			//already covered in WM_Char
			//case VK_ESCAPE:
			//	PostCharEvent(appCharEvent::e_ESC);
			//break;
			}
		break;

		case WM_SYSKEYDOWN:
			if (VK_F4 == wParam && (536870912 & lParam))
			{
				//let alt-f4 thru
				pass_to_def_proc = true;
			}
			else
			{
				//	don't let windows handle this - because of the SYSMENU we make the game window with, ALT
				//will pause the game in a bad way, but we need SYSMENU for the Icon to appear in the taskbar
				//therefore we will now suppress this
				pass_to_def_proc = false;
			}
		break;

		case WM_CLOSE:
			PostQuitRequestEvent();
			//	don't let windows handle this - we'll deal with it ourselves
			pass_to_def_proc = false;
		break;
	}
	
	if( pass_to_def_proc )
	{
		if( l_MustUseANSI )
			return ::DefWindowProcA(hWnd, uMsg, wParam, lParam);
		else
			return ::DefWindowProcW(hWnd, uMsg, wParam, lParam);
	}
	else
		return ret_val;
}
*/
//====================================================================
//	IsSuspended is true if the application doesn't have focus
//====================================================================
bool appApplicationPAC::IsSuspended()
{
	return false;
}

//========================================================================
//	Don't call Init() and CleanUp() yourself; they are called 
//	by the package Init and Cleanup.
//========================================================================
void appApplicationPAC::Init()
{
}

void appApplicationPAC::CleanUp() throw()
{
}

/*
//========================================================================
//========================================================================
void appApplicationPAC::create_window()
{
	BOOL ret_val;

	//WS_CAPTION causes misbehavior on XP (minimiing when clicking the upper left and right corners
	//however, it is needed for windowed mode to function properly
	//this means that hotspots are slightly off in windowed mode on XP
	if (envSystemDataPAC::e_WindowsXP == envSystemDataPAC::GetWindowsOSType())
	{
		l_Style = WS_SYSMENU | WS_VISIBLE;
	}
	else
	{
		l_Style = WS_SYSMENU | WS_CAPTION | WS_VISIBLE;
	}

	l_ExStyle = 0;

	// Register a window class
	//
	if ( l_MustUseANSI )
	{
		WNDCLASSEXA wc;
		memset(&wc, 0, sizeof(wc));
		wc.cbSize = sizeof(wc);
		wc.style = 0;
		wc.lpfnWndProc = LocalWindowProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = l_HINSTANCE;
		wc.hIcon = l_HICON;
		wc.hCursor = NULL;
		wc.hbrBackground = NULL;
		wc.lpszMenuName = NULL;
		wc.lpszClassName = GetWindowClassName();
		wc.hIconSm = l_HICON;

		// we'll just always use the ANSI version here, and let the system
		// do the conversion of the class name on NT/2000.
		ATOM register_result = ::RegisterClassExA(&wc);
		DBG_ASSERT0(register_result != 0, "Couldn't register windows class");

		RECT rect;
		rect.left = 0;
		rect.top = 0;
		rect.right = m_Width;
		rect.bottom = m_Height;

		ret_val = ::AdjustWindowRectEx(&rect, l_Style, false, l_ExStyle);
		DBG_ASSERT0(ret_val != 0, "Couldn't calculate window rect");

		// Create a window
		//	
		char work_string[512];
		int num_chars = ::WideCharToMultiByte(	::GetACP(),		// ANSI code page
												0,			// no "lo-performance" flags
												m_Title.GetString(),
												m_Title.GetLength(),
												work_string,
												512,	// size of target buffer
												NULL,
												NULL);

		// for some reason, this function doesn't seem to terminate the 
		// work_string
		work_string[num_chars] = 0;

		l_HWND = ::CreateWindowExA(	l_ExStyle,
									GetWindowClassName(),
									work_string,
									l_Style,
									m_X,
									m_Y,
									rect.right - rect.left,
									rect.bottom - rect.top,
									NULL,
									NULL,
									l_HINSTANCE,
									NULL);
	}
	else
	{
		WNDCLASSEXW wc;
		itString class_name(GetWindowClassName());
		memset(&wc, 0, sizeof(wc));
		wc.cbSize = sizeof(wc);
		wc.style = 0;
		wc.lpfnWndProc = LocalWindowProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = l_HINSTANCE;
		wc.hIcon = l_HICON;
		wc.hCursor = NULL;
		wc.hbrBackground = NULL;
		wc.lpszMenuName = NULL;
		wc.lpszClassName = class_name.GetString();
		wc.hIconSm = l_HICON;

		// we'll just always use the ANSI version here, and let the system
		// do the conversion of the class name on NT/2000.
		ATOM register_result = ::RegisterClassExW(&wc);
		DBG_ASSERT0(register_result != 0, "Couldn't register windows class");

		RECT rect;
		rect.left = 0;
		rect.top = 0;
		rect.right = m_Width;
		rect.bottom = m_Height;

		ret_val = ::AdjustWindowRectEx(&rect, l_Style, false, l_ExStyle);
		DBG_ASSERT0(ret_val != 0, "Couldn't calculate window rect");

		// Create a window
		//	
		l_HWND = ::CreateWindowExW(	l_ExStyle,
									class_name.GetString(),
									m_Title.GetString(),
									l_Style,
									m_X,
									m_Y,
									rect.right - rect.left,
									rect.bottom - rect.top,
									NULL,
									NULL,
									l_HINSTANCE,
									NULL);
	}

	::SetForegroundWindow(l_HWND);
	::SetActiveWindow(l_HWND);
}

//========================================================================
//========================================================================
void appApplicationPAC::message_loop(appApplication* i_App)
{
	// Run message loop
	//
	if( l_MustUseANSI )
	{			
		while ( !m_Quit )
		{
			MSG msg;
			BOOL got_msg = 0;

			// if we are suspended we'll use GetMessage to not consume idle time
			// otherwise use PeekMessage so we can do idle processing.
			if( l_Suspended )
				got_msg = ::GetMessageA( &msg, l_HWND, 0, 0 );
			else
				got_msg = ::PeekMessageA( &msg, l_HWND, 0, 0, PM_REMOVE );

			if( got_msg )
			{
				::TranslateMessage(&msg);
				::DispatchMessageA(&msg);
			}
			else
			{
				// no messages, so do some idle processing
				if( !l_Suspended )
					i_App->Think();
			}
		}
	}
	else
	{
		while ( !m_Quit )
		{
			MSG msg;
			BOOL got_msg = 0;

			// if we are suspended we'll use GetMessage to not consume idle time
			// otherwise use PeekMessage so we can do idle processing.
			if( l_Suspended )
				got_msg = ::GetMessageW( &msg, l_HWND, 0, 0 );
			else
				got_msg = ::PeekMessageW( &msg, l_HWND, 0, 0, PM_REMOVE );

			if( got_msg )
			{
				::TranslateMessage(&msg);
				::DispatchMessageW(&msg);
			}
			else
			{
				// no messages, so do some idle processing
				if( !l_Suspended )
					i_App->Think();
			}
		}
	}
}
*/



//====================================================================
//	SetNonClientValid is meant to be called by other PAC components
//	(like g2dScreen).  It should be set to true if the screen is
//	in fullscreen mode and therefore the non-client area is a valid
//	area of the application.  It is false if the screen is in windowed
//	mode and the non-client area is the title bar of the window, 
//	which the application shouldn't get mouse events from.
//====================================================================
void appApplicationPAC::SetNonClientValid(bool i_Valid)
{
	l_NonClientValid = i_Valid;
}

//========================================================================
//	DialogLoopTasks should be called by things which use a tight loop
//	(render multiple times without returning control from
//	appApplication::Think) so that OS tasks can continue to be updated -
//	for instance, windows message handling.
//========================================================================
void appApplicationPAC::DialogLoopTasks()
{
}

//========================================================================
//	WindowClassName returns the name of the Windows windows class used
//	for the main app window.
//========================================================================
const char* appApplicationPAC::GetWindowClassName()
{
	return "Terawatt App Window";
}

//========================================================================
//	SetCursorFunction sets the function that will be called when
//  we receive the SetCursor window event
//========================================================================
void appApplicationPAC::SetCursorFunction( void (*i_pSetCursor)() )
{
	l_pSetCursorFunction = i_pSetCursor;
}


