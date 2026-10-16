/****************************************************************************\
**	tma3dRenderView.hpp
**
**	Class for handling a single render pane within multiple views.
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef TMA3D_RENDERVIEW_HPP
#error tma3dRenderView.hpp multiply included
#endif
#define TMA3D_RENDERVIEW_HPP

#ifndef ENV_TYPE_HPP
#include "Core/env/envType.hpp"
#endif

#ifndef G3D_PREFS_HPP
#include "Graphics/g3d/g3dPrefs.hpp"
#endif

#include <vector>


//============================================================================
//============================================================================
class camCamera;
class g2dWindow;
class g3dPickInfo;
class g3dSceneRenderer;
class g3dViewer;
class gpxCamera;
class scrText;
class pick3dPickBuffer;
class g3dLayer;


//============================================================================
//============================================================================
class tma3dRenderView
{
public:
	// Enumeration for which renderer style to use in this view
	enum RendererType
	{
		e_Default = 0,
		e_HDR,
		e_AmbientOcclusion,
		e_Depth,
		e_ShadowMask,
		e_Normals,
		e_Materials,
		e_VelocityMap,
		e_IlluminationOnly,
		e_ReflectionOnly,
		e_GlobalIllumination,
		e_Glow,
		e_NumTypes
	};

	//--------------------------------------------------------------------
	// i_Window should have been created by a g2dSystem, usually with
	//	CreateSubWindow. Since the system maintains ownership of the
	//	window, it will not be deleted by this class in the destructor.
	//
	// A g3dViewer will be created in this constructor if the
	//	i_pViewer argument is NULL. Otherwise, it will take 
	//	ownership of the passed in viewer.
	//
	// Three renderers will be created, you can control which style
	//	of renderer is used with the SetRenderer function.
	//--------------------------------------------------------------------
	tma3dRenderView(g2dWindow* i_pWindow, 
					g3dViewer *i_pViewer, 
					const std::vector<g3dLayer*>& i_ViewerLayers);

	//--------------------------------------------------------------------
	// Deletes the renderer and viewer owned by this class.
	//--------------------------------------------------------------------
	virtual ~tma3dRenderView();

	//--------------------------------------------------------------------
	// Call this to enable or disable rendering of this viewer.
	// This is usually hooked up to the Enabled flag of the 
	//	rendering panel.
	//--------------------------------------------------------------------
	void EnableViewer(bool i_bEnable);

	//--------------------------------------------------------------------
	// Return pointer to window, viewer
	//--------------------------------------------------------------------
	inline g2dWindow* GetWindow();
	inline g3dViewer* GetViewer();

	//--------------------------------------------------------------------
	// Set Camera for viewer to use by giving it the proxy to use
	//	in camera manipulations.
	//--------------------------------------------------------------------
	void SetCameraProxy(gpxCamera* i_pCamera);
	//void SetCamera(camCamera* i_pCamera);
	inline gpxCamera* GetCameraProxy();

	//--------------------------------------------------------------------
	// Set Camera into cam3dMgr in order to activate the camera 
	//	manipulator for our camera.
	//--------------------------------------------------------------------
	void ActivateCameraManipulator();
	
	//--------------------------------------------------------------------
	// Get the depth based on the camera target position
	//--------------------------------------------------------------------
	float GetCameraDepth();
	
	//--------------------------------------------------------------------
	// Set style of renderer using enumeration
	//--------------------------------------------------------------------
	void SetRenderer(RendererType i_Type, g3dPrefs::g3dRenderPrefs* i_pPrefs = NULL);

	//--------------------------------------------------------------------
	// Resize back buffer of window
	//--------------------------------------------------------------------
	void ResizeWindow(int i_Width, int i_Height, int iWidthDIP=0, int iHeightDIP=0);

	//--------------------------------------------------------------------
	// Resize back buffer of window to the panel size, render at the
	//	given render size, and scale the image to fit the panel.
	//	The virtual resolution is set from the given DIP size.
	//--------------------------------------------------------------------
	void ResizeWindowToFit(int i_PanelWidth, int i_PanelHeight,
						   int i_RenderWidth, int i_RenderHeight,
						   int i_RenderWidthDIP, int i_RenderHeightDIP);

	//--------------------------------------------------------------------
	// Convert a position in panel pixels to a pixel in the rendered
	//	image. The result is outside the image in the black bars.
	//--------------------------------------------------------------------
	void PanelToRenderPixel(int& io_X, int& io_Y) const;

	//--------------------------------------------------------------------
	// Size of the rendered image in pixels
	//--------------------------------------------------------------------
	void GetRenderSize(int& o_Width, int& o_Height) const;

	//--------------------------------------------------------------------
	// Render objects at given pixel with color encodings.
	// Returns pick code that can be used to search for the picked
	// g3dSceneNode in the scene graph. A returned value of 0
	// means nothing was picked.
	//--------------------------------------------------------------------
	void DoPickRender(int i_X, int i_Y, float i_Time, g3dPickInfo& o_PickInfo,
						envType::UInt32 i_PickMask  = 0);

//========================================================================
// Static functions
//========================================================================

	//--------------------------------------------------------------------
	// Only one render view can have the focus at a time. 
	//--------------------------------------------------------------------
	static void SetActiveRenderView(tma3dRenderView *i_pRenderView);
	static tma3dRenderView* GetActiveRenderView();

private:
	g2dWindow*			m_pWindow;
	std::vector<g3dSceneRenderer*>	m_Renderers;
	RendererType		m_CurrentRenderer;
	g3dViewer*			m_pViewer;
	gpxCamera*			m_pCameraProxy;
	bool				m_bViewerEnabled;
	pick3dPickBuffer*	m_pPickBuffer;
	std::vector<g3dLayer*> m_ViewerLayers;

	static tma3dRenderView* sm_pActiveRenderView;

	g3dPrefs::g3dRenderPrefs m_RenderPrefs;
};


//--------------------------------------------------------------------
// Return pointer to window
//--------------------------------------------------------------------
inline g2dWindow* tma3dRenderView::GetWindow()
{
	return m_pWindow;
}
inline g3dViewer* tma3dRenderView::GetViewer()
{
	return m_pViewer;
}
inline gpxCamera* tma3dRenderView:: GetCameraProxy()
{
	return m_pCameraProxy;
}