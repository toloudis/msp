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

#ifndef IT_STRING_HPP
#include "itString.hpp"
#endif

class appApplication;

class appApplicationPAC
{
	public:

		//====================================================================
		//====================================================================
		appApplicationPAC();

		//====================================================================
		//====================================================================
		~appApplicationPAC();

		//====================================================================
		//	Run() is called to start the application.  When control
		//	returns from Run(), the application is finished.
		//====================================================================
		void Run(appApplication* i_App);

		//====================================================================
		//	Exit() is called by the client when it decides it is
		//	ready to end the program.
		//====================================================================
		void Exit();

		//====================================================================
		//	SetWindowSize is used to resize the application window.  This
		//	function can be called from a appStartEvent handler, before the
		//	window has been created.  The width and height refer to the client
		//	area of the window (the part inside the title bar and border).
		//====================================================================
		void SetWindowSize(int i_X, int i_Y, int i_Width, int i_Height);

		//====================================================================
		//	SetWindowTitle is used to set the text that appears in the
		//	window's title bar.  This function can be called from a 
		//	appStartEvent handler, before the window has been created. 
		//====================================================================
		void SetWindowTitle(const itString& i_Title);

		//====================================================================
		//	The windows appApplicationPAC defines this Instance function
		//	for use by other Windows PAC components. 
		//====================================================================
		static appApplicationPAC* Instance();

		//========================================================================
		//	Don't call Init() and CleanUp() yourself; they are called 
		//	by the package Init and Cleanup.
		//========================================================================
		static void Init();
		static void CleanUp() throw();

		//====================================================================
		//	IsSuspended is true if the application doesn't have focus
		//====================================================================
		static bool IsSuspended();

		//========================================================================
		//	DialogLoopTasks should be called by things which use a tight loop
		//	(render multiple times without returning control from
		//	appApplication::Think) so that OS tasks can continue to be updated -
		//	for instance, windows message handling.
		//========================================================================
		static void DialogLoopTasks();

	private:

		bool m_Quit;
};


