/****************************************************************************\
**  appApplicationPACWin.hpp
**
**      appApplicationPACWin.hpp defines the windows PAC for the
**	appApplication.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef APP_APPLICATIONPACWIN_HPP
#error appApplicationPACWin.hpp multiply included
#endif
#define APP_APPLICATIONPACWIN_HPP

#include <windows.h>

#ifndef IT_STRING_HPP
#include "Core/it/itString.hpp"
#endif


//============================================================================
//============================================================================
class appApplication;
class envAppVersion;


//============================================================================
//============================================================================
class appApplicationPAC
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		appApplicationPAC();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~appApplicationPAC();

		//--------------------------------------------------------------------
		//	called before the message loop in Run()
		//--------------------------------------------------------------------
		void PreMessageLoop();

		//--------------------------------------------------------------------
		//	called after the message loop in Run()
		//--------------------------------------------------------------------
		void PostMessageLoop();

		//--------------------------------------------------------------------
		//	Run() is called to start the application.  When control
		//	returns from Run(), the application is finished.
		//--------------------------------------------------------------------
		void Run(appApplication* i_App);

		//--------------------------------------------------------------------
		//	Exit() is called by the client when it decides it is
		//	ready to end the program.
		//--------------------------------------------------------------------
		void Exit();

		//--------------------------------------------------------------------
		//	SetWindowSize is used to resize the application window.  This
		//	function can be called from a appStartEvent handler, before the
		//	window has been created.  The width and height refer to the client
		//	area of the window (the part inside the title bar and border).
		//--------------------------------------------------------------------
		void SetWindowSize(int i_X, int i_Y, int i_Width, int i_Height);

		//--------------------------------------------------------------------
		//	SetWindowTitle is used to set the text that appears in the
		//	window's title bar.  This function can be called from a 
		//	appStartEvent handler, before the window has been created. 
		//--------------------------------------------------------------------
		void SetWindowTitle(const itString& i_Title);

		//--------------------------------------------------------------------
		//	The windows appApplicationPAC defines this Instance function
		//	for use by other Windows PAC components. 
		//--------------------------------------------------------------------
		static appApplicationPAC* Instance();

		//--------------------------------------------------------------------
		//	The HINSTANCE must be set by the client, from the value passed
		//	to WinMain.
		//--------------------------------------------------------------------
		static void SetHINSTANCE(HINSTANCE i_Instance);

		//--------------------------------------------------------------------
		//	The HINSTANCE of the application is supplied here for use by
		//	other Windows PAC components.
		//--------------------------------------------------------------------
		static HINSTANCE GetHINSTANCE();

		//--------------------------------------------------------------------
		//	The HICON of the engine window can be created outside to be used in
		//  other applications.
		//--------------------------------------------------------------------
		static void SetHICON(HICON i_HICON);
		
		//--------------------------------------------------------------------
		//	The HWND of the application window is supplied here for use by
		//	other Windows PAC components.
		//--------------------------------------------------------------------
		static HWND GetHWND();

		//--------------------------------------------------------------------
		//	The HWND of the engine window can be created outside to be used in
		//  other applications.
		//--------------------------------------------------------------------
		static void SetHWND(HWND i_HWND);
		
		//--------------------------------------------------------------------
		//	DisableStickyKeys disables this XP feature 
		//--------------------------------------------------------------------
		static void DisableStickyKeys();
		
		//--------------------------------------------------------------------
		//	This is the window procedure for the application.
		//--------------------------------------------------------------------
		LRESULT WindowProc(	HWND hwnd,      // handle to window
							UINT uMsg,      // message identifier
							WPARAM wParam,  // first message parameter
							LPARAM lParam   // second message parameter
							);

		//------------------------------------------------------------------------
		//	Don't call Init() and CleanUp() yourself; they are called 
		//	by the package Init and Cleanup.
		//------------------------------------------------------------------------
		static void Init();
		static void CleanUp() throw();

		//--------------------------------------------------------------------
		//	IsSuspended is true if the application doesn't have focus
		//--------------------------------------------------------------------
		static bool IsSuspended();

		//--------------------------------------------------------------------
		//	SetNonClientValid is meant to be called by other PAC components
		//	(like g2dScreen).  It should be set to true if the screen is
		//	in fullscreen mode and therefore the non-client area is a valid
		//	area of the application.  It is false if the screen is in windowed
		//	mode and the non-client area is the title bar of the window, 
		//	which the application shouldn't get mouse events from.
		//--------------------------------------------------------------------
		static void SetNonClientValid(bool i_Valid);

		//--------------------------------------------------------------------
		// Set true when you want a WM_CLOSE message to stop the application.
		//--------------------------------------------------------------------
		static void SetWantCloseMessage(bool i_bClose);

		//--------------------------------------------------------------------
		// Turn off this flag to not allow the application to suspend when
		// minimized (to allow background rendering).
		//--------------------------------------------------------------------
		static void SetAllowSuspended(bool i_bSuspend);

		//--------------------------------------------------------------------
		// Set false when you want all windows created by the application
		//	to be hidden
		//--------------------------------------------------------------------
		static void SetShowWindows(bool i_bShow);

		//------------------------------------------------------------------------
		//	DialogLoopTasks should be called by things which use a tight loop
		//	(render multiple times without returning control from
		//	appApplication::Think) so that OS tasks can continue to be updated -
		//	for instance, windows message handling.
		//------------------------------------------------------------------------
		static void DialogLoopTasks();

		//------------------------------------------------------------------------
		//	SuppressAltTab determines whether to suppress the alt-tab windows event
		//	to prevent windows from switching away from the progrram
		//------------------------------------------------------------------------
		static void SuppressAltTab(bool i_bSuppress);

		//------------------------------------------------------------------------
		//	GetWindowClassName returns the name of the Windows windows class used
		//	for the main app window.
		//------------------------------------------------------------------------
		static const char* GetWindowClassName();

		//------------------------------------------------------------------------
		//	SetCursorFunction sets the function that will be called when
		//  we receive the SetCursor window event
		//------------------------------------------------------------------------
		static void SetCursorFunction( void (*i_pSetCursor)() );
		
		//--------------------------------------------------------------------
		//	Create a secondary window for the application
		//--------------------------------------------------------------------
		static HWND CreateSubWindow(int i_Width, int i_Height, 
									int i_X, int i_Y, const itString& i_Title);

		//--------------------------------------------------------------------
		//	Destory secondary window created by previous call to 
		//		CreateSubWindow()
		//--------------------------------------------------------------------
		static void DestroySubWindow(HWND i_Hwnd);

		//--------------------------------------------------------------------
		//	Used for creating an application that doesn't use Run()
		//--------------------------------------------------------------------
		static void CreateMainWindow(int i_Width, int i_Height, 
									 int i_X, int i_Y, const itString& i_Title);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		bool IsQuitting() const;

		//----------------------------------------------------------------------------
		//	GetAppVersion - returns true, and the envAppVersion, if app has a valid version. 
		//----------------------------------------------------------------------------
		bool GetAppVersion( envAppVersion& o_Version );

	protected:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void create_main_window();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void message_loop(appApplication* i_App);

	private:
		int m_X, m_Y, m_Width, m_Height;
		bool m_bQuit;
		itString m_Title;
		char m_LeadByte;	// lead byte for multi-byte characters
		bool m_bHasLeadByte;
};


