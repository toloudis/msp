/****************************************************************************\
**	g3dThreadControl.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Graphics/g3d/g3dThreadControl.hpp"

#include "Core/Dbg/dbgMsg.hpp"
#include "Core/Env/envThread.hpp"
#include "Core/Env/envWaitCondition.hpp"

#ifdef ENV_USE_THREADS
#include <Windows.h>  // for Sleep()
#endif

//============================================================================
//============================================================================
namespace
{
	envMutex l_ControlMutex;
	bool l_bRenderThreadActive = false;
	envWaitCondition l_StartCondition;
	bool l_bAbortRenderThread = false;
	//envWaitCondition l_AbortCondition;
}


//--------------------------------------------------------------------
// Returns true if the render thread is currently running
//--------------------------------------------------------------------
bool g3dThreadControl::IsRenderThreadActive()
{
	// Since this is a simple boolean flag read, can we do it without the mutex?
	return l_bRenderThreadActive;
}

//--------------------------------------------------------------------
// Ask the render thread to abort as soon as possible, and wait for 
// the thread to stop before returning from this function.
//--------------------------------------------------------------------
void g3dThreadControl::AbortRenderThread()
{
	g3dThreadControl::RequestRenderThreadAbort();
	
#ifdef ENV_USE_THREADS
	// How best should we wait for the render thread to abort?
	// We can wait here in a loop checking for the l_bRenderThreadActive 
	// flag to change, or we could join the thread if we had the thread id.
	while (l_bRenderThreadActive)
	{
		::Sleep(1);
	}
#endif

	//const bool reset_flag = true;
	//l_AbortCondition.WaitForNotify(reset_flag);
}
//--------------------------------------------------------------------
// Wait for render thread to start before continuing,
// prevents race conditions where more than one render thread
// gets started at the same time.
//--------------------------------------------------------------------
void g3dThreadControl::WaitForRenderThreadStart()
{
	// this should be changed to boost::conditional_variable soon
	//while (!l_bRenderThreadStart)
	//{
	//	::Sleep(1);
	//}
	//l_bRenderThreadStart = false;

	const bool reset_flag = true;
	l_StartCondition.WaitForNotify(reset_flag);
}

//--------------------------------------------------------------------
// Ask the render thread to abort as soon as possible. This function
// does not wait for the render thread to stop.
//--------------------------------------------------------------------
void g3dThreadControl::RequestRenderThreadAbort()
{
	envScopedLock thread_control_lock(l_ControlMutex);
	if (l_bRenderThreadActive)
		l_bAbortRenderThread = true;
}

//--------------------------------------------------------------------
// Functions in the render thread should call this function 
// often and should abort the current render if this function
// returns true.
//--------------------------------------------------------------------
bool g3dThreadControl::ShouldRenderThreadAbort()
{
	return l_bAbortRenderThread;
}

//--------------------------------------------------------------------
// Use the render thread wrapper class as an exception safe way to 
// mark the beginning and end of the render thread. Just create
// a local variable of this type when the render thread starts so that
// its destructor is called when the render thread is finished.
// This class will call BeginRenderThread() and EndRenderThread()
//--------------------------------------------------------------------
g3dThreadControl::RenderThreadWrapper::RenderThreadWrapper()
{
	g3dThreadControl::BeginRenderThread();
}
g3dThreadControl::RenderThreadWrapper::~RenderThreadWrapper()
{
	g3dThreadControl::EndRenderThread();
}

//--------------------------------------------------------------------
// RenderThreadWrapper class calls these functions when starting and 
// stopping the render thread in order to track when the render 
// thread is active.
//--------------------------------------------------------------------
void g3dThreadControl::BeginRenderThread()
{
	envScopedLock thread_control_lock(l_ControlMutex);
	DBG_ASSERT(!l_bRenderThreadActive, "Render thread is already active");
	l_bRenderThreadActive = true;

	l_StartCondition.NotifyFinished();
//	l_bRenderThreadStart = true;
}
void g3dThreadControl::EndRenderThread()
{
	envScopedLock thread_control_lock(l_ControlMutex);
	l_bRenderThreadActive = false;
	//if (l_bAbortRenderThread)
	//	l_AbortCondition.NotifyFinished();
	l_bAbortRenderThread = false;
}

