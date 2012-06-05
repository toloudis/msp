/*****************************************************************************
**  rpnCameraChoice.hpp
**
**     Choice box displaying current and available cameras and
**	director's cuts
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#ifdef RPN_CAMERACHOICE_HPP
#error rpnCameraChoice.hpp multiply included
#endif
#define RPN_CAMERACHOICE_HPP

#ifndef TWX_WIDGETS_HPP
#include "ToolUIWx/twx/twxWidgets.hpp"
#endif

#ifdef USE_WXWIDGETS

//============================================================================
//============================================================================
class camCamera;
class rpnRenderPanel;

//============================================================================
// Define a new control type derived from a combo box
//============================================================================
class rpnCameraChoice : public wxChoice
{
public:
	//--------------------------------------------------------------------
	//	Static pointer to the instance of the form, will be cleared
	//	when the object dialog is deleted.
	//--------------------------------------------------------------------
	static rpnCameraChoice* Instance;

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
    rpnCameraChoice(wxWindow* parent);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
    ~rpnCameraChoice();

	//------------------------------------------------------------------------
	// Set list of camera names 
	//------------------------------------------------------------------------
    void Update();

	//------------------------------------------------------------------------
	// This function is called from outside when a render panel
	// changes, all we need to do is set the index, not do any notifying
	//------------------------------------------------------------------------
	void SetSelectedIndex(int i_Index);

	//------------------------------------------------------------------------
	// Swap editor and scripted camera
	//------------------------------------------------------------------------
    void DoSwapCam();

private:
	//------------------------------------------------------------------------
	// Set camera in render panels based on selection
	//------------------------------------------------------------------------
	int update_selected_index();

	//------------------------------------------------------------------------
	// set the editor camera active
	//------------------------------------------------------------------------
	void select_editor_cam();

	//------------------------------------------------------------------------
	// event callbacks
	//------------------------------------------------------------------------
	void SelectedIndexChanged(wxCommandEvent& i_Event);

	int m_CameraLastIndex;
	bool m_bDisableNotify;
};

#endif // USE_WXWIDGETS
