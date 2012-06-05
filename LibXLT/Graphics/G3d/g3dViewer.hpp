/****************************************************************************\
**	g3dViewer.hpp
**
**		A g3dViewer is a convenience class for packaging a scene,
**	a window and a camera for easy rendering.
**
**		Note: this base class does not own any of its pointers.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef G3D_VIEWER_HPP
#error g3dViewer.hpp already included
#endif
#define G3D_VIEWER_HPP

#ifndef G3D_TARGETRENDERER_HPP
#include "Graphics/g3d/g3dTargetRenderer.hpp"
#endif
#ifndef G2D_FONTHANDLE_HPP
#include "Graphics/g2d/g2dFontHandle.hpp"
#endif
#ifndef IT_STRING_HPP
#include "Core/it/itString.hpp"
#endif


//============================================================================
//============================================================================
class g2dWindow;


//============================================================================
//============================================================================
class g3dViewer : public g3dTargetRenderer
{
	public:
		//----------------------------------------------------------------------------
		// The root node is not owned by the layer, just pointed to
		//----------------------------------------------------------------------------
		g3dViewer();
		g3dViewer(g2dWindow* i_pWindow, g3dSceneRenderer* i_pRenderer);
		g3dViewer(g2dWindow* i_pWindow, g3dSceneRenderer* i_pRenderer,
			g3dScene* i_pScene, camCamera* i_pCamera);

		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		virtual ~g3dViewer();

		//----------------------------------------------------------------------------
		// The window is not owned by the viewer, just pointed to
		//----------------------------------------------------------------------------
		g2dWindow* GetWindow() const;
		void SetWindow(g2dWindow* i_pWindow);

		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		virtual void Present();

		//----------------------------------------------------------------------------
		// Set text string to display on given line.
		//----------------------------------------------------------------------------
		void SetTextMessage(int i, const itString &i_Msg);

		//----------------------------------------------------------------------------
		// Remove any text messages
		//----------------------------------------------------------------------------
		void ClearTextMessages();

		//----------------------------------------------------------------------------
		// Add TargetRenderer to management. The caller retains ownership of the target
		//	and should call RemoveTargetRenderer before deleting. 
		//----------------------------------------------------------------------------
		void AddTargetRenderer( g3dTargetRenderer *i_pTargetRenderer );

		//----------------------------------------------------------------------------
		// Remove TargetRenderer from management
		//----------------------------------------------------------------------------
		void RemoveTargetRenderer( g3dTargetRenderer *i_pTargetRenderer );

		//------------------------------------------------------------------------
		//	This is an externally calculated frame rate that should encapsulate
		//	only the time the app is rendering.  It should not include time for
		//	UI updating, File I/O, etc.
		//	This value will be displayed in the debug display ('~').
		//------------------------------------------------------------------------
		void SetFrameRateRunningAverage( float i_FPS );

	protected:
		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		virtual void RenderTargets( float i_fSimTime );

		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		virtual void RenderOverlay();

	private:
		g2dWindow*	m_pWindow;

		// This data is for displaying text strings on the window.
		// This arguably could be in a specialized class, not the base.
		std::vector<itString> m_Messages;
		g2dFontHandle m_Font;

		// Each viewer gets a list of view-dependent render targets that
		// the main view renderer depends on.
		std::vector<g3dTargetRenderer*> m_TargetRenderers;

		float m_fCalculatedFrameRate;
};


