/*****************************************************************************
**	tma3dViewerMgr.cpp
**
**		tma3dViewerMgr maintains viewers for subwindows (for having
**	more than one render window open at once).
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Tool/tma3d/tma3dViewerMgr.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Graphics/G3d/g3dThreadControl.hpp"
#include "Graphics/g3d/g3dViewer.hpp"

#include <vector>


//============================================================================
//============================================================================
namespace tma3dViewerMgr
{
	namespace
	{
		std::vector<g3dViewer*> l_Viewers;
	}

	//------------------------------------------------------------------------
	// Add Viewer to management
	//------------------------------------------------------------------------
	void AddViewer( g3dViewer *i_pViewer )
	{
		l_Viewers.push_back(i_pViewer);
	}

	//------------------------------------------------------------------------
	// Remove Viewer from management
	//------------------------------------------------------------------------
	void RemoveViewer( g3dViewer *i_pViewer )
	{
		envSTLHelpers::RemoveOneValue(l_Viewers, i_pViewer);

	}

	//------------------------------------------------------------------------
	// Render all views
	//------------------------------------------------------------------------
	void RenderViews(float i_Time)
	{
		int nViews = l_Viewers.size();
		for (int i=0; i<nViews; i++)
		{
			// See if we should abort the render thread before doing the next target
			if (g3dThreadControl::ShouldRenderThreadAbort())
				break; // could return false here to signify that render did not finish correctly

			l_Viewers[i]->Render(i_Time);
			l_Viewers[i]->Present();
		}
	}

	//------------------------------------------------------------------------
	// Give each viewer a render target to render first. This lets all viewers
	// share same render target. The g3dTargetRenderer is owned by the caller.
	//------------------------------------------------------------------------
	void AddTargetRendererToViewers( g3dTargetRenderer* i_pRenderer )
	{
		int nViews = l_Viewers.size();
		for (int i=0; i<nViews; i++)
		{
			l_Viewers[i]->AddTargetRenderer(i_pRenderer);
		}
	}

	//------------------------------------------------------------------------
	// Remove target renderer from each viewer
	//------------------------------------------------------------------------
	void RemoveTargetRendererFromViewers( g3dTargetRenderer* i_pRenderer )
	{
		int nViews = l_Viewers.size();
		for (int i=0; i<nViews; i++)
		{
			l_Viewers[i]->RemoveTargetRenderer(i_pRenderer);
		}
	}

	//------------------------------------------------------------------------
	//	This is an externally calculated frame rate that should encapsulate
	//	only the time the app is rendering.  It should not include time for
	//	UI updating, File I/O, etc.
	//	This value will be displayed in the debug display ('~').
	//------------------------------------------------------------------------
	void SetFrameRateRunningAverage( float i_FPS )
	{
		int nViews = l_Viewers.size();
		for (int i=0; i<nViews; i++)
		{
			l_Viewers[i]->SetFrameRateRunningAverage(i_FPS);
		}
	}
};
