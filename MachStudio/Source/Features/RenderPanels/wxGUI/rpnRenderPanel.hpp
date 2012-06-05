/*****************************************************************************
**	rpnRenderPanel.hpp
**
**		Window using wxWidgets for doing MachStudio rendering
**
**	StudioGPU
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
class g3dPickInfo;
class gpxCamera;
class camsDirectorsCut;
class rpnPanelViewer;
class tma3dRenderView;
class twxContextMenu;


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

	//------------------------------------------------------------------------
	// Set whether this render panel should display the context menu items
	// related to changing the render pass.
	//------------------------------------------------------------------------
	void SetCanChangeRenderPass(bool i_bEnable);

	//----------------------------------------------------------------------------
	// Enable rendering in this pane.
	//----------------------------------------------------------------------------
	void EnableViewer(bool i_bEnabled);

	//----------------------------------------------------------------------------
	// Turn visibility of text label on or off
	//----------------------------------------------------------------------------
	void SetTextVisible(bool i_bShow);

	//----------------------------------------------------------------------------
	// Set pass label.
	//----------------------------------------------------------------------------
	void SetPass(const std::string& i_Label);

	//----------------------------------------------------------------------------
	// Set camera and label. If this view is active, the camera manipulator
	//	will also be set.
	//----------------------------------------------------------------------------
	void SetCamera(gpxCamera *i_pCameraProxy, const std::string& i_Label);

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
	//	Return true if this render pane has keyboard focus
	//--------------------------------------------------------------------
	bool GetHasFocus();

	//--------------------------------------------------------------------
	//	Return true if this render pane is using a directors cut
	//--------------------------------------------------------------------
	bool HasDirectorsCut();

	//--------------------------------------------------------------------
	//	Set the focus distance to the given depth
	//--------------------------------------------------------------------
	void FocusAtDepth(const float& i_Depth);

	//----------------------------------------------------------------------------
	// Select the camera object for the camera used in the view.
	// Switching out of directors cut mode if necessary.
	//----------------------------------------------------------------------------
	void SelectCamera();

	//----------------------------------------------------------------------------
	// SetMaxRenderSize - set maximum size allowed for the render area.
	//	Use -1 -1 in order to remove the constraints and allow any resizing.
	//----------------------------------------------------------------------------
	void SetMaxRenderSize(int i_Width, int i_Height);

protected:
	//------------------------------------------------------------------------
	// Override the wxWidgets SetSize function in order to enforce a
	//	maximum size for the render area.
	//------------------------------------------------------------------------
	virtual void DoSetSize(int x, int y,
						   int width, int height,
						   int sizeFlags);

private:
	//------------------------------------------------------------------------
	// private functions
	//------------------------------------------------------------------------
	void do_resize();
	void add_cameras_to_context_menu(twxContextMenu &i_Menu);
	void add_renderpasses_to_context_menu(twxContextMenu &io_ContextMenu);
	void do_pick_at_cursor(int i_X, int i_Y, g3dPickInfo& o_PickInfo);
	void build_text_string();

	//------------------------------------------------------------------------
	// event handlers (these functions should _not_ be virtual)
	//------------------------------------------------------------------------
	void OnPaint(wxPaintEvent& i_Event);
	void OnMouseDown(wxMouseEvent& i_Event);
	void OnMouseMove(wxMouseEvent& i_Event);
	void OnEnter(wxMouseEvent& i_Event);
	void OnLeave(wxMouseEvent& i_Event);
	void OnSize(wxSizeEvent& i_Event);
	void OnKeyUp(wxKeyEvent& i_Event);
	void OnFocus(wxFocusEvent& i_Event);
	void OnLoseFocus(wxFocusEvent& i_Event);

	//------------------------------------------------------------------------
	// context menu handlers
	//------------------------------------------------------------------------
	void clear_selection();
	void select_camera();
	void focus_camera(const float& i_Depth);
	void OnEditorPersp();
	void OnEditorTop();
	void OnEditorFront();
	void OnEditorSide();
	void cameraScripted_Click(int i_Index);
	void directorsCut_Click(int i_Index);

private:
	tma3dRenderView* m_pRenderView;
	rpnPanelViewer *m_pPanelViewer; // just a cast of the viewer owned by m_pRenderView
	int m_MaxWidth, m_MaxHeight;
	bool m_bHasFocus;
	bool m_bCanChangeRenderPass;
	std::string	m_CameraName;
	std::string	m_PassName;

	DECLARE_EVENT_TABLE()
};

#endif // USE_WXWIDGETS
