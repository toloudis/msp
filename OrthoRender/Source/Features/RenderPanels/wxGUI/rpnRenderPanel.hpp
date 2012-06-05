/*****************************************************************************
**  rpnRenderPanel.hpp
**
**     Window using wxWidgets for doing MachStudio rendering
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#ifdef RPN_RENDERPANEL_HPP
#error rpnRenderPanel.hpp multiply included
#endif
#define RPN_RENDERPANEL_HPP

#ifndef MA_POINT3D_HPP
#include "Core/Ma/maPoint3d.hpp"
#endif 

#ifndef TWX_WIDGETS_HPP
#include "ToolUIWx/twx/twxWidgets.hpp"
#endif

#ifdef USE_WXWIDGETS

//============================================================================
//============================================================================
class camCamera;
class camsDirectorsCut;
class rpnPanelViewer;
class tma3dRenderView;

//============================================================================
// Define a new frame type: this is going to be our main frame
//============================================================================
class rpnRenderPanel : public wxControl
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
    rpnRenderPanel(wxWindow* parent);

	//------------------------------------------------------------------------
	// Set render view to control
	//------------------------------------------------------------------------
	void SetRenderView(tma3dRenderView* i_pRenderView);

	//----------------------------------------------------------------------------
	// Enable rendering in this pane.
	//----------------------------------------------------------------------------
	void EnableViewer(bool i_bEnabled);

	//----------------------------------------------------------------------------
	// Turn visibility of text label on or off
	//----------------------------------------------------------------------------
	void SetTextVisible(bool i_bShow);

	//----------------------------------------------------------------------------
	// Set camera and label. If this view is active, the camera manipulator
	//	will also be set.
	//----------------------------------------------------------------------------
	void SetCamera(camCamera *i_pCamera, const std::string& i_Label);

	//----------------------------------------------------------------------------
	// Set directors cut and label. If this view is active, the camera
	//	manipulator will de disabled.
	//----------------------------------------------------------------------------
	void SetDirectorsCut(camsDirectorsCut *i_pDirectorsCut, const std::string& i_Label);

	//----------------------------------------------------------------------------
	// ConfirmCameraManip - make sure that the camera manip for this viewer is 
	//	tracking changes in the scripted camera. Should be called every
	//	Think() cycle.
	//----------------------------------------------------------------------------
	void ConfirmCameraManip();

	//----------------------------------------------------------------------------
	// ConfirmCamera - make sure that the camera for this viewer is one
	//	of the editor or scripted cameras. This should be called when 
	//	cameras are deleted.
	//----------------------------------------------------------------------------
	void ConfirmCamera();
	
	//--------------------------------------------------------------------
	// Focus_Camera centers camera with respect to the point
	//--------------------------------------------------------------------
	void FocusCamera(const maPoint3d& i_Focus, float i_Radius);

	//--------------------------------------------------------------------
	//	Return true if this render pane is the active view
	//--------------------------------------------------------------------
	bool IsActiveView();

	//--------------------------------------------------------------------
	//	Return true if this render pane is using a directors cut
	//--------------------------------------------------------------------
	bool HasDirectorsCut();

	//----------------------------------------------------------------------------
	// Select the camera object for the camera used in the view.
	// Switching out of directors cut mode if necessary.
	//----------------------------------------------------------------------------
	void SelectCamera();

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

	//------------------------------------------------------------------------
	// context menu handlers
	//------------------------------------------------------------------------
	void OnClearSelection(wxCommandEvent& i_Event);
	void OnSelectCamera(wxCommandEvent& i_Event);
	void OnEditorPersp(wxCommandEvent& i_Event);
	void OnEditorTop(wxCommandEvent& i_Event);
	void OnEditorFront(wxCommandEvent& i_Event);
	void OnEditorSide(wxCommandEvent& i_Event);
	void cameraScripted_Click(wxCommandEvent& i_Event);
	void directorsCut_Click(wxCommandEvent& i_Event);

	tma3dRenderView* m_pRenderView;
	rpnPanelViewer *m_pPanelViewer; // just a cast of the viewer owned by m_pRenderView

    DECLARE_EVENT_TABLE()
};

#endif // USE_WXWIDGETS
