/*****************************************************************************
**	tma3dViewerMgr.hpp
**
**		tma3dViewerMgr maintains viewers for subwindows (for having
**	more than one render window open at once).
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef TMA3D_VIEWERMGR_HPP
#error tma3dViewerMgr.hpp multiply included
#endif
#define TMA3D_VIEWERMGR_HPP


//============================================================================
//============================================================================
class g3dTargetRenderer;
class g3dViewer;


//============================================================================
//============================================================================
namespace tma3dViewerMgr
{
	//------------------------------------------------------------------------
	// Add Viewer to management. The caller retains ownership of the viewer
	//	and should call RemoveViewer before deleting.
	//------------------------------------------------------------------------
	void AddViewer( g3dViewer *i_pViewer );

	//------------------------------------------------------------------------
	// Remove Viewer from management
	//------------------------------------------------------------------------
	void RemoveViewer( g3dViewer *i_pViewer );

	//------------------------------------------------------------------------
	// Render all views
	//------------------------------------------------------------------------
	void RenderViews(float i_Time);	

	//------------------------------------------------------------------------
	// Give each viewer a render target to render first. This lets all viewers
	// share same render target. The g3dTargetRenderer is owned by the caller.
	//------------------------------------------------------------------------
	void AddTargetRendererToViewers( g3dTargetRenderer* i_pRenderer );

	//------------------------------------------------------------------------
	// Remove target renderer from each viewer
	//------------------------------------------------------------------------
	void RemoveTargetRendererFromViewers( g3dTargetRenderer* i_pRenderer );

	//------------------------------------------------------------------------
	//	This is an externally calculated frame rate that should encapsulate
	//	only the time the app is rendering.  It should not include time for
	//	UI updating, File I/O, etc.
	//	This value will be displayed in the debug display ('~').
	//------------------------------------------------------------------------
	void SetFrameRateRunningAverage( float i_FPS );
};
