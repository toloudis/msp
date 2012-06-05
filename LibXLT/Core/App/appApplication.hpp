/****************************************************************************\
**  appApplication.hpp
**
**      appApplication.hpp supplies the basic application class.  To create
**	a Terawatt application, with a window and message handling, clients
**	can inherit an object from this class and use the protected interface
**	to implement application specific behaviors.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef APP_APPLICATION_HPP
#error appApplication.hpp multiply included
#endif
#define APP_APPLICATION_HPP


//============================================================================
//============================================================================
class itString;
class appApplicationPAC;


//============================================================================
//============================================================================
class appApplication
{
	public:

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		appApplication();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~appApplication();

		//--------------------------------------------------------------------
		//	Run() should be called to start the application.  When control
		//	returns from Run(), the application is finished.
		//--------------------------------------------------------------------
		virtual void Run();

		//------------------------------------------------------------------------
		//	Don't call Init() and CleanUp() yourself; they are called 
		//	by the package Init and Cleanup.
		//------------------------------------------------------------------------
		static void Init();
		static void CleanUp() throw();

		//------------------------------------------------------------------------
		//	DialogLoopTasks should be called by things which use a tight loop
		//	(render multiple times without returning control from
		//	appApplication::Think) so that OS tasks can continue to be updated -
		//	for instance, windows message handling.
		//------------------------------------------------------------------------
		static void DialogLoopTasks();

		//--------------------------------------------------------------------
		//	IsSuspended is true if the application doesn't have focus
		//--------------------------------------------------------------------
		static bool IsSuspended();

		//--------------------------------------------------------------------
		//	SetNonClientValid is meant to be called by other components.
		//	It should be set to true if the screen is in fullscreen mode 
		//  and therefore the non-client area is a valid area of the 
		//  application.  It is false if the screen is in windowed
		//	mode and the non-client area is the title bar of the window, 
		//	which the application shouldn't get mouse events from.
		//--------------------------------------------------------------------
		static void SetNonClientValid(bool i_Valid);
		
		//--------------------------------------------------------------------
		//	Return OS-dependent handle to main window.
		//--------------------------------------------------------------------
		static void* GetMainWindowHandle();

		//--------------------------------------------------------------------
		//	SetWindowSize is used to resize the application window.  This
		//	function can be called from a appStartEvent handler, before the
		//	window has been created.  The width and height refer to the client
		//	area of the window (the part inside the title bar and border).
		//--------------------------------------------------------------------
		static void SetMainWindowSize(int i_X, int i_Y, int i_Width, int i_Height);
		
		//--------------------------------------------------------------------
		//	Create a secondary window for the application, return 
		//		a void* handle to the window, the type of the
		//		handle is based on the OS.
		//--------------------------------------------------------------------
		static void* CreateSubWindow(int i_Width, int i_Height, 
				int i_X, int i_Y, const itString& i_Title);

		//--------------------------------------------------------------------
		//	Destory secondary window created by previous call to 
		//		CreateSubWindow(), pass in the handle returned 
		//		from the CreateSubWindow() call.
		//--------------------------------------------------------------------
		static void DestroySubWindow(void* i_Hwnd);

		//--------------------------------------------------------------------
		//	Used for creating an application that doesn't use Run()
		//--------------------------------------------------------------------
		static void CreateMainWindow(int i_Width, int i_Height, 
				int i_X, int i_Y, const itString& i_Title);

	protected:
		friend class appApplicationPAC;
	
		//--------------------------------------------------------------------
		//	Think() should be overridden by each application to implement
		//	it's own per-frame processing.  The client should not call this
		//	function.		
		//--------------------------------------------------------------------
		virtual void Think() = 0;

		//--------------------------------------------------------------------
		//	Exit() should be called by the client when it decides it is
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
		//	returns true if the app is in a state of quitting
		//--------------------------------------------------------------------
		bool IsQuitting() const;

	protected:
		appApplicationPAC* m_pImp;
};
