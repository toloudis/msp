/*****************************************************************************
**  tqcRenderPanel.hpp
**
**     Window using wxWidgets for doing MachStudio rendering
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef TQC_RENDERPANEL_HPP
#error tqcRenderPanel.hpp multiply included
#endif
#define TQC_RENDERPANEL_HPP

#ifndef TQT_WIDGETS_HPP
#include "ToolUIQt/tqt/tqtWidgets.hpp"
#endif

#ifdef QT_FINISH_PORT

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
// Define a new frame type: this is going to be our main frame
//============================================================================
class tqcRenderPanel : public wxControl
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
    tqcRenderPanel(QWidget* parent, g2dSystem* i_pSystem);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
    ~tqcRenderPanel();

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
	// The camera is not owned by the viewer, just pointed to
	//----------------------------------------------------------------------------
	camCamera* GetCamera() const;
	void SetCamera(camCamera* i_pCamera);

	//----------------------------------------------------------------------------
	// Set background color
	//----------------------------------------------------------------------------
	void SetBackgroundColor(const g2dRGBColor& i_Color);
private:
	//------------------------------------------------------------------------
	// private functions
	//------------------------------------------------------------------------
	void do_resize();

	//------------------------------------------------------------------------
    // event handlers (these functions should _not_ be virtual)
	//------------------------------------------------------------------------
	void OnMouseDown(wxMouseEvent& i_Event);
	void OnMouseMove(wxMouseEvent& i_Event);
	void OnEnter(wxMouseEvent& i_Event);
	void OnLeave(wxMouseEvent& i_Event);
	void OnSize(wxSizeEvent& i_Event);
	void OnKeyUp(wxKeyEvent& i_Event);
	void OnFocus(wxFocusEvent& i_Event);
	void OnEraseBackground(wxEraseEvent& i_Event);
	void OnPaint(wxPaintEvent& i_Event);

	g2dSystem* m_pSystem;
	g2dWindow* m_pWindow;
	g3dViewer* m_pViewer;

    DECLARE_EVENT_TABLE()
};

#endif // USE_QT
