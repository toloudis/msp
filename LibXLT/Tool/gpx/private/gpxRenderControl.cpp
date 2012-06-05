/*****************************************************************************
**	gpxRenderControl.cpp
**
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Tool/gpx/gpxRenderControl.hpp"

#include "Graphics/G3d/g3dThreadControl.hpp"


//============================================================================
//============================================================================
namespace gpxRenderControl
{
	namespace
	{
		bool l_bRenderNeeded = true; // starts with a need to render?

	}	// end of namespace

	//--------------------------------------------------------------------
	// Returns true if a new render is needed.
	//--------------------------------------------------------------------
	bool IsRenderNeeded()
	{
		return l_bRenderNeeded;
	}

	//--------------------------------------------------------------------
	//	Call this function when a new render is needed from a change
	//	that could not be proxied.
	//--------------------------------------------------------------------
	void SetNeedsNewRender()
	{
		// Do we need a mutex here?
		l_bRenderNeeded = true;
	}

	//--------------------------------------------------------------------
	// Call this function when the rendering is started
	// so that the flag marking the need for a new render can be cleared.
	//--------------------------------------------------------------------
	void RenderStarted()
	{
		// Do we need a mutex here?
		l_bRenderNeeded = false;
	}

	//--------------------------------------------------------------------
	//	When about to make a structural change that cannot be proxied,
	//	call this function to abort the rendering thread and set the
	//	flag for needing a new render.
	//--------------------------------------------------------------------
	void ConfirmSingleThread()
	{
		// bga - putting this together into a single convenience function
		// so that we can use the compiler defines to alter what needs to
		// be done in this function.
		g3dThreadControl::AbortRenderThread();
		gpxRenderControl::SetNeedsNewRender();
	}
}

