/*****************************************************************************
**  rpnRenderPanel.cpp
**
**     Window using wxWidgets for doing MachStudio rendering
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Features/RenderPanels/wxGUI/rpnRenderPanel.hpp"

#include "Features/FilmGates/fgtFrameMgr.hpp"
#include "Features/Prefs/prefsQuickMgr.hpp"
#include "Features/RenderPanels/rpnOperations.hpp"
#include "Features/RenderPanels/rpnPanelViewer.hpp"

#include "Support/cams/camsCameraMgr.hpp"
#include "Support/cams/camsDirectorsCutMgr.hpp"
#include "Support/mnm/mnmConstants.hpp"
#include "Support/mnm/mnmTimeCodeMgr.hpp"

#include "Core/dbg/dbgLog.hpp"
#include "Tool/cam3d/cam3dMgr.hpp"
#include "Tool/gui/guiStatusBarMgr.hpp"
#include "Tool/sel3d/sel3dMgr.hpp"
#include "Tool/tma3d/tma3dCursorMgr.hpp"
#include "Tool/tma3d/tma3dRenderView.hpp"
#include "Tool/tma3d/tma3dScreenUtil.hpp"
#include "ToolUIWx/twx/twxMessaging.hpp"


#ifdef USE_WXWIDGETS

namespace
{
	//--------------------------------------------------------------------
	// IDs for the menu commands
	//--------------------------------------------------------------------
	enum
	{
		MENU_CLEARSELECTION = wxID_HIGHEST,
		MENU_SELECTCAMERA,
		MENU_EDITORPERSP,
		MENU_EDITORTOP,
		MENU_EDITORFRONT,
		MENU_EDITORSIDE,
		MENU_SCRIPTED = wxID_HIGHEST + 200,
		MENU_DCUT = MENU_SCRIPTED + 100
	};
}

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them.
BEGIN_EVENT_TABLE(rpnRenderPanel, wxControl)
	EVT_LEFT_DOWN(rpnRenderPanel::OnMouseDown)
	EVT_MIDDLE_DOWN(rpnRenderPanel::OnMouseDown)
	EVT_RIGHT_DOWN(rpnRenderPanel::OnMouseDown)
	EVT_MOTION(rpnRenderPanel::OnMouseMove)
	EVT_ENTER_WINDOW(rpnRenderPanel::OnEnter)
	EVT_LEAVE_WINDOW(rpnRenderPanel::OnLeave)
    EVT_SIZE(rpnRenderPanel::OnSize)
	EVT_KEY_UP(rpnRenderPanel::OnKeyUp)
	EVT_SET_FOCUS(rpnRenderPanel::OnFocus)
    EVT_MENU(MENU_CLEARSELECTION, rpnRenderPanel::OnClearSelection)
    EVT_MENU(MENU_SELECTCAMERA, rpnRenderPanel::OnSelectCamera)
    EVT_MENU(MENU_EDITORPERSP, rpnRenderPanel::OnEditorPersp)
    EVT_MENU(MENU_EDITORTOP, rpnRenderPanel::OnEditorTop)
    EVT_MENU(MENU_EDITORFRONT, rpnRenderPanel::OnEditorFront)
    EVT_MENU(MENU_EDITORSIDE, rpnRenderPanel::OnEditorSide)
END_EVENT_TABLE()


//--------------------------------------------------------------------
// canvas constructor
//--------------------------------------------------------------------
rpnRenderPanel::rpnRenderPanel(wxWindow* parent)
	: wxControl(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER),
	 m_pRenderView(NULL)
{
	this->SetBackgroundColour( *wxBLACK );
}

//--------------------------------------------------------------------
// Set render view to control
//--------------------------------------------------------------------
void rpnRenderPanel::SetRenderView(tma3dRenderView* i_pRenderView)
{
	m_pRenderView = i_pRenderView;
	if (m_pRenderView)
	{
		m_pPanelViewer = dynamic_cast<rpnPanelViewer*>(m_pRenderView->GetViewer());
		this->do_resize();
	}
	else
		m_pPanelViewer = NULL;
}

//----------------------------------------------------------------------------
// Enable rendering in this pane.
//----------------------------------------------------------------------------
void rpnRenderPanel::EnableViewer(bool i_bEnabled)
{
	if (this->m_pRenderView != NULL)
	{
		this->m_pRenderView->EnableViewer( i_bEnabled );
	}
}

//----------------------------------------------------------------------------
// Turn visibility of text label on or off
//----------------------------------------------------------------------------
void rpnRenderPanel::SetTextVisible(bool i_bShow)
{
	if (m_pPanelViewer)
	{
		m_pPanelViewer->SetTextVisible( i_bShow );
	}
}

//----------------------------------------------------------------------------
// Set camera and label. If this view is active, the camera manipulator
//	will also be set.
//----------------------------------------------------------------------------
void rpnRenderPanel::SetCamera(camCamera *i_pCamera, const std::string& i_Label)
{
	if (m_pRenderView)
	{
		this->m_pRenderView->SetCamera( i_pCamera );

		if (this->m_pPanelViewer)
		{
			this->m_pPanelViewer->SetTextString( i_Label );
			this->m_pPanelViewer->SetDirectorsCut( NULL );
		}

		//if (this->Focused)
		if (IsActiveView())
		{
			// Set our camera into cam3dMgr in order to activate the 
			//	camera manipulator
			this->m_pRenderView->ActivateCameraManipulator();

			// Highlight selection in GUI
			rpnOperations::HighlightCamera( i_pCamera );

			// Use our camera name in the status bar panel
			guiStatusBarMgr::SetText( mnmConstants::e_SBPanel_Camera, i_Label.c_str() );
			std::string tool_tip("Active Camera: ");
			tool_tip += i_Label;
			guiStatusBarMgr::SetToolTip(mnmConstants::e_SBPanel_Camera, tool_tip.c_str() );
		}
	}
}

//----------------------------------------------------------------------------
// Set directors cut and label. If this view is active, the camera
//	manipulator will de disabled.
//----------------------------------------------------------------------------
void rpnRenderPanel::SetDirectorsCut(camsDirectorsCut *i_pDirectorsCut, const std::string& i_Label)
{
	if (m_pRenderView)
	{
		if (this->m_pPanelViewer)
		{
			this->m_pPanelViewer->SetTextString( i_Label );
			this->m_pPanelViewer->SetDirectorsCut( i_pDirectorsCut );
		}

		//if (this->Focused)
		if (IsActiveView())
		{
			// Deactivate the camera manipulator while viewing the
			// directors cut - can't edit camera because the camera
			// is not selected in order to key the changes.
			cam3dMgr::EnableManip(false);

			// Highlight selection in GUI
			rpnOperations::HighlightDirectorsCut( i_pDirectorsCut );

			// Use our directors cut name in the status bar panel
			guiStatusBarMgr::SetText( mnmConstants::e_SBPanel_Camera, i_Label.c_str() );
			std::string tool_tip("Director's Cut: ");
			tool_tip += i_Label;
			guiStatusBarMgr::SetToolTip(mnmConstants::e_SBPanel_Camera, tool_tip.c_str() );
		}
	}
}


//----------------------------------------------------------------------------
// ConfirmCameraManip - make sure that the camera manip for this viewer is 
//	tracking changes in the scripted camera. Should be called every
//	Think() cycle.
//----------------------------------------------------------------------------
void rpnRenderPanel::ConfirmCameraManip()
{
	if (m_pRenderView && IsActiveView() && cam3dMgr::IsManipEnabled())
	{
		this->m_pRenderView->ActivateCameraManipulator();
	}
}

//----------------------------------------------------------------------------
// ConfirmCamera - make sure that the camera for this viewer is one
//	of the editor or scripted cameras. This should be called when 
//	cameras are deleted.
//----------------------------------------------------------------------------
void rpnRenderPanel::ConfirmCamera()
{
	if (m_pRenderView)
	{
		camCamera* pCamera = m_pRenderView->GetViewer()->GetCamera();

		// Check editor cameras
		if ((pCamera != &cam3dMgr::GetEditorCamera()) &&
			(pCamera != &cam3dMgr::GetTopCamera()) &&
			(pCamera != &cam3dMgr::GetSideCamera()) &&
			(pCamera != &cam3dMgr::GetFrontCamera()))
		{
			// Check scripted cameras
			int index = camsCameraMgr::GetIndexForCamera( pCamera );
			if (index < 0)
			{
				// Camera not found, so it was probably deleted,
				// switch to editor camera.
				this->SetCamera( &cam3dMgr::GetEditorCamera(), "Editor" );
			}
		}
	
		// Confirm director's cut still exists also
		if (this->m_pPanelViewer &&
			this->m_pPanelViewer->GetDirectorsCut())
		{
			int index = camsDirectorsCutMgr::GetIndexForDirectorsCut( this->m_pPanelViewer->GetDirectorsCut() );
			if (index < 0)
			{
				// Director's cut not found, so it was probably deleted,
				// switch to editor camera.
				this->SetCamera( &cam3dMgr::GetEditorCamera(), "Editor" );
			}
		}
	}
}

//--------------------------------------------------------------------
// Focus_Camera centers camera with respect to the point
//--------------------------------------------------------------------
void rpnRenderPanel::FocusCamera(const maPoint3d& i_Focus, float i_Radius)
{
	if (m_pRenderView)
	{
		// I'd rather be doing this through the camera manip interface,
		// but only the active panel has a camera manip.
		//
		camCamera* pCamera = m_pRenderView->GetViewer()->GetCamera();

		maVector3d view = pCamera->GetPosition() - pCamera->GetTarget();
		pCamera->LookAt(i_Focus + view, i_Focus, pCamera->GetUp());

		if (pCamera->IsOrthographic())
		{
			const float width_modifier = 1.6f;
			pCamera->SetOrthoWidth( width_modifier * i_Radius );
		}
	}
}

//--------------------------------------------------------------------
//	Return true if this render pane is the active view
//--------------------------------------------------------------------
bool rpnRenderPanel::IsActiveView()
{
	return (tma3dRenderView::GetActiveRenderView() == this->m_pRenderView);
}

//--------------------------------------------------------------------
//	Return true if this render pane is using a directors cut
//--------------------------------------------------------------------
bool rpnRenderPanel::HasDirectorsCut()
{
	return (this->m_pPanelViewer && 
			this->m_pPanelViewer->HasDirectorsCut() );
}

//----------------------------------------------------------------------------
// Select the camera object for the camera used in the view.
// Switching out of directors cut mode if necessary.
//----------------------------------------------------------------------------
void rpnRenderPanel::SelectCamera()
{
	if (m_pRenderView)
	{
		int cam_index = camsCameraMgr::GetIndexForCamera( m_pRenderView->GetViewer()->GetCamera() );
		if (cam_index < camsCameraMgr::GetNumCameras())
		{
			if ( this->HasDirectorsCut() )
			{
				// Switch from directors cut to the camera that 
				// was active in the cut at this time
				nameString name_str;
				camsCameraMgr::GetCameraName(cam_index, name_str);

				this->SetCamera(camsCameraMgr::GetCamera(cam_index),
											name_str.GetString());
			}
		
			rpnOperations::SelectCamera( cam_index );
		}
	}
}
//------------------------------------------------------------------------
//------------------------------------------------------------------------
void rpnRenderPanel::do_resize()
{
	wxRect rect = this->GetClientRect();
//	DBG_LOG2("Render window size: %d %d", rect.GetWidth(), rect.GetHeight());
	
	if (this->m_pRenderView != NULL)
	{		
		if (rect.GetWidth() > 0 && rect.GetHeight() > 0)
		{
			// Resize the underlying window
			this->m_pRenderView->ResizeWindow(rect.GetWidth(), rect.GetHeight());

			// Update the ScreenUtil function if we are the active render view
			if (tma3dRenderView::GetActiveRenderView() == this->m_pRenderView)
			{
				// The mouse move events later are going to be relative to our
				// screen position, so we don't need to give those offsets
				// to the screen util.
				tma3dScreenUtil::SetWindowSize( maPoint2d( 0, 0 ),
					maPoint2d( rect.GetWidth(), rect.GetHeight() ) );
				//tma3dScreenUtil::SetWindowSize( maPoint2d( rect.GetLeft(), rect.GetTop() ),
				//	maPoint2d( rect.GetWidth(), rect.GetHeight() ) );

				//	update the frame gate
				fgtFrameMgr::ResizeFrames();

				// Update the time code manager to adjust the text to
				// the new window size and aspect ratio
				mnmTimeCodeMgr::Update();
			}
		}
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void rpnRenderPanel::OnMouseDown(wxMouseEvent& i_Event)
{	
	if (!i_Event.LeftDown()) // Left mouse takes focus anyway when the event is skipped.
		this->SetFocus();
	tma3dCursorMgr::CursorOverViewCallback(true);		
	tma3dCursorMgr::CursorPosChangedCallback( i_Event.GetX(), i_Event.GetY() );

	if (i_Event.RightDown() && !i_Event.AltDown())
	{
		wxMenu menu;
		
		wxMenu *camera_menu = new wxMenu();

		// Add in scripted cameras and director's cuts
		std::vector<nameString> camera_names;
		camsCameraMgr::GetCameraNames(camera_names);
		for (int i=0; i<camera_names.size(); ++i)
		{
			wxMenuItem* pItem = camera_menu->Append(MENU_SCRIPTED+i, camera_names[i].GetString());
			this->Connect( pItem->GetId(), wxEVT_COMMAND_MENU_SELECTED,
				wxCommandEventHandler(rpnRenderPanel::cameraScripted_Click) );
		}
		if (!camera_names.empty())
			camera_menu->AppendSeparator();
		std::vector<nameString> dircut_names;
		camsDirectorsCutMgr::GetDirectorsCutNames(dircut_names);
		for (int i=0; i<dircut_names.size(); ++i)
		{
			wxMenuItem* pItem = camera_menu->Append(MENU_DCUT+i, dircut_names[i].GetString());
			this->Connect( pItem->GetId(), wxEVT_COMMAND_MENU_SELECTED,
				wxCommandEventHandler(rpnRenderPanel::directorsCut_Click) );
		}
		if (!dircut_names.empty())
			camera_menu->AppendSeparator();

		// Editor cameras at end
		camera_menu->Append(MENU_EDITORPERSP,  _T("Editor"));
		camera_menu->Append(MENU_EDITORTOP,  _T("Top"));
		camera_menu->Append(MENU_EDITORFRONT,  _T("Front"));
		camera_menu->Append(MENU_EDITORSIDE,  _T("Side"));

		// flattening the menus to make it easier to select
		//wxMenu *scripted_menu = new wxMenu();
		//wxMenu *dcut_menu = new wxMenu();

		//camera_menu->AppendSubMenu(editor_menu, _T("Editor"));
		//camera_menu->AppendSubMenu(scripted_menu, _T("Scripted"));
		//camera_menu->AppendSubMenu(dcut_menu, _T("Director's Cut"));
		menu.AppendSubMenu(camera_menu, _T("Camera"));
		
		//wxMenu *select_menu = new wxMenu();
		//select_menu->Append(MENU_CLEARSELECTION,  _T("Clear Selection"));
		//select_menu->Append(MENU_SELECTCAMERA,  _T("Select Camera"));
		//menu.AppendSubMenu(select_menu, _T("Select")); 
		menu.Append(MENU_CLEARSELECTION,  _T("Clear Selection"));
		menu.Append(MENU_SELECTCAMERA,  _T("Select Camera"));
		PopupMenu(&menu, i_Event.GetX(), i_Event.GetY());
	}
	if (i_Event.MiddleDown() && !i_Event.AltDown())
	{
		//	sum up the local locations to get the screen coordinates
		//
		//int x,y,w,h;
		//GetPosition(&x,&y);
		//GetSize(&w,&h);
		//prefsQuickMgr::Show(x+(w/2),y+(h/2));
		int sx,sy;
		GetScreenPosition(&sx,&sy);
		int x,y;
		x = sx + (i_Event.GetX());
		y = sy + (i_Event.GetY());
		DBG_LOG2("mouse middle-press %d,%d", x,y);
		prefsQuickMgr::Show( x, y );
	}

	i_Event.Skip();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void rpnRenderPanel::OnMouseMove(wxMouseEvent& i_Event)
{	
	if (this->IsActiveView())
	{
		//DBG_LOG3("OnMouseMove: %s (%d, %d)", m_pPanelViewer->GetTextString().c_str(), i_Event.GetX(), i_Event.GetY());
		tma3dCursorMgr::CursorPosChangedCallback( i_Event.GetX(), i_Event.GetY() );
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void rpnRenderPanel::OnEnter(wxMouseEvent& i_Event)
{	
	//DBG_LOG1("OnEnter: %s", m_pPanelViewer->GetTextString().c_str());
	if (this->IsActiveView())
		tma3dCursorMgr::CursorOverViewCallback(true);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void rpnRenderPanel::OnLeave(wxMouseEvent& i_Event)
{	
	//DBG_LOG1("OnLeave: %s", m_pPanelViewer->GetTextString().c_str());
	if (this->IsActiveView())
		tma3dCursorMgr::CursorOverViewCallback(false);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void rpnRenderPanel::OnSize(wxSizeEvent& i_Event)
{		
	this->do_resize();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void rpnRenderPanel::OnKeyUp(wxKeyEvent& i_Event)
{	
	// Handle hot keys here
	//twxMessaging::ProcessKeyEvent(i_Event);
	i_Event.Skip();

}
//--------------------------------------------------------------------
//--------------------------------------------------------------------
void rpnRenderPanel::OnFocus(wxFocusEvent& i_Event)
{	
	if (this->m_pRenderView)
	{
		//DBG_LOG1("Focus: %s", m_pPanelViewer->GetTextString().c_str());
		tma3dRenderView::SetActiveRenderView(this->m_pRenderView);

		wxRect rect = this->GetRect();
		if (rect.GetWidth() > 0 && rect.GetHeight() > 0)
		{
			// The mouse move events later are going to be relative to our
			// screen position, so we don't need to give those offsets
			// to the screen util.
			tma3dScreenUtil::SetWindowSize( maPoint2d( 0, 0 ),
				maPoint2d( rect.GetWidth(), rect.GetHeight() ) );
			//tma3dScreenUtil::SetWindowSize( maPoint2d( (float)rect.GetLeft(), (float)rect.GetTop() ),
			//	maPoint2d( (float)rect.GetWidth(), (float)rect.GetHeight() ) );

			//	update the frame gate
			fgtFrameMgr::ResizeFrames();
		}

		if (this->HasDirectorsCut())
		{
			// Deactivate the camera manipulator while viewing the
			// directors cut - can't edit camera because the camera
			// is not selected in order to key the changes.
			cam3dMgr::EnableManip(false);

			// Highlight selection in GUI
			rpnOperations::HighlightDirectorsCut( this->m_pPanelViewer->GetDirectorsCut() );

			// Status bar tool tip
			std::string tool_tip("Director's Cut: ");
			tool_tip += this->m_pPanelViewer->GetTextString();
			guiStatusBarMgr::SetToolTip(mnmConstants::e_SBPanel_Camera, tool_tip.c_str() );
		}
		else
		{
			// Set our camera into cam3dMgr in order to activate the 
			//	camera manipulator
			this->m_pRenderView->ActivateCameraManipulator();

			// Highlight selection in GUI
			rpnOperations::HighlightCamera( this->m_pRenderView->GetViewer()->GetCamera() );

			// Status bar tool tip
			std::string tool_tip("Active Camera: ");
			tool_tip += this->m_pPanelViewer->GetTextString();
			guiStatusBarMgr::SetToolTip(mnmConstants::e_SBPanel_Camera, tool_tip.c_str() );
		}

		if (this->m_pPanelViewer)
		{
			// Use our camera name in the status bar panel
			guiStatusBarMgr::SetText( mnmConstants::e_SBPanel_Camera, 
				this->m_pPanelViewer->GetTextString().c_str() );
		}
	}
}

//------------------------------------------------------------------------
// context menu handlers
//------------------------------------------------------------------------
void rpnRenderPanel::OnClearSelection(wxCommandEvent& i_Event)
{
	sel3dMgr::CreateUndoOperation();
	sel3dMgr::ClearSelection();
}
void rpnRenderPanel::OnSelectCamera(wxCommandEvent& i_Event)
{
	this->SelectCamera();
}
void rpnRenderPanel::OnEditorPersp(wxCommandEvent& i_Event)
{
	this->SetCamera( &cam3dMgr::GetEditorCamera(), "Editor" );
}
void rpnRenderPanel::OnEditorTop(wxCommandEvent& i_Event)
{
	this->SetCamera( &cam3dMgr::GetTopCamera(), "Top" );
}
void rpnRenderPanel::OnEditorFront(wxCommandEvent& i_Event)
{
	this->SetCamera( &cam3dMgr::GetFrontCamera(), "Front" );
}
void rpnRenderPanel::OnEditorSide(wxCommandEvent& i_Event)
{
	this->SetCamera( &cam3dMgr::GetSideCamera(), "Side" );
}
void rpnRenderPanel::cameraScripted_Click(wxCommandEvent& i_Event)
{
	int index = i_Event.GetId() - MENU_SCRIPTED;
	camCamera *pCamera = camsCameraMgr::GetCamera(index);
	nameString name;
	camsCameraMgr::GetCameraName(index, name);
	this->SetCamera( pCamera, name.GetString() );
}
void rpnRenderPanel::directorsCut_Click(wxCommandEvent& i_Event)
{
	int index = i_Event.GetId() - MENU_DCUT;
	camsDirectorsCut *pDirectorsCut = camsDirectorsCutMgr::GetDirectorsCut(index);
	nameString name;
	camsDirectorsCutMgr::GetDirectorsCutName(index, name);
	this->SetDirectorsCut( pDirectorsCut, name.GetString() );
}

#endif // USE_WXWIDGETS