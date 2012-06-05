/****************************************************************************\
**	g3dThreadControl.hpp
**
**		The g3dThreadControl maintains the state of the rendering thread
**	and provides a mechanism for aborting it early.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef G3D_THREADCONTROL_HPP
#error g3dThreadControl.hpp multiply included
#endif
#define G3D_THREADCONTROL_HPP


//============================================================================
//============================================================================
class g3dThreadControl
{
public:
	//--------------------------------------------------------------------
	// Returns true if the render thread is currently running
	//--------------------------------------------------------------------
	static bool IsRenderThreadActive();

	//--------------------------------------------------------------------
	// Ask the render thread to abort as soon as possible, and wait for 
	// the thread to stop before returning from this function.
	//--------------------------------------------------------------------
	static void AbortRenderThread();

	//--------------------------------------------------------------------
	// Wait for render thread to start before continuing,
	// prevents race conditions where more than one render thread
	// gets started at the same time.
	//--------------------------------------------------------------------
	static void WaitForRenderThreadStart();

	//--------------------------------------------------------------------
	// Ask the render thread to abort as soon as possible. This function
	// does not wait for the render thread to stop.
	//--------------------------------------------------------------------
	static void RequestRenderThreadAbort();
	
	//--------------------------------------------------------------------
	// Functions in the render thread should call this function 
	// often and should abort the current render if this function
	// returns true.
	//--------------------------------------------------------------------
	static bool ShouldRenderThreadAbort();

	//--------------------------------------------------------------------
	// Use the render thread wrapper class as an exception safe way to 
	// mark the beginning and end of the render thread. Just create
	// a local variable of this type when the render thread starts so that
	// its destructor is called when the render thread is finished.
	// This class will call BeginRenderThread() and EndRenderThread()
	//--------------------------------------------------------------------
	class RenderThreadWrapper
	{
	public:
		RenderThreadWrapper();
		~RenderThreadWrapper();
	};

private:
	//--------------------------------------------------------------------
	// RenderThreadWrapper class calls these functions when starting and 
	// stopping the render thread in order to track when the render 
	// thread is active.
	//--------------------------------------------------------------------
	static void BeginRenderThread();
	static void EndRenderThread();
};


