/****************************************************************************\
**	g3dTargetRenderer.hpp
**
**		A g3dTargetRenderer is a convenience class for rendering a scene
**	to a texture. It does not maintain a debug display like the g3dViewer class.
**
**		Note: this base class does not own any of its pointers.
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef G3D_TARGETRENDERER_HPP
#error g3dTargetRenderer.hpp already included
#endif
#define G3D_TARGETRENDERER_HPP

#ifndef G2D_RGBCOLOR_HPP
#include "Graphics/g2d/g2dRGBColor.hpp"
#endif

#include <vector>


//============================================================================
//	Forward References
//============================================================================
class camCamera;
class g2dRenderTarget;
class g3dLayer;
class g3dSceneRenderer;
class g3dScene;
class g3dRenderPrefs;


//============================================================================
//============================================================================
namespace g3dPrefs
{
	struct g3dRenderPrefs;
}


//============================================================================
//============================================================================
class g3dTargetRenderer
{
	public:
		//----------------------------------------------------------------------------
		// The root node is not owned by the layer, just pointed to
		//----------------------------------------------------------------------------
		g3dTargetRenderer();
		g3dTargetRenderer(g2dRenderTarget* i_pTarget, g3dSceneRenderer* i_pRenderer);
		g3dTargetRenderer(g2dRenderTarget* i_pTarget, g3dSceneRenderer* i_pRenderer,
							g3dScene* i_pScene, camCamera* i_pCamera);

		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		virtual ~g3dTargetRenderer();

		//----------------------------------------------------------------------------
		// The window is not owned by the viewer, just pointed to
		//----------------------------------------------------------------------------
		g2dRenderTarget* GetTarget() const;
		void SetTarget(g2dRenderTarget* i_pTarget);

		//----------------------------------------------------------------------------
		// The renderer is not owned by the viewer, just pointed to
		//----------------------------------------------------------------------------
		g3dSceneRenderer* GetRenderer() const;
		void SetRenderer(g3dSceneRenderer* i_pRenderer);

		//----------------------------------------------------------------------------
		// The scene is not owned by the viewer, just pointed to
		//----------------------------------------------------------------------------
		g3dScene* GetScene() const;
		void SetScene(g3dScene* i_pScene);
		
		//----------------------------------------------------------------------------
		// Gets the aspect ratio that is set by cptrModeRender.cpp
		//----------------------------------------------------------------------------
		void SetAspectRatio(int PixelAspectRatio);
		float GetAspectRatio();

		//----------------------------------------------------------------------------
		// The camera is not owned by the viewer, just pointed to
		//----------------------------------------------------------------------------
		camCamera* GetCamera() const;
		void SetCamera(camCamera* i_pCamera);

		//----------------------------------------------------------------------------
		// If this flag is set to true, then the aspect ratio of the camera
		//	will be set to match the window's width and height.
		//	It is FALSE by default.
		//** Note that this behavior is now controlled by the camera.**
		//----------------------------------------------------------------------------
		//bool GetMatchAspectToWindow() const;
		//void SetMatchAspectToWindow(bool i_pVal);

		//--------------------------------------------------------------------
		//	Render renders the scene node hierarchy and an optional set 
		//	of layers specific to a view.
		//--------------------------------------------------------------------
		virtual void Render( float i_fSimTime, bool i_bClear = true );
		virtual void Render( float i_fSimTime, 
							 const std::vector<g3dLayer*>& i_ViewerLayers, 
							 bool i_bClear = true);

		//--------------------------------------------------------------------
		// Present results of render to window
		//--------------------------------------------------------------------
		virtual void Present();

		//----------------------------------------------------------------------------
		// Returns number of primitives rendered in last rendered frame
		//----------------------------------------------------------------------------
		int GetNumPrimitivesRendered();

		//----------------------------------------------------------------------------
		// Set background color for window
		//----------------------------------------------------------------------------
		const g2dRGBColor& GetBackgroundColor() const;
		void SetBackgroundColor(const g2dRGBColor& i_Color);

		//----------------------------------------------------------------------------
		// Set render preferences for this viewer's renders
		//----------------------------------------------------------------------------
		void SetPrefs(g3dPrefs::g3dRenderPrefs* i_pRenderPrefs);

	protected:
		//--------------------------------------------------------------------
		//	Render any targets before the main render 
		//--------------------------------------------------------------------
		virtual void RenderTargets( float i_fSimTime ) {}

		//--------------------------------------------------------------------
		// render anything needed after renderer is done
		//--------------------------------------------------------------------
		virtual void RenderOverlay() {}

	protected:
		int					m_NumPrimitivesRendered;
		g3dSceneRenderer*	m_pRenderer;
		g2dRGBColor			m_BackgroundColor;
		g2dRenderTarget*	m_pTarget;
		g3dScene*			m_pScene;
		camCamera*			m_pCamera;

		g3dPrefs::g3dRenderPrefs* m_pPrefs;

	private:
		float m_fAspectRatio;
		//bool m_bMatchAspectToWindow;
};

