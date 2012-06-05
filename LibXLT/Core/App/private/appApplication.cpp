/****************************************************************************\
**  appApplication.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "Core/app/appApplication.hpp"

#include "Core/app/private/appApplicationPAC.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
appApplication::appApplication()
:	m_pImp(new appApplicationPAC)
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
appApplication::~appApplication()
{
	delete m_pImp;
}

//--------------------------------------------------------------------
//	Run() should be called to start the application.  When control
//	returns from Run(), the application is finished.
//--------------------------------------------------------------------
void appApplication::Run()
{
	m_pImp->Run(this);
}

//--------------------------------------------------------------------
//	Exit() should be called by the client when it decides it is
//	ready to end the program.
//--------------------------------------------------------------------
void appApplication::Exit()
{
	m_pImp->Exit();
}

//--------------------------------------------------------------------
//	SetWindowSize is used to resize the application window.  This
//	function can be called from a appStartEvent handler, before the
//	window has been created.  The width and height refer to the client
//	area of the window (the part inside the title bar and border).
//--------------------------------------------------------------------
void appApplication::SetWindowSize(int i_X, int i_Y, int i_Width, int i_Height)
{
	m_pImp->SetWindowSize(i_X, i_Y, i_Width, i_Height);
}

//--------------------------------------------------------------------
//	SetWindowTitle is used to set the text that appears in the
//	window's title bar.  This function can be called from a 
//	appStartEvent handler, before the window has been created. 
//--------------------------------------------------------------------
void appApplication::SetWindowTitle(const itString& i_Title)
{
	m_pImp->SetWindowTitle(i_Title);
}

//------------------------------------------------------------------------
//	Don't call Init() and CleanUp() yourself; they are called 
//	by the package Init and Cleanup.
//------------------------------------------------------------------------
void appApplication::Init()
{
	appApplicationPAC::Init();
}

void appApplication::CleanUp() throw()
{
	appApplicationPAC::CleanUp();
}

//------------------------------------------------------------------------
//	DialogLoopTasks should be called by things which use a tight loop
//	(render multiple times without returning control from
//	appApplication::Think) so that OS tasks can continue to be updated -
//	for instance, windows message handling.
//------------------------------------------------------------------------
void appApplication::DialogLoopTasks()
{
	appApplicationPAC::DialogLoopTasks();
}

//--------------------------------------------------------------------
//	IsSuspended is true if the application doesn't have focus
//--------------------------------------------------------------------
bool appApplication::IsSuspended()
{
	return appApplicationPAC::IsSuspended();
}

//--------------------------------------------------------------------
//	SetNonClientValid is meant to be called by other components.
//	It should be set to true if the screen is in fullscreen mode 
//  and therefore the non-client area is a valid area of the 
//  application.  It is false if the screen is in windowed
//	mode and the non-client area is the title bar of the window, 
//	which the application shouldn't get mouse events from.
//--------------------------------------------------------------------
void appApplication::SetNonClientValid(bool i_Valid)
{
	appApplicationPAC::SetNonClientValid(i_Valid);
}

//--------------------------------------------------------------------
//	Return OS-dependent handle to main window.
//--------------------------------------------------------------------
void* appApplication::GetMainWindowHandle()
{
	return (void*) appApplicationPAC::GetHWND();
}

//--------------------------------------------------------------------
//	SetWindowSize is used to resize the application window.  This
//	function can be called from a appStartEvent handler, before the
//	window has been created.  The width and height refer to the client
//	area of the window (the part inside the title bar and border).
//--------------------------------------------------------------------
void appApplication::SetMainWindowSize(int i_X, int i_Y, int i_Width, int i_Height)
{
	appApplicationPAC::Instance()->SetWindowSize(i_X, i_Y, i_Width, i_Height);
}

//--------------------------------------------------------------------
//	Create a secondary window for the application, return 
//		a void* handle to the window, the type of the
//		handle is based on the OS.
//--------------------------------------------------------------------
void* appApplication::CreateSubWindow(int i_Width, int i_Height, 
		int i_X, int i_Y, const itString& i_Title)
{
	return (void*)appApplicationPAC::CreateSubWindow(i_Width, i_Height, i_X, i_Y, i_Title);
}

//--------------------------------------------------------------------
//	Destory secondary window created by previous call to 
//		CreateSubWindow(), pass in the handle returned 
//		from the CreateSubWindow() call.
//--------------------------------------------------------------------
void appApplication::DestroySubWindow(void* i_Hwnd)
{
	appApplicationPAC::DestroySubWindow((HWND)i_Hwnd);
}

//--------------------------------------------------------------------
//	Used for creating an application that doesn't use Run()
//--------------------------------------------------------------------
void appApplication::CreateMainWindow(int i_Width, int i_Height, 
		int i_X, int i_Y, const itString& i_Title)
{
	appApplicationPAC::CreateMainWindow(i_Width, i_Height, i_X, i_Y, i_Title);
}

//--------------------------------------------------------------------
//	returns true if the app is in a state of quitting
//--------------------------------------------------------------------
bool appApplication::IsQuitting() const
{
	return appApplicationPAC::Instance()->IsQuitting();
}

