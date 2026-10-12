/*****************************************************************************
**  tqcRenderPanel.hpp
**
**     Window using Qt for doing simple rendering of a scene through its
**	own viewer (used by tool windows such as the UV editor).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef TQC_RENDERPANEL_HPP
#error tqcRenderPanel.hpp multiply included
#endif
#define TQC_RENDERPANEL_HPP

#ifndef TQC_NATIVEWINDOW_HPP
#include "ToolUIQt/tqc/tqcNativeWindow.hpp"
#endif

#ifdef USE_QT

//============================================================================
//============================================================================
class camCamera;
class g2dRGBColor;
class g2dSystem;
class g2dWindow;
class g3dScene;
class g3dSceneRenderer;
class g3dViewer;

//============================================================================
//============================================================================
class tqcRenderPanel : public tqcNativeWindow
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
    tqcRenderPanel(QWidget* parent, g2dSystem* i_pSystem);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
    ~tqcRenderPanel();

	//------------------------------------------------------------------------
	// The renderer is not owned by the viewer, just pointed to
	//------------------------------------------------------------------------
	g3dSceneRenderer* GetRenderer() const;
	void SetRenderer(g3dSceneRenderer* i_pRenderer);

	//------------------------------------------------------------------------
	// The scene is not owned by the viewer, just pointed to
	//------------------------------------------------------------------------
	g3dScene* GetScene() const;
	void SetScene(g3dScene* i_pScene);

	//------------------------------------------------------------------------
	// The camera is not owned by the viewer, just pointed to
	//------------------------------------------------------------------------
	camCamera* GetCamera() const;
	void SetCamera(camCamera* i_pCamera);

	//------------------------------------------------------------------------
	// Set background color
	//------------------------------------------------------------------------
	void SetBackgroundColor(const g2dRGBColor& i_Color);

protected:
	//------------------------------------------------------------------------
	// Qt event handlers
	//------------------------------------------------------------------------
	virtual void resizeEvent(QResizeEvent* i_pEvent);
	virtual void paintEvent(QPaintEvent* i_pEvent);

private:
	//------------------------------------------------------------------------
	// private functions
	//------------------------------------------------------------------------
	void do_resize();

	g2dSystem* m_pSystem;
	g2dWindow* m_pWindow;
	g3dViewer* m_pViewer;
};

#endif // USE_QT
