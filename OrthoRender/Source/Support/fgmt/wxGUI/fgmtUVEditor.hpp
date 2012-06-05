/*****************************************************************************
**  fgmtUVEditor.hpp
**
**     MainForm when using wxWidgets
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#ifdef FGMT_UVEDITOR_HPP
#error fgmtUVEditor.hpp multiply included
#endif
#define FGMT_UVEDITOR_HPP

#ifndef TWX_WIDGETS_HPP
#include "ToolUIWx/twx/twxWidgets.hpp"
#endif

#ifdef USE_WXWIDGETS

class camCamera;
class twcRenderPanel;
class g3dFragment;
class g3dScene;
class g3dSceneNode;
class g3dSceneRenderer;

//============================================================================
//============================================================================
class fgmtUVEditor : public wxPanel
{
public:
	//--------------------------------------------------------------------
    // ctor(s)
	//--------------------------------------------------------------------
    fgmtUVEditor(wxWindow* parent, const std::string& i_Title);

	//--------------------------------------------------------------------
    // dtor
	//--------------------------------------------------------------------
    ~fgmtUVEditor();

	//--------------------------------------------------------------------
	// Access to the render area
	//--------------------------------------------------------------------
	twcRenderPanel* GetRenderWindow();

	//--------------------------------------------------------------------
	// Set the fragment to edit.
	//--------------------------------------------------------------------
	void SetFragment(g3dFragment* i_pFragment);

private:
	//--------------------------------------------------------------------
    // event handlers (these functions should _not_ be virtual)
	//--------------------------------------------------------------------
	void OnClose(wxCloseEvent& i_Event);
	void OnResize(wxSizeEvent& i_Event);
	void OnKeyUp(wxKeyEvent& i_Event);

    // any class wishing to process wxWidgets events must use this macro
    DECLARE_EVENT_TABLE()

	wxBoxSizer*	m_pSizer;
    twcRenderPanel* m_pRenderPanel;

	// we have a scene with a single node to draw, in a single layer
	g3dScene* m_pScene;
	g3dSceneNode* m_pRootNode;
	g3dSceneRenderer* m_pRenderer;
	camCamera* m_pCamera;
};

#endif // USE_WXWIDGETS
