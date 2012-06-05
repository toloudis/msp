/*****************************************************************************
**	gpxRenderControl.hpp
**
**		This manager tracks whether a render is needed for changes that
**	do not fit into the proxy system.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef GPX_RENDERCONTROL_HPP
#error gpxRenderControl.hpp multiply included
#endif
#define GPX_RENDERCONTROL_HPP


//============================================================================
//============================================================================
namespace gpxRenderControl
{
	//--------------------------------------------------------------------
	// Returns true if a new render is needed.
	//--------------------------------------------------------------------
	bool IsRenderNeeded();

	//--------------------------------------------------------------------
	//	Call this function when a new render is needed from a change
	//	that could not be proxied.
	//--------------------------------------------------------------------
	void SetNeedsNewRender();

	//--------------------------------------------------------------------
	// Call this function when the rendering is started
	// so that the flag marking the need for a new render can be cleared.
	//--------------------------------------------------------------------
	void RenderStarted();

	//--------------------------------------------------------------------
	//	When about to make a structural change that cannot be proxied,
	//	call this function to abort the rendering thread and set the
	//	flag for needing a new render.
	//--------------------------------------------------------------------
	void ConfirmSingleThread();
}
